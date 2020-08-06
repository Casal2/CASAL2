#-------------------------------------------------------------------------------
# File: libboa.q
# Description: Library of core functions for the Bayesian Output Analysis
#    Program (BOA)
# Author: Brian J. Smith <brian-j-smith@uiowa.edu>
#-------------------------------------------------------------------------------


boa.acf <- function(link, lags)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   pnames <- boa.pnames(link)
   result <- matrix(NA, nrow = ncol(link), ncol = length(lags),
                    dimnames = list(pnames, paste("Lag", lags)))
   lags <- lags[lags <= (nrow(link) - 1)]
   n.lags <- length(lags)
   if(n.lags > 0) {
      idx <- 1:n.lags
      lag.max <- max(lags)
      for(i in pnames) {
         result[i, idx] <- acf(link[, i], lag.max = lag.max,
                               plot = FALSE)$acf[lags + 1]
      }
   }

   return(result)
}


boa.batchMeans <- function(link, size)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   riter <- range(boa.iter(link))
   iter.first <- riter[1]
   iter.last <- iter.first + size - 1
   while(iter.last <= riter[2]) {
      result <- cbind(result, colMeans(boa.getiter(link, iter.first:iter.last)))
      iter.first <- iter.last + 1
      iter.last <- iter.last + size
   }
   if(is.null(result))  result <- matrix(NA, nrow = ncol(link), ncol = 1)
   dimnames(result) <- list(boa.pnames(link), 1:ncol(result))

   return(result)
}


boa.chain <- function(...)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   if(nargs() == 0)  return(.boa.chain)
   temp <- list(...)
   if((length(temp) == 1) && is.null(names(temp))) {
      arg <- temp[[1]]
      switch(mode(arg),
         list = temp <- arg,
         character = return(.boa.chain[[arg]]),
         temp <- NULL
      )
   }
   idx <- intersect(names(temp), names(.boa.chain))
   if(length(idx) > 0) {
      current <- .boa.chain
      current[idx] <- temp[idx]
      assign(".boa.chain", current, envir = globalenv())
   } else {
      cat("Warning: invalid arguments\n")
   }
   invisible()
}


boa.getiter <- function(link, iter)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   idx <- is.element(dimnames(link)[[1]], iter)
   if(any(idx))  result <- link[idx, , drop = FALSE]

   return(result)
}


boa.getparms <- function(link, pnames)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   idx <- is.element(dimnames(link)[[2]], pnames)
   if(any(idx))  result <- link[, idx, drop = FALSE]

   return(result)
}


boa.geweke <- function(link, p.first, p.last)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   iter <- unique(boa.iter(link))
   n <- length(iter)
   link.first <- boa.getiter(link, iter[1:round(p.first * n)])
   link.last <- boa.getiter(link, iter[(n - round(p.last * n) + 1):n])
   result <- (colMeans(link.first) - colMeans(link.last)) /
                       sqrt(boa.gewekePwr(link.first) / nrow(link.first) +
                            boa.gewekePwr(link.last) / nrow(link.last))
   result <- cbind(result, 2 * (1 - pnorm(abs(result))))
   dimnames(result)[[2]] <- c("Z-Score", "p-value")

   return(result)
}


boa.gewekePwr <- function(link)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   spec <- NULL
   pnames <- boa.pnames(link)
   n <- nrow(link)
   nspans <- 1 + sqrt(n) / 0.3
   if(nspans >= n)  nspans <- max(n - 1, 1)
   for(i in pnames) {
      spec <- c(spec, spec.pgram(link[, i], spans = nspans, demean = TRUE,
                                 detrend = FALSE, plot = FALSE)$spec[1])
   }
   names(spec) <- pnames

   return(10^(spec / 10))
}


boa.handw <- function(link, error, alpha)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   pnames <- boa.pnames(link)
   stest <- keep <- I <- htest <- xbar <- halfwidth <-
            structure(rep(NA, length(pnames)), names = pnames)
   iter <- unique(boa.iter(link))
   n <- length(iter)
   n.min <- round(0.5 * n)
   idx.drop <- -1 * 1:max(round(0.10 * n), 1)
   S0 <- boa.gewekePwr(boa.getiter(link, iter[(n - n.min + 1):n]))
   q.upper <- qnorm(1 - alpha / 2)
   for(i in pnames) {
      parm <- boa.getparms(link, i)
      piter <- iter
      keep[i] <- n
      stest[i] <- "failed"
      htest[i] <- "failed"
      while((keep[i] >= n.min) && (stest[i] == "failed")) {
         n.parm <- nrow(parm)
         xbar[i] <- mean(parm)
         halfwidth[i] <- q.upper * sqrt(boa.gewekePwr(parm) / n.parm)
         if(abs(halfwidth[i] / xbar[i]) <= error)  htest[i] <- "passed"
         B <- cumsum(parm) - xbar[i] * 1:n.parm
         Bsq <- (B * B) / (n.parm * S0[i])
         I[i] <- (2 * sum(Bsq[seq(2, n.parm - 2, by = 2)]) +
                  4 * sum(Bsq[seq(1, n.parm - 1, by = 2)]) + Bsq[n.parm]) /
                 (3 * n.parm)
         if(I[i] < 0.46) {
            stest[i] <- "passed"
         } else {
            piter <- piter[idx.drop]
            keep[i] <- length(piter)
            parm <- boa.getiter(parm, piter)
         }
      }
   }
   result <- data.frame(stest, keep, n - keep, I, htest, xbar, halfwidth)
   names(result) <- c("Stationarity Test", "Keep", "Discard", "C-von-M",
                      "Halfwidth Test", "Mean", "Halfwidth")

   return(result)
}


boa.hpd <- function(x, alpha)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   n <- length(x)
   m <- max(1, ceiling(alpha * n))

   y <- sort(x)
   a <- y[1:m]
   b <- y[(n - m + 1):n]

   i <- order(b - a)[1]

   structure(c(a[i], b[i]), names = c("Lower Bound", "Upper Bound"))
}


boa.importASCII <- function(prefix, path = NULL)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   link <- NULL
   filename <- paste(path, prefix, boa.par("ASCIIext"), sep = "")
   if(file.exists(filename)) {
      data <- read.table(filename, header = TRUE)
      idx <- match("iter", names(data), nomatch = 0)
      if(idx > 0) {
         dimnames(data)[[1]] <- data[[idx]]
         data[[idx]] <- NULL
      }
      link <- as.matrix(data)
   } else {
      cat("Warning: could not find '", filename, "' to import\n", sep = "")
   }

   return(link)
}


boa.importBUGS <- function(prefix, path = NULL)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   link <- NULL
   filenames <- paste(path, prefix, c(".ind", ".out"), sep = "")
   if(all(file.exists(filenames))) {
      ind <- scan(filenames[1], list(pnames = "", first = 0, last = 0))
      out <- scan(filenames[2], list(iter = 0, parms = 0))
      iter.first <- max(out$iter[ind$first])
      iter.last <- min(out$iter[ind$last])
      if(iter.first <= iter.last) {
         idx <- match(c(iter.first, iter.last), out$iter)
         iter <- out$iter[idx[1]:idx[2]]
         link <- matrix(out$parms[(iter.first <= out$iter) &
                                  (out$iter <= iter.last)],
                        nrow = length(iter), ncol = length(ind$pnames),
                        dimnames = list(iter, ind$pnames))
      } else {
         cat("Warning: no common iterations to import\n")
      }
   } else {
      cat("Warning: could not find '", filenames[1], "' or '", filenames[2],
          "' to import\n", sep = "")
   }

   return(link)
}


boa.iter <- function(link)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   return(as.numeric(dimnames(link)[[1]]))
}


boa.license <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   ver <- boa.version()
   cat("\nBayesian Output Analysis Program (BOA)\n",
       "Version ", paste(c(ver$major, ver$minor, ver$revision), collapse = "."),
       " for ", ver$system, "\n",
       "Copyright (c) 2001 Brian J. Smith <brian-j-smith@uiowa.edu>\n\n",
       "This program is free software; you can redistribute it and/or\n",
       "modify it under the terms of the GNU General Public License\n",
       "as published by the Free Software Foundation; either version 2\n",
       "of the License or any later version.\n\n",
       "This program is distributed in the hope that it will be useful,\n",
       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n",
       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n",
       "GNU General Public License for more details.\n\n",
       "For a copy of the GNU General Public License write to the Free\n",
       "Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,\n",
       "MA  02111-1307, USA, or visit their web site at\n",
       "http://www.gnu.org/copyleft/gpl.html\n\n", sep = "")
   invisible()
}


boa.load <- function(name, envir = globalenv())
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   loaded <- FALSE
   if(exists(name, envir = envir)) {
      obj <- get(name, envir = envir)
      if(is.character(obj$version$name) && is.numeric(obj$version$major) &&
         is.numeric(obj$version$minor)) {
         ver <- boa.version()
         loaded <- (obj$version$name == ver$name) &&
                   ((obj$version$major < ver$major) ||
                       ((obj$version$major == ver$major)
                       && (obj$version$minor >= ver$minor)))
      }
      if(loaded) {
         boa.par(obj$par)
         boa.par(dev.list = numeric(0))
         boa.chain(obj$chain)
      } else {
         cat("Warning: object is incompatible with this version of BOA\n")
      }
   } else {
      cat("Warning: object not found\n")
   }

   return(loaded)
}


