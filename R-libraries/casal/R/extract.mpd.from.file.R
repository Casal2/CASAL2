# $Id: extract.mpd.from.file.R 1840 2007-12-05 00:44:36Z adunn $
"extract.mpd.from.file"<-
function(file,path)
{
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  file <- casal.get.lines(file, clip.to.match = "In a format suitable for -i")
  file<-file[1:2]
  return(file)
}
