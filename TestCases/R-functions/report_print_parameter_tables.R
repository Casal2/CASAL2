C2_est_params   <- cas2_mpd[[1]]$estimated_values$values
c2_est_params_1 <- C2_est_params
C2_obj_fun      <- cas2_mpd[[1]]$obj_fun$values

C2_pd_est_params <- C2_est_params - C2_est_params

if (num_C2_models > 1)
{
    for (c in 2:num_C2_models)
    {
        c2_est_params_c  <- cas2_mpd[[c]]$estimated_values$values
        C2_est_params    <- bind_rows(C2_est_params, c2_est_params_c)

        C2_pd_est_params <- bind_rows(C2_pd_est_params, 100.0 * (1.0 - (c2_est_params_c / c2_est_params_1)))

        C2_obj_fun       <- bind_rows(C2_obj_fun, cas2_mpd[[c]]$obj_fun$values)
    }
}


C1_est_params <- cbind(unlist(cas_mpd$free), unlist(cas_mpd_sens1$free), 100.0 * (1.0 - (unlist(cas_mpd_sens1$free) / unlist(cas_mpd$free))))
colnames(C1_est_params) <- c('Base_Model', 'Sensitivity_1', 'Percent_Diff')
hux_C1_est_params <- reformat_to_huxtable(C1_est_params, 'CASAL parameter estimates', 5, TRUE, 4)
# print(hux_C1_est_params)


C2_est_params <- rotate_and_label_cols(C2_est_params, C2_subdir)
hux_C2_est_params <- reformat_to_huxtable(C2_est_params, 'Casal2 parameter estimates', 5)
# print(hux_C2_est_params)


C2_pd_est_params <- rotate_and_label_cols(C2_pd_est_params, C2_subdir)
hux_C2_pd_est_params <- reformat_to_huxtable(C2_pd_est_params, paste('Casal2 parameter estimates: Percent Difference from ', C2_subdir[1], sep=''), 3, TRUE, -1)
# print(hux_C2_pd_est_params)

