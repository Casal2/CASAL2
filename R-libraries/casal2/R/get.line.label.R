#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"get.line.label" <-
function (line) {
        line = string.to.vector.of.words(line)
        if (length(line)==2 && substr(line[2],1,1)=="{" && substr(line[2],nchar(line[2]),nchar(line[2]))=="}") {
            label = line[1]
        } else if (substr(line[1],nchar(line[1]),nchar(line[1]))==":")
            type = substr(line[1],1,nchar(line[1])-1)
        else label=""
}
