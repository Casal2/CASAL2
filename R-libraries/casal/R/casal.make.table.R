# $Id: casal.make.table.R 1747 2007-11-12 21:01:38Z adunn $
"casal.make.table"<-
function(lines)
{
  columns <- casal.string.to.vector.of.words(lines[1])
  if(length(lines) < 2) return(NA)
  data <- matrix(0, length(lines) - 1, length(columns))
  for(i in 2:length(lines)) {
    data[i - 1,  ] <- casal.string.to.vector.of.numbers(lines[i])
  }
  data <- data.frame(data)
  names(data) <- columns
  data
}