boa.par <- function(...)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   if(nargs() == 0)  return(.boa.par)
   temp <- list(...)
   if((length(temp) == 1) && is.null(names(temp))) {
      arg <- temp[[1]]
      switch(mode(arg),
         list = temp <- arg,
         character = return(.boa.par[[arg]]),
         temp <- NULL
      )
   }
   changed <- NULL
   globals <- names(temp)
   idx <- is.element(globals, names(.boa.par))
   if(!all(idx)) {
      cat("Warning: invalid arguments\n")
      print(globals[!idx])
   }
   if(any(idx)) {
      globals <- globals[idx]
      pclass <- unlist(lapply(.boa.par[globals], "data.class"))
      idx <- unlist(lapply(temp[globals], "data.class")) == pclass
      if(!all(idx)) {
         cat("Warning: arguments must be of type\n")
         print(pclass[!idx])
      }
      if(any(idx)) {
         globals <- globals[idx]
         current <- .boa.par
         changed <- current[globals]
         current[globals] <- temp[globals]
         assign(".boa.par", current, envir = globalenv())
      }
   }
   invisible(changed)
}


boa.pardesc <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   return(.boa.pardesc)
}


boa.pnames <- function(link)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   return(dimnames(link)[[2]])
}


boa.quit <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   cat("Exiting BOA session...\n")
   remove(".boa.par", envir = globalenv())
   remove(".boa.pardesc", envir = globalenv())
   remove(".boa.chain", envir = globalenv())
   remove(".boa.version", envir = globalenv())
   invisible()
}


boa.randl <- function(link, q, error, prob, delta)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   n <- nrow(link)
   phi <- qnorm(0.5 * (1 + prob))
   n.min <- ceiling(q * (1 - q) * (phi / error)^2)
   if(n.min <= n) {
      pnames <- boa.pnames(link)
      for(i in pnames) {
         dichot <- ifelse(link[, i] <= quantile(link[, i], probs = q), 1, 0)
         thin <- 0
         bic <- 1
         while(bic >= 0) {
            thin <- thin + 1
            test <- dichot[seq(1, n, by = thin)]
            n.test <- length(test)
            temp <- test[1:(n.test - 2)] + 2 * test[2:(n.test - 1)] +
                    4 * test[3:(n.test)]
            tran.test <- array(tabulate(temp + 1, nbins = 8), dim=c(2, 2, 2))
            g2 <- 0
            for(i1 in c(1, 2)) {
               for(i2 in c(1, 2)) {
                  for(i3 in c(1, 2)) {
                     if(tran.test[i1, i2, i3] > 0) {
                        fitted <- log(sum(tran.test[i1, i2, ])) +
                                  log(sum(tran.test[i1, , i3])) -
                                  log(sum(tran.test[i1, , ]))
                        g2 <- g2 + 2 * tran.test[i1, i2, i3] *
                                   (log(tran.test[i1, i2, i3]) - fitted)
                     }
                  }
               }
            }
            bic <- g2 - 2 * log(n.test - 2)
         }
         tran.final <- tabulate(test[1:(n.test - 1)] + 2 * test[2:n.test] + 1,
                                nbins = 4)
         alpha <- tran.final[3] / (tran.final[1] + tran.final[3])
         beta <- tran.final[2] / (tran.final[2] + tran.final[4])
         burnin <- ceiling(log(delta * (alpha + beta) / max(alpha, beta)) /
                           log(abs(1 - alpha - beta))) * thin
         keep <- ceiling((2 - alpha - beta) * alpha * beta * phi^2 /
                         (error^2 * (alpha + beta)^3)) * thin
         total <- burnin + keep
         result <- rbind(result, c(thin, burnin, total, n.min, total / n.min))
      }
      dimnames(result) <- list(pnames, c("Thin", "Burn-in", "Total",
                                         "Lower Bound", "Dependence Factor"))
   } else {
      result <- n.min
      names(result) <- "Lower Bound"
   }

   return(result)
}


boa.save <- function(name, envir = globalenv())
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   saved <- !is.element(name, c(".boa.par", ".boa.pardesc", ".boa.chain",
                                ".boa.version"))
   if(saved) {
      assign(name, list(par = boa.par(),
                        chain = boa.chain(),
                        version = boa.version()), envir = envir)
   } else {
      cat("Warning: object name is in use by the program\n")
   }

   return(saved)
}


boa.sortiter <- function(link)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   idx <- order(as.numeric(dimnames(link)[[1]]))

   return(link[idx, , drop = FALSE])
}


boa.sortparms <- function(link)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   idx <- order(dimnames(link)[[2]])

   return(link[, idx, drop = FALSE])
}


boa.start <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   assign(".boa.par",
          list(acf.lags     = c(1, 5, 10, 50),
               alpha        = 0.05,
               bandwidth    = function(x)  0.5 * diff(range(x)) /
                                           (log(length(x)) + 1),
               batch.size   = 50,
               dev          = "windows",
               dev.list     = numeric(0),
               ASCIIext     = ".txt",
               gandr.bins   = 20,
               gandr.win    = 0.5,
               geweke.bins  = 10,
               geweke.first = 0.1,
               geweke.last  = 0.5,
               handw.error  = 0.1,
               kernel       = "gaussian",
               path         = "",
               plot.mfdim   = c(3, 2),
               plot.new     = FALSE,
               plot.onelink = FALSE,
               plot.oneparm = TRUE,
               quantiles    = c(0.025, 0.5, 0.975),
               randl.error  = 0.005,
               randl.delta  = 0.001,
               randl.q      = 0.025,
               title        = "Bayesian Output Analysis"), envir = globalenv())
   assign(".boa.pardesc",
          structure(rbind(
             c("Analysis", "Brooks, Gelman & Rubin", "Alpha Level", "alpha",
               ""),
             c("Analysis", "Brooks, Gelman & Rubin", "Window Fraction",
               "gandr.win", ""),
             c("Analysis", "Geweke", "Window 1 Fraction", "geweke.first", ""),
             c("Analysis", "Geweke", "Window 2 Fraction", "geweke.last", ""),
             c("Analysis", "Heidelberger & Welch", "Accuracy", "handw.error",
               ""),
             c("Analysis", "Heidelberger & Welch", "Alpha Level", "alpha",
               ""),
             c("Analysis", "Raftery & Lewis", "Accuracy", "randl.error", ""),
             c("Analysis", "Raftery & Lewis", "Alpha Level", "alpha", ""),
             c("Analysis", "Raftery & Lewis", "Delta", "randl.delta", ""),
             c("Analysis", "Raftery & Lewis", "Quantile", "randl.q", ""),
             c("Analysis", "Statistics", "ACF Lags", "acf.lags", ""),
             c("Analysis", "Statistics", "Alpha Level", "alpha", ""),
             c("Analysis", "Statistics", "Batch Size", "batch.size", ""),
             c("Analysis", "Statistics", "Quantiles", "quantiles", ""),
             c("Data", "Files", "Working Directory", "path",
               "Specified directory must end with a slash"),
             c("Data", "Files", "ASCII File Ext", "ASCIIext", ""),
             c("Plot", "Brooks & Gelman", "Number of Bins", "gandr.bins", ""),
             c("Plot", "Brooks & Gelman", "Window Fraction", "gandr.win", ""),
             c("Plot", "Density", "Bandwidth", "bandwidth",
               "This defines the standard deviation of the smoothing kernel"),
             c("Plot", "Density", "Kernel", "kernel",
               "Possible kernels are gaussian, rectangular, triangular, or cosine"),
             c("Plot", "Gelman & Rubin", "Alpha Level", "alpha", ""),
             c("Plot", "Gelman & Rubin", "Number of Bins", "gandr.bins", ""),
             c("Plot", "Gelman & Rubin", "Window Fraction", "gandr.win", ""),
             c("Plot", "Geweke", "Alpha Level", "alpha", ""),
             c("Plot", "Geweke", "Number of Bins", "geweke.bins", ""),
             c("Plot", "Geweke", "Window 1 Fraction", "geweke.first", ""),
             c("Plot", "Geweke", "Window 2 Fraction", "geweke.last", ""),
             c("Plot", "Graphics", "Device Name", "dev", ""),
             c("Plot", "Graphics", "Keep Previous Plots", "plot.new", ""),
             c("Plot", "Graphics", "Plot Layout", "plot.mfdim", ""),
             c("Plot", "Graphics", "Separate Plot/Chain", "plot.onelink", ""),
             c("Plot", "Graphics", "Separate Plot/Var", "plot.oneparm", ""),
             c("Plot", "Graphics", "Title", "title", "")),
             dimnames =
                list(NULL, c("group", "method", "desc", "par", "note"))),
          envir = globalenv())
   assign(".boa.chain",
          list(master         = list(),
               master.support = list(),
               work           = list(),
               work.support   = list(),
               work.sync      = TRUE), envir = globalenv())
   assign(".boa.version",
          list(name     = "BOA",
               major    = 1,
               minor    = 0,
               revision = 1,
               system   = "Windows R"), envir = globalenv())
   boa.license()
   invisible()
}


boa.stats <- function(link, probs, batch.size)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   niter <- nrow(link)
   nparms <- ncol(link)
   sd <- sqrt(colVars(link))
   batch <- t(boa.batchMeans(link, batch.size))
   result <- cbind(colMeans(link),
                   sd,
                   sd / sqrt(niter),
                   sqrt(boa.gewekePwr(link) / niter),
                   sqrt(colVars(batch) / nrow(batch)),
                   boa.acf(batch, 1),
                   t(apply(link, 2, quantile, probs = probs)),
                   matrix(rep(range(boa.iter(link)), nparms), nrow = nparms,
                          ncol = 2, byrow = TRUE),
                   rep(niter, nparms))
   dimnames(result)[[2]] <- c("Mean", "SD", "Naive SE", "MC Error", "Batch SE",
                              "Batch ACF", probs, "MinIter", "MaxIter",
                              "Sample")

   return(result)
}


