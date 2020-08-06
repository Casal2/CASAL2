"extract.partition" <-
function (file,path)
{
    if(missing(path)) path<-""
    filename<-casal.make.filename(path=path,file=file)
    file <- casal.convert.to.lines(filename)
    parlines <- grep("Partition:", file)
    npart <- length(parlines)
    steplines <- parlines[grep("State at the end of step", file[parlines - 1])]
    yearlines <- parlines[grep("State at the end of year", file[parlines - 1])]
    if (length(yearlines) > 0 & length(steplines) > 0) {
        npart <- npart - length(yearlines)
        yearlines <- c()
    }
    initline <- parlines[grep("Initial", file[parlines - 1])]
    finalline <- parlines[grep("Final", file[parlines - 1])]
    step <- rep(NA, length(parlines))
    year <- rep(NA, length(parlines))
    if (length(steplines) > 0) {
        tmp <- strsplit(file[steplines - 1], split = " ")
        step[match(steplines, parlines)] <- unlist(lapply(tmp, function(x) x[7]))
        tmp2 <- unlist(lapply(tmp, function(x) x[10]))
        year[match(steplines, parlines)] <- substring(tmp2, 1, nchar(tmp2) - 1)
    }
    else if (length(yearlines) > 0) {
        tmp <- strsplit(file[yearlines - 1], split = " ")
        tmp2 <- unlist(lapply(tmp, function(x) x[7]))
        year[match(yearlines, parlines)] <- substring(tmp2, 1, nchar(tmp2) - 1)
    }
    out <- list()
    partype <- rep("", length(parlines))
    if (length(initline) != 0) {
        out$initial <- 0
        partype[match(initline, parlines)] <- "initial"
    }
    if (length(finalline) != 0) {
        out$final <- 0
        partype[match(finalline, parlines)] <- "final"
    }
    if (any(!is.na(year))) {
        uyear <- paste(sort(as.numeric(unique(year[!is.na(year)]))))
        if (any(!is.na(step))) {
            for (yr in uyear) out[[yr]] <- list()
            partype[match(steplines, parlines)] <- "step"
        }
        else {
            for (yr in uyear) out[[yr]] <- 0
            partype[match(yearlines, parlines)] <- "year"
        }
    }
    old.warn <- options()$warn
    options(warn = -1)
    tmp <- casal.string.to.vector.of.numbers(file[(parlines[1] +  1)])
    options(warn = old.warn)
    n.partition.characters <- sum(is.na(tmp))
    colnames <- tmp[!is.na(tmp)]
    ncol <- length(colnames)
    SSBlines <- grep("SSBs:", file)
    nrow <- SSBlines[SSBlines > parlines[1]][1] - parlines[1] - 2
    tmp <- matrix(casal.string.to.vector.of.words(file[(parlines[1] + 2):(parlines[1] + nrow + 1)]), nrow, byrow = T)
    rownames <- if (n.partition.characters > 0)
        apply(cbind(tmp[, 1:n.partition.characters]), 1, function(x) paste(x, collapse = "."))
    else ""
    for (indx in 1:length(parlines)) {
        tmp <- matrix(casal.string.to.vector.of.words(file[(parlines[indx] + 2):(parlines[indx] + nrow + 1)]), nrow, byrow = T)
        if (n.partition.characters > 0)
            tmp <- tmp[, -(1:n.partition.characters)]
        mat <- matrix(as.numeric(tmp), nrow, dimnames = list(rownames,
            colnames))
        if (partype[indx] == "initial" | partype[indx] == "final")
            out[[partype[indx]]] <- mat
        else if (partype[indx] == "year")
            out[[year[indx]]] <- mat
        else out[[year[indx]]][[step[indx]]] <- mat
    }
    out
}
