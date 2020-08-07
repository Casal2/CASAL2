# $Id: casal.convert.to.lines.R 1747 2007-11-12 21:01:38Z adunn $
"casal.convert.to.lines"<-
function(filename)
{
# note blank lines are not included - I would rather they were but am not sure how
  scan(filename, what = "", sep = "\n")
}
