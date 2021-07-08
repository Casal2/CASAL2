#This is an example script for CASAL2 post processing with HAK4 assessment (MPD)a s an example 

library(casal2)
library(ggplot2)
setwd("C:/") ## user defined working direction
#or use mpd<-extract.mpd(file.choose())


##load mpd report from working direction
mpd<-extract.mpd("mpd.txt")
## to check the object class
class(mpd)
##should return "CASAL2MPD"
## to get the list of mpd object
summary.default(mpd)
#should return:
# Length Class  Mode
# Init          1     -none- list
# Sep_Feb      38     -none- list
# Mar_May      38     -none- list
# Jun_Aug      38     -none- list
# summary       1     -none- list
# objective     1     -none- list
# Rec           1     -none- list
# SSB           1     -none- list
# obs_tan       1     -none- list
# tan_at_age    1     -none- list
# eastF_at_age  1     -none- list
# westF_at_age  1     -none- list
# exploit       1     -none- list
# chatTANq      1     -none- list

## model fit to observation
#to see the objective score by category
split_obj(mpd, "objective")
# Label    Value
# 1  chatTANbiomass -19.9440
# 2      chatTANage 341.7019
# 3      chatOBSwst 227.2263
# 4      chatOBSest 127.0473
# 5 Total Objective 685.8110

## to plot fit of each category
plot.fits(mpd,report_label = "obs_tan", main = "obs_tan")
plot.fits(mpd,report_label = "tan_at_age", main = "tan_at_age") ## plot.fits function calculate and plot mean age if it is catch-at-age observation
plot.fits(mpd,report_label = "eastF_at_age") 
plot.fits(mpd,report_label = "westF_at_age") 

## to plot fit of catch-at-age data
plot.year.class(model = mpd, report_label = "tan_at_age")
## use the same code for the other observation blocks
## chathan Rise east Fishery catch-at-age observaton
chaplot.year.class(model = mpd, report_label = "eastF_at_age")
chaplot.year.class(model = mpd, report_label = "WestF_at_age")

#Or to customise your plot, use folling ggplot code
chatOBSwst<-data.table(mpd$westF_at_age$`1`$Comparisons)
##the expected age frequency v.s. the observed age frequency
westF_at_age_plot<-ggplot(chatOBSwst,aes(x = age, y = expected, shape = factor(category))) +
  geom_point(size = 1.2, colour = "red") +
  geom_bar(stat = "identity", aes(age, observed), colour = "grey", fill = NA)+
  facet_wrap(~ year, scales = "free") +
  theme_bw()+ ylim(0, 0.5)
##
westF_at_age_plot + theme(legend.position="none") + ggtitle("catch-at-age observation chatOBSwst")

## the residual plot
westF_at_age_re<-ggplot(chatOBSwst,aes(x = age, y = residual, shape = factor(category))) +
  geom_point(size = 1.2, colour = "red") +
  facet_wrap(~ year, scales = "free") +
  theme_bw()+geom_hline(yintercept = 0) + geom_hline(yintercept = c(-2, 2), colour ="grey60") + ylim(-3, 8)
##
westF_at_age_re + theme(legend.position="none") + ggtitle("catch-at-age observation chatOBSwst residual")


## to plot SSB
plot.derived_quantities(mpd, "SSB")

## to plot ycs
plot.ycs(mpd, "Rec")

## to plot catchability q
plot.pressure(mpd, "exploit", col = "black")

################################
##extract mcmc output
################################
mcmc<-extract.mcmc(samples.file = "mcmc_samples.out", objectives.file = "mcmc_objectives.out", path = "", return_covariance = F)
class(mcmc)
summary(mcmc)

## to see the trace diagnostic plot of the MCMC chain for estimate of B0
name(mcmc)
indexb0 <- as.vector((casal.regexpr(paste("b0", sep = ""), 
                                   dimnames(mcmc)[[2]]) > 0))
trace<-mcmc[, indexb0]
plot(trace, type = "l", col = "red", ylab = "Biomass (t)")

## to plot ycs
plot.ycs(mcmc)
