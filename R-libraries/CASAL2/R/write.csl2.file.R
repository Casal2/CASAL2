#' Model configuration write function
#'
#' This function will write a Casal2 configuration file based on a list object in R. Ususally this function will be used once a model has been read into R using extract.csl2.file and modified. This function will then print our the configuration to a new file where it can be re run into Casal2
#'
#' @author Craig Marsh
#' @param object An R list object that follows the same structure that extract.csl2.file would produce
#' @param file Optionally, the file name
#' @param path Optionally, the path to ouput the file
#' @export
#'
"write.csl2.file" <-
    function(object, file, path = "") {
    if (missing(path)) 
        path <- ""
    filename = make.filename(path = path, file = file)
    ## initialise file
    cat("", file = filename, sep = "", fill = F, labels = NULL, append = F)
    ## these are blocks that have no label's associated with them
    exception_commands = c("model","categories")

    for (i in 1:length(object)) {
      Command = names(object)[i];
      if (Command %in% exception_commands) {
        Comment = paste("@",Command,sep ="")
        cat(Comment, file = filename, sep = "", fill = F, labels = NULL, append = T)
        ## drop next line
        cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
      } else {
        ## the block has a label and we need to deal with it
        index1 = regexpr("\\[", Command);
        index2 = regexpr("\\]", Command);        
        Comment = paste("@",substr(Command,1,index1 - 1)," ",substr(Command,index1 + 1,index2 - 1) ,sep ="")
        cat(Comment, file = filename, sep = "", fill = F, labels = NULL, append = T)  
        cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
      }
      subcommands = names(object[[i]]);
      for (j in 1:length(subcommands)) {
        if (subcommands[j] == "Table") {
          ## find out how many tables are in teh block
          tables = names(object[[i]][[j]])
          for ( k in 1:length(tables)) {
            ## initial line
            initial = paste("table", tables[k]);
            cat(initial, file = filename, sep = "", fill = F, labels = NULL, append = T)  
            cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
            ## Add colnames
            this_table = eval(parse(text= paste("object[[i]][[j]]$",tables[k],sep="")));
            header = paste(names(this_table), collapse = " ");
            cat(header, file = filename, sep = "", fill = F, labels = NULL, append = T)  
            cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
            Nrows = length(this_table[[1]]);
            for (rows in 1:Nrows) {
              values = vector();
              for (cols in 1:length(names(this_table))) {
                values[cols] = this_table[[cols]][rows];
              }
              Line = paste(values,collapse = " ");
              cat(Line, file = filename, sep = "", fill = F, labels = NULL, append = T)  
              cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)              
            }
            cat("end_table", file = filename, sep = "", fill = F, labels = NULL, append = T)  
            cat("\n\n\n", file = filename, sep = "", fill = F, labels = NULL, append = T)                 
          }
          cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
        } else {
          ## if its a scalar or a vector we can jsut paste them back to together
          if (!is.null(names(object[[i]][[j]]))) {
            ## We are dealing with a vector
            values = eval(parse(text= paste("object[[i]][[j]]$",names(object[[i]][[j]]),sep="")));
            Comment = paste(subcommands[j] , paste(values,collapse = " "), sep = " ")
            cat(Comment, file = filename, sep = "", fill = F, labels = NULL, append = T)
            cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)            
          } else {
            ## its a scalar, easily dealt with 
            Comment = paste(subcommands[j] ," ",paste(object[[i]][[j]], sep = "", collapse = " ") ,sep ="")
            cat(Comment, file = filename, sep = "", fill = F, labels = NULL, append = T)
            cat("\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
          }                        
        }
      }
  ## add a double space between blocks for ease of reading  
  cat("\n\n", file = filename, sep = "", fill = F, labels = NULL, append = T)
  }
}