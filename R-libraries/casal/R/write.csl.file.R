# $Id: write.csl.file.R 1828 2007-11-30 01:16:07Z adunn $
"write.csl.file"<-
function(object,file,path) {
  # create filename and write file
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  cat("", file=filename, sep="", fill=F, labels=NULL, append=F)
  for(i in 1:length(object)) {
    command<-paste("@",object[[i]]$command," ",paste(object[[i]]$value,collapse=" "),"\n",sep="",collapse="")
    cat(command, file=filename, sep="", fill=F, labels=NULL, append=T)
    subcommands<-object[[i]]
    subcommands<-subcommands[!casal.isin(names(subcommands),c("command","value"))]
    if(length(subcommands)>0) {
      for(j in 1:length(subcommands)) {
        line<-paste(subcommands[[j]],sep="",collapse=" ")
        line<-paste(names(subcommands)[j]," ",line,"\n",sep="")
        cat(line, file=filename, sep="", fill=F, labels=NULL, append=T)
      }
    }
  cat("\n", file=filename, sep="", fill=F, labels=NULL, append=T)
  }
  invisible()
}
