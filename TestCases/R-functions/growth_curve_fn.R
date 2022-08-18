growth_curve_fn <- function(pars, length_vec=seq(0, 100), nyears=100, sex='male', initial_length = 1, initial_cv = 0.1){
  g_a <- pars$g1; g_b <- pars$g2; l_a <- pars$l1; l_b <- pars$l2; cv <- pars$cv; min_s <- pars$min_sigma
  
  l_mid = 0.5*(head(length_vec, -1)+length_vec[-1])
  m = g_a+(g_b-g_a)*(l_mid-l_a)/(l_b-l_a)
  
  
  # Get growth matrix
  trans_matrix = matrix(0, nrow = length(l_mid), ncol = length(l_mid))
  
  for (j in 1:length(l_mid)) {
    trans_matrix[j, j] = pnorm(0, m[j], max(cv*m[j], min_s))
    if (j < length(l_mid)) { # don't do for last box
      trans_matrix[j, (j+1):ncol(trans_matrix)] = dnorm(l_mid[(j+1):length(l_mid)]-l_mid[j], m[j], max(cv*m[j], min_s))
    }
  }
  
  # Now follow cohorts through time
  time_vec = 1:nyears # number of years to simulate
  size_distribution = matrix(0, ncol = length(l_mid), nrow = length(time_vec))
  
  # Set initial distribution - mean size of 10mm, c.v. of 0.4
  initial_sd <- initial_length * initial_cv
  size_distribution[1, ] = 1e3*dnorm(l_mid, initial_length, initial_sd) # 1000 individuals distributed
  
  # Now move through timesteps, growing individuals
  for (j in 2:length(time_vec)) {
    temp = rep(0, length(l_mid))
    
    size_distribution[j, ] = size_distribution[j-1, ] %*% trans_matrix # growth
  }
  
  size_tib = tibble(time = factor(time_vec)) %>% 
    bind_cols(as_tibble(size_distribution)) %>%
    setNames(c('Time', l_mid)) %>%
    pivot_longer(-Time, names_to = 'Bin') %>%
    mutate(Bin = as.numeric(Bin), sex = sex) %>%
    relocate(sex) %>%
    group_by(Time) %>%
    mutate(mean_length = sum(Bin*value)/sum(value)) %>%
    ungroup()
  
  return(size_tib)
}
