/**
    @file
    @author  Alexander Sherikov

    @copyright 2017 Alexander Sherikov. Licensed under the Apache License,
    Version 2.0. (see LICENSE or http://www.apache.org/licenses/LICENSE-2.0)

    @brief
*/

#pragma once


namespace qpmad
{
    class FactorizationData
    {
        public:
            QPMatrix    QLi_aka_J;
            QPMatrix    R;
            MatrixIndex primal_size_;
#ifdef QPMAD_USE_HOUSEHOLDER
            QPMatrix    householder_workspace_;
#endif
            MatrixIndex length_nonzero_head_d_;


        public:
            template <class t_MatrixType>
                void initialize(const t_MatrixType &H,
                                const MatrixIndex primal_size)
            {
                primal_size_ = primal_size;

                QLi_aka_J.resize(primal_size_, primal_size_);
                QLi_aka_J.triangularView<Eigen::Lower>().setZero();
                TriangularInversion::compute(QLi_aka_J, H);

                R.resize(primal_size_, primal_size_ + 1);
                length_nonzero_head_d_ = primal_size_;
#ifdef QPMAD_USE_HOUSEHOLDER
                householder_workspace_.resize(primal_size_, primal_size_);
#endif
            }


            bool update(const MatrixIndex R_col,
#ifdef QPMAD_USE_HOUSEHOLDER
                        const bool /*is_simple*/,
#else
                        const bool is_simple,
#endif
                        const double tolerance)
            {
#ifdef QPMAD_USE_HOUSEHOLDER
                double tau;
                double beta;


                R.col(R_col).segment(R_col, length_nonzero_head_d_ - R_col).makeHouseholderInPlace(tau, beta);
                R(R_col, R_col) = beta;
                QLi_aka_J.middleCols(R_col, length_nonzero_head_d_ - R_col).transpose().applyHouseholderOnTheLeft(
                        R.col(R_col).segment(R_col+1, length_nonzero_head_d_ - R_col - 1), tau, householder_workspace_.data());
                /*
                R.col(R_col).tail(primal_size_ - R_col).makeHouseholderInPlace(tau, beta);
                QLi_aka_J.rightCols(primal_size_ - R_col).transpose().applyHouseholderOnTheLeft(
                        R.col(R_col).tail(primal_size_ - R_col - 1), tau, householder_workspace_.data());
                R(R_col, R_col) = beta;
                */


                return ( std::abs(beta) > tolerance );
#else
                GivensRotation<double>    givens;
                if (is_simple)
                {
                    for (MatrixIndex i = length_nonzero_head_d_-1; i > R_col;)
                    {
                        MatrixIndex j;
                        for (j = i-1; (0.0 == R(j, R_col)) && (j > R_col); --j)
                        {}
                        givens.computeAndApply(R(j, R_col), R(i, R_col), 0.0);
                        givens.applyColumnWise(QLi_aka_J, 0, primal_size_, j, i);
                        i = j;
                    }
                }
                else
                {
                    for (MatrixIndex i = length_nonzero_head_d_-1; i > R_col; --i)
                    {
                        givens.computeAndApply(R(i-1, R_col), R(i, R_col), 0.0);
                        givens.applyColumnWise(QLi_aka_J, 0, primal_size_, i-1, i);
                    }
                }

                return ( std::abs(R(R_col, R_col)) > tolerance );
#endif
            }


            void downdate(  const MatrixIndex R_col_index,
                            const MatrixIndex R_cols)
            {
                GivensRotation<double>    givens;
                for (MatrixIndex i = R_col_index + 1; i < R_cols; ++i)
                {
                    givens.computeAndApply(R(i-1, i), R(i, i), 0.0);
                    givens.applyColumnWise(QLi_aka_J, 0, primal_size_, i-1, i);
                    // 'R_cols+1' -- update 'd' as well
                    givens.applyRowWise(R, i+1, R_cols+1, i-1, i);

                    R.col(i-1).segment(0, i) = R.col(i).segment(0, i);
                }
                // vector 'd'
                R.col(R_cols-1) = R.col(R_cols);
            }


            template<class t_VectorType>
                void computeEqualityPrimalStep( t_VectorType            & step_direction,
                                                const MatrixIndex       simple_bound_index,
                                                const MatrixIndex       active_set_size)
            {
                // vector 'd'
                R.col(active_set_size) = QLi_aka_J.row(simple_bound_index).transpose();

                computePrimalStepDirection(step_direction, active_set_size);
            }


