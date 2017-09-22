#' @title extract.mcmc function for casal2 output
#'
#' @description
#' An extract function that reads objective and sample output that are produced from a 'casal2 -m' model run. This funciton
#' also create a 'casal2.mcmc' class which can be used in plotting and summary functions.
#'
#' @author C. Marsh
#' @param samples.file <string> the name of the input file containing the samples.file output by casal2
#' @param objectives.file <string> the name of the input file containing the objectives.file output by casal2
#' @param path Optional<string>, the path to the file
#' @param fileEncoding Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.
#' @param return_covariance Optional<bool>, Whether you want to extract the covariance matrix with the mcmc object?
#' @export
#' @return a 'casal2MCMC' that can be integrated using the str() function.
#'
"extract.mcmc" <-
function (samples.file = "mcmc_samples.out.0",objectives.file = "mcmc_objectives.out.0", path = "", return_covariance = F, fileEncoding = "") {
  set.class <- function(object,new.class){
    # use in the form
    #  object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class,attributes(object)$class[attributes(object)$class!=new.class])
    object
  }
  if(missing(path)) 
    path = ""
    
  obj_filename = make.filename(path = path, file =  objectives.file)
  sample_filename = make.filename(path = path, file = samples.file)
  sample_file = convert.to.lines(sample_filename, fileEncoding = fileEncoding)
  temp = get.lines(sample_file, starts.with = "\\*",fixed=F)
  is.samples = FALSE
  if (temp != "*mcmc (mcmc_sample)") {
    stop(Paste("expected the header of " , samples.file , " to read *mcmc (mcmc_sample), please check this is mcmc output generated from CASAL2"))
  }
  #########################
  ## Deal with the samples.
  #########################
  if (length(sample_file) < 3)
    stop(Paste("No parameter values found in samples file ", samples.file));
  header = string.to.vector.of.words(sample_file[2]);
  ## check that the last row has the same number of columns as the first column. Sometimes MCMC quit early and the report is interrupted

  first_params = string.to.vector.of.words(sample_file[3]);
  last_params = string.to.vector.of.words(sample_file[length(sample_file)]);
  if (length(first_params) != length(last_params)) {
    ## remove the last row
    sample_file = sample_file[-length(sample_file)]
  }
  ## create a data.frame  
  Dataframe = read.table(textConnection(sample_file[3:length(sample_file)]))
  colnames(Dataframe) = header
  #########################
  ## Deal with the Objectives
  #########################  
  objective_file = convert.to.lines(obj_filename, fileEncoding = fileEncoding);
  covar = get.lines(objective_file, starts.with = "starting_covariance_matrix",fixed=F);
  object = get.lines(objective_file, starts.with = "samples",fixed=F);
  covar_lines = get.lines(objective_file, clip.to = covar);
  covar_lines = get.lines(covar_lines, clip.from = object);
  ## build covariance matrix
  columns <- string.to.vector.of.words(covar_lines[1])
  covar_mat <- matrix(0, length(columns), length(columns))
  for (i in 2:length(covar_lines)) {
      line = string.to.vector.of.numbers(covar_lines[i])
      if (length(line) != length(columns)) {
          stop(paste(covar_lines[i], "is not the same length as", 
              covar_lines[1]))
      }
      covar_mat[i - 1, ] <- line
  }
  dimnames(covar_mat) = list(columns,columns)
  
  ## now pull out the objectives table
  object_lines = get.lines(objective_file, clip.to = object);
  
  if (length(object_lines) > (nrow(Dataframe) + 1)) { # + 1 to take into account the header
    # assume there is a one to one relationship between objecives and samples
    object_lines = object_lines[1:(nrow(Dataframe) + 1)]
  } else if (length(object_lines) <= nrow(Dataframe)) {
    stop("less objectives rows than samples, this is not allowed, please check output, this function expects there to be the number of objective print outs as samples")
  }
  obj_header = string.to.vector.of.words(object_lines[1])
  OBJ_Dataframe = read.table(textConnection(object_lines[2:length(object_lines)]))
  colnames(OBJ_Dataframe) = as.character(obj_header);
  if(nrow(OBJ_Dataframe) != nrow(Dataframe)) {
    stop("Something went wrong in the code there are different numbers of samples than objects, you will need to look at the R code.")
  }
  
  #########################
  ## Merge the Two dataframes and print result
  #########################  
  result = cbind(OBJ_Dataframe,Dataframe)
  result = set.class(result,"casal2MCMC")
  if(!return_covariance) {
    result
  } else {
    final_result = list(Data = result, Covariance = covar_mat)
    final_result
  }
}
