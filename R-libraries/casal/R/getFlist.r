"getFlist" <-
function(part, stt, fnn, Mm, tt, sex=F, agesex=F, wg=0){
# Given multiple years partition, convert to numbers at age (and sex) in each time step
# apply getF to obtain eq. F for each age (and sex), then use
# age weighting (or max if wg==0) to obtain single eq. annual F and selectivity
# for each year.
# Return vector of max Fs and vector of weighted Fs and matrix of selectivities
# Now also allows for M at age and sex

numpart <- get.tot.age(part, sex)
#browser()
sumwg <- sum(wg)
if (sex) selMat <- list(male=NULL, fem=NULL) else selMat <- NULL
annF <- NULL
wgF <- NULL
if (length(wg)==1) wgT <- F else wgT <- T
if (sex) {
        if (agesex) {
                if (length(Mm$male) <= 1) stop("Age and sex specific M required")
                for (ll in numpart) {
                        if (length(dim(ll$male)) > 0) {
                                nages <- dim(ll$male)[2]

                                mFs <- NULL
                                for (i in 1:nages) {
                                        mF <- getF(ll$male[,i],stt,fnn,Mm$male[i],tt)
                                        mFs <- c(mFs,mF)
                                }
                                fFs <- NULL
                                for (i in 1:nages) {
                                        fF <- getF(ll$fem[,i],stt,fnn,Mm$fem[i],tt)
                                        fFs <- c(fFs,fF)
                                }

                                mx <- max(c(mFs,fFs))
                                annF <- c(annF, mx)
                                if (wgT) wgF <- c(wgF, sum(wg*rbind(mFs,fFs))/sumwg)
                                selMat$male <- cbind(selMat$male, mFs/mx)
                                selMat$fem <- cbind(selMat$fem, fFs/mx)
                        }
                }
        } else {
for (ll in numpart) {
        if (length(dim(ll$male)) > 0) {
                mFs <- apply(ll$male, MARGIN=2, FUN=getF, st=stt, fn=fnn, M=Mm$male, t=tt)
                fFs <- apply(ll$fem, MARGIN=2, FUN=getF, st=stt, fn=fnn, M=Mm$fem, t=tt)
                mx <- max(c(mFs,fFs))
                annF <- c(annF, mx)
                if (wgT) wgF <- c(wgF, sum(wg*rbind(mFs,fFs))/sumwg)
                selMat$male <- cbind(selMat$male, mFs/mx)
                selMat$fem <- cbind(selMat$fem, fFs/mx)
#       browser()
        }
        }
}
} else {
for (ll in numpart) {
        if (length(dim(ll)) > 0) {
                Fs <- apply(ll, MARGIN=2, FUN=getF, st=stt, fn=fnn, M=Mm, t=tt)
                annF <- c(annF, max(Fs))
                if (wgT) wgF <- c(wgF, sum(wg*Fs)/sumwg)
                selMat <- cbind(selMat, Fs/max(Fs))
        }
}
}

return(list(wgF=wgF, annF=annF, selMat=selMat, years=as.numeric(names(part))))
}
