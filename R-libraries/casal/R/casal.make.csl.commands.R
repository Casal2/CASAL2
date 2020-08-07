# $Id: casal.make.csl.commands.R 2264 2008-09-10 04:53:12Z adunn $
"casal.make.csl.commands"<-
function(file="dictionary.cpp", path)
{
  set.class <- function(object, new.class) {
    # use in the form
    # object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class, attributes(object)$class[attributes(object)$class != new.class])
    object
  }
  if(missing(path)) path=""
  if(path != "" & substring(path, nchar(path) - 1) != "\\") path <- paste(path, "\\", sep = "")
  # create filename and read in file
  filename <- paste(path, file, sep = "")
  res <- casal.convert.to.lines(filename)
  res<-casal.get.lines(res, clip.to.match = "//Start extracting csl.commands", clip.from.match = "//End extracting csl.commands")
  res <- res[substring(res, 1, 11) == "set_command"]
  type<-substring(res,18,casal.regexpr("(",res)-1)
  command<-substring(res,casal.regexpr("(",res)+1,casal.regexpr(",",res)-1)
  ans<-data.frame("command"=as.character(command),"type"=as.character(type),stringsAsFactors=FALSE)
  return(ans)
}
#csl.commands<-casal.make.csl.commands(file="dictionary.cpp",path="c:\\cygwin\\home\\dunn\\CASAL\\src")
#cat("# $Id: casal.make.csl.commands.R 2264 2008-09-10 04:53:12Z adunn $\n",file="c:\\cygwin\\home\\dunn\\CASAL\\R-libraries\\casal\\R\\csl.commands.R")
#dump("csl.commands", file="c:\\cygwin\\home\\dunn\\CASAL\\R-libraries\\casal\\R\\csl.commands.R",append=T,control=NULL)
