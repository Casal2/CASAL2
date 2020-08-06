"qqDist"<-
function(x, standardise = F, add.median = F, ...)
{
  n<-length(x)
  seq.length<-min(1000,n)
  if(standardise) {
    SEQ<-seq(1,2*n+1,length=seq.length)/2
    U<-qnorm(qbeta(0.975,SEQ,rev(SEQ)))
    L<-qnorm(qbeta(0.025,SEQ,rev(SEQ)))
    if(add.median) M<-qnorm(qbeta(0.5,SEQ,rev(SEQ)))
  } else {
    SD<-sqrt(var(x)*(n+1)/n)
    SEQ<-seq(1,2*n+1,length=seq.length)/2
    U<-mean(x)+SD*qt(qbeta(0.975,SEQ,rev(SEQ)),n-1)
    L<-mean(x)+SD*qt(qbeta(0.025,SEQ,rev(SEQ)),n-1)
    if(add.median) M<-mean(x)+SD*qt(qbeta(0.5,SEQ,rev(SEQ)),n-1)
  }
  X<-qnorm((SEQ-0.25)/(n+0.5))
  qqnorm(x,main="",...)
  lines(X,U,type="l")
  lines(X,L,type="l")
  if(add.median) lines(X,M,type="l")
  invisible()
}
