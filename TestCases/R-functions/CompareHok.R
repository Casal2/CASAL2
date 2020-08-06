library(casal)
library(casal2)

## bring in functions
source("AuxillaryFunsToincludeInLibrary.r")
#############
## Comapre CASAL and Casal2 with the exact same parameters
#############
## read in parameters
casal_pars = casal::extract.free.parameters.from.table("../HOK/CASAL/params_est.out")
casal2_pars = casal2::extract.parameters("../HOK/Casal2/params_est.out")

names(casal_pars)
names(casal2_pars)

#cas_mpd = casal::extract.mpd("../HOK/CASAL/estimate.log")
cas_mpd = casal::extract.mpd("../HOK/CASAL/run.log")
#cas_orig = casal::extract.mpd("../HOK/CASAL/orig_run.log")
cas2_mpd = casal2::extract.mpd("../HOK/Casal2/run.log")

## create Casal2 catch table
catches = data.frame(year = cas_mpd$quantities$actual_catches$year, Ensp1	= cas_mpd$quantities$actual_catches$Ensp1, Ensp2 = cas_mpd$quantities$actual_catches$Ensp2,	Wnsp1 = cas_mpd$quantities$actual_catches$Wnsp1,	Wnsp2 =cas_mpd$quantities$actual_catches$Wnsp2,	Esp =cas_mpd$quantities$actual_catches$Esp,	Wsp=cas_mpd$quantities$actual_catches$Wsp)
write.table(x = catches, file = "../HOK/Casal2/Catch_table.txt", quote = F, row.names = F)

## look at B0
cas_mpd$quantities$B0
cas2_mpd$Recruit_E$`1`$b0
cas2_mpd$Recruit_W$`1`$b0

## look at R0
cas_mpd$quantities$R0$E - cas2_mpd$Recruit_E$`1`$r0
cas_mpd$quantities$R0$W - cas2_mpd$Recruit_W$`1`$r0

# look at m
cas_mpd$free$natural_mortality.all
cas2_mpd$Mortality$`1`$m

