# $Id: extract.MSY.R 1828 2007-11-30 01:16:07Z adunn $
"extract.MSY"<-
function(file,path="") {
  # Extracts deterministic MSY data from a file output from casal
  #
  # The MSY component is a list with components
  #    "MSY"   "B.MSY" "dat" and (possibly) MSY.current
  #
  # The dat components contain the results for each trial value of H
  #
  # Harvest rates for which CASAL reports
  # "fishing pressure limit exceeded in deterministic sim" are ignored
  #
  string.match <- function(x, y) return((1:length(y))[casal.regexpr(x, y) > 0])
  unlapply <- function(lst, fun, ...) unlist(lapply(lst, fun, ...))
  out <- list()
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  first.chars <- unlapply(file, function(x) substring(x, 1, 3))
  Hline <- string.match("F ", first.chars)
  if(length(Hline) > 0) {
    n.stocks <- 1
    overfish <- string.match("fishing pressure limit exceeded", file[Hline])
    if(length(overfish) > 0)  Hline <- Hline[ - overfish]
    nH <- length(Hline)
    tmp <- array(unlapply(file[Hline], casal.string.to.vector.of.words), c(2, 3, nH))
    mat <- matrix(as.numeric(tmp[2,  ,  ]), ncol = 3, byrow = T, dimnames = list(paste(1:nH), c("F", "Cf", "SSBf")))
    H <- mat
  } else {
    stop("Unable to read data from file")
  }
  mat <- as.data.frame(mat)
  MSY <- list()
  MSYrows <- 1:nH
  MSYdat <- mat[MSYrows,  ]
  MSYline <- string.match("MSY", first.chars)
  MSY$MSY <- as.numeric(casal.string.to.vector.of.words(file[[MSYline]])[3])
  MSYline <- string.match("B_M", first.chars)
  MSY$B.MSY <- as.numeric(casal.string.to.vector.of.words(file[[MSYline]])[3])
  MSYline <- string.match("F_M", first.chars)
  MSY$F.MSYdet <- as.numeric(casal.string.to.vector.of.words(file[[MSYline]])[3])
  MSY$dat <- MSYdat
  out$MSY <- MSY
  return(out)
}
