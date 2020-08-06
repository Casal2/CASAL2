"Boxplot"<-
function(x,...,name=names(x))
{
  old.par<-par("tcl")
  par("tcl"=0)
  if(missing(name) || is.null(name)) name<-F
  centers<-boxplot(x, boxcol = -1, medlwd = 2, staplewex = 0, staplehex = 0, outpch = 1, outline = F, whisklty = 1, medcol = 1, range = 0, names=name,...)
  par("tcl"=old.par)
  invisible(1:length(x))
}
