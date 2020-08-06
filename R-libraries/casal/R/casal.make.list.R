# $Id: casal.make.list.R 1747 2007-11-12 21:01:38Z adunn $
"casal.make.list"<-
function(lines)
{
  result <- list()
  for(i in 1:length(lines)) {
    label <- casal.string.to.vector.of.words(lines[i])[1]
    contents <- casal.string.to.vector.of.numbers(casal.remove.first.words(lines[i],1))
    result[[label]] <- contents
  }
  result
}
