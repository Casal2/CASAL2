# Validate Dataweighting methods
#
source("Init.R")

## read in MPD
csl_mpd = extract.mpd(file = file.path("..","casal2","inst","extdata","estimate.log"))
names(csl_mpd)   

## validate the MethodTA1.8 method for data weighitng multinomial 
## for three unisex age-comps
age_comp_west = reformat.compositional.data(model = csl_mpd, report_label = "westF_at_age")
age_comp_tan = reformat.compositional.data(model = csl_mpd, report_label = "tan_at_age")
age_comp_east  = reformat.compositional.data(model = csl_mpd, report_label = "eastF_at_age")

## west
w_west = DataWeighting::Method.TA1.8(fit = age_comp_west, plotit = F)
w_west2 = Method.TA1.8(model = csl_mpd, observation_labels = "westF_at_age")
w_west - w_west2
par(mfrow = c(1,2))
DataWeighting::Method.TA1.8(fit = age_comp_west, plotit = T)
Method.TA1.8(model = csl_mpd, observation_labels = "westF_at_age", plot.it = T)

## east
w_east = DataWeighting::Method.TA1.8(fit = age_comp_east, plotit = F)
w_east2 = Method.TA1.8(model = csl_mpd, observation_labels = "eastF_at_age")
w_east - w_east2

## TAN
w_tan = DataWeighting::Method.TA1.8(fit = age_comp_tan, plotit = F)
w_tan2 = Method.TA1.8(model = csl_mpd, observation_labels = "tan_at_age")
w_tan - w_tan2


#################
## Mulitple Observations
################
w = DataWeighting::Method.TA1.8(fit = list(age_comp_west, age_comp_tan, age_comp_east), plotit = F, multiple = T)
w2 = Method.TA1.8(model = csl_mpd, observation_labels = c("westF_at_age", "tan_at_age", "eastF_at_age"), plot.it = F)
w - w2
w
w2

#######################
### Sexed Observations
#######################
csl_mpd_sex = extract.mpd(file = file.path("..","casal2","inst","extdata","estimate_comp.log"))
names(csl_mpd_sex)  

age_comp_CRsumage = reformat.compositional.data(model = csl_mpd_sex, report_label = "CRsumage")
age_comp_SAsumage = reformat.compositional.data(model = csl_mpd_sex, report_label = "SAsumage")
age_comp_Espage  = reformat.compositional.data(model = csl_mpd_sex, report_label = "Espage")

## CR sum
w_crsum = DataWeighting::Method.TA1.8(fit = age_comp_CRsumage, plotit = F, multiple = F)
w_crsum2 = Method.TA1.8(model = csl_mpd_sex, observation_labels = c("CRsumage"), plot.it = F)

w_crsum - w_crsum2
w_crsum
w_crsum2

## CR SA Aut
w_saaut = DataWeighting::Method.TA1.8(fit = age_comp_SAsumage, plotit = F, multiple = F)
w_saaut2 = Method.TA1.8(model = csl_mpd_sex, observation_labels = c("SAsumage"), plot.it = F)

w_saaut - w_saaut2
w_saaut
w_saaut2

#######################
### Multiple obs sexed
#######################
w = DataWeighting::Method.TA1.8(fit = list(age_comp_CRsumage, age_comp_SAsumage, age_comp_Espage), plotit = F, multiple = T)
w2 = Method.TA1.8(model = csl_mpd_sex, observation_labels = c("CRsumage", "SAsumage", "Espage"), plot.it = F)
w - w2
w
w2
