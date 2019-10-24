# $Id: casal.pos.match.R 1747 2007-11-12 21:01:38Z adunn $
"casal.pos.match"<-
function(vector, regexp)
{
  min((1:length(vector))[casal.regexpr(regexp, vector) > 0])
}
