#' @title extract Tabular function for readin in Casal2 output that has been generated from a -r, -e, -f, -p run mode with the --tabular.  
#'
#' @description
#' An extract function that reads Casal2 output that are produced from a '-r' or '-e' or '-f' or '-p' model run. This funciton
#' also create a 'casal2TAB' class which can be used in plotting and summary functions. See the casal2 manual for more information.
#'
#' @author Craig Marsh
#' @param file the name of the input file containing model output to extract
#' @param path Optionally, the path to the file
#' @param fileEncoding Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.
#' @param quiet suppress print or cat statements to screen.
#' @return a 'casal2TAB' object which is essentially a list, that can be integrated using the str() function.
#' @export
#'
#'
"extract.tabular" <-
function(file, path = "", fileEncoding = "", quiet = FALSE) {
  set.class <- function(object, new.class) {
    # use in the form
    #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class, attributes(object)$class[attributes(object)$class != new.class])
    object
  }

  filename = make.filename(path = path, file = file)
  file = convert.to.lines(filename, fileEncoding = fileEncoding, quiet = quiet)
  original_file = file;
  end_file_locations = which("*end" == file);

  ## Check this isn't a tabular report by looking at the Call:
  if (!grepl(pattern = "--tabular", x = file[2]))
    stop("This model was NOT run with the command '--tabular'. Please use the extract.mpd() function to import model runs without --tabular")

  temp = get.lines(file, starts.with = "\\*", fixed = F)
  if (length(temp) != 0) {
    if (!is.even(length(temp))) {
      ## find the report which doesn't have a *end
      nd_element = seq(2, length(temp), by = 2)
      ndx = which(temp[nd_element] != "*end")[1]
      stop(paste0("Each report section must begin with '*' and end with '*end'. The report beginning with ", temp[ndx], " does not have a trailing '*end'."))
    }
    temp = temp[is.odd(1:length(temp))]
    counter = length(temp)

    result = list()
    for (i in 1:counter) {
      header = split.header(temp[i])
      label = header[1]
      type = header[2]
      report = get.lines(file, clip.to = temp[i])
      report = get.lines(report, clip.from = "*end")
      print(paste0("loading report '", label, "'"))
      if (type == "warnings_encounted") {
        warning("Found a warning in the report. Skipping that report.")
        next;
      }
      ## report = make.list(report)
      temp_result = list()
      start_ndx = which(temp[i] == original_file) + 1;
      line_no = 1;
      while (line_no <= length(report)) {
        current_line = report[line_no]
        report_type = get.line.type(current_line)
        report_label = get.line.label(current_line)

        if (report_type == "L_E") {
          temp_result[[report_label]] = make.list_element(current_line)
          line_no = line_no + 1
          start_ndx = start_ndx + 1;
        } else if (report_type == "d") {
          header = string.to.vector.of.words(original_file[start_ndx + 1])
          ##header1 = read.table(file = filename, skip = (start_ndx + 1 + (i - 1)), nrows = 1, stringsAsFactors = FALSE,  sep = " ", header = F,strip.white=FALSE, fill = FALSE)
          Data = read.table(file = filename, skip = (start_ndx + 2 + (i - 1)), nrows = (end_file_locations[i] - start_ndx - 3), stringsAsFactors = FALSE, sep = " ", header = F, strip.white = FALSE, fill = FALSE, fileEncoding = fileEncoding)
          Data = Data[, - ncol(Data)]
          colnames(Data) = header
          temp_result$values = Data
          line_no = length(report) + 1
        } else {
          warning(paste0("Cannot parse tabular report of type '", report_type, "'"))
        }
      }
      temp_result$type = type
      result[[label]] = temp_result
      file = get.lines(file, clip.to = "*end")
    }
    result <- set.class(result, "casal2TAB")
    return(result)
  } else {
    warning("File is empty, no reports found")
  }
}

