# read in CASAL MPD results
cas_mpd_filename <- file.path(base_dir, 'CASAL', mpd_run)
cas_mpd  <- casal::extract.mpd(cas_mpd_filename)
cas_part <- casal::extract.partition(cas_mpd_filename)
cas_corr <- casal::extract.correlation.matrix(cas_mpd_filename)
cas_conv <- get_convergence_information(cas_mpd_filename)
c1_quant <- cas_mpd$quantities

cas_mpd_sens1_filename <- file.path(base_dir, 'CASAL_sens1', mpd_run)
cas_mpd_sens1  <- casal::extract.mpd(cas_mpd_sens1_filename)
cas_sens1_part <- casal::extract.partition(cas_mpd_sens1_filename)
cas_sens1_corr <- casal::extract.correlation.matrix(cas_mpd_sens1_filename)
cas_sens1_conv <- get_convergence_information(cas_mpd_sens1_filename)
c1_sens1_quant <- cas_mpd_sens1$quantities

