#' @useDynLib qpmadr, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @importFrom checkmate assert assertMatrix assertNumeric assertLogical assertIntegerish assertNumber
#' @importFrom utils modifyList
NULL



.onUnload <- function (libpath) {
  library.dynam.unload("qpmadr", libpath)
}

