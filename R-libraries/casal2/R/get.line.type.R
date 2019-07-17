#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"get.line.type" <-
function (line) {
    type =""
    if(line=="*end") 
        type = "*end"
    else if (line =="end {L}")
        type = "end {L}"
    else {
        line = string.to.vector.of.words(line)
        if (length(line)==2 && substr(line[2],1,1)=="{" && substr(line[2],nchar(line[2]),nchar(line[2]))=="}") {
            type =substr(line[2],2,nchar(line[2])-1) 
            if (type !="L" && type !="v" && type !="c" && type !="C" && type !="d" && type != "m" && type != "s" && type != "M" && type != "d_r")
                stop("type from {} must be one of 'L','v','c','C','d','m', 'M', 's'!, 'd_r'")
        } else if (substr(line[1],nchar(line[1]),nchar(line[1]))==":")
            type = "L_E"
    }
    type
}
