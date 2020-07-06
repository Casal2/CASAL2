time_series_match <- function(t1_vec, t2_vec)
{
    if (length(t1_vec) == length(t2_vec))
    {
        return (ifelse(max(abs(range(t1_vec - t2_vec))) == 0, 'yes', 'no'))
    }

    return ('no')
}

