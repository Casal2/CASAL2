# $Id: extract.quantities.from.table.R 1828 2007-11-30 01:16:07Z adunn $
"extract.quantities.from.table"<-
function(file,path)
{
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  lines <- casal.convert.to.lines(filename)
  lines <- casal.get.lines(lines, clip.to.match = "Quantity values")
  quantities <- casal.make.table(lines)
  return(quantities)
}