boa.transform <- function(x, support, inv = FALSE)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   transform <- x
   support <- range(support)
   bounded <- is.finite(support)
   if(!inv) {
      if(all(bounded)) {
         p <- (x - support[1]) / abs(diff(support))
         transform <- log(p / (1 - p))
      } else if(bounded[1]) {
         transform <- log(x - support[1])
      } else if(bounded[2]) {
         transform <- log(support[2] - x)
      }
   } else {
      if(all(bounded)) {
         transform <- abs(diff(support)) / (1 + exp(-x)) + support[1]
      } else if(bounded[1]) {
         transform <- exp(x) + support[1]
      } else if(bounded[2]) {
         transform <- support[2] - exp(x)
      }
   }

   return(transform)
}


boa.version <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   return(.boa.version)
}


#-------------------------------------------------------------------------------
# File: libboa_chain.q
# Description: Library of chain functions for the Bayesian Output Analysis
#    Program (BOA)
# Author: Brian J. Smith <brian-j-smith@uiowa.edu>
#-------------------------------------------------------------------------------


boa.chain.add <- function(link, lname)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   added <- FALSE
   if(is.matrix(link) && is.numeric(link)) {
      added <- TRUE
      dn <- dimnames(link)
      if(is.null(dn)) {
         dimnames(link) <- list(1:nrow(link),
                                paste("par", 1:ncol(link), sep = ""))
      } else if(length(dn[[1]]) == 0) {
         dimnames(link)[[1]] <- 1:nrow(link)
      } else if(length(dn[[2]]) == 0) {
         dimnames(link)[[2]] <- paste("par", 1:ncol(link), sep = "")
      }
      master <- boa.chain("master")
      master.support <- boa.chain("master.support")
      master[[lname]] <- boa.sortparms(link)
      master.support[[lname]] <- matrix(c(-Inf, Inf), nrow = 2,
                                        ncol = ncol(link))
      dimnames(master.support[[lname]]) <- list(c("Min", "Max"),
                                                boa.pnames(master[[lname]]))
      if(boa.chain("work.sync")) {
         boa.chain(master = master, master.support = master.support,
                   work = master, work.support = master.support)
      } else {
         boa.chain(master = master, master.support = master.support)
      }
   } else {
      cat("Warning: object must be a numeric matrix\n")
   }

   return(added)
}


boa.chain.collapse <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   lnames <- names(work)
   pnames <- boa.pnames(work[[1]])
   for(i in lnames[-1]) {
      pnames <- intersect(boa.pnames(work[[i]]), pnames)
   }
   link <- NULL
   for(i in lnames) {
      link <- rbind(link, boa.getparms(work[[i]], pnames))
   }
   collapse <- is.matrix(link)
   if(collapse) {
      lname <- paste(lnames, collapse = "::")
      work <- list(boa.sortiter(link))
      names(work) <- lname
      work.support <- list(boa.getparms(boa.chain("work.support")[[1]],
                                        boa.pnames(work[[1]])))
      names(work.support) <- lname
      boa.chain(work = work, work.support = work.support, work.sync = FALSE)
   }

   return(collapse)
}


boa.chain.del <- function(lnames, pnames)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   chain <- list(boa.chain("master"), boa.chain("work"))
   chain.support <- list(boa.chain("master.support"), boa.chain("work.support"))
   for(i in 1:2) {
      if(!missing(lnames)) {
         cnames <- intersect(lnames, names(chain[[i]]))
         for(j in cnames) {
            chain[[i]][[j]] <- NULL
            chain.support[[i]][[j]] <- NULL
         }
      }
      if(!missing(pnames)) {
         cnames <- names(chain[[i]])
         for(j in cnames) {
            keep <- setdiff(boa.pnames(chain[[i]][[j]]), pnames)
            chain[[i]][[j]] <- chain[[i]][[j]][, keep]
            chain.support[[i]][[j]] <- chain.support[[i]][[j]][, keep]
         }
      }
   }
   boa.chain(master = chain[[1]], master.support = chain.support[[1]],
             work = chain[[2]], work.support = chain.support[[2]])
   invisible()
}


boa.chain.eval <- function(expr, pname)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   clash <- FALSE
   chain <- list(boa.chain("master"), boa.chain("work"))
   chain.support <- list(boa.chain("master.support"), boa.chain("work.support"))
   for(i in 1:2) {
      for(j in names(chain[[i]])) {
         pnames <- boa.pnames(chain[[i]][[j]])
         clash <- clash || is.element(pname, pnames)
         if(clash) break
         pnames <- c(pnames, pname)
         chain[[i]][[j]] <- cbind(chain[[i]][[j]],
                                  eval(expr, as.data.frame(chain[[i]][[j]])))
         dimnames(chain[[i]][[j]])[[2]] <- pnames
         chain.support[[i]][[j]] <- cbind(chain.support[[i]][[j]], c(-Inf, Inf))
         dimnames(chain.support[[i]][[j]])[[2]] <- pnames
      }
   }
   if(clash) {
      cat("Warning:", pname, "already exists in master, operation terminated\n")
   } else {
      boa.chain(master = chain[[1]], master.support = chain.support[[1]],
                work = chain[[2]], work.support = chain.support[[2]])
   }
   invisible()
}


boa.chain.gandr <- function(chain, chain.support, alpha, pnames, window, to)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   lnames <- names(chain)
   if(missing(pnames)) {
      pnames <- boa.pnames(chain[[1]])
   } else {
      pnames <- intersect(boa.pnames(chain[[1]]), pnames)
   }
   iter <- boa.iter(chain[[1]])
   if(missing(window))  window <- 0.5
   if(!missing(to))  iter <- iter[iter <= to]
   for(i in lnames[-1]) {
      pnames <- intersect(boa.pnames(chain[[i]]), pnames)
      iter <- intersect(boa.iter(chain[[i]]), iter)
   }
   n <- length(iter)
   keep <- iter[(n - round(window * n) + 1):n]

   m <- length(lnames)
   n <- length(keep)
   p <- length(pnames)
   q.upper <- 1 - alpha / 2

   if(m < 2) {
      cat("Warning: must supply at least two sequences to analyze\n")
   } else if(p == 0) {
      cat("Warning: no common parameters to analyze\n")
   } else if(n < (p + 1)) {
      cat("Warning: must supply at least", 2 * p + 1,
          "common iterations to analyze\n")
   } else {
      psi <- matrix(NA, nrow = n, ncol = p, dimnames = list(NULL, pnames))
      xbar.within <- sxx.within <- matrix(NA, nrow = m, ncol = p,
                                          dimnames = list(lnames, pnames))
      W <- 0
      for(i in lnames) {
         rows <- is.element(boa.iter(chain[[i]]), keep)
         for(j in pnames) {
            psi[, j] <- boa.transform(chain[[i]][rows, j],
                                      chain.support[[i]][, j])
         }
         xbar.within[i, ] <- colMeans(psi)
         sxx <- var(psi)
         sxx.within[i, ] <- diag(sxx)
         W <- W + sxx
      }
      W <- W / m
      B <- n * var(xbar.within)

      w <- diag(W)
      b <- diag(B)
      xbar <- colMeans(xbar.within)
      var.w <- colVars(sxx.within) / m
      df.w <- 2 * w^2 / var.w
      var.b <- 2 * b^2 / (m - 1)
      var.wb <- NULL
      for(j in pnames) {
         var.wb <- c(var.wb,
                     var(sxx.within[, j], xbar.within[, j]^2) -
                     2 * xbar[j] * var(sxx.within[, j], xbar.within[, j]))
      }
      var.wb <- (n / m) * var.wb
      v <- ((n - 1) * w + (1 + 1 / m) * b) / n
      var.v <- ((n - 1)^2 * var.w + (1 + 1 / m)^2 * var.b + 2 * (n - 1) *
                (1 + 1 / m) * var.wb) / n^2
      df.v <- 2 * v^2 / var.v

      psrf <- sqrt(v / w)
      names(psrf) <- pnames
      csrf <- sqrt((df.v + 3) / (df.v + 1) * cbind(v / w, (1 - 1 / n) +
                   qf(q.upper, m - 1, df.w) * (1 + 1 / m) * b / (n * w)))
      dimnames(csrf) <- list(pnames, c("Estimate", q.upper))
      mpsrf <- sqrt((1 - 1 / n) + (1 + 1 / m) * eigen(solve(W) %*% B / n,
                    symmetric = TRUE, only.values = TRUE)$values[1])
      result <- list(psrf = psrf, csrf = csrf, mpsrf = mpsrf,
                     window = range(keep))
   }
   if(is.null(result)) {
      result <- list(psrf = structure(rep(NA, p), names = pnames),
                     csrf = matrix(NA, nrow = p, ncol = 2,
                            dimnames = list(pnames, c("Estimate", q.upper))),
                     mpsrf = NA, window = c(NA, NA))
   }

   return(result)
}


boa.chain.import <- function(prefix, path = boa.par("path"), type = "ASCII")
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   link <- NULL
   switch(type,
      "ASCII" = link <- boa.importASCII(prefix, path),
      "BUGS"  = link <- boa.importBUGS(prefix, path),
      cat("Warning: import type not supported\n")
   )

   return(is.matrix(link) && boa.chain.add(link, prefix))
}


boa.chain.info <- function(chain, chain.support)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   result <- NULL
   if(length(chain) > 0) {
      lnames <- names(chain)
      pnames <- list()
      iter <- list()
      iter.range <- NULL
      support <- list()
      for(i in lnames) {
         pnames[[i]] <- boa.pnames(chain[[i]])
         iter[[i]] <- boa.iter(chain[[i]])
         iter.range <- rbind(iter.range, c(range(iter[[i]]), length(iter[[i]])))
         support[[i]] <- chain.support[[i]]
      }
      dimnames(iter.range) <- list(lnames, c("Min", "Max", "Sample"))
      result <- list(lnames = lnames, pnames = pnames, iter = iter,
                     iter.range = iter.range, support = support)
   }

   return(result)
}


