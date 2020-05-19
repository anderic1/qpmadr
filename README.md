
<!-- README.md is generated from README.Rmd. Please edit that file -->

# qpmadR

<!-- badges: start -->

<!-- badges: end -->

`qpmadR` provides R-bindings to the quadratic programming-solver
`qpmad`, written by [Alexander Sherikov](https://github.com/asherikov).

## Installation

You can install the released version of qpmadR from
[CRAN](https://CRAN.R-project.org) with:

``` r
install.packages("qpmadR")
```

## Example

This is an example which shows you how to solve a simple problem:

\[ \min_{\rm x}{ \space x'H x} \]

\[ s.t. \space \sum_{i}{x_i} = n \]

\[ -2 \leq x_i \leq 2 \]

where \(H\) is a random positive definite matrix of size \(n \times n\),
and \(x\) is a (column) vector of size \(n\).

The code below will run a benchmark against the quadprog solver for
n=100, checking that both give the same results.

``` r
library(qpmadR)
library(quadprog)
library(microbenchmark)


set.seed(42)

n = 100

H = crossprod(matrix(rnorm(n*n), n))

# constraint specification for qpmadR
lb = -2
ub = 2
A = matrix(1, 1, n)
Alb = n
Aub = n

# constraint specification for quadprog
At = cbind(rep_len(1, n), diag(1, n, n), diag(-1, n, n))
b = c(n, rep_len(-2, 2*n))



bm = microbenchmark(
  check    = "equal",
  qpmadR   = qpmadR::solveqp(H, lb=lb, ub=ub, A=A, Alb=Alb, Aub=Aub)$solution,
  quadprog = quadprog::solve.QP(H, numeric(n), At, b, meq=1)$solution
)


knitr::kable(summary(bm, "relative"), digits=1)
```

| expr     | min |  lq | mean | median |  uq | max | neval |
| :------- | --: | --: | ---: | -----: | --: | --: | ----: |
| qpmadR   | 1.0 | 1.0 |  1.0 |    1.0 | 1.0 | 1.0 |   100 |
| quadprog | 2.5 | 2.5 |  2.3 |    2.6 | 2.3 | 1.7 |   100 |

Timings are relative.

## C++-interface

The solver is a c++ header-only library can be used in other packages
via the LinkingTo: field
