# $Id: casal.make.filename.R 1834 2007-12-02 21:32:26Z adunn $
"casal.make.filename"<-
function(file,path="")
{
  if(path != "") {
    plc<-substring(path, nchar(path))
    if(!(plc== "\\" | plc=="/")) path <- paste(path, "/", sep = "")
  }
  filename<-paste(path, file, sep = "")
  return(filename)
}
