C2_subdir <- c('betadiff_casal_flags_on',
               'betadiff_casal_flags_off',
               'betadiff_casal_flags_on_low_tol',
               'adolc_casal_flags_on',
               'adolc_casal_flags_off',
               'adolc_casal_flags_on_low_tol')


# find the non-zero-sized parameter estimate files for the C2 models
C2_subdir <- unlist(sapply(C2_subdir, function(c_dir) if ( file.access(file.path(base_dir, 'Casal2', c_dir, params_est), 4) == 0 &
                                                           file.size(file.path(base_dir, 'Casal2', c_dir, params_est)) > 0 )
                                                         { return (c_dir) }, USE.NAMES=FALSE ))


num_C2_models <- length(C2_subdir)

C2_color <- c('blue', 'green3', 'red', 'gold', 'cyan', 'magenta', 'brown', 'orange')

problem_str <- '* grad_f'
new_str     <- ' grad_f'

# read in Casal2 MPD results
cas2_mpd  <- list()
cas2_corr <- list()
cas2_conv <- list()
for (c in 1:num_C2_models)
{
    cas2_mpd_filename <- file.path(base_dir, 'Casal2', C2_subdir[c], mpd_run)

    cas2_mpd[[c]]  <- Casal2::extract.mpd(cas2_mpd_filename)

    cas2_corr[[c]] <- cas2_mpd[[c]]$Corr$correlation_matrix

    cas2_conv[[c]] <- get_convergence_information(cas2_mpd_filename)
}

