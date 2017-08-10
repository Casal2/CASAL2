#' @title extract Tabular function for readin in Casal2 output that has been generated from a -r, -e, -f, -p run mode with the --tabular.  
#'
#' @description
#' An extract function that reads Casal2 output that are produced from a '-r' or '-e' or '-f' or '-p' model run. This funciton
#' also create a 'casal2TAB' class which can be used in plotting and summary functions. See the casal2 manual for more information.
#'
#' @author Craig Marsh
#' @param file the name of the input file containing model output to extract
#' @param path Optionally, the path to the file
#' @return a 'casal2TAB' object which is essentially a list, that can be integrated using the str() function.
#' @export
#'
#'
"extract.tabular" <-
function (file, path = "") {
  set.class <- function(object,new.class){
    # use in the form
    #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class,attributes(object)$class[attributes(object)$class!=new.class])
    object
  }

  filename = make.filename(path = path, file = file)
  file = convert.to.lines(filename)
  
  ## Check this isn't a tabular report by looking at the Call:
  if (!grepl(pattern = "--tabular", x = file[2]))
   stop("This model was NOT run with the command '--tabular', please use the extract.mpd() function to import model runs without --tabular")  
  
  temp = get.lines(file, starts.with = "\\*",fixed=F)
  if (length(temp) != 0) {
    if(!is.even(length(temp))) {
        ## find the report which doesn't have a *end
        nd_element = seq(2, length(temp), by = 2)
        ndx = which(temp[nd_element] != "*end")[1]
        stop (Paste("Each report section must begin with '*' and end with '* end'. The report beginning with ", temp[ndx], " did not have a trailing *end"))
    }
    temp = temp[is.odd(1:length(temp))]
    counter = length(temp)

    result = list()
    for (i in 1:counter) {
        header = string.to.vector.of.words( temp[i])
        label = substring(header[1],2)
        type = substring(header[2],2,nchar(header[2])-1)
        report = get.lines(file, clip.to = temp[i])
        report = get.lines(report,clip.from = "*end")
        report = make.list(report)
        report$type = type
        
        result[[label]] = report

        file = get.lines(file, clip.to = "*end")

    } 
    result<-set.class(result,"casal2TAB")
    result
  } else {
    warning("File is empty, no reports found")
  }
}


  
