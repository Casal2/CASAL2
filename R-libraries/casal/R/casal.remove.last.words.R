# $Id: casal.remove.last.words.R 1747 2007-11-12 21:01:38Z adunn $
"casal.remove.last.words"<-
function(string, words = 1)
{
  temp <- casal.unpaste(string, sep = " ")
  to.drop <- length(temp) - (0:(words - 1))
  paste(unlist(temp[ - to.drop]), collapse = " ")
}