# plot SSB
cas2_ssb = plot.derived_quantities(cas2_mpd, "SSB", plot.it = F)
par(mfrow = c(1,2))
plot(rownames(cas2_ssb), cas2_ssb[,"SSB_E"], ylim = c(0,1e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Eastern stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$E, lty = 2, lwd = 2, col = "red")


plot(rownames(cas2_ssb), cas2_ssb[,"SSB_W"], ylim = c(0,1.6e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Western stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$W, lty = 2, lwd = 2, col = "red")
legend('bottomleft', legend = c("Casal2","CASAL"),col = c("black","red"), lwd = 2, cex = 0.6)

## Recruitment
par(mfrow = c(1,2))
plot(cas_mpd$quantities$true_YCS$year, cas_mpd$quantities$true_YCS$E, type = "l", lwd = 2, col = "red", ylab  = "True YCS", xlab = "years", ylim = c(0,3), main = "Eastern")
lines(cas2_mpd$Recruit_E$`1`$ycs_years, cas2_mpd$Recruit_E$`1`$true_ycs, col = "black", lwd =2 ,lty = 2)
legend('topright', legend = c("Casal2","CASAL"),col = c("black","red"), lwd = 2, cex = 0.6)

plot(cas_mpd$quantities$true_YCS$year, cas_mpd$quantities$true_YCS$W, type = "l", lwd = 2, col = "red", ylab  = "", xlab = "years", ylim = c(0,3), main = "Western")
lines(cas2_mpd$Recruit_W$`1`$ycs_years, cas2_mpd$Recruit_W$`1`$true_ycs, col = "black", lwd =2 ,lty = 2)

## Fishing
## CASAL catches = catches
cas2_catches = data.frame(year = cas2_mpd$Mortality$`1`$year, Ensp1 = cas2_mpd$Mortality$`1`$`catch[Ensp1]`,
                          Ensp2 = cas2_mpd$Mortality$`1`$`catch[Ensp2]`, Wnsp1 = cas2_mpd$Mortality$`1`$`catch[Wnsp1]`,
                          Wnsp2 =cas2_mpd$Mortality$`1`$`catch[Wnsp2]`, Esp = cas2_mpd$Mortality$`1`$`catch[Esp]`, Wsp = cas2_mpd$Mortality$`1`$`catch[Wsp]`)

catches - cas2_catches

## explioitation
round(rbind(cas2_mpd$Mortality$`1`$`fishing_pressure[Ensp1]`,
            cas_mpd$quantities$fishing_pressures$Ensp1),5)

round(rbind(cas2_mpd$Mortality$`1`$`fishing_pressure[Wnsp1]`,
cas_mpd$quantities$fishing_pressures$Wnsp1),5)
## suggests selectivities are off
round(rbind(cas_mpd$quantities$`Ogive parameter values`$`selectivity[Enspsl].all`,
cas2_mpd$Enspsl$`1`$Values),4)

round(rbind(cas_mpd$quantities$`Ogive parameter values`$`selectivity[Espsl].all`,
            cas2_mpd$Espsl$`1`$Values),4)

####
## Look at observations
####
cbind(cas_mpd$fits$CRsumbio$fits,
cas2_mpd$CRsumbio$`1`$Values$expected)
cas2_mpd$Qs$`1`$CSacous
cas_mpd$free$`q[CSacous].q`

cas_obj = cas_mpd$objective.function$components
## custom function, 
cas2_obj = split_obj(cas2_mpd, label = "objective")
sum(cas2_obj$Value) -cas2_mpd$objective$`1`$values["total_score"] 
cas2_mpd$objective$`1`$values["total_score"] 
obs_across_both = sum(cas_obj$label %in% cas2_obj$Label)
comp = matrix(NA, nrow = obs_across_both, ncol = 3)
colnames(comp) = c("CASAL", "Casal2","Diff")
labels = cas_obj$label[cas_obj$label %in% cas2_obj$Label]
nonlabels = cas_obj$label[!cas_obj$label %in% cas2_obj$Label]

rownames(comp) = labels
for(i in 1:obs_across_both) {
  comp[i,1] = cas_obj$value[cas_obj$label == labels[i]]
  comp[i,2] = cas2_obj$Value[cas2_obj$Label == labels[i]]
  comp[i,3] = comp[i,1] - comp[i,2]
}
## CASAL calculates the spawn wrong in the objective - calculation, it accumulates
## both pspawn and pspawn_1993 see line: 137 in CASAL\run.log
comp
# pspawn for CASAL should be -13.3195
# to prove it here are the observed values and expected values.
cas_mpd$fits$pspawn$fits -
matrix(cas2_mpd$pspawn$`1`$Values$expected, nrow = 2, byrow = T)

cas_mpd$fits$pspawn$obs - 
matrix(cas2_mpd$pspawn$`1`$Values$observed, nrow = 2, byrow = T)

cas_mpd$fits$pspawn$error.value - 
  matrix(cas2_mpd$pspawn$`1`$Values$error_value, nrow = 2, byrow = T)

## look at the other objective function stuff
cas_obj$value[cas_obj$label == "prior_on_selectivity[Wspsl].shift_a"]
cas2_obj$Value[cas2_obj$Label == "Wspsl_shift_param"]

cas_obj$value[cas_obj$label == "prior_on_relative_abundance[CRsumbio].cv_process_error"]
cas2_obj$Value[cas2_obj$Label == "CR_process_error"]

cas_obj$value[cas_obj$label == "prior_on_relative_abundance[SAsumbio].cv_process_error"]
cas2_obj$Value[cas2_obj$Label == "SA_process_error"]

cas_obj$value[cas_obj$label == "prior_on_log_B0_total"]
cas2_obj$Value[cas2_obj$Label == "B0_E_with_total_log_b0_prior"]

cas_obj$value[cas_obj$label == "prior_on_B0_prop_stock1"]
cas2_obj$Value[cas2_obj$Label == "B0_W_with_proportion_prior"]

cas_obj$value[cas_obj$label == "prior_on_recruitment[E].YCS"]
cas2_obj$Value[cas2_obj$Label == "YCS_E"]

cas_obj$value[cas_obj$label == "prior_on_recruitment[W].YCS"]
cas2_obj$Value[cas2_obj$Label == "YCS_W"]

########################
## Check Start conditions, similar to above but these will be furthur away from the 
## solution space
########################
cas_mpd = casal::extract.mpd("../HOK/CASAL/check_start.log")
cas2_mpd = casal2::extract.mpd("../HOK/Casal2/check_start.log")
## look at B0
cas_mpd$quantities$B0
cas2_mpd$Recruit_E$`1`$b0
cas2_mpd$Recruit_W$`1`$b0

# look at m
cas_mpd$free$natural_mortality.all
cas2_mpd$Mortality$`1`$m

# plot SSB
cas2_ssb = plot.derived_quantities(cas2_mpd, "SSB", plot.it = F)
par(mfrow = c(1,2))
plot(rownames(cas2_ssb), cas2_ssb[,"SSB_E"], ylim = c(0,1e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Eastern stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$E, lty = 2, lwd = 2, col = "red")


plot(rownames(cas2_ssb), cas2_ssb[,"SSB_W"], ylim = c(0,1.6e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Western stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$W, lty = 2, lwd = 2, col = "red")
legend('bottomleft', legend = c("Casal2","CASAL"),col = c("black","red"), lwd = 2, cex = 0.6)

####
## Look at observations
####
cbind(cas_mpd$fits$CRsumbio$fits,
      cas2_mpd$CRsumbio$`1`$Values$expected)
cas2_mpd$Qs$`1`$CSacous
cas_mpd$free$`q[CSacous].q`

cas_obj = cas_mpd$objective.function$components
## custom function, 
cas2_obj = split_obj(cas2_mpd, label = "objective")

obs_across_both = sum(cas_obj$label %in% cas2_obj$Label)
comp = matrix(NA, nrow = obs_across_both, ncol = 3)
colnames(comp) = c("CASAL", "Casal2","Diff")
labels = cas_obj$label[cas_obj$label %in% cas2_obj$Label]
nonlabels = cas_obj$label[!cas_obj$label %in% cas2_obj$Label]

rownames(comp) = labels
for(i in 1:obs_across_both) {
  comp[i,1] = cas_obj$value[cas_obj$label == labels[i]]
  comp[i,2] = cas2_obj$Value[cas2_obj$Label == labels[i]]
  comp[i,3] = comp[i,1] - comp[i,2]
}

comp

## look at the other objective function stuff
cas_obj$value[cas_obj$label == "prior_on_selectivity[Wspsl].shift_a"]
cas2_obj$Value[cas2_obj$Label == "Wspsl_shift_param"]

cas_obj$value[cas_obj$label == "prior_on_relative_abundance[CRsumbio].cv_process_error"]
cas2_obj$Value[cas2_obj$Label == "CR_process_error"]

cas_obj$value[cas_obj$label == "prior_on_relative_abundance[SAsumbio].cv_process_error"]
cas2_obj$Value[cas2_obj$Label == "SA_process_error"]

cas_obj$value[cas_obj$label == "prior_on_log_B0_total"]
cas2_obj$Value[cas2_obj$Label == "B0_E_with_total_log_b0_prior"]

cas_obj$value[cas_obj$label == "prior_on_B0_prop_stock1"]
cas2_obj$Value[cas2_obj$Label == "B0_W_with_proportion_prior"]

cas_obj$value[cas_obj$label == "prior_on_recruitment[E].YCS"]
cas2_obj$Value[cas2_obj$Label == "YCS_E"]

cas_obj$value[cas_obj$label == "prior_on_recruitment[W].YCS"]
cas2_obj$Value[cas2_obj$Label == "YCS_W"]

## penalties
sum(c(
cas_obj$value[cas_obj$label == "Umax_Ensp1"],
cas_obj$value[cas_obj$label == "Umax_Ensp2"],
cas_obj$value[cas_obj$label == "Umax_Wnsp1"],
cas_obj$value[cas_obj$label == "Umax_Wnsp2"],
cas_obj$value[cas_obj$label == "Umax_Esp"],
cas_obj$value[cas_obj$label == "Umax_Wsp"]))
cas2_obj$Value[cas2_obj$Label %in% "CatchMustBeTaken[Instant_mortality]"]

## look at actual catches, explotiation, and actual catches
cas2_actual_catches = data.frame(year = cas2_mpd$Mortality$`1`$year, Ensp1 = cas2_mpd$Mortality$`1`$`actual_catch[Ensp1]`,
                          Ensp2 = cas2_mpd$Mortality$`1`$`actual_catch[Ensp2]`, Wnsp1 = cas2_mpd$Mortality$`1`$`actual_catch[Wnsp1]`,
                          Wnsp2 =cas2_mpd$Mortality$`1`$`actual_catch[Wnsp2]`, Esp = cas2_mpd$Mortality$`1`$`actual_catch[Esp]`, Wsp = cas2_mpd$Mortality$`1`$`actual_catch[Wsp]`)
cas_actual_catches = data.frame(year = cas_mpd$quantities$actual_catches$year, Ensp1	= cas_mpd$quantities$actual_catches$Ensp1, Ensp2 = cas_mpd$quantities$actual_catches$Ensp2,	Wnsp1 = cas_mpd$quantities$actual_catches$Wnsp1,	Wnsp2 =cas_mpd$quantities$actual_catches$Wnsp2,	Esp =cas_mpd$quantities$actual_catches$Esp,	Wsp=cas_mpd$quantities$actual_catches$Wsp)
cas2_actual_catches - cas_actual_catches
## explioitation
round(rbind(cas2_mpd$Mortality$`1`$`fishing_pressure[Ensp1]`,
            cas_mpd$quantities$fishing_pressures$Ensp1),5)

round(rbind(cas2_mpd$Mortality$`1`$`fishing_pressure[Wnsp1]`,
            cas_mpd$quantities$fishing_pressures$Wnsp1),5)
## This would imply there is a calculation wrong with the penalty function

########################
## Estimation 
## this is the real test
## on my machine 
## CASAL was 4.86 mins
## Casal2 was 5.55 mins
########################
cas_mpd = casal::extract.mpd("../HOK/CASAL/estimate.log")
cas2_mpd = casal2::extract.mpd("../HOK/Casal2/estimate.log")

## look at B0
cas_mpd$quantities$B0
cas2_mpd$Recruit_E$`1`$b0
cas2_mpd$Recruit_W$`1`$b0

# look at m
cas_mpd$free$natural_mortality.all
cas2_mpd$Mortality$`1`$m

# plot SSB
cas2_ssb = plot.derived_quantities(cas2_mpd, "SSB", plot.it = F)
par(mfrow = c(1,2))
plot(rownames(cas2_ssb), cas2_ssb[,"SSB_E"], ylim = c(0,1e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Eastern stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$E, lty = 2, lwd = 2, col = "red")


plot(rownames(cas2_ssb), cas2_ssb[,"SSB_W"], ylim = c(0,1.6e6), lwd = 2, type = "l",
     xlab = "years", ylab = "SSB", main = "Western stock")
lines(rownames(cas2_ssb), cas_mpd$quantities$SSBs$W, lty = 2, lwd = 2, col = "red")
legend('bottomleft', legend = c("Casal2","CASAL"),col = c("black","red"), lwd = 2, cex = 0.6)

## Recruitment
par(mfrow = c(1,2))
plot(cas_mpd$quantities$true_YCS$year, cas_mpd$quantities$true_YCS$E, type = "l", lwd = 2, col = "red", ylab  = "True YCS", xlab = "years", ylim = c(0,3), main = "Eastern")
lines(cas2_mpd$Recruit_E$`1`$ycs_years, cas2_mpd$Recruit_E$`1`$true_ycs, col = "black", lwd =2 ,lty = 2)
legend('topright', legend = c("Casal2","CASAL"),col = c("black","red"), lwd = 2, cex = 0.6)

plot(cas_mpd$quantities$true_YCS$year, cas_mpd$quantities$true_YCS$W, type = "l", lwd = 2, col = "red", ylab  = "", xlab = "years", ylim = c(0,3), main = "Western")
lines(cas2_mpd$Recruit_W$`1`$ycs_years, cas2_mpd$Recruit_W$`1`$true_ycs, col = "black", lwd =2 ,lty = 2)


####
## Look at observations
####
cbind(cas_mpd$fits$CRsumbio$fits,
      cas2_mpd$CRsumbio$`1`$Values$expected)
cas2_mpd$Qs$`1`$CSacous
cas_mpd$free$`q[CSacous].q`

cas_obj = cas_mpd$objective.function$components
## custom function, 
cas2_obj = split_obj(cas2_mpd, label = "objective")

obs_across_both = sum(cas_obj$label %in% cas2_obj$Label)
comp = matrix(NA, nrow = obs_across_both, ncol = 3)
colnames(comp) = c("CASAL", "Casal2","Diff")
labels = cas_obj$label[cas_obj$label %in% cas2_obj$Label]
nonlabels = cas_obj$label[!cas_obj$label %in% cas2_obj$Label]

rownames(comp) = labels
for(i in 1:obs_across_both) {
  comp[i,1] = cas_obj$value[cas_obj$label == labels[i]]
  comp[i,2] = cas2_obj$Value[cas2_obj$Label == labels[i]]
  comp[i,3] = comp[i,1] - comp[i,2]
}
round(comp,5)

## non invertable matrix? need to see how betadiff if calculating the Hessian if 
## I can't invert it in R? (hopefully me just being an idiot and not a serious issue!!)
isSymmetric(cas2_mpd$Hess$`1`$hessian_matrix)
solve(cas2_mpd$Hess$`1`$hessian_matrix)
isSymmetric(cas2_mpd$Covar$`1`$covariance_matrix)
solve(cas2_mpd$Covar$`1`$covariance_matrix)



## Speed test run CASAL 10 times, with the same number of iterations as Casal2
## betadiff 400 iterations and evaluations. Both models can't find a minimum
## after 400 iterations so should roughly be doing the same amount of work.
## 
CASAL_time = vector()
Casal2_time = vector()
N_runs = 10
setwd("../HOK/CASAL")
casal_cmd = "cmd & casal -e -i start_pars.txt > a.log 2> b.log"
casal2_cmd = "cmd & casal2 -e -i start_pars.txt > a.log 2> b.log"

## CASAL
for(i in 1:N_runs) {
  start_time <- Sys.time()
  shell(cmd = casal_cmd, mustWork = FALSE)
  CASAL_time[i] = Sys.time() - start_time
}
setwd("../Casal2")
## Casal2
for(i in 1:N_runs) {
  start_time <- Sys.time()
  shell(cmd = casal2_cmd, mustWork = FALSE)
  Casal2_time[i] = Sys.time() - start_time
}
## go back to normal directory
setwd("../../RStudio")

summary(CASAL_time)
#Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#27.30   27.80   28.70   31.58   29.53   58.32 
## about 30 seconds
CASAL_time
#[1] 58.32045 28.40155 28.10000 29.31127 29.60712 27.69924 30.61005 27.49335
#[9] 29.00511 27.29818
summary(Casal2_time)
#Min. 1st Qu.  Median    Mean 3rd Qu.    Max. 
#1.340   1.344   1.356   1.359   1.363   1.403 
# about 1.5 minutes
Casal2_time
#[1] 1.402804 1.339903 1.347733 1.340978 1.342607 1.381387 1.364987 1.356603
#[9] 1.356565 1.354933
