
#' @title expand_category_block
#'
#' @description
#' A utility function for expanding short hand syntax in @category blocks in casal2 config files
#'
#' @author Craig Marsh
#' @param categories string of categories
#' @return a vector of strings
#' @keywords internal
#' @examples
#' \dontrun{
#' expand_category_block("stock")
#' expand_category_block("stock.male,female")
#' expand_category_block("stock.male,female.untagged,1990")
#' }
expand_category_block <- function(categories) {
  expanded_labels = vector();
  groups = strsplit(categories, split = ".", fixed = TRUE)[[1]]
  category_list = list()
  for(i in 1:length(groups)) {
    if(grepl(groups[i], pattern = ",")) {
      sub_groups = strsplit(groups[i], split = ",", fixed = TRUE)[[1]]
      category_list[[i]] = sub_groups
    } else {
      category_list[[i]] = groups[i]
    }
  }
  ## now permutate them
  all_perms <- expand.grid(category_list, stringsAsFactors = FALSE)
  new_perms = NULL
  ## need to re-order based on first factor
  i = 1
  for(j in 1:length(category_list[[i]])) {
    ndx = all_perms$Var1 == category_list[[i]][j]
    new_perms = rbind(new_perms, all_perms[ndx,])
  }
  for(i in 1:nrow(new_perms)) {
    expanded_labels = c(expanded_labels, paste0(new_perms[i,], collapse = "."))
  }
  return(expanded_labels)
}

#' @title expand_category_shorthand
#'
#' @description
#' A utility function for expanding short hand syntax of categories used in subcommands throught casal2 config files
#'
#' @author Craig Marsh
#' @param shorthand_categories shorthand values to expand
#' @param reference_categories string of categories expanded from the @category block of a config. derived from expand_category_block
#' @param category_format the format defined in the @categories block
#' @return a vector of strings
#' @keywords internal
#' @examples
#' \dontrun{
#' expand_category_shorthand(shorthand_categories ="format=*.EN.*", reference_categories = c("BOP.EN.male"))
#' expand_category_shorthand(shorthand_categories ="format=*.EN.*", reference_categories = c("BOP.EN.male", "BOP.EN.female"))
#' expand_category_shorthand(shorthand_categories ="format=*.EN.*", reference_categories = c("BOP.EN.male", "BOP.EN.female", "HAGU.EN.male", "HAGU.EN.female"))
#' expand_category_shorthand(shorthand_categories ="format = *.EN.+", reference_categories = c("BOP.EN.male", "BOP.EN.female"))
#' expand_category_shorthand(shorthand_categories ="format = *.EN.+", reference_categories = c("BOP.EN.male", "BOP.EN.female", "HAGU.EN.male", "HAGU.EN.female"))
#' expand_category_shorthand(shorthand_categories ="*", reference_categories = c("BOP.EN.male", "BOP.EN.female", "HAGU.EN.male", "HAGU.EN.female"))
#' expand_category_shorthand(shorthand_categories ="stock=BOP", reference_categories = c("BOP.EN.male", "BOP.EN.female", "HAGU.EN.male", "HAGU.EN.female"), category_format = "stock.area.sex")
#' }
expand_category_shorthand <- function(shorthand_categories, reference_categories, category_format = NULL) {
  expanded_categories = vector();
  formats = NULL
  if(!is.null(category_format)) {
    formats = strsplit(category_format, ".", fixed = T)[[1]]
  }
  broken_ref_vals = Reduce(rbind, strsplit(reference_categories, ".", fixed = T))
  if(is.null(dim(broken_ref_vals))) {
    broken_ref_vals = matrix(broken_ref_vals, nrow =1)
  }
  # strip whitespace
  shorthand_categories_no_space = gsub(" ", "", shorthand_categories, fixed = TRUE)
  expanded_categories = shorthand_categories_no_space
  # if format strip that out
  format_found = grepl(shorthand_categories_no_space, pattern = "format=", fixed = TRUE)
  if(format_found) {
    shorthand_categories_no_space = gsub("format=", "", shorthand_categories_no_space, fixed = TRUE)
  }
  # if other shorthand
  format_found = grepl(shorthand_categories_no_space, pattern = "=", fixed = TRUE)
  if(format_found) {
    expanded_categories = NULL
    temp_formats = strsplit(shorthand_categories_no_space, "=", fixed = T)[[1]]
    cat_ndx = which(formats == temp_formats[1])
    ref_ndx = broken_ref_vals[,cat_ndx] == temp_formats[2]
    these_categories = broken_ref_vals[ref_ndx, ]
    for(i in 1:nrow(these_categories)) {
      expanded_categories = c(expanded_categories, paste0(these_categories[i,], collapse = "."))
    }
  }


  ## now should be only category labels, periods ('.'), astrix, or +
  rep_syntax = grepl(shorthand_categories_no_space, pattern = "*", fixed = TRUE)
  category_list = list();
  if(rep_syntax) {
    if(shorthand_categories_no_space == "*") {
      # return all categories
      expanded_categories = reference_categories
    } else {
      ## need to repeat some category levels
      broken_vals = strsplit(shorthand_categories_no_space, ".", fixed = T)[[1]]
      for(i in 1:length(broken_vals)) {
        if(broken_vals[i] == "*") {
          category_list[[i]] = unique(broken_ref_vals[,i])
        } else {
          category_list[[i]] = broken_vals[i]
        }
      }
      ## now permutate them
      all_perms <- expand.grid(category_list, stringsAsFactors = FALSE)
      for(i in 1:nrow(all_perms)) {
        expanded_categories = c(expanded_categories, paste0(all_perms[i,], collapse = "."))
      }
    }
  }
  return(expanded_categories)
}

