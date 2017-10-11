#' apply.dataweighting.to.csl2 reads in your observation.csl2 file and applys a reweighting factor and saves as a new .csl2 file.
#'
#' This function reads in your observation.csl2 file and applys a reweighting factor and saves as a new .csl2 file, pretty much just trying to 
#' automate the dataweighting process. The problem with this method, is that it strips out all the comments from the original observation.csl2 file.
#'
#' @author Craig Marsh
#' @param Observation_csl2_file the name of the configuration file containing the @observation blocks.
#' @param Observation_label the label of the observation you want to apply the weighting to.
#' @param weighting_factor - the multiplier for the effective sample sizes for the compositional data.
#' @param Path path to the directory containing the Observation.csl2 file.
#' @param Observation_out_filename the file name of the Observation.csl2 file to be saved after applying weighting.
#' @param fileEncoding Optional, allows the R-library to read in files that have been encoded in alternative UTF formats, see the manual for the error message that would indicate when to use this switch.
#' @return a file named 'Observation_out_filename.out' in the path directory
#' @export
#'

'apply.dataweighting.to.csl2' = function(Path = "",weighting_factor, Observation_csl2_file = "Observations.csl2",Observation_label = "", Observation_out_filename = "Observation.csl2.0", fileEncoding = "") {
  observation_config = extract.csl2.file(file = Observation_csl2_file, path = Path);
  ## pull out the estimates only, they are the only components interested in at the moment
  observation_ndx = grepl(pattern = "observation\\[", x = names(observation_config))  
  observation_block_labels = names(observation_config)[observation_ndx]
  ## Check Observation_label exists
  if (!any(grepl(pattern = Observation_label, x = observation_block_labels))) {
    stop(Paste("could not find the @observation block labelled ",Observation_label, " the observations found are ", paste(observation_block_labels, collapse = ", ")))
  }
  this_obs = observation_config[observation_block_labels][grepl(pattern = Observation_label, x = observation_block_labels)][[1]]
  error_labels = names(this_obs$Table$error_values)
  this_obs$Table$error_values = as.character(round(as.numeric(this_obs$Table$error_values) * weighting_factor,3))
  names(this_obs$Table$error_values) = error_labels
  ## now save it back
  observation_config[observation_block_labels][grepl(pattern = Observation_label, x = observation_block_labels)][[1]] = this_obs
  ## Write the new Observation.csl2
  write.csl2.file(object = observation_config, path = Path, file = Observation_out_filename)
}