# $Id: casal.version.R 2329 2008-10-30 22:59:10Z adunn $
"casal.version"<-
function()
{
  cat(paste("CASAL R package ",casal.Rpackage.version(),"\n  for use with CASAL ",casal.binary.version(),"\n",sep=""))
}
