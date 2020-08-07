# $Id: casal.string.to.vector.of.numbers.R 1747 2007-11-12 21:01:38Z adunn $
"casal.string.to.vector.of.numbers"<-
function(string)
{
  as.numeric(casal.string.to.vector.of.words(string))
}
