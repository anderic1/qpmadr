
#include "RcppEigen.h"
#include <string>
#include "qpmad/solver.h"



// [[Rcpp::export]]
Rcpp::List solveqpImpl(Eigen::MatrixXd& H,
                       const Eigen::VectorXd& h ,
                       const Eigen::VectorXd& lb  ,
                       const Eigen::VectorXd& ub  ,
                       const Eigen::MatrixXd& A   ,
                       const Eigen::VectorXd& Alb,
                       const Eigen::VectorXd& Aub,
                       bool isFactorized,
                       int maxIter,
                       double tol,
                       bool checkPD)
{

  using namespace Rcpp;


  Eigen::VectorXd x(H.rows());


  qpmad::Solver   solver;
  qpmad::SolverParameters pars;


  pars.tolerance_ = tol;
  pars.max_iter_ = maxIter;
  if (isFactorized)
  {
    if (checkPD && (H.diagonal().array() < DBL_EPSILON).any())
    {
      x.setConstant(NA_REAL);

      List reslist = List::create(_["solution"] = x,
                                  _["status"] = -1,
                                  _["message"] = "Numerical issue, matrix (probably) not positive definite");
      return reslist;
    }
    pars.hessian_type_ = qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR;
  }
  else if (checkPD)
  {
    Eigen::LLT<Eigen::Ref<Eigen::MatrixXd>, Eigen::Lower> chol(H);
    if (chol.info() == Eigen::NumericalIssue)
    {

      x.setConstant(NA_REAL);
      List reslist = List::create(_["solution"] = x,
                                  _["status"] = -1,
                                  _["message"] = "Numerical issue, matrix (probably) not positive definite");
      return reslist;
    }
    pars.hessian_type_ = qpmad::SolverParameters::HESSIAN_CHOLESKY_FACTOR;
  }



  qpmad::Solver::ReturnStatus return_value;

  try
  {
    return_value = solver.solve(x, H, h, lb, ub, A, Alb, Aub, pars);

  }
  catch(std::exception &e)
  {
    std::string emsg(e.what());
    Rcpp::stop(emsg);
  }

  Rcpp::String msg;

  switch(return_value)
  {
    case qpmad::Solver::ReturnStatus::OK:
      msg = "Ok";
      break;
    case qpmad::Solver::ReturnStatus::INCONSISTENT:
      msg = "Inconsistent constraints";
      break;
    case qpmad::Solver::ReturnStatus::INFEASIBLE_EQUALITY:
      msg = "Infeasible equality";
      break;
    case qpmad::Solver::ReturnStatus::INFEASIBLE_INEQUALITY:
      msg = "Infeasible inequality";
      break;
    case qpmad::Solver::ReturnStatus::MAXIMAL_NUMBER_OF_ITERATIONS:
      msg = "Maximal number of iterations reached";
      break;
    default:
      Rcpp::stop("Unknown status code returned [%i]", static_cast<int>(return_value));
    break;
  }


  return List::create(_["solution"] = x,
                      _["status"] = static_cast <int>(return_value),
                      _["message"] = msg);
}




