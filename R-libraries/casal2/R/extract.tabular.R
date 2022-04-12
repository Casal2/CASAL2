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

# 2021-08-30
# {d} -> {dataframe}
# {d_r} -> {dataframe_with_row_labels}
# {m} -> {matrix}
# {L} -> {list}
# {L_E} -> {end_list} ?
# end {L} -> {end_list}
# {v} -> {vector}
# {s} -> {string}
# {c} and {C} not used

"extract.tabular" <-
function(file, path = "", fileEncoding = "", quiet = FALSE) {
  set.class <- function(object, new.class) {
    # use in the form
    #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class, attributes(object)$class[attributes(object)$class != new.class])
    object
  }

  filename = make.filename(path = path, file = file)
  file_all_white_space = scan(filename, what = "", sep = "\n", fileEncoding = fileEncoding, quiet = quiet, blank.lines.skip = F)
  file = convert.to.lines(filename, fileEncoding = fileEncoding, quiet = quiet)
  original_file = file;
  end_file_locations = which("*end" == file);

  ## Check this isn't a tabular report by looking at the Call:
  if (!grepl(pattern = "--tabular", x = file[2]) & !grepl(pattern = "-t", x = file[2]))
    stop("This model was NOT run with the command '--tabular'. Please use the extract.mpd() function to import model runs without --tabular or -t")

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
      global_line_counter = which(file_all_white_space == temp[i])

      header = split.header(temp[i])
      label = header[1]
      type = header[2]
      report = get.lines(file, clip.to = temp[i])
      report = get.lines(report, clip.from = "*end")
      if(!quiet)
        print(paste0("loading tabular report '", label, "' of type '", type, "'"))

      if (type == "info") {
        if(!quiet)
          print("Found informational messages in the output. Skipping that report.")
        next;
      }
      if (type == "warnings") {
        if(!quiet)
          print("Found warning messages in the output. Skipping that report.")
        next;
      }

      ## report = make.list(report)
      last_line_for_df = length(report)
      temp_result = list()
      start_ndx = which(temp[i] == original_file) + 1;
      line_no = 1;
      while (line_no <= length(report)) {
        current_line = report[line_no]
        report_type = get.line.type(current_line)
        report_label = get.line.label(current_line)
        if (report_type == "L_E") {
          if(report_label == "type")
            report_label = "sub_type"
          temp_result[[report_label]] = make.list_element(current_line)
          line_no = line_no + 1
          global_line_counter = global_line_counter + 1
          start_ndx = start_ndx + 1;
          last_line_for_df = last_line_for_df - 1
        } else if (report_type == "dataframe") {
          # header = string.to.vector.of.words(original_file[start_ndx + 1])
          ###header1 = read.table(file = filename, skip = (start_ndx + 1 + (i - 1)), nrows = 1, stringsAsFactors = FALSE,  sep = " ", header = F,strip.white=FALSE, fill = FALSE)
          ## skip pass "values {dataframe}" line
          global_line_counter = global_line_counter + 1

          #file_all_white_space[global_line_counter + 1]
          #file_all_white_space[global_line_counter + 2]
          #file_all_white_space[global_line_counter + 1 + length(report) - 2]
          #print(file_all_white_space[global_line_counter])
          if(file_all_white_space[global_line_counter + 1] == "*end") {
            ## some reports are empty, particularly for processess
            line_no = length(report) + 1
            next;
          }
          Data = read.table(file = filename, skip = global_line_counter + 1, nrows = (last_line_for_df - 2), stringsAsFactors = FALSE, sep = " ", header = F, strip.white = FALSE, fill = FALSE, fileEncoding = fileEncoding)
          ## deal with trailing white space that is read in as a column of NAs
          if(all(is.na(Data[,ncol(Data)])))
            Data = Data[,-ncol(Data)]
          header = string.to.vector.of.words(file_all_white_space[global_line_counter + 1])
          # Data = Data[, - ncol(Data)]
          colnames(Data) = header
          # temp_result$values = Data
          temp_result$values = Data
          line_no = length(report) + 1
          global_line_counter = length(report) + 1

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

