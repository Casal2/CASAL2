# $Id: extract.MCYCAY.R 4923 2013-04-17 20:44:26Z Dunn $
"extract.MCYCAY"<-
function(file,path="")
{
# Extracts HOK MCY and/or CAY data from a file output from casal
#
# Returns a list with components MCY and/or CAY (depending on whether
# the file contains MCY results or CAY results or both)
#
# The MCY component is a list with components
#    "MCY"   "B.MCY" "dat" and (possibly) MCY.current
# The CAY component is a list with components
#   "F.CAY" "CAY"   "MAY"   "B.MAY" "dat"
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
Hline <- string.match("H ", first.chars)
if(length(Hline) > 0) {
  n.stocks <- 1
  overfish <- string.match("fishing pressure limit exceeded", file[Hline])
  if(length(overfish) > 0)  Hline <- Hline[ - overfish]
  nH <- length(Hline)
  tmp <- array(unlapply(file[Hline], casal.string.to.vector.of.words), c(2, 6, nH))
  mat <- matrix(as.numeric(tmp[2,  ,  ]), ncol = 6, byrow = T, dimnames = list(paste(1:nH), c("H", "Cav", "Bav", "Prisk", "SSBn", "SSBn+1")))
  H <- mat[, "H"]
} else {
  Hline <- string.match("At ", first.chars)
  overfish <- string.match("fishing pressure limit exceeded", file[Hline])
  if(length(overfish) > 0) Hline <- Hline[ - overfish]
  nH <- length(Hline)
  H <- unlapply(file[Hline], function(x)
  as.numeric(casal.unpaste(casal.unpaste(x, sep = " ")[4], sep = ",")[1]))
  n.stocks <- diff(Hline[1:2]) - 3
  stock.names <- unlapply(file[(Hline[1] + 2):(Hline[1] + 1 + n.stocks)], function(x)
  casal.string.to.vector.of.words(x)[1])
  mat <- matrix(NA, nH, 3 + 3 * n.stocks, dimnames = list(paste(1:nH), c("H", paste(rep(c("Cav", "Bav", "Prisk"), rep(n.stocks, 3)), rep(stock.names, 3), sep = "."), "SSBn", "SSBn+1")))
  mat[, "H"] <- H
  for(ist in 1:n.stocks) {
    tmp <- matrix(unlapply(file[Hline + 1 + ist], casal.string.to.vector.of.words), ncol = 4, byrow = T)
    mat[, paste(c("Cav", "Bav", "Prisk"), stock.names[ist], sep = ".")] <- matrix(as.numeric(tmp[, -1]), ncol = 3)
  }
  tmp <- matrix(unlapply(file[Hline + 2 + n.stocks], casal.string.to.vector.of.words), ncol = 4, byrow = T)
  mat[, "SSBn"] <- as.numeric(tmp[, 2])
  mat[, "SSBn+1"] <- as.numeric(tmp[, 4])
}
mat <- as.data.frame(mat)
MCYline <- string.match("MCY", first.chars)
CAYlines <- string.match("CAY", first.chars)
is.MCY <- length(MCYline) > 0
if (is.MCY && length(MCYline) > 1) stop("More than one MCY line in file")
is.CAY <- length(CAYlines) > 0
if (is.CAY && length(CAYlines) != n.stocks) stop("Wrong number of CAY lines in file")
if(is.MCY) {
  MCY <- list()
  if(is.CAY) {
    MCYrows <- if(MCYline < CAYlines[1]) (1:nH)[Hline < MCYline] else (1:nH)[Hline > (CAYlines[1])]
  } else MCYrows <- 1:nH
  MCYdat <- mat[MCYrows,  ]
  MCYdat <- MCYdat[match(sort(unique(MCYdat[, "H"])), MCYdat[, "H"]),  ]  # order & drop dups
  MCY$MCY <- as.numeric(casal.string.to.vector.of.words(file[[MCYline]])[3])
  tmp <- string.match("current", file)
  if(length(tmp) > 0) {
    tmp <- casal.string.to.vector.of.words(file[tmp])
    MCY$MCY.current <- as.numeric(tmp[length(tmp)])
  }
  tmp <- matrix(unlapply(file[string.match("B_MCY", file)], casal.string.to.vector.of.words), ncol = n.stocks)
  MCY$B.MCY <- as.numeric(tmp[nrow(tmp),  ])
  if(n.stocks > 1) names(MCY$B.MCY) <- stock.names
#  if(n.stocks == 1) {
#    MCY$Prisk <- MCYdat[MCYdat$H == MCY$MCY, "Prisk"]
#  } else {
#    MCY$Prisk <- unlist(MCYdat[MCYdat$H == MCY$MCY, paste("Prisk", stock.names, sep = ".")])
#    names(MCY$Prisk) <- stock.names
#  }
  MCY$dat <- MCYdat
  out$MCY <- MCY
}
if(is.CAY) {
  CAY <- list()
  if(is.MCY) {
    CAYrows <- if(MCYline < CAYlines[1]) (1:nH)[Hline > MCYline] else (1:nH)[Hline < CAYlines[1]]
  } else CAYrows <- 1:nH
  CAYdat <- mat[CAYrows,  ]
  CAYdat <- CAYdat[match(sort(unique(CAYdat[, "H"])), CAYdat[, "H"]),  ]  # order & drop dups
  tmp <- casal.string.to.vector.of.words(file[first.chars == "F_C"])
  CAY$F.CAY <- as.numeric(tmp[3])
  tmp <- matrix(unlapply(file[CAYlines], casal.string.to.vector.of.words), ncol = n.stocks)
  CAY$CAY <- as.numeric(tmp[nrow(tmp),  ])
  if(n.stocks > 1) names(CAY$CAY) <- stock.names
  tmp <- matrix(unlapply(file[first.chars == "MAY"], casal.string.to.vector.of.words), ncol = n.stocks)
  CAY$MAY <- as.numeric(tmp[nrow(tmp),  ])
  if(n.stocks > 1) names(CAY$MAY) <- stock.names
  tmp <- matrix(unlapply(file[string.match("B_MAY", file)], casal.string.to.vector.of.words), ncol = n.stocks)
  CAY$B.MAY <- as.numeric(tmp[nrow(tmp),  ])
  if(n.stocks > 1) names(CAY$B.MAY) <- stock.names
#  if(n.stocks == 1) {
#    CAY$Prisk <- CAYdat[CAYdat$H == CAY$F.CAY, "Prisk"]
#  } else {
#    CAY$Prisk <- unlist(CAYdat[CAYdat$H == CAY$F.CAY, paste("Prisk", stock.names, sep = ".")])
#    names(CAY$Prisk) <- stock.names
#  }
  CAY$dat <- CAYdat
  out$CAY <- CAY
}
if(length(out) == 0) stop("No MCY or CAY data in file")
return(out)
}
