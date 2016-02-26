make.filename <- function(file,path="")
{
  if(path != "") {
    plc<-substring(path, nchar(path))
    if(!(plc== "\\" | plc=="/")) path <- paste(path, "/", sep = "")
  }
  filename<-paste(path, file, sep = "")
  return(filename)
}

