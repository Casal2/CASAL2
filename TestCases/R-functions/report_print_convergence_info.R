print('CASAL base model convergence information')
print(cas_conv)
print('')

print('CASAL sensitivity 1 model convergence information')
print(cas_sens1_conv)

for (c in 1:num_C2_models)
{
    print('')
    print(paste('Casal2', C2_subdir[c], 'model convergence information'))
    print(cas2_conv[[c]])
}

