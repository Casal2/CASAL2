C1_obj_fun <- data.frame(Component=cas_mpd$objective.function$components$label,
                         Base_Model=cas_mpd$objective.function$components$value,
                         Sensitivity_1=cas_mpd_sens1$objective.function$components$value)
C1_obj_fun <- bind_rows(C1_obj_fun,
                        data.frame(Component='Total', Base_Model=cas_mpd$objective.function$value, Sensitivity_1=cas_mpd_sens1$objective.function$value))
hux_C1_obj_fun <- reformat_to_huxtable(C1_obj_fun, 'CASAL objective function component values', add_rownames=FALSE)
hux_C1_obj_fun


C2_obj_fun <- rotate_and_label_cols(C2_obj_fun, C2_subdir)
hux_C2_obj_fun <- reformat_to_huxtable(C2_obj_fun, 'Casal2 objective function component values')
hux_C2_obj_fun

