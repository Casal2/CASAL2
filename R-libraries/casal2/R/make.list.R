#' Utility extract function
#'
#' @author Dan Fu
#' @keywords internal
#'
"make.list" <-
function(lines) {
    result=list()
    line_no = 1;
    while (line_no <= length(lines)) {
        current_line = lines[line_no]
        type = get.line.type(current_line)
        label = get.line.label(current_line)
        if (label == "type")
          label = 
        if(type=="L") { # List
               next_no = line_no + 1
               test = 1
                while (test  > 0 & next_no <=  length(lines)) {
                    next_line = lines[next_no]
                    next_type = get.line.type(next_line)
                    if(next_type=="L")
                        test = test + 1
                     if(next_type == "end {L}")
                        test = test - 1
                   if(test == 0)                       break
                    next_no = next_no + 1
                }
                if (test == 0) {
                   if (next_no-line_no > 1) 
                        result[[label]] = make.list(lines[(line_no+1):(next_no-1)])
                     else 
                        result[[label]] = list()
                } else {
                    stop(paste("Cannot find a matched 'end (list)' for",current_line))
                }
                line_no = next_no + 1
            } else if (type == "v") { #vector
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.vector(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = vector()
                  line_no  = next_no
                
            } else if (type == "s") { #string vector
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = as.character(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = vector()
                  line_no  = next_no
                
            } else if (type=="c") { #complete vector
                next_no = line_no + 1
                if (next_no <= length(lines)) {
                    next_line = lines[next_no]
                    next_type = get.line.type(next_line)
                    if(next_type == "")
                        next_no = next_no + 1
                }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.complete_vector(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = vector()
                  line_no  = next_no   
                
            } else if (type=="C") { #named complete vector
                next_no = line_no + 1
                for (i in 1:2) {
                    if (next_no <= length(lines)) {
                        next_line = lines[next_no]
                        next_type = get.line.type(next_line)
                        if(next_type != "")
                            break
                        next_no = next_no + 1
                    }
                }
                if (next_no-line_no > 1)
                    result[[label]] = make.named_complete_vector(lines[(line_no+1):(next_no-1)])
                else if (next_no-curent_no == 1)
                     result[[label]] = vector()
                line_no  = next_no

            } else if(type =="d") { #data.frame
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.data.frame(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = data.frame()
                  line_no  = next_no     

            } else if(type =="d_r") { #data.frame
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.data_frame_with_row_labs(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = data.frame()
                  line_no  = next_no     

            } else if (type =="m") { #matrix
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.matrix(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = data.frame()
                  line_no  = next_no
      

            } else if (type =="M") { # matrix with header
                  next_no = line_no + 1
                  while (next_no <= length(lines)) {
                      next_line = lines[next_no]
                      next_type = get.line.type(next_line)
                      if(next_type != "")
                          break
                      next_no = next_no + 1
                  }
                  if (next_no-line_no > 1) 
                        result[[label]] = make.matrix_with_header(lines[(line_no+1):(next_no-1)])
                  else 
                        result[[label]] = data.frame()
                  line_no  = next_no
      

            } else if (type == "L_E") {
                result[[label]]= make.list_element(current_line)
                line_no = line_no + 1
            } else {
                if (type !="")
                    stop(paste("Unknown type",type,"from line ",current_line))
                else
                    stop(paste("Expecting type  from ",current_line)) 
            }
        }
    result
}
