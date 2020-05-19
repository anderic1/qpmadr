library(qpmadR)


## Assume we want to minimize: -(0 5 0) %*% b + 1/2 b^T b
## under the constraints:      A^T b >= b0
## with b0 = (-8,2,0)^T
## and      (-4  2  0)
##      A = (-3  1 -2)
##          ( 0  0  1)
## we can use solve.QP as follows:
## s1 = solve.QP(Dmat, dvec, Amat, bvec)

Dmat       <- matrix(0,3,3)
diag(Dmat) <- 1
dvec       <- c(0,5,0)
Amat       <- matrix(c(-4,-3,0,2,1,0,0,-2,1),3,3)
bvec       <- c(-8,2,0)



s2 = solveqp(Dmat,-dvec, c(0, 0, 0) ,NULL,t(Amat),bvec, NULL)
# print(s2)
expect_equivalent(s2$solution, c(0.476190, 1.047619, 2.095238), 1e-6)


