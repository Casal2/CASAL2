# $Id: casal.dplot.R 5436 2015-07-26 23:03:27Z largeke $
"casal.dplot"<-
function(..., name=T, quantiles=c(0.5), plot.mean=F, main="", xlab="", ylab="", ylim, srtx=0, bw="nrd0", adjust=1/3, adj=0, fill=F, gap=0.2, las=1)
{
  dlines <- function(y, offset, Q, bw, adjust, gap)
  {
    y <- y[!is.na(y)]
    if(min(y) == max(y)) {
      ans <- list(x = min(y), y = 1)
      y1 <- ans$x
      x1 <- ans$y/max(ans$y) * (1-gap)
      q.y1 <- quantile(y, probs = c(0, Q, 1))
      q.x1 <- offset + x1
      q.x2 <- offset
      mean.y1 <- mean(y)
      mean.x1 <- offset + x1
      mean.x2 <- offset
    }
    else {
      ans <- density(y, n = 100, bw=bw, adjust=adjust, from = min(y), to = max(y))
      y1 <- ans$x
      x1 <- ans$y/max(ans$y) * (1-gap)
      q.y1 <- quantile(y, probs = c(0, Q, 1))
      q.x1 <- offset + approx(y1, x1, xout = q.y1)$y
      q.x2 <- offset
      mean.y1 <- mean(y)
      mean.x1 <- offset + approx(y1, x1, xout = mean.y1)$y
      mean.x2 <- offset
      x1<-c(offset,q.x1[1],offset+x1,rev(q.x1)[1],offset)
      y1<-c(q.y1[1],q.y1[1],y1,rev(q.y1)[1],rev(q.y1)[1])
    }
    return(list(x1 =  x1, y1 = y1, q.x1 = q.x1, q.y1 = q.y1, q.x2 = q.x2, mean.x1 = mean.x1, mean.x2 = mean.x2, mean.y1 = mean.y1)
      )
  }
  all.x <- list(...)
  nam <- character(0)
  if(is.list(all.x[[1]])) {
    all.x <- all.x[[1]]
    if(is.logical(name) && name)
      name <- names(...)
  }
  n <- length(all.x)
  centers <- seq(from = 0, by = 1.0, length = n) + 0.1
  ymax <- max(sapply(all.x, max, na.rm = T),na.rm=T)
  if(is.na(ymax)) stop("Error: list of empty vectors")
  ymin <- min(sapply(all.x, min, na.rm = T),na.rm=T)
  xmax <- max(centers) + 0.95
  xmin <- 0
  if(gap>1) gap<-0.2
  if(!missing(ylim)) plot(c(xmin, xmax), c(ymin, ymax), type = "n", main = main, xlab = xlab, ylab = ylab, xaxt = "n", ylim=ylim)
  else plot(c(xmin, xmax), c(ymin, ymax), type = "n", main = main, xlab = xlab, ylab = ylab, xaxt = "n")
  for(i in 1:n) {
    if(length(all.x[[i]][!is.na(all.x[[i]])]) > 0) {
      plot.values <- dlines(all.x[[i]], centers[i], quantiles, bw, adjust, gap)
      if(fill & length(plot.values$x1)>2) polygon(plot.values$x1, plot.values$y1, col=fill)
      lines(plot.values$x1, plot.values$y1)
      segments(centers[i], min(plot.values$y1), centers[i], max(plot.values$y1))
      segments(plot.values$q.x1, plot.values$q.y1, plot.values$q.x2, plot.values$q.y1)
      if(plot.mean)
        segments(plot.values$mean.x1, plot.values$mean.y1, plot.values$mean.x2, plot.values$mean.y1, lwd = 3)
      }
  }
  if(is.logical(name)) {
    if(name)
      axis(1, las=las, centers, sapply(substitute(list(...)), deparse)[2:(n + 1)], srt = srtx, adj = if(srtx == 0) adj else 1)
  }
  else axis(1, centers, name, srt = srtx, adj = if(srtx == 0) adj else 1)
  invisible(centers)
}

