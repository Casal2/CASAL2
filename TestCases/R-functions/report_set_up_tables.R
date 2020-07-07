require(plyr)
require(dplyr)
require(huxtable)


rotate_and_label_cols <- function(df_1, labels)
{
    loc_df <- t(df_1)
    colnames(loc_df) <- labels

    return (loc_df)
}


reformat_to_huxtable <- function(df_1, caption_str=NA, format_num=3, reformat_col=FALSE, col_num=-1, add_rownames=TRUE)
{
    hux_table <- as_hux(df_1, add_colnames=TRUE, add_rownames=add_rownames)

    hux_nr <- nrow(hux_table)
    hux_nc <- ncol(hux_table)

    top_border(hux_table)[1,] <- 1
    bottom_border(hux_table)[c(1,hux_nr),] <- 1
    left_border(hux_table)[,1:2] <- 1
    right_border(hux_table)[,hux_nc] <- 1
    caption(hux_table) <- caption_str

    # reformat numbers in table except for the top row and left column
    if (format_num >= 0)
    {
        hux_table <- set_number_format(hux_table, -1, -1, format_num)
    }

    # to reformat and highlight specific columns
    align(hux_table)[-1,col_num] <- '.'
    if (reformat_col & col_num != 0)
    {
        hux_table <- set_number_format(hux_table, -1, col_num, 3)
        hux_table <- map_background_color(hux_table, -1, col_num,
                                          by_ranges(c(-20, -10, 0, 10, 20), c('orange', 'pink', 'white', 'white', 'cyan', 'yellow')))
    }

    return (hux_table)
}


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

