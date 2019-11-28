## This is a list of functions used to post process CASAL2 output from the extract function

## The first function will pull out the components that make up the 
## objective function that is the same as casal
## @param extract_list = an object of class list that is the result of 
#   extract() from the CASAL2 library
## @param label = the label of your objective_function report.type
split_obj = function(extract_list,label) {
  if(class(extract_list) != "casal2MPD")
    warning("extract_list should be of class 'list'");

  if (!(label %in% names(extract_list)))
    stop("Your label is not in the list check names of extract_list");
  multi_run = FALSE
  n_runs = 1
  if (any(names(get(label,extract_list)) == "type")) {
    multi_run = TRUE
    n_runs = length(names(get(label,extract_list)))
  }
  obj = NULL
  if(multi_run) {
    obj = get(label,extract_list)$values;
  } else {
    obj = get(label,extract_list)$"1"$values;
  }
  
  elements = names(obj);
  values = as.numeric(obj);
  nam = vector();
  value = vector();
  list_index = 1;
  check_labels_for_double_accounting = c();
  
  ## Observations will be broken by obs -> label - year
  ## prior will be prior -> label -> parameter
  ## penalty will be penalty -> label
  ## We want to break down by class and label so find all the unique comninations of that
  obs = elements[grep("observation->" , elements)];
  prior = elements[grep("prior->" , elements)]; 
  penalty = elements[grep("penalty->" , elements)]; 
  jacobian = elements[grep("jacobian->" , elements)]; 
  
  store_vec = vector();
  if (length(obs) > 0) {
    nd = strsplit(obs, "->");
    for (i in 1:length(obs))
      store_vec[i] = strsplit(nd[[i]], "-")[[2]][1];
    
    unique_store_vec = unique(store_vec);
    for (k in 1:length(unique_store_vec)) {
      #ndx = which(elements %in% paste("observation", store_vec[k], sep = "->"))
      ndx = grep(paste("observation", unique_store_vec[k], sep = "->"),elements);
      nam[list_index]= unique_store_vec[k];
      ## check no double counting
      if (any(elements[ndx] %in% check_labels_for_double_accounting)) {
        # do something
        ndx = ndx[-c(which(elements[ndx] %in% check_labels_for_double_accounting))]
      }
      value[list_index] = sum(values[ndx]);
      check_labels_for_double_accounting = c(check_labels_for_double_accounting, elements[ndx])
      list_index = list_index + 1;
    }
  }
  
  ## these will be taken by the prior-> call
  #store_vec = vector();
  #if (length(add_prior) > 0) {
  #  nd = strsplit(add_prior, "->");
  #  for (i in 1:length(add_prior))
  #    store_vec[i] = strsplit(nd[[i]], "-")[[2]][1];
  #  store_vec = unique(store_vec);
    
  #  for (k in 1:length(store_vec)) {
  #    ndx = grep(paste("additional_prior", store_vec[k], sep = "->"),elements);
  #    nam[list_index]= store_vec[k];
  #    value[list_index] = sum(values[ndx]);
  #    list_index = list_index + 1;
  #  }
  #}
  store_vec = vector();
  if (length(jacobian) > 0) {
    nd = strsplit(jacobian, "->");
    for (i in 1:length(jacobian))
      store_vec[i] = strsplit(nd[[i]], "-")[[2]][1];
    store_vec = unique(store_vec);
    
    for (k in 1:length(store_vec)) {
      ndx = grep(paste("jacobian", store_vec[k], sep = "->"),elements);
      nam[list_index]= store_vec[k];
      if (any(elements[ndx] %in% check_labels_for_double_accounting)) {
        # do something
        ndx = ndx[-c(which(elements[ndx] %in% check_labels_for_double_accounting))]
      }
      check_labels_for_double_accounting = c(check_labels_for_double_accounting, elements[ndx])
      
      value[list_index] = sum(values[ndx]);
      list_index = list_index + 1;
    }
  }
  store_vec = vector();  
  if (length(prior) > 0) {
  nd = strsplit(prior, "->");
  for (i in 1:length(prior)) #{
    store_vec[i] = strsplit(nd[[i]], "-")[[2]][1];
    #cat(i, " ", store_vec[i],"\n")
  #}
    store_vec = unique(store_vec);

  for (k in 1:length(store_vec)) {
    ndx = grep(paste("prior",store_vec[k],sep="->"),elements);
    nam[list_index]= store_vec[k];
    value[list_index] = sum(values[ndx]);
    list_index = list_index + 1;
    }
  }
  store_vec = vector();  
  if (length(penalty) > 0) {
    nd = strsplit(penalty, "->");
  for (i in 1:length(penalty))
    store_vec[i] = strsplit(nd[[i]], "-")[[2]][1];
  store_vec = unique(store_vec);

  # "penalty->CatchMustBeTaken(Instant_mortality)" 
  #ndx = grep(pattern = "penalty->CatchMustBeTaken[Instant_mortality]", elements)
  for (k in 1:length(store_vec)) {
    ndx = which(elements %in% paste("penalty",store_vec[k], sep="->"))
    if (any(elements[ndx] %in% check_labels_for_double_accounting)) {
      # do something
      ndx = ndx[-c(which(elements[ndx] %in% check_labels_for_double_accounting))]
    }
    check_labels_for_double_accounting = c(check_labels_for_double_accounting, elements[ndx])
    
    nam[list_index]= store_vec[k];
    value[list_index] = sum(values[ndx]);
    list_index = list_index + 1;
    }
  } 
  if(abs(sum(value) - values[length(values)]) > 0.001)
    warning("You may have missed a component of the objective function difference between total and sum > 0.001")
  nam[list_index] = "Total Objective";
  value[list_index] = values[length(values)];
  ## Convert this to a dataframe because they are nicer to deal with
  
  return(data.frame(Label = nam, Value = value))
}