#' @title expand_shorthand_syntax
#'
#' @description
#' A utility function for expanding short hand syntax for based on - format=*.EN.*, label*5, and + syntax
#'
#' @author Craig Marsh
#' @param syntax string of the syntax to expand
#' @return a vector of strings
#' @keywords internal
#' @examples
#' \dontrun{
#' # currently can handle these shorthand
#' expand_shorthand_syntax(syntax = "age_length")
#' expand_shorthand_syntax(syntax = "age_length * 8")
#' expand_shorthand_syntax(syntax ="age_length*8")
#' expand_shorthand_syntax(syntax ="age_length_BP*4 age_length_EN*4")
#' expand_shorthand_syntax(syntax = "1990:2000")
#' expand_shorthand_syntax(syntax ="age_length_BP*4,age_length_EN*4")
#' expand_shorthand_syntax(syntax ="0.75*3")
#' expand_shorthand_syntax(syntax ="0.75 * 3 ")
#' # cant deal with
#' expand_shorthand_syntax(syntax ="0.75*3 0.75*3")
#' # the below fails, can't deal with numbers in the string name
#' # expand_shorthand_syntax(syntax ="FishingSel_1_male*4FishingSel_1_female*4")
#' }
expand_shorthand_syntax <- function(syntax) {
  syntax = paste(syntax, collapse = "")
  ## strip whitespace out of syntax to make it easier
  syntax_no_space = gsub(" ", "", syntax, fixed = TRUE)
  # we wont deal with  ',' so strip them out to
  syntax_no_space = gsub(",", "", syntax_no_space, fixed = TRUE)

  rep_syntax = grepl(syntax_no_space, pattern = "*", fixed = TRUE)
  colon_syntax = grepl(syntax_no_space, pattern = ":", fixed = TRUE)

  ## deal with repeat labels
  repeated_values = vector();
  if(rep_syntax) {
    ## simple expansion
    rep_components = strsplit(syntax_no_space, split = "*", fixed = TRUE)[[1]]
    convert_vals = suppressWarnings(as.numeric(rep_components))
    if(all(!is.na(convert_vals))) {
      ## all numbers so must be 0.45 * 2
      #   pairs = length(rep_components) / 2
      if(length(rep_components) != 2)
        stop("unknown syntax")
      repeated_values = rep(convert_vals[1], convert_vals[2])
    } else {
      # Numbers and words
      for(i in 1:length(rep_components)) {
        # no numbers
        if(gsub("[^\\d]+", "", rep_components[i], perl=TRUE) == "") {
          ## simple case "label"
          value = rep_components[i]
        } else if(gsub("[^\\d]+", "", rep_components[i], perl=TRUE) != "" & (nchar(gsub("[^\\d]+", "", rep_components[i], perl=TRUE)) != nchar(rep_components[i]))) {
          ## has numbers and words...
          ## get number
          rep_num = as.numeric(gsub("[^\\d]+", "", rep_components[i], perl=TRUE))
          repeated_values = c(repeated_values, rep(value, rep_num))
          ## get the other word
          value = gsub('[[:digit:]]+', '', rep_components[i])
        } else {
          ## just number repeat value by this number
          repeated_values = c(repeated_values, rep(value, as.numeric(gsub("[^\\d]+", "", rep_components[i], perl=TRUE))))
        }
      }
    }
  } else if(colon_syntax) {
    ## assumes only numbers lowerval:upperval
    seq_components = strsplit(syntax_no_space, split = ":", fixed = TRUE)[[1]]
    repeated_values = as.numeric(seq_components[1]):as.numeric(seq_components[2])
  } else {
    repeated_values = syntax
  }
  return(repeated_values)
}
