# $Id: casal.area.R 1754 2007-11-13 04:32:41Z adunn $
"casal.area"<-
function(corners)
{
# Ian Doonan
# Find the area of a polygon
# corners$x,$y corners of polygon
# Area(list(x=c(0,2,3,4,4),y=c(0,2,1,2,0))) -->5
#
  if(length(corners$x) < 3 | length(corners$y) < 3) {
    print("ERROR Area(): need 3 or more points ************")
    return(NULL)
  }
  if(length(corners$x) != length(corners$y)) {
    print("ERROR Area(): points in corners$y not equal to corners$x ************")
    return(NULL)
  }
  TotArea <- 0
  SubArea <- 0
  n <- length(corners$x)
  I0 <- 0
  for(j in c(1:(n))) {
    if(!is.na(corners$y[j]) & !is.na(corners$x[j]) & I0 == 0)
      I0 <- j
  }
  for(j in c(I0:(n))) {
    i <- j
    i1 <- j + 1
    if(i1 > n)
      i1 <- I0
    if(is.na(corners$y[i1]) | is.na(corners$x[i1])) i1 <- I0  # still problems if 2 NA's in a row
    if(!is.na(corners$y[i]) & !is.na(corners$x[i])) {
      SubArea <- SubArea + corners$x[i] * corners$y[i1] - corners$x[i1] * corners$y[i]
    }
    else {
      TotArea <- TotArea + abs(SubArea)
      SubArea <- 0
      I0 <- i1
    }
  }
  TotArea <- (TotArea + abs(SubArea))/2
  return(TotArea)
}