            template<   class t_VectorType,
                        class t_RowVectorType>
                void computeEqualityPrimalStep( t_VectorType            & step_direction,
                                                const t_RowVectorType   & ctr,
                                                const MatrixIndex       active_set_size)
            {
                // vector 'd'
                R.col(active_set_size).noalias() = QLi_aka_J.transpose() * ctr.transpose();

                computePrimalStepDirection(step_direction, active_set_size);
            }


            template<class t_VectorType0>
                void   computeInequalityPrimalStep( t_VectorType0           & primal_step_direction,
                                                    const ActiveSet         & active_set)
            {
                computePrimalStepDirection(primal_step_direction, active_set.size_);
            }



            template<   class t_VectorType,
                        class t_MatrixType>
                void computeInequalityDualStep( t_VectorType            & dual_step_direction,
                                                const ChosenConstraint  & chosen_ctr,
                                                const t_MatrixType      & A,
                                                const ActiveSet         & active_set)
            {
                if (chosen_ctr.is_simple_)
                {
                    if (chosen_ctr.is_lower_)
                    {
                        R.col(active_set.size_) = - QLi_aka_J.row(chosen_ctr.index_).transpose();
                    }
                    else
                    {
                        R.col(active_set.size_) =   QLi_aka_J.row(chosen_ctr.index_).transpose();
                    }
                    for (   length_nonzero_head_d_ = primal_size_-1;
                            (0.0 == R(length_nonzero_head_d_,active_set.size_)) && (length_nonzero_head_d_ > active_set.size_);
                            --length_nonzero_head_d_)
                    {}
                    ++length_nonzero_head_d_;
                }
                else
                {
                    if (chosen_ctr.is_lower_)
                    {
                        R.col(active_set.size_).noalias() =
                            - QLi_aka_J.transpose() * A.row(chosen_ctr.general_constraint_index_).transpose();
                    }
                    else
                    {
                        R.col(active_set.size_).noalias() =
                            QLi_aka_J.transpose() * A.row(chosen_ctr.general_constraint_index_).transpose();
                    }
                    length_nonzero_head_d_ = primal_size_;
                }

                computeDualStepDirection(dual_step_direction, active_set);
            }


            template<   class t_VectorType0,
                        class t_VectorType1>
                void   updateStepsAfterPartialStep( t_VectorType0           & primal_step_direction,
                                                    t_VectorType1           & dual_step_direction,
                                                    const ActiveSet         & active_set)
            {
                primal_step_direction.noalias() -= QLi_aka_J.col(active_set.size_) * R(active_set.size_, active_set.size_);
                computeDualStepDirection(dual_step_direction, active_set);
            }


            template<   class t_VectorType0,
                        class t_VectorType1>
                void updateStepsAfterPureDualStep(  t_VectorType0           & primal_step_direction,
                                                    t_VectorType1           & dual_step_direction,
                                                    const ActiveSet         & active_set)
            {
                primal_step_direction.noalias() = -QLi_aka_J.col(active_set.size_) * R(active_set.size_, active_set.size_);
                computeDualStepDirection(dual_step_direction, active_set);
            }


        private:
            template<class t_VectorType>
                void computePrimalStepDirection(t_VectorType            & step_direction,
                                                const MatrixIndex       active_set_size)
            {
                step_direction.noalias() =
                    - QLi_aka_J.middleCols(active_set_size, length_nonzero_head_d_ - active_set_size)
                    * R.col(active_set_size).segment(active_set_size, length_nonzero_head_d_ - active_set_size);
            }


            template<class t_VectorType>
                void computeDualStepDirection(  t_VectorType            & step_direction,
                                                const ActiveSet         &active_set)
            {
                step_direction.segment(active_set.num_equalities_, active_set.num_inequalities_).noalias() =
                    - R.block(active_set.num_equalities_,
                            active_set.num_equalities_,
                            active_set.num_inequalities_,
                            active_set.num_inequalities_).triangularView<Eigen::Upper>().solve(
                                R.col(active_set.size_).segment(
                                    active_set.num_equalities_,
                                    active_set.num_inequalities_));
            }
    };
}
