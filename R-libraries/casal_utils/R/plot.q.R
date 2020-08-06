# Plot OBS/fitted
"plot.q"<-
function(model,label="",xlim,ylim,add=F,...){
  UseMethod("plot.q",model)
}

"plot.q.casal.mpd"<-
function(model,label="",xlim,ylim,add,...)
{
  print("plot.q is not implemented for mpd fits")
  invisible()
}

"plot.q.casal.mcmc"<-
function(model, label = "", xlim, ylim, add, ...)
{
  if(label != "") label <- paste("q\\[", label, "\\]", sep = "")
  else stop("Specify a label for the q to plot")
  index <- as.vector((casal.regexpr(label, dimnames(model)[[2]]) > 0))
  YCS <- model[, index]
  y <- density(YCS, from = min(YCS), to = max(YCS))
  y$y <- c(0, y$y, 0)
  y$x <- c(min(y$x), y$x, max(y$x))
  y$y<-y$y/Area(y)
  if(missing(xlim)) xlim <- range(YCS)
  if(missing(ylim))ylim <- range(c(0, 1.01*max(y$y)))
  if(!add) {
    plot(xlim, ylim, type = "n", xlab = "q", ylab = "", axes = F)
    axis(side = 1)
    box()
    mtext(side = 2, "Density", line = 0.6)
  }
  lines(y$x, y$y, ...)
  invisible()
}