boa.chain.subset <- function(lnames, pnames, iter)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   work.support <- boa.chain("work.support")
   if(!missing(lnames)) {
      for(i in setdiff(names(work), lnames)) {
         work[[i]] <- NULL
         work.support[[i]] <- NULL
      }
   }
   for(i in names(work)) {
      link <- work[[i]]
      link.support <- work.support[[i]]
      if(!missing(pnames)) {
         link <- boa.getparms(link, pnames)
         link.support <- boa.getparms(link.support, pnames)
      }
      if(!missing(iter))  link <- boa.getiter(link, iter)
      work[[i]] <- link
      work.support[[i]] <- link.support
   }
   subset <- length(work) > 0
   if(subset)  boa.chain(work = work, work.support = work.support,
                         work.sync = FALSE)

   return(subset)
}


boa.chain.support <- function(lnames, pnames, limits)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   changed.master <- FALSE
   changed.work <- FALSE
   master <- boa.chain("master")
   master.support <- boa.chain("master.support")
   if(missing(lnames)) {
      lnames.master <- names(master.support)
   } else {
      lnames.master <- intersect(names(master.support), lnames)
   }
   for(i in lnames.master) {
      ipnames <- intersect(boa.pnames(master.support[[i]]), pnames)
      for(j in ipnames) {
         prange <- range(master[[i]][, j])
         if((limits[1] <= prange[1]) && (prange[2] <= limits[2])) {
            changed.master <- TRUE
            master.support[[i]][, j] <- limits
         } else {
            cat("Warning: support for '", j, "' in '", i, "' must include (",
                prange[1], ", ", prange[2], ")\n", sep ="")
         }
      }
   }
   work.sync <- boa.chain("work.sync")
   if(changed.master && work.sync) {
      changed.work <- TRUE
      work.support <- master.support
   } else if(!work.sync) {
      work <- boa.chain("work")
      work.support <- boa.chain("work.support")
      lnames.work <- names(work.support)
      if(!missing(lnames)) {
         idx <- pmatch(paste(lnames, "::", sep = ""),
                     paste(lnames.work, "::", sep = ""), nomatch = 0)
         lnames.work <- lnames.work[sort(idx)]
      }
      for(i in lnames.work) {
         ipnames <- intersect(boa.pnames(work.support[[i]]), pnames)
         for(j in ipnames) {
            prange <- range(work[[i]][, j])
            if((limits[1] <= prange[1]) && (prange[2] <= limits[2])) {
               changed.work <- TRUE
               work.support[[i]][, j] <- limits
            } else {
               cat("Warning: support for '", j, "' in 'work$", i,
                   "' must include (", prange[1], ", ", prange[2], ")\n",
                   sep ="")
            }
         }
      }
   }
   if(changed.master)  boa.chain(master.support = master.support)
   if(changed.work)  boa.chain(work.support = work.support)
   changed <- c(changed.master, changed.work)
   names(changed) <- c("master", "work")

   return(changed)
}


boa.chain.reset <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   boa.chain(work = boa.chain("master"),
              work.support = boa.chain("master.support"), work.sync = TRUE)
   invisible()
}


#-------------------------------------------------------------------------------
# File: libboa_menu.q
# Description: Library of menu functions for the Bayesian Output Analysis
#    Program (BOA)
# Author: Brian J. Smith <brian-j-smith@uiowa.edu>
#-------------------------------------------------------------------------------


boa.menu <- function(recover = FALSE)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   if(!recover) {
      boa.start()
      cat("NOTE: if the menu unexpectedly terminates, type \"boa.menu(recover",
          "= TRUE)\" to\n",
          "restart and recover your work\n", sep = "")
   }
   mtitle <- "\nBOA MAIN MENU\n*************"
   choices <- c("File     >>", "Data     >>", "Analysis >>", "Plot     >>",
                "Options  >>", "Window   >>")
   idx <- 1
   while(idx != 99) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = idx <- boa.menu.file(),
         "2" = boa.menu.data(),
         "3" = boa.menu.analysis(),
         "4" = boa.menu.plot(),
         "5" = boa.menu.par(),
         "6" = boa.menu.window()
      )
   }
   boa.quit()
   invisible()
}


