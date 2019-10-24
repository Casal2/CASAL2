\name{plot.prior}
\alias{plot.prior}
\title{Plots priors using equations and variable names as specified in the CASAL manual}
\description{
Creates a plot of the specified prior using the specified parameters.
}
\usage{
plot.prior(type="lognormal",mu,sd,cv,A=0,B=1,bounds,xlim,label=F,xlab="x",ylab="Density",logx=F,add=F,dump=F,...))
}
\arguments{
  \item{type}{character string giving the name of the prior. Can be one of lognormal, normal-by-stdev, uniform, uniform-log, normal-log, or beta.}
  \item{mu}{Mean of the lognormal, normal-by-stdev, normal-log, or beta priors.}
  \item{sd}{Standard deviation of the normal-by-stdev, normal-log or beta priors.}
  \item{cv}{C.v. of the lognormal prior.}
  \item{A}{A parameter for the beta prior.}
  \item{B}{B parameter for the beta prior.}
  \item{bounds}{Lower and upper bounds of the prior.}
  \item{xlim}{xlim of the plot.}
  \item{label}{A label to print at the top of the plot.}
  \item{xlab}{x-axis label.}
  \item{ylab}{y-axis label.}
  \item{logx}{Plot the x-axis on a log scale?.}
  \item{add}{Add the plot of the prior to an existing plot?.}
  \item{dump}{return the x-vlaues and respective densities?.}
}
\value{
  if dump=T, a data.frame with x and y values
}
\seealso{
\code{\link{casal-package}}
}

