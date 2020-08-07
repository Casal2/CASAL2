# $Id: get.csl.command.R 1750 2007-11-13 00:45:01Z adunn $
"get.csl.command"<-
function(object,command,label,where.subcommand) {
  if(!missing(label) & !missing(where.subcommand)) stop("You cannot specify both the arguments 'label' and 'where.subcommand'\n")
  if(class(object)!="casalCSLfile") stop("The input object is not of class 'casalCSLfile'. See the function 'extract.csl.file' for details\n")
  index<-unlist(lapply(object,function(x) x$command))
  if(missing(command)) return(as.vector(index))
  else ans<-object[command==index]
  if(length(ans)==1) ans<-unlist(ans,recursive=F)
  if(!missing(label)) {
    index<-unlist(lapply(ans,function(x) x$value))
    ans<-ans[label==index]
    if(length(ans)==1) ans<-unlist(ans,recursive=F)
  }
  if(!missing(where.subcommand)) {
    if(length(where.subcommand)!=2) stop("The 'where.subcommand' must be a vector of 2 elements. The first is the name of the 'subcommand', and the second is its label")
    index<-unlist(lapply(ans,function(x,where.subcommand) if(x[[where.subcommand[1]]]==where.subcommand[2]) return(T) else return(F) ,where.subcommand))
    ans<-ans[index]
  }
  return(ans)
}
