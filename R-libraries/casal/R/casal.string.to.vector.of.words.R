# $Id: casal.string.to.vector.of.words.R 1747 2007-11-12 21:01:38Z adunn $
"casal.string.to.vector.of.words"<-
function(string)
{
  temp <- casal.unpaste(string, sep = " ")
  return(temp[temp != ""])
}
