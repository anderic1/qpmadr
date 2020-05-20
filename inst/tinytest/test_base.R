
library(qpmadr)

n = 6
k = 3

set.seed(42)

Q = crossprod(matrix(rnorm(n*n), n))
Q0 = Q
Q0[1,1] = 0
A = matrix(5, k, n)

Alb = c(1, numeric(k-1))
l = rep_len(1, n)


expect_error(solveqp(Q))
expect_error(solveqp(Q, A=A))
expect_true(solveqp(Q, lb = l)$status == 0)
expect_true(solveqp(Q0, lb = l)$status == -1)
expect_true(all(is.finite(solveqp(Q, A=A, Alb=Alb)$solution)))



resLlt = solveqp(Q, A=A, Alb=Alb)
resQpm = solveqp(Q, A=A, Alb=Alb, pars = list(checkPD = FALSE))
resChol = solveqp(t(chol(Q)), A=A, Alb=Alb, pars = list(isFactorized=TRUE, checkPD = FALSE))

expect_equal(resLlt, resQpm)
expect_equal(resLlt, resChol)

set.seed(42)
Q1 = crossprod(matrix(rnorm(n*n), n))
expect_equal(Q, Q1)

