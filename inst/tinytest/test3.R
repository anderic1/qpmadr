
library(qpmadR)
# The test problem is the following:
# Given:
# G =  4 -2   g0^T = [6 0]
#     -2  4
# Solve:
#   min f(x) = 1/2 x G x + g0 x
# s.t.
# x_1 + x_2 = 3
# x_1 >= 0
# x_2 >= 0
# x_1 + x_2 >= 2
# The solution is x^T = [1 2] and f(x) = 1S2

G = matrix(c(4, -2, -2, 4), 2)
g0 = c(6, 0)

A = rbind(c(1, 1),
          c(1, 1))
Alb = c(3, 2)
Aub = c(3, Inf)
res = solveqp(G, g0, c(0, 0), c(Inf, Inf), A, Alb, Aub)
# print(res)
sol = c(1, 2)
expect_equal(res$solution, sol)

# quadprog::solve.QP(G, -g0, cbind(t(A), diag(2)), c(3, 2, 0, 0), meq=1)
