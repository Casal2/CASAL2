# $Id: casal.is.whole.number.R 1747 2007-11-12 21:01:38Z adunn $
"casal.is.whole.number"<-
function(string)
{
# this function is very dicey - works in its intended context but ...
  for(i in 1:nchar(string)) {
    digit <- substring(string, i, i)
    if(!casal.isin(digit, as.character(0:9))) {
      return(F)
    }
  }
  return(T)
}
