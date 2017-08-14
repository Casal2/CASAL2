#' Read in multiple sets of Simualted data for a single observation
#'
#' This function reads in a set of simulated observations generated from Casal2 in simulation mode. These functions read in all the simulated obs as a list, for visualising and summarising in R
#'
#' @author Craig Marsh
#' @param filename the name of simulated obs for an observation. For example if you generated 100 sets of simulated observations named "SubAntarticObs". Casal2 will generate 100 of these with the following extensions SubAntarticObs.001, SubAntarticObs.002, SubAntarticObs.003.,,, SubAntarticObs.100. filename = SubAntarticObs.
#' @param path Optionally, the path to the file, default is current working directory.
#' @export
#'
ReadSimulatedData = function(filename,path = "") {
  if (path == "") 
      Files = list.files()    
  else 
      Files = list.files(path)         
  ## Stolen from casal's extract package
  index = grepl(filename,Files)
  if (!path == "") 
    Files = paste(path,"\\",Files,sep ="")
  Files_to_cycle_through = Files[index]
  store_obs = store_error = list();
  for(i in 1:length(Files_to_cycle_through)) {
    file = convert.to.lines(Files_to_cycle_through[i])
    label = strsplit(get.lines(file, starts.with = "\\@", fixed = F),split = " ")[[1]][2]
    type = strsplit(get.lines(file, starts.with = "type", fixed = F),split = " ")[[1]][2]
    Label = paste("'",type,"[",label,"]'",sep="")
    if (type == "process_removals_by_age" || type == "proportions_at_age") {
      start_table = which(grepl("table obs", file));
      end_table = which(grepl("end_table", file));    
      end_table_ndx = end_table > start_table
      final_table = end_table[end_table_ndx][which.min(end_table[end_table_ndx])]
      error_final_table = end_table[!end_table %in% final_table]
      for(k in (start_table + 1):(final_table - 1)) {
        ## get the obs
        temp <- string.to.vector.of.words(file[k])
        store_obs[[temp[1]]][[as.character(i)]] = as.numeric(temp[-1])
      }
      ## get the errors
      start_table = which(grepl("table error_values", file));
      for(k in (start_table + 1):(error_final_table - 1)) {
        ## get the obs
        temp <- string.to.vector.of.words(file[k])
        store_error[[temp[1]]][[as.character(i)]] = unique(as.numeric(temp[-1]))
      }        
    } else if (type == "abundance" || type == "biomass") {
      years = string.to.vector.of.words(get.lines(file,starts.with = "years", fixed = F))[-1];
      obs = string.to.vector.of.words(get.lines(file,starts.with = "obs", fixed = F))[-1];
      error = string.to.vector.of.words(get.lines(file,starts.with = "error_value", fixed = F))[-1];
      for (k in 1:length(years)) {
        store_obs[[years[k]]][[as.character(i)]] = obs[k]
        store_error[[years[k]]][[as.character(i)]] = error[k]          
      }
    }
  }
  return(list=c("obs" = store_obs,"error" = store_error))
}

