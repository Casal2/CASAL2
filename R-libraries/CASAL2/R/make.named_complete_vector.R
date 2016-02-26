#' Utility extract function
#'
#' @author Dan Fu
#'
"make.named_complete_vector" <-
function(lines) {
   
   names = string.to.vector.of.words(lines[1])
   data = string.to.vector.of.numbers(lines[2])
   if (length (names) != length(data)) {
       stop (paste(lines[1],"is not the the same length as",lines[2]))
   }
   names(data)=names
   data
}

