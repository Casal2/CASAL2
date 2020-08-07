# $Id: extract.header.R 1828 2007-11-30 01:16:07Z adunn $
"extract.header"<-
function(file,path)
{
  header <- list()
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  header$call <- casal.remove.first.words(casal.get.lines(file,starts.with = "Call:"), 1)
  header$date <- casal.remove.first.words(casal.get.lines(file,starts.with = "Date:"), 1)
  header$version <- casal.get.lines(file, starts.with = "v")
  header$user <- casal.remove.first.words(casal.get.lines(file,starts.with = "User name:"), 2)
  header$machine <- casal.remove.first.words(casal.get.lines(file,starts.with = "Machine name:"), 2)
  return(header)
}
