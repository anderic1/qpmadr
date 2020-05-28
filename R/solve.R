


#' Quadratic Programming
#'
#' Solves
#' \deqn{argmin 0.5 x' H x + h' x}
#' s.t.
#' \deqn{lb_i \leq x_i \leq ub_i}{lb_i \le x_i \le ub_i}
#' \deqn{Alb_i \leq (A x)_i \leq Aub_i}{Alb_i \le (A x)_i \le Aub_i}
#'
#' @param H Symmetric \strong{positive definite} matrix, n*n. Only the lower triangular part is used.
#' @param h \emph{Optional}, vector of length n.
#' @param lb,ub \emph{Optional}, lower/upper bounds of \code{x}. Will be repeated n times if length is one.
#' @param A  \emph{Optional}, constraints matrix of dimension p*n, where each row corresponds to a constraint. For equality constraints let corresponding elements in \code{Alb} equal those in \code{Aub}
#' @param Alb,Aub \emph{Optional}, lower/upper bounds for \eqn{Ax}.
#' @param pars \emph{Optional}, qpmad-solver parameters, conveniently set with \code{\link{qpmadParameters}}
#'
#' @seealso \code{\link{qpmadParameters}}
#'
#' @return At least one of \code{lb}, \code{ub} or \code{A} must be specified. If \code{A} has been
#' specified then also at least one of \code{Alb} or \code{Aub}. Returns a list with elements \code{solution} (the solution vector),
#' \code{status} (a status code) and \code{message} (a human readable message). If \code{status} = \code{0} the algorithm has converged.
#' Possible status codes:
#' \itemize{
#'   \item{\code{0}: Ok}
#'   \item{\code{-1}: Numerical issue, matrix (probably) not positive definite}
#'   \item{\code{1}: Inconsistent}
#'   \item{\code{2}: Infeasible equality}
#'   \item{\code{3}: Infeasible inequality}
#'   \item{\code{4}: Maximal number of iterations}
#' }
#'
#' @examples
#' ## Assume we want to minimize: -(0 5 0) %*% b + 1/2 b^T b
#' ## under the constraints:      A^T b >= b0
#' ## with b0 = (-8,2,0)^T
#' ## and      (-4  2  0)
#' ##      A = (-3  1 -2)
#' ##          ( 0  0  1)
#' ## we can use solveqp as follows:
#' ##
#' Dmat       <- diag(3)
#' dvec       <- c(0,-5,0)
#' Amat       <- t(matrix(c(-4,-3,0,2,1,0,0,-2,1),3,3))
#' bvec       <- c(-8,2,0)
#' solveqp(Dmat,dvec,A=Amat,Alb=bvec)
#' @export
solveqp = function(H, h=NULL, lb=NULL, ub=NULL, A=NULL, Alb=NULL, Aub=NULL, pars=list()) {

  assert(!is.null(lb), !is.null(ub), !is.null(A) && (!is.null(Alb) || !is.null(Aub)))

  pars = validateQpmadPars(pars)

  n = ncol(H)

  if (is.null(h)) {
    h = numeric()
  } else {
    assertNumeric(h, finite = TRUE, len = n)
  }

  if (!is.null(lb) || !is.null(ub)) {
    if (is.null(lb)) {
      lb = rep_len(-Inf, n)
    } else {
      if (length(lb) == 1)
        lb = rep_len(lb[[1]], n)
      assertNumeric(lb, any.missing = FALSE, len = n)
    }

    if (is.null(ub)) {
      ub = rep_len(Inf, n)
    } else {
      if (length(ub) == 1)
        ub = rep_len(ub[[1]], n)
      assertNumeric(ub, any.missing = FALSE, len = n)
    }
  } else {
    lb = ub = numeric()
  }

  if (is.null(A)) {
    A = matrix(numeric(), nrow = 0, ncol = 0)
    Alb = Aub = numeric()
  } else {
    assertMatrix(A, mode="numeric", any.missing = FALSE, ncols = n)
    k = nrow(A)

    if (is.null(Alb)) {
      Alb = rep_len(-Inf, k)
    } else {
      assertNumeric(Alb, any.missing = FALSE, len = k)
    }

    if (is.null(Aub)) {
      Aub = rep_len(Inf, k)
    } else {
      assertNumeric(Aub, any.missing = FALSE, len = k)
    }
  }


  result = solveqpImpl(H, h, lb, ub, A, Alb, Aub, pars$isFactorized, pars$maxIter, pars$tol, pars$checkPD)

  result
}




quadprog.solve.QP = function(Dmat, d, Amat, bvec, meq=0, factorized = FALSE) {

  Aub = rep_len(Inf, length(bvec))

  Aub[seq_len(meq)] = bvec[seq_len(meq)]

  if (isTRUE(factorized))
    Dmat = t(solve(Dmat))

  res = solveqp(Dmat, -d, A=t(Amat), Alb=bvec, Aub=Aub, pars=list(isFactorized = factorized))

  x = res$solution
  return(list(solution = x,
              value = drop(x %*% Dmat %*% x / 2 - d %*% x) ,
              unconstrained.solution = solve(Dmat, -d),
              iterations = NULL,
              Lagrangian = NULL,
              iact = NULL,
              status = res$status))
}




#' Set qpmad parameters
#'
#' @param isFactorized If \code{TRUE} then \code{H} is a lower Cholesky factor.
#' @param maxIter Maximum number of iterations, if not positive then no limit.
#' @param tol Convergence tolerance.
#' @param checkPD If \code{FALSE} then \code{H} is assumed to be positive definite and no checks are made.
#'
#' @return a list suitable to be used as the pars-argument to \code{\link{solveqp}}
#' @seealso \code{\link{solveqp}}
#'
#' @examples
#'
#' qpmadParameters(checkPD = FALSE)
#'
#' @export
qpmadParameters = function(isFactorized = FALSE, maxIter = -1, tol = 1e-12, checkPD = TRUE) {

  list(isFactorized=isFactorized, maxIter=maxIter, tol=tol, checkPD=checkPD)

}


.defaultPars = list(isFactorized = FALSE, maxIter = -1, tol = 1e-12, checkPD = TRUE)

validateQpmadPars = function(pars) {

  pars = modifyList(.defaultPars, pars)

  assertNumber(pars$tol, lower = 0, .var.name="tol")
  pars$tol = max(pars$tol, .Machine$double.eps)

  assertIntegerish(pars$maxIter, any.missing = FALSE, len = 1, .var.name="maxIter")

  assertLogical(pars$isFactorized, any.missing = FALSE, len = 1, .var.name="isFactorized")

  assertLogical(pars$checkPD, any.missing = FALSE, len = 1, .var.name="checkPD")

  pars
}
