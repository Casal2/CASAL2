
require(casal)
require(CASAL2)

require(ggplot2)


base_dir <- '../TestCases/primary/LIN56'
if (Sys.info()["user"] == "Dell User")
  base_dir <- '../LIN56'

## My process For any run where I 'think' I have the same parameters and configurations, between CASAL and CAsal2
# - look at SSB (not the same check initial age structure)
# - look at R0  if R0's no close, this is telling you either M, growth, annual timestep, SSB calculations are wrong
# - once you are happy that M, R0 and you get the same initial age-structure and you SSB start moving over the show
# - check fishing exploitation rates - could be a selectivity
# - check recruitment, ycs are annoying with the range and standardisations, they always catch me out
# - once you are happy SSB's and exploitation rates are the same between the two then start bringing in observations.



# MPD files are params_est.out, run_estimation.txt, and mpd.out for both CASAL and Casal2
mpd_run    <- 'run.log'

# read in CASAL MPD results
cas_mpd <- casal::extract.mpd(mpd_run, file.path(base_dir, 'CASAL'))
cas2_mpd <- casal2::extract.mpd(mpd_run, file.path(base_dir, 'Casal2'))

## check SSB's
c1_quant <- cas_mpd$quantities
casal2::plot.derived_quantities(cas2_mpd,report_label = "DQs")
cas2_ssb = casal2::plot.derived_quantities(cas2_mpd,report_label = "DQs", plot.it = F)

lines(c1_quant$SSBs$year, c1_quant$SSBs$SSB, col = "red")

##
cbind(cas2_ssb, c1_quant$SSBs$SSB)

cas_mpd$quantities$R0 
cas2_mpd$Rec$`1`$r0

cas_mpd$quantities$B0
cas2_mpd$Rec$`1`$b0
## check Obs's
cas_fits = cas_mpd$fits
cas_fits$Tangaroa_bio_autumn$obs
cas2_mpd$Tangaroa_bio_autumn$`1`$Values$observed
cas_fits$Tangaroa_bio_autumn$fits
cas2_mpd$Tangaroa_bio_autumn$`1`$Values$expected

## Double check selectivities
cbind(cas_mpd$quantities$`Ogive parameter values`$`selectivity[surveyaut_sel].male`,
      cas2_mpd$autumnTANSel_male$`1`$Values)
cbind(cas_mpd$quantities$`Ogive parameter values`$`selectivity[surveyaut_sel].female`,
      cas2_mpd$autumnTANSel_female$`1`$Values)
cbind(cas_mpd$quantities$`Ogive parameter values`$`natural_mortality.ogive_all`,
      cas2_mpd$NaturalMortalityOgive_all$`1`$Values)

## Check exploitation
round(cbind(cas_mpd$quantities$fishing_pressures$trawl,
cas2_mpd$Mortality$`1`$`fishing_pressure[FishingTrwl]`),4)