boa.menu.analysis <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nANALYSIS MENU\n============="
   choices <- c("Descriptive Statistics  >>", "Convergence Diagnostics >>",
                "Options...", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = idx <- boa.menu.stats(),
         "2" = idx <- boa.menu.coda(),
         "3" = boa.menu.setpar("Analysis"),
         "4" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.chains <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nCHAINS MENU\n-----------"
   choices <- c("Combine All", "Delete", "Subset", "Back",
                "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = { if(boa.chain.collapse()) {
                    cat("+++ Successfully combined chains +++\n")
                 } else {
                    cat("--- No common parameters to combine ---\n")
                 }
               },
         "2" = { cat("\nDELETE CHAINS\n",
                     "=============\n",
                     "\nChains:\n",
                     "-------\n\n", sep = "")
                 lnames <- names(boa.chain("master"))
                 names(lnames) <- seq(lnames)
                 print(lnames)
                 cat("\nSpecify chain index or vector of indices [none]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0)
                    boa.chain.del(lnames = lnames[eval(parse(text = value))])
               },
         "3" = { chain.args <- list()
                 info <- boa.chain.info(boa.chain("work"),
                                        boa.chain("work.support"))
                 cat("\nSUBSET CHAINS\n",
                     "=============\n",
                     "Specify the indices of the items to be included in the ",
                     "subset.  Alternatively,\n",
                     "items may be excluded by supplying negative indices.  ",
                     "Selections should be in\n",
                     "the form of a number or numeric vector.\n",
                     "\nChains:\n",
                     "-------\n\n", sep = "")
                 names(info$lnames) <- seq(info$lnames)
                 print(info$lnames)
                 cat("\nSpecify chain indices [all]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0) {
                    lnames <- info$lnames[eval(parse(text = value))]
                    chain.args$lnames <- lnames
                 } else {
                    lnames <- info$lnames
                 }
                 cat("\nParameters:\n",
                     "-----------\n\n", sep = "")
                 info$pnames <- unique(unlist(info$pnames[lnames]))
                 names(info$pnames) <- seq(info$pnames)
                 print(info$pnames)
                 cat("\nSpecify parameter indices [all]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0) {
                    value <- eval(parse(text = value))
                    chain.args$pnames <- info$pnames[value]
                 }
                 cat("\nIterations:\n",
                     "+++++++++++\n\n", sep = "")
                 print(info$iter.range[lnames, , drop = FALSE])
                 cat("\nSpecify iterations [all]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0)
                    chain.args$iter <- eval(parse(text = value))
                 do.call("boa.chain.subset", args = chain.args)
               },
         "4" = idx <- -1,
         "5" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.coda <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nCONVERGENCE DIAGNOSTICS MENU\n----------------------------"
   choices <- c("Brooks, Gelman & Rubin", "Geweke", "Heidelberger & Welch",
                "Raftery & Lewis", "Back", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = boa.print.gandr(),
         "2" = boa.print.geweke(),
         "3" = boa.print.handw(),
         "4" = boa.print.randl(),
         "5" = idx <- -1,
         "6" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.data <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nDATA MANAGEMENT MENU\n===================="
   choices <- c("Chains                  >>",
                "Parameters              >>", "Display Working Dataset",
                "Display Master Dataset", "Reset", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      sync <- boa.chain("work.sync")
      if(sync) {
         choices[5] <- "*****"
      } else {
         choices[5] <- "Reset"
      }
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = idx <- boa.menu.chains(),
         "2" = idx <- boa.menu.parms(),
         "3" = { boa.print.info()
                 cat("\nPress <ENTER> to continue")
                 readline()
               },
         "4" = { boa.print.info("master")
                 cat("\nPress <ENTER> to continue")
                 readline()
               },
         "5" = { if(!sync) {
                    boa.chain.reset()
                    cat("+++ Master Dataset successfully copied to Working",
                        "Dataset +++\n")
                 }
               },
         "6" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.file <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nFILE MENU\n========="
   choices <- c("Import Data         >>", "Load Session", "Save Session",
                "Return to Main Menu", "Exit BOA")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = idx <- boa.menu.import(),
         "2" = { cat("\nEnter name of object to load [none]\n")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(length(value) && boa.load(value))
                    cat("+++ Data successfully loaded +++\n")
               },
         "3" = { saveas <- "y"
                 cat("\nEnter name of object to which to save the session",
                     "data [none]\n")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(length(value)) {
                    if(exists(value)) {
                       cat("Object already exists.  Overwrite (y/n) [n]?\n")
                       saveas <- scan(what = "", n = 1, strip.white = TRUE)
                    }
                    if(length(saveas) > 0 && (saveas == "y")
                       && boa.save(value))
                       cat("+++ Data successfully saved +++\n")
                 }
               },
         "4" = idx <- 0,
         "5" = { cat("\nDo you really want to EXIT (y/n) [n]?\n")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(length(value) > 0 && value == "y")  idx <- -99
               }
      )
   }

   return(abs(idx))
}


boa.menu.import <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nIMPORT DATA MENU\n----------------"
   choices <- c("BUGS Output File", "Flat ASCII File", "Data Matrix Object",
                "View Format Specifications", "Options...", "Back",
                "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = { cat("\nEnter filename prefix without the .ind or .out ",
                     "extension [Working Directory: ",
                     deparse(boa.par("path")), "]\n", sep = "")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(boa.chain.import(value, type = "BUGS"))
                    cat("+++ Data successfully imported +++\n")
               },
         "2" = { cat("\nEnter filename prefix without the ",
                     boa.par("ASCIIext"), " extension [Working Directory: ",
                     deparse(boa.par("path")), "]\n", sep = "")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(boa.chain.import(value, type = "ASCII"))
                    cat("+++ Data successfully imported +++\n")
               },
         "3" = { cat("\nEnter object name [none]\n")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(length(value)) {
                    if(!exists(value)) {
                       cat("Warning: object not found\n")
                    } else if(boa.chain.add(get(value), value)) {
                       cat("+++ Object successfully imported +++\n")
                    }
                 }
               },
         "4" = { cat("\nBUGS\n",
                     "- Bayesian inference Using Gibbs Sampling output files",
                     " (*.ind and *.out)\n",
                     "- files must be located in the Working Directory (see",
                     " Options)\n",
                     "\nASCII\n",
                     "- ASCII file (*", boa.par("ASCIIext"), ") containing",
                     " the monitored parameters from one run of the\n",
                     "  sampler\n",
                     "- file must be located in the Working Directory (see",
                     " Options)\n",
                     "- parameters are stored in space, comma, or tab",
                     " delimited columns\n",
                     "- parameter names must appear in the first row\n",
                     "- iteration numbers may be specified in a column",
                     " labeled 'iter'\n",
                     "\nMatrix Object\n",
                     "- S or R numeric matrix whose columns contain the",
                     " monitored parameters from one\n",
                     "  run of the sampler\n",
                     "- iteration numbers and parameter names may be",
                     " specified in the dimnames\n", sep = "")
                 cat("\nPress <ENTER> to continue")
                 readline()
               },
         "5" = boa.menu.setpar("Data"),
         "6" = idx <- -1,
         "7" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.par <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nGLOBAL OPTIONS MENU\n==================="
   choices <- c("Analysis...", "Data...", "Plot...", "All...",
                "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = boa.menu.setpar("Analysis"),
         "2" = boa.menu.setpar("Data"),
         "3" = boa.menu.setpar("Plot"),
         "4" = boa.menu.setpar(),
         "5" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.parms <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nPARAMETERS MENU\n---------------"
   choices <- c("Set Bounds", "Delete", "New", "Back", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = { chain.args <- list()
                 info <- boa.chain.info(boa.chain("master"),
                                        boa.chain("master.support"))
                 cat("\nSET PARAMETER BOUNDS\n",
                     "====================\n",
                     "\nParameters:\n",
                     "-----------\n\n", sep = "")
                 info$pnames <- unique(unlist(info$pnames))
                 names(info$pnames) <- seq(info$pnames)
                 print(info$pnames)
                 cat("\nSpecify parameter index or vector of indices [none]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0) {
                    value <- eval(parse(text = value))
                    chain.args$pnames <- info$pnames[value]

                    cat("\nSpecify lower and upper bounds as a vector",
                        "[c(-Inf, Inf)]\n")
                    value <- scan(what = "", n = 1, sep = "\n")
                    if(length(value) > 0) {
                       chain.args$limits <- eval(parse(text = value))
                    } else {
                       chain.args$limits <- c(-Inf, Inf)
                    }
                    do.call("boa.chain.support", args = chain.args)
                 }
               },
         "2" = { info <- boa.chain.info(boa.chain("master"),
                                        boa.chain("master.support"))
                 cat("\nDELETE PARAMETERS\n",
                     "=================\n",
                     "\nParameters:\n",
                     "-----------\n\n", sep = "")
                 pnames <- unique(unlist(info$pnames))
                 names(pnames) <- seq(pnames)
                 print(pnames)
                 cat("\nSpecify parameter index or vector of indices [none]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0)
                    boa.chain.del(pnames = pnames[eval(parse(text = value))])
               },
         "3" = { info <- boa.chain.info(boa.chain("work"),
                                        boa.chain("work.support"))
                 cat("\nNEW PARAMETER\n",
                     "=============\n",
                     "\nWorking Parameters:\n",
                     "-------------------\n\n", sep = "")
                 pnames <- info$pnames[[1]]
                 for(i in info$lnames[-1]) {
                    pnames <- intersect(info$pnames[[i]], pnames)
                 }
                 print(pnames)
                 cat("\nNew parameter name [none]\n")
                 value <- scan(what = "", n = 1, sep = "\n")
                 if(length(value) > 0) {
                    cat("\nDefine the new parameter as a function of the",
                        "parameters listed above\n")
                    expr <- parse(text = scan(what = "", n = 1, sep = "\n"))
                    boa.chain.eval(expr, value)
                 }
               },
         "4" = idx <- -1,
         "5" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.plot <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nPLOT MENU\n========="
   choices <- c("Descriptive             >>", "Convergence Diagnostics >>",
                "Options...", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = idx <- boa.menu.plotdesc(),
         "2" = idx <- boa.menu.plotcoda(),
         "3" = boa.menu.setpar("Plot"),
         "4" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.plotdesc <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nDESCRIPTIVE PLOT MENU\n---------------------"
   choices <- c("Autocorrelations", "Density", "Running Mean", "Trace",
                "Back", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = boa.plot("acf"),
         "2" = boa.plot("density"),
         "3" = boa.plot("history"),
         "4" = boa.plot("trace"),
         "5" = idx <- -1,
         "6" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.plotcoda <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- paste("\nCONVERGENCE DIAGNOSTICS PLOT MENU",
                   "\n---------------------------------", sep = "")
   choices <- c("Brooks & Gelman", "Gelman & Rubin", "Geweke", "Back",
                "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = boa.plot("bandg"),
         "2" = boa.plot("gandr"),
         "3" = boa.plot("geweke"),
         "4" = idx <- -1,
         "5" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.setpar <- function(group)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   if(missing(group)) {
      value <- boa.print.par()
   } else {
      value <- boa.print.par(group)
   }
   par.names <- value[, "par"]
   par.notes <- value[, "note"]
   cat("\n")
   idx <- ""
   while((length(idx) > 0) && !is.element(idx, seq(par.names))) {
      cat("Select parameter to change or press <ENTER> to continue\n")
      idx <- scan(what = "", n = 1, strip.white = TRUE)
   }
   if(length(idx) > 0) {
      cat("\n")
      idx <- as.numeric(idx)
      if(nchar(par.notes[idx]))  cat("NOTE:", par.notes[idx], "\n")
      switch(data.class(boa.par(par.names[idx])),
         "numeric"   = { cat("Enter new numeric value\n")
                         value <- eval(parse(text = scan(what = "", n = 1,
                                  sep = "\n")))
                       },
         "character" = { cat("Enter new character string\n")
                         value <- scan(what = "", n = 1, sep = "\n")
                       },
         "logical"   = { cat("Enter new logical value\n")
                         value <- eval(parse(text = scan(what = "", n = 1)))
                       },
         "function"  = { cat("Enter new function followed by a blank line\n")
                         value <- eval(parse(text = paste(scan(what = "",
                                  sep = "\n"), collapse = "\n")))
                       },
         value <- NULL
      )
      boa.par(structure(list(value), names = par.names[idx]))
   }
   invisible()
}


boa.menu.stats <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtitle <- "\nDESCRIPTIVE STATISTICS MENU\n---------------------------"
   choices <- c("Autocorrelations", "Correlation Matrix",
                "Highest Probability Density Intervals",
                "Summary Statistics",
                "Back", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = boa.print.acf(),
         "2" = boa.print.cor(),
         "3" = boa.print.hpd(),
         "4" = boa.print.stats(),
         "5" = idx <- -1,
         "6" = idx <- 0
      )
   }

   return(abs(idx))
}


boa.menu.window <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   choices <- c("Previous", "Next", "Save to Postscript File", "Close",
                "Close All", "Return to Main Menu")
   idx <- 1
   while(idx > 0) {
      mtitle <- dev.cur()
      mtitle <- paste("\nWINDOW ", mtitle, " MENU\n============",
                      paste(rep("=", nchar(mtitle)), collapse = ""), sep = "")
      idx <- menu(choices, title = mtitle)
      switch(idx,
         "1" = dev.set(dev.prev()),
         "2" = dev.set(dev.next()),
         "3" = { cat("\nEnter name of file to which to save the plot [none]\n")
                 value <- scan(what = "", n = 1, strip.white = TRUE)
                 if(length(value) > 0) {
                    dev.print(device = postscript,
                              file = paste(boa.par("path"), value, sep = ""))
                 }
               },
         "4" = boa.plot.close(),
         "5" = boa.plot.close(boa.par("dev.list")),
         "6" = idx <- 0
      )
   }

   return(abs(idx))
}


#-------------------------------------------------------------------------------
# File: libboa_plot.q
# Description: Library of plot functions for the Bayesian Output Analysis 
#    Program (BOA)
# Author: Brian J. Smith <brian-j-smith@uiowa.edu>
#-------------------------------------------------------------------------------


boa.plot <- function(type, dev = boa.par("dev"), mfdim = boa.par("plot.mfdim"),
                     newplot = boa.par("plot.new"),
                     onelink = boa.par("plot.onelink"),
                     oneparm = boa.par("plot.oneparm"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   drawn <- FALSE
   switch(type,
      "acf"     = { foo <- "boa.plot.acf"
                    foo.args <- expression(list(largs[[idx[i]]],
                                                pargs[[idx[i]]]))
                    onelink <- TRUE
                    oneparm <- TRUE },
      "bandg"   = { foo <- "boa.plot.bandg"
                    foo.args <- list()
                    onelink <- FALSE
                    oneparm <- FALSE },
      "density" = { foo <- "boa.plot.density"
                    foo.args <- expression(list(largs[[idx[i]]],
                                                pargs[[idx[i]]])) },
      "gandr"   = { foo <- "boa.plot.gandr"
                    foo.args <- expression(list(pargs[[idx[i]]]))
                    onelink <- FALSE
                    oneparm <- TRUE },
      "geweke"  = { foo <- "boa.plot.geweke"
                    foo.args <- expression(list(largs[[idx[i]]],
                                                pargs[[idx[i]]]))
                    onelink <- TRUE
                    oneparm <- TRUE },
      "history" = { foo <- "boa.plot.history"
                    foo.args <- expression(list(largs[[idx[i]]],
                                                pargs[[idx[i]]])) },
      "trace"   = { foo <- "boa.plot.trace"
                    foo.args <- expression(list(largs[[idx[i]]],
                                                pargs[[idx[i]]])) },
      { foo <- NULL
        cat("Warning: plot type does not exist\n") }
   )
   if(is.character(foo)) {
      work <- boa.chain("work")
      lnames <- names(work)
      largs <- pargs <- list(0)
      pidx <- NULL
      for(i in lnames) {
         pnames <- boa.pnames(work[[i]])
         if(oneparm) {
            for(j in pnames) {
               if(onelink) {
                  n <- length(largs)
                  largs[[n + 1]] <- i
                  pargs[[n + 1]] <- j
                  pidx <- c(pidx, paste(j, i))
               } else if(is.element(j, pidx)) {
                  largs[[j]] <- c(largs[[j]], i)
               } else {
                  largs[[j]] <- i
                  pargs[[j]] <- j
                  pidx <- c(pidx, j)
               }
            }
         } else if(onelink) {
            largs[[i]] <- i
            pargs[[i]] <- pnames
            pidx <- c(pidx, i)
         } else if(length(pidx) > 0) {
            largs[[2]] <- c(largs[[2]], i)
            pargs[[2]] <- union(pargs[[2]], pnames)
         } else {
            largs[[2]] <- i
            pargs[[2]] <- pnames
            pidx <- 2
         }
      }
      largs[[1]] <- pargs[[1]] <- NULL

      if(!newplot)  boa.plot.close(boa.par("dev.list"))

      idx <- order(pidx)
      n <- length(idx)
      size <- prod(mfdim)
      newdim <- mfdim
      imin <- ifelse(mfdim[1] <= mfdim[2], 1, 2)
      imax <- imin %% 2 + 1
      ratio <- mfdim[imin] / mfdim[imax]
      for(i in 1:n) {
         if((size == 1) || ((i %% size) == 1)) {
            boa.plot.open(dev)
            nleft <- n - i + 1
            while((nleft <= prod(newdim)) && (i == 1)) {
               if(newdim[1] > 1) {
                  mfdim <- c(ceiling(nleft / newdim[2]), newdim[2])
                  newdim[imax] <- newdim[imax] - 1
                  newdim[imin] <- round(ratio * newdim[imax])
               } else {
                  mfdim <- c(1, nleft)
                  newdim <- 0
               }
            }
            boa.plot.par(mfdim)
         }
         drawn <- do.call(foo, args = eval(foo.args)) || drawn
         boa.plot.title()
      }
   }

   return(drawn)
}


boa.plot.acf <- function(lname, pname)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   drawn <- FALSE
   parm <- boa.getparms(boa.chain("work")[[lname]], pname)
   if(is.matrix(parm)) {
      drawn <- TRUE
      result <- acf(parm, plot = FALSE)
      plot(result$lag, result$acf, xlab = "Lag", ylab = "Autocorrelation",
           ylim = c(-1, 1), type = "h")
      abline(0, 0)
      usr <- par("usr")
      key(x = usr[2], y = 1, corner = c(1, 1),
          text = paste(pname, ":", substring(lname, first = 1,
                      last = 12)), adj = 1, transparent = TRUE)
      title("Autocorrelation Plot")
   }

   return(drawn)
}


boa.plot.bandg <- function(bins = boa.par("gandr.bins"),
                           win = boa.par("gandr.win"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   drawn <- FALSE
   work <- boa.chain("work")
   work.support <- boa.chain("work.support")
   riter <- NULL
   for(i in names(work))  riter <- range(riter, boa.iter(work[[i]]))
   x <- unique(round(seq(min(riter[1] + 49, riter[2]), riter[2],
                         length = bins)))
   Rp <- NULL
   Rmax <- NULL
   for(i in x) {
      result <- boa.chain.gandr(work, work.support, 1, window = win, to = i)
      Rp <- c(Rp, result$mpsrf)
      Rmax <- c(Rmax, max(result$psrf))
   }
   idx <- is.finite(Rp)
   if(any(idx)) {
      drawn <- TRUE
      x <- x[idx]
      Rp <- spline(x, Rp[idx])
      Rmax <- spline(x, Rmax[idx])
      ylim <- range(1, Rp$y, Rmax$y)
      plot(Rmax, xlab = "Last Iteration in Segment", ylab = "Shrink Factor",
           ylim = ylim, type = "l")
      lines(Rp, lty = 2)
      abline(1, 0, lty = 3)
      usr <- par("usr")
      key(x = usr[2], y = ylim[2], corner = c(1, 1),
          text = list(c("Rp", "Rmax")), lines = list(lty = c(2, 1)),
          transparent = TRUE)
      title("Brooks & Gelman Multivariate Shrink Factors")
   }

   return(drawn)
}


boa.plot.close <- function(which = dev.cur())
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   shutdown <- NULL
   current <- boa.par("dev.list")
   idx <- is.element(current, which)
   for(i in intersect(current[idx], dev.list())) {
      shutdown <- dev.off(i)
   }
   boa.par(dev.list = current[!idx])

   return(shutdown)
}


boa.plot.density <- function(lnames, pnames, bandwidth = boa.par("bandwidth"),
                             window = boa.par("kernel"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   ipnames <- list()
   xydensity <- list()
   xlim <- NULL
   ylim <- NULL
   key.names <- NULL
   lnames <- intersect(names(work), lnames)
   k <- 0
   for(i in lnames) {
      ipnames[[i]] <- intersect(boa.pnames(work[[i]]), pnames)
      for(j in ipnames[[i]]) {
         k <- k + 1
         width <- bandwidth(work[[i]][, j])
         xydensity[[k]] <- density(work[[i]][, j], n = 100, width = width,
                                   window = window)
         xlim <- range(xlim, xydensity[[k]]$x)
         ylim <- range(ylim, xydensity[[k]]$y)
      }
      key.names <- c(key.names, paste(ipnames[[i]], ":",
                     substring(i, first = 1, last = 12)))
   }
   drawn <- k > 0
   if(drawn) {
      plot(xlim, ylim, xlab = "Parameter Value", ylab = "Density", xlim = xlim,
           ylim = ylim, type = "n")
      k <- 0
      for(i in lnames) {
         for(j in ipnames[[i]]) {
            k <- k + 1
            lines(xydensity[[k]], lty = k)
            parm <- work[[i]][, j]
            points(parm, rep(0, length(parm)), pch = k)
         }
      }
      key(x = xlim[2], y = ylim[2], corner = c(1, 1), text = list(key.names),
          lines = list(lty = 1:k), transparent = TRUE)
      title("Density Plot")
   }

   return(drawn)
}


boa.plot.gandr <- function(pname, bins = boa.par("gandr.bins"),
                           alpha = boa.par("alpha"),
                           win = boa.par("gandr.win"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   drawn <- FALSE
   work <- boa.chain("work")
   work.support <- boa.chain("work.support")
   riter <- NULL
   for(i in names(work)) {
      if(is.element(pname, boa.pnames(work[[i]])))
         riter <- range(riter, boa.iter(work[[i]]))
   }
   x <- unique(round(seq(min(riter[1] + 49, riter[2]), riter[2],
                         length = bins)))
   R <- NULL
   Rq <- NULL
   for(i in x) {
      result <- boa.chain.gandr(work, work.support, alpha, pname, win, i)
      R <- c(R, result$csrf[1, 1])
      Rq <- c(Rq, result$csrf[1, 2])
   }
   idx <- is.finite(R)
   if(any(idx)) {
      drawn <- TRUE
      x <- x[idx]
      R <- spline(x, R[idx])
      Rq <- spline(x, Rq[idx])
      ylim <- range(1, R$y, Rq$y)
      plot(R, xlab = "Last Iteration in Segment", ylab = "Shrink Factor",
           ylim = ylim, type = "l")
      lines(Rq, lty = 2)
      abline(1, 0, lty = 3)
      usr <- par("usr")
      key(x = usr[2], y = ylim[2], corner = c(1, 1),
          text = list(c(pname, 1 - alpha / 2, "Median")),
          lines = list(lty = 3:1), type = c("n", "l", "l"), transparent = TRUE)
      title("Gelman & Rubin Shrink Factors")
   }

   return(drawn)
}


boa.plot.geweke <- function(lname, pname, bins = boa.par("geweke.bins"),
                            p.first = boa.par("geweke.first"),
                            p.last = boa.par("geweke.last"),
                            alpha = boa.par("alpha"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   drawn <- FALSE
   parm <- boa.getparms(boa.chain("work")[[lname]], pname)
   if(is.matrix(parm)) {
      riter <- range(boa.iter(parm))
      x <- unique(round(seq(riter[2], riter[1], length = bins + 1)))[-1]
      y <- NULL
      for(i in x) {
         y <- c(y, boa.geweke(boa.getiter(parm, i:riter[2]),
                              p.first, p.last)[1,"Z-Score"])
      }
      if(any(is.finite(y))) {
         drawn <- TRUE
         q.upper <- qnorm(1 - alpha / 2)
         ylim <- range(y, -q.upper, q.upper, na.rm = TRUE)
         plot(x, y, xlab = "First Iteration in Segment", ylab = "Z-Score",
              ylim = ylim)
         abline(q.upper, 0, lty = 2)
         abline(0, 0)
         abline(-q.upper, 0, lty = 2)
         usr <- par("usr")
         key(x = usr[2], y = ylim[2], corner = c(1, 1),
             text = paste(pname, ":", substring(lname, first = 1,
                         last = 12)), adj = 1, transparent = TRUE)
         title("Geweke Convergence Diagnostic")
      }
   }

   return(drawn)
}


boa.plot.history <- function(lnames, pnames)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   ipnames <- list()
   ybar <- list()
   xlim <- NULL
   ylim <- NULL
   key.names <- NULL
   lnames <- intersect(names(work), lnames)
   k <- 0
   for(i in lnames) {
      ipnames[[i]] <- intersect(boa.pnames(work[[i]]), pnames)
      for(j in ipnames[[i]]) {
         k <- k + 1
         parm <- work[[i]][, j]
         ybar[[k]] <- cumsum(parm) / seq(parm)
         ylim <- range(ylim, ybar[[k]])
      }
      xlim <- range(xlim, boa.iter(work[[i]]))
      key.names <- c(key.names, paste(ipnames[[i]], ":",
                     substring(i, first = 1, last = 12)))
   }
   drawn <- k > 0
   if(drawn) {
      plot(xlim, ylim, xlab = "Iteration", ylab = "Parameter Value",
           xlim = xlim, ylim = ylim, type = "n")
      k <- 0
      for(i in lnames) {
         for(j in ipnames[[i]]) {
            k <- k + 1
            lines(boa.iter(work[[i]]), ybar[[k]], lty = k)
         }
      }
      key(x = xlim[2], y = ylim[2], corner = c(1, 1), text = list(key.names),
          lines = list(lty = 1:k), transparent = TRUE)
      title("Running Mean Plot")
   }

   return(drawn)
}


boa.plot.open <- function(which = boa.par("dev"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   created <- NULL
   devices <- c("graphsheet", "motif", "openlook", "win.graph", "windows",
                "X11")
   if(is.element(which, devices) && exists(which)) {
      do.call(which, args = list())
      created <- dev.cur()
      boa.par(dev.list = intersect(c(boa.par("dev.list"), created), dev.list()))
   } else {
      cat("Warning: graphics device not supported on this platform\n")
   }

   return(created)
}


boa.plot.par <- function(mfdim = c(1, 1))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   par(mfrow = mfdim, oma = c(0, 0, 3 + max(mfdim), 0))
   invisible()
}


boa.plot.title <- function(text = boa.par("title"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   mtext(text, line = 1, outer = TRUE, cex = 2)
   invisible()
}


boa.plot.trace <- function(lnames, pnames)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   ipnames <- list()
   ybar <- list()
   xlim <- NULL
   ylim <- NULL
   key.names <- NULL
   lnames <- intersect(names(work), lnames)
   for(i in lnames) {
      ipnames[[i]] <- intersect(boa.pnames(work[[i]]), pnames)
      ylim <- range(ylim, work[[i]][, ipnames[[i]]])
      xlim <- range(xlim, boa.iter(work[[i]]))
      key.names <- c(key.names, paste(ipnames[[i]], ":",
                     substring(i, first = 1, last = 12)))
   }
   drawn <- is.vector(key.names)
   if(drawn) {
      plot(xlim, ylim, xlab = "Iteration", ylab = "Parameter Value",
           xlim = xlim, ylim = ylim, type = "n")
      k <- 0
      for(i in lnames) {
         for(j in ipnames[[i]]) {
            k <- k + 1
            lines(boa.iter(work[[i]]), work[[i]][, j], lty = k)
         }
      }
      key(x = xlim[2], y = ylim[2], corner = c(1, 1), text = list(key.names),
          lines = list(lty = 1:k), transparent = TRUE)
      title("Trace Plot")
   }

   return(drawn)
}


#-------------------------------------------------------------------------------
# File: libboa_print.q
# Description: Library of print functions for the Bayesian Output Analysis 
#    Program (BOA)
# Author: Brian J. Smith <brian-j-smith@uiowa.edu>
#-------------------------------------------------------------------------------


boa.print.acf <- function(lags = boa.par("acf.lags"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "LAGS AND AUTOCORRELATIONS:\n",
       "==========================\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      print(boa.acf(work[[i]], lags))
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.cor <- function()
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "CROSS-CORRELATION MATRIX:\n",
       "=========================\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      corr <- round(cor(work[[i]]), digits = options()$digits)
      lt <- lower.tri(corr, diag = TRUE)
      corr[!lt] <- ""
      print(corr, quote = FALSE)
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.gandr <- function(alpha = boa.par("alpha"),
                            win = boa.par("gandr.win"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{    work <- boa.chain("work")
   work.support <- boa.chain("work.support")
   cat("\n",
       "BROOKS, GELMAN AND RUBIN CONVERGENCE DIAGNOSTICS:\n",
       "=================================================\n\n", sep = "")
   gandr <- boa.chain.gandr(work, work.support, alpha, window = win)
   cat("Iterations used = ", paste(gandr$window, collapse=":"), "\n\n",
       sep = "")
   cat("Potential Scale Reduction Factors\n",
       "---------------------------------\n\n", sep = "")
   print(gandr$psrf)
   cat("\n",
       "Multivariate Potential Scale Reduction Factor = ",
       round(gandr$mpsrf, digits = options()$digits), "\n", sep = "")
   cat("\n",
       "Corrected Scale Reduction Factors\n",
       "---------------------------------\n\n", sep = "")
   print(gandr$csrf)
   cat("\nPress <ENTER> to continue")
   readline()
   invisible()
}


boa.print.geweke <- function(p.first = boa.par("geweke.first"),
                             p.last = boa.par("geweke.last"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "GEWEKE CONVERGENCE DIAGNOSTIC:\n",
       "==============================\n\n",
       "Fraction in first window = ", p.first, "\n",
       "Fraction in last window = ", p.last, "\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      print(t(boa.geweke(work[[i]], p.first, p.last)))
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.handw <- function(error = boa.par("handw.error"),
                            alpha = boa.par("alpha"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "HEIDLEBERGER AND WELCH STATIONARITY AND INTERVAL HALFWIDTH TESTS:\n",
       "=================================================================\n\n",
       "Halfwidth test accuracy = ", error, "\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      print(boa.handw(work[[i]], error, alpha))
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.hpd <- function(alpha = boa.par("alpha"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "HIGHEST PROBABILITY DENSITY INTERVALS:\n",
       "======================================\n\n",
       "Alpha level = ", alpha, "\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      link <- work[[i]]
      pnames <- boa.pnames(link)
      R <- NULL
      for(j in pnames) {
         R <- rbind(R, boa.hpd(link[, j], alpha))
      }
      dimnames(R)[[1]] <- pnames
      print(R)
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.info <- function(which = "work")
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   switch(which,
      "work" = { chain <- boa.chain("work")
                 chain.support <- boa.chain("work.support") },
      "master" = { chain <- boa.chain("master")
                 chain.support <- boa.chain("master.support") },
      chain <- NULL
   )
   cat("\n",
       "CHAIN SUMMARY INFORMATION:\n",
       "==========================\n\n", sep = "")
   chain.info <- boa.chain.info(chain, chain.support)
   if(is.list(chain.info)) {
      cat("Iterations:\n",
          "+++++++++++\n\n", sep = "")
      print(chain.info$iter.range)
      for(i in chain.info$lnames) {
         header <- paste("\nSupport: ", i, "\n", sep = "")
         cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
         print(chain.info$support[[i]])
      }
   } else {
      cat("Warning: chain contains no data\n")
   }
   invisible(chain.info)
}


boa.print.par <- function(group)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   pardesc <- NULL
   if(missing(group)) {
      pardesc <- boa.pardesc()
   } else if(is.element(group, c("Analysis", "Data", "Plot"))) {
      pardesc <- boa.pardesc()
      pardesc <- pardesc[pardesc[, "group"] == group, ]
   }
   if(is.matrix(pardesc)) {
      globals <- boa.par()[pardesc[, "par"]]
      heading1 <- heading2 <- ""
      mar1 <- nchar(seq(globals))
      mar2 <- nchar(pardesc[, "desc"])
      col12 <- max(mar1) + max(mar2) + 4
      mar1 <- max(mar1) - mar1 + 1
      mar2 <- max(mar2) - mar2 + 1
      for(i in seq(globals)) {
         if(heading1 != pardesc[i, "group"]) {
            heading1 <- pardesc[i, "group"]
            cat("\n", heading1, " Parameters\n",
                rep("=", nchar(heading1) + 11), "\n", sep = "")
            heading2 <- ""
         }
         if(heading2 != pardesc[i, "method"]) {
            heading2 <- pardesc[i, "method"]
            cat("\n", heading2, "\n",
                rep("-", nchar(heading2)), "\n", sep = "")
         }
         value <- deparse(globals[[i]])
         cat(i, ")", rep(" ", mar1[i]), pardesc[i, "desc"], ":",
             rep(" ", mar2[i]), value[1], "\n", sep = "")
         for(j in seq(value)[-1])
            cat(rep(" ", col12), value[j], "\n", sep = "")
      }
   } else {
      cat("Warning: parameter group does not exist\n")
   }
   invisible(pardesc)
}


boa.print.randl <- function(q = boa.par("randl.q"),
                            error = boa.par("randl.error"),
                            prob = 1 - boa.par("alpha"),
                            delta = boa.par("randl.delta"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "RAFTERY AND LEWIS CONVERGENCE DIAGNOSTIC:\n",
       "=========================================\n\n",
       "Quantile = ", q, "\n",
       "Accuracy = +/- ", error, "\n",
       "Probability = ", prob, "\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      randl <- boa.randl(work[[i]], q, error, prob, delta)
      if(is.matrix(randl)) {
         print(randl)
      } else {
         cat("******* Warning *******\n",
             "Available chain length is ", nrow(work[[i]]), ".\n",
             "Re-run simulation for at least ", randl, " iterations\n",
             "OR reduce the quantile, accuracy, or probability to be ",
             "estimated.\n", sep = "")
      }
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


boa.print.stats <- function(probs = boa.par("quantiles"),
                            batch.size = boa.par("batch.size"))
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
{
   work <- boa.chain("work")
   cat("\n",
       "SUMMARY STATISTICS:\n",
       "===================\n\n",
       "Bin size for calculating Batch SE and (Lag 1) ACF = ", batch.size,
       "\n", sep = "")
   for(i in names(work)) {
      header <- paste("\nChain: ", i, "\n", sep = "")
      cat(header, rep("-", nchar(header) - 2), "\n\n", sep = "")
      print(boa.stats(work[[i]], probs, batch.size))
      cat("\nPress <ENTER> to continue")
      readline()
   }
   invisible()
}


#-------------------------------------------------------------------------------
# File: patch.r
# Description: Functions needed by R users to get the Bayesian Output Analysis
#    Program (BOA) working.  Included are prototypes of the relevant
#    functions supplied with S-PLUS 4.0+, but not with R 0.64
# Maintained by: Brian J. Smith <brian-j-smith@uiowa.edu>
#                Simon Fear <fears@roycastle.liv.ac.uk>
#-------------------------------------------------------------------------------


acf <- function(x, lag.max, type = "correlation", plot = TRUE) 
#-------------------------------------------------------------------------------
# Author: Martyn Plummer
#-------------------------------------------------------------------------------
{
   legal.types <- c("correlation", "covariance")
   check.type <- pmatch(type, legal.types)
   if(is.na(check.type)) {
      stop("Unknown type")
   } else if (check.type == 0) {
      stop("Ambiguous type")
   } else {
      type <- legal.types[check.type]
   }
   series <- deparse(substitute(x))
   x.freq <- frequency(as.ts(x))
   attr(x, "tsp") <- NULL
   x <- as.matrix(x)
   n.used <- nrow(x)
   if(missing(lag.max))  lag.max <- floor(10 * log10(n.used))
   lag.max <- min(lag.max, n.used - 1)
   lag <- seq(from = 0, to = lag.max)
   acf <- matrix(nrow = lag.max + 1, ncol = ncol(x))
   x1 <- x2 <- sweep(x, 2, apply(x, 2, mean))
   for (i in 1:nrow(acf)) {
      acf[i, ] <- apply(x1 * x2, 2, sum) / n.used
      x1 <- x1[-1, , drop = FALSE]
      x2 <- x2[-nrow(x2), , drop = FALSE]
   }
   if(type == "correlation")  acf <- sweep(acf, 2, acf[1, ], "/")
   if(ncol(acf) == 1)  acf <- as.vector(acf)
   return(acf = acf, lag = lag / x.freq, n.used = n.used, type = type,
          series = series)
}


colMeans <- function(x, na.rm = FALSE)
{
   apply(x, 2, mean, na.rm = na.rm)
}


colVars <- function(x, na.rm = FALSE, unbiased = TRUE, SumSquares = FALSE)
{
   apply(x, 2, var)
}


key <- function(x = c(par("usr")[1:2] %*% c(0.01, 0.99)),
                y = c(par("usr")[3:4] %*% c(0.01, 0.99)), ..., title = "",
                align = TRUE, background = 0, border = 0, between = 1,
                corner = c(missing(x), 1), divide = 3, transparent = FALSE,
                cex = par("cex"), cex.title = max(cex), col = par("col"),
                lty = par("lty"), lwd = par("lwd"), font = par("font"),
                pch = par("pch"), adj = 0, type = "l", size = 5, columns = 1,
                between.columns = 3, angle = 0, density = -1, plot = TRUE,
                text.width.multiplier = 1)
{
   oldpar <- par("xpd")
   on.exit(par(oldpar))
   rest <- list(...)

   colnames <- names(rest)
   for(i in seq(colnames)[colnames == "text"])
      if(!is.list(rest[[i]]))  rest[[i]] <- list(rest[[i]])
   actions <- c("lines", "points", "text")
   colnames <- actions[match(colnames, actions, nomatch = 0)]

   pxy <- par("pin") / par("fin")
   nrows <- max(sapply(unlist(rest, recursive = FALSE), length))
   ncols <- length(colnames)
   text.adj <- width <- height <- matrix(0, nrows, ncols)
   between <- rep(pxy[1] * strwidth(" ") * between, ncols)

   replen <- function(a, b, n)  rep(if(is.null(a)) b else a, n)
   for(j in 1:ncols) {
      this <- rest[[j]]
      this$size <- replen(this$size, size, nrows)
      this$type <- replen(this$type, type, nrows)
      this$col <- replen(this$col, col, nrows)
      this$lty <- replen(this$lty, lty, nrows)
      this$lwd <- replen(this$lwd, lwd, nrows)
      this$adj <- replen(this$adj, adj, nrows)
      this$font <- replen(this$font, font, nrows)
      this$pch <- replen(this$pch, pch, nrows)
      rest[[j]] <- this
      for(i in 1:nrows) {
         cxy <- pxy * c(strwidth(" "), strheight(" "))
         switch(colnames[j],
            points = { width[i, j] <- cxy[1]
                       height[i, j] <- cxy[2] },
            lines  = { width[i, j] <- cxy[1] * this$size[i]
                       height[i, j] <- cxy[2] },
            text   = { width[i, j] <- pxy[1] * text.width.multiplier *
                                      strwidth(this[[1]][i])
                       height[i, j] <- cxy[2]
                       text.adj[i, j] <- this$adj[i] }
         )
      }
      if(align)  width[, j] <- max(width[, j])
   }

   xpos <- matrix(x, nrows, ncols)
   ypos <- matrix(y - cumsum(height[, 1]), nrows, ncols)
   for(j in seq(length = ncols - 1))
      xpos[, j + 1] <- xpos[, j] + width[, j] + between[j]
   xmax <- max(xpos[, ncols] + width[, ncols])
   i <- (text.adj != 0)
   if(any(i))  xpos[i] <- xpos[i] + width[i]
   title.excess <- max(0, x + text.width.multiplier * pxy[1] *
                          strwidth(title) - xmax)
   xmax <- xmax + title.excess
   if(nchar(title) > 0)
      ypos <- ypos - pxy[2] * strheight(title)
   ymin <- min(ypos)
   if(!plot)  return(c(xmax - x, y - ymin))
   x.offset <- (x - xmax) * corner[1]
   y.offset <- (y - ymin) * (1 - corner[2])
   xpos <- xpos + x.offset + title.excess / 2
   ypos <- ypos + y.offset + height / 2

   if(!transparent)
      polygon(c(x, xmax, xmax, x) + x.offset, c(y, y, ymin, ymin) + y.offset,
              col = background, border = border)

   if(nchar(title) > 0)
      text((x + xmax) / 2 + x.offset, y + y.offset - pxy[2] *
           strheight(title) / 2, title, adj = 0.5)

   for(j in 1:ncols) {
      this <- rest[[j]]
      for(i in 1:nrows) {
         switch(colnames[j],
            points = { points(xpos[i, j], ypos[i, j],
                              col = this$col[i], font = this$font[i],
                              pch = this$pch[i]) },
            lines  = { if(this$type[i] != "p") {
                          lines(xpos[i, j] +
                                seq(0, width[i, j], length = divide),
                                rep(ypos[i, j], divide),
                                lwd = this$lwd[i], type = this$type[i],
                                lty = this$lty[i], pch = this$pch[i],
                                font = this$font[i], col = this$col[i])
                          if(this$type[i] == "b" || this$type[i] == "o")
                             points(xpos[i, j] +
                                    seq(0, width[i, j], length = divide),
                                    rep(ypos[i, j], divide),
                                    lwd = this$lwd[i], type = "p", lty = 1,
                                    pch = this$pch[i], font = this$font[i],
                                    col = this$col[i])
                       } else {
                          points(xpos[i, j] + width[i, j] / 2, ypos[i, j],
                                 lwd = this$lwd[i], type = this$type[i],
                                 lty = this$lty[i], pch = this$pch[i],
                                 font = this$font[i], col = this$col[i])
                       } },
            text   = { text(xpos[i, j], ypos[i, j], this[[1]][i],
                            adj = this$adj[i], col = this$col[i],
                            font = this$font[i]) }
         )
      }
   }
   invisible()
}


spec.pgram <- function (x, spans = 1, taper = 0.1, demean = FALSE,
                        detrend = TRUE, pad = FALSE, plot = FALSE)
#-------------------------------------------------------------------------------
# Author: Martyn Plummer
#-------------------------------------------------------------------------------
{
   N <- length(x)
   if(detrend) {
      t <- 1:N
      x <- residuals(lm(x ~ t))
   } else if(demean) {
      x <- x - mean(x)
   }
   if (taper > 0.5 || taper < 0) {
      stop("taper must be between 0 and 0.5")
   } else if (taper > 0) {
      w <- rep(1, N)
      n <- max(round(N * taper), 1)
      w[1:n] <- sin(((1:n - 0.5) * pi) / (2 * n))^2
      w[N:(N - n + 1)] <- w[1:n]
      x <- x * w
   }
   if(pad)  x <- c(x, rep(0, nextn(N) - N))
   Nspec <- ceiling(N / 2) + 1
   spec <- (Mod(fft(x))^2)[1:Nspec] / N
   for (i in spans) {
      m <- floor(i / 2)
      if (m > 0) {
           filter <- c(0.5, rep(1, 2 * m - 1), 0.5) / (2 * m)
           spec0 <- c(spec[(m + 1):2], spec, spec[Nspec - 1:m])
           for (j in 1:Nspec) spec[j] <- sum(spec0[j:(j + 2 * m)] * filter)
      }
   }
   spec <- 10 * log10(spec)
   freq <- seq(from = 0, to = 0.5, length = Nspec)
   return(spec = spec, freq = freq)
}


