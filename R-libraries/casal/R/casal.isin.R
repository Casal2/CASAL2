# $Id: casal.isin.R 1747 2007-11-12 21:01:38Z adunn $
"casal.isin"<-
function(x, y)
{
  !is.na(match(x, y))
}
