print('CASAL model warnings')
print(cas_mpd$parameters.at.bounds)
print('')

print('CASAL sensitivity 1 model warnings')
print(cas_mpd_sens1$parameters.at.bounds)

for (c in 1:num_C2_models)
{
    print('')
    print(paste('Casal2', C2_subdir[c], 'model warnings'))
    print(cas2_mpd[[c]]$warnings_encounted)
}

