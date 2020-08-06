# $Id: casal.recodevector.R 1747 2007-11-12 21:01:38Z adunn $
"casal.recodevector"<-
function(in.data, from.vals, to.vals)
{
  data <- in.data
  for(count in 1:length(from.vals)) {
    data[in.data == from.vals[count]] <- to.vals[count]
  }
  return(data)
}
