#' Utility extract function
#'
#' @author Dan Fu
#' @param file the name of the input file containing model output to extract
#' @param path Optionally, the path to the file
#' @param validate Optionally, validate the structure of each element in the report section
#' @param conversion Optionally, where possible, convert elements from list_element and data.frame into numeric
#' @export
#'
"extract" <-
function (file, path = "") {


  filename = make.filename(path = path, file = file)
  file = convert.to.lines(filename)
  temp = get.lines(file, starts.with = "\\*",fixed=F)
  if(!is.even(length(temp))) {
      stop ("Each report section must begin with '*' and end with '* end'")
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
      if (!is.in(label, names(result))) {       
         result[[label]] = list()
         result[[label]][["1"]] = report
      } else {
           result[[label]][[as.character(length(result[[label]])+1)]] = report
      }
      file = get.lines(file, clip.to = "*end")
      
  }
  result


}


  
