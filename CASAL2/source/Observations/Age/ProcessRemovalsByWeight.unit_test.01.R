
#
# generate unit test sample
# version 1: 1cm length bins
#

# real BNS growth
$ M 0.08
# F 0.1

#files:
# "Data/MD.growth.txt"
# "Data/MD.growth.female.txt"



growth <- read.table("Data/MD.growth.txt",F)  #male
growth <- t(growth)
growth <- growth[-1,]

growth1 <- data.frame(age=as.numeric(growth[,1]),mL=as.numeric(growth[,2]))
growth1



growth <- read.table("Data/MD.growth.female.txt",F)  #female

growth <- t(growth)
growth <- growth[-1,]

growth2 <- data.frame(age=as.numeric(growth[,1]),mL=as.numeric(growth[,2]))
growth2

plot(c(0,80),c(0,105),pch=" ",las=1,xlab="Age",ylab="Length (cm)")

xxx<-seq(from=1,by=5,to=80)
lines(growth2$age[xxx],growth2$mL[xxx],pch="f",lwd=2,type="p")
lines(growth1$age[xxx],growth1$mL[xxx],pch="m",lwd=2,type="p")

yyy<-growth2$mL
yyy[xxx] <- NA
lines(growth2$age,yyy,pch="f",lwd=2,type="l")

yyy<-growth1$mL
yyy[xxx] <- NA
lines(growth1$age,yyy,pch="f",lwd=2,type="l")



## Build up an hypothetical Age structure

age <-1:80 #ignore + gp
M   <-0.08
FF  <- 0.1	$fishing F

# try sharper LHS
fish_sel <- d_norm(age, 7.96874 ,.28739, 88.0876 ) ## This selects vulnerable biomass Retained + Discard
par(new=T)
lines(1:80,fish_sel*80,xaxt="n",yaxt="n")


N<-10000 * exp(-M*(age-1))
#apply fishing to equilb
Fi <- fish_sel*FF
Z <- M + Fi
for( i in 1:300){
	C <- Fi/Z*(1-exp(-Z))*N
	N <- exp(-Z)*N
	N<- c(10000,N[-length(N)])
	}
#plot(age,N)
cat(N/2,"\n") #start 2001 age structure, by sex
# 5000 4615.582 4260.719 3933.139 3630.745 3351.6 3093.917 2855.937 2385.479 1992.538 1664.369 1390.312 1161.455 970.3478 810.7656 677.5068 566.2265 473.2956 395.6836 330.8603 276.713 231.4784 193.6843 162.1023 135.7069 113.6422 95.19409 79.76633 66.86143 56.06414 47.02784 39.46316 33.12855 27.82233 23.37606 19.6491 16.52393 13.90241 11.7025 9.85565 8.304543 7.001253 5.905697 4.984337 4.209107 3.55651 3.006872 2.543711 2.153214 1.823804 1.545772 1.310972 1.112568 0.9448203 0.8029072 0.6827773 0.5810241 0.4947825 0.4216413 0.3595706 0.3068602 0.2620689 0.2239814 0.1915723 0.1639761 0.1404618 0.1204115 0.1033029 0.08869376 0.07621007 0.06553482 0.05639933 0.04857575 0.04187071 0.03611998 0.03118404 0.02694423 0.02329962 0.02016425 0.01746491 
#>

par(new=T)
xxx <- N*fish_sel
lines(age,xxx/max(xxx) * 80,lty=2,lwd=2) #scale to 80

Wbin <- seq(from =0.3 , to =20, by =0.1 )
#Wbin <- c(8,Wbin,94 ) #covers WF @ 50000 sample
len.bin <- read.table("Data/length.bins.n.txt",F)
		# number of fish in a box at each length bin (3cm width)
N_retained <- N *  fish_sel  * exp(0.5  * Z) #mid fishing year, 6mth fishing
#catch 2001
 sum( Fi/Z*(1-exp(-Z))*N) #5047.413
sum(C) #3166.217

# plot(N_retained)


#mean length & weight by age
#len = VB(age,K,L_inf, t0)
len <- growth1$mL	#ambig depends on when grwoth data collected (time of year etc0
weig = basic(len, a, b)* (1 + VBcv)^(b*(b-1)/2) #correction for mean-at-age

#flen = VB(age,fK,fL_inf, ft0)
flen <- growth2$mL
fweig = basic(flen, a, b)* (1 + VBcv)^(b*(b-1)/2)

#catch 2001
 sum( Fi/Z*(1-exp(-Z))*N*(fweig+weig)/2)/1000 #50:50 m:f as selectivity is on age
	# fish = 5047.413
	# 468.89 t	
sum(C) #3166.217

# plot(N_retained)


## Parameters for Mean length weight relationship

CONTROL <- list(		#parameters needed for mega sampling
	VBcv  =  VBcv,
	Wcv   =  Wcv,
	#units<-"tonnes"
	a     = 9.63e-9*1000,  #kg
	b     =3.173,
	len.bin2   = len.bin2,
	len.bin2.n = len.bin2.n,
	Wbin       = Wbin ,
	weig       = weig,
	fweig      = fweig,
	len        = len,
	flen       = flen,
	N_retained  = N_retained ,
	age        = age,
	Nlarge     = 150000)	#mega sample size

CONTROL <-  add.length.grid (len.bin,CONTROL) #default is bin widths if 1 cm

source("Source/functions.R")
# get WF etc that represent true freq via mega sampling
first  <- sim.age.L.W(CONTROL)	# takes a while
second <- sim.age.L.W(CONTROL)	# do twice, is sample size large enough?
names(first)
	# 1] "true.mWF" "WFin2"    "Lweight1" "Lweight2" "true.LF"  "Llength1" "Llength2"
	#Lweight1 etc are one fish/weight version
	# true.mWF bins lengths, then take mean weight from m fish and replicates thei wean m times
	# = how observed WF are samplied

#
# sample AF
# do 2 times for assessing repeatability
plot(density(first$Llength2,width=.5),type="l",xlab="Length (cm)",main=" ")
lines(density(first$Llength1,width=.5),lwd=2,lty=2)
legend("topright",legend=c("Female","Male"),lty=c(1,2))

# 2nd sample
lines(density(second$Llength2,width=.5),lwd=2,lty=4,col=2)
lines(density(second$Llength1,width=.5),lwd=2,lty=3,col=2)
	#looks good
#
# repeat sample and over plot with first
#

plot(density(first$Lweight1),type="l",xlab="Weight (Kg)",main=" ")
lines(density(first$Lweight2),col=2)
#legend("topright",legend=c("Male","Female"),lty=1,col=c(1,2))

lines(density(second$Lweight2),col=3)
lines(density(second$Lweight1),col=3)
	#good, v. small differece at the female mode

# 
# one fish/weight datum WF, combined sex

Lweight<-c(first$Lweight1,first$Lweight2)	#combined sex
n4density <- (max(Lweight) - min(Lweight) + 3*2*.05 )*10 # get 0.1 bins
xxx<- density(Lweight,n=n4density,bw=0.05)
xxx$x[2]-xxx$x[1]
# 0.100254

# eg WF from BNS data 
# plot one fish/weight WF + obs WF (2016) + smoothed obs WF using cv = 10%
#
plot(c(0,15),c(0,0.14),pch=" ",xlab="Weight (Kg)",main=" ",ylab="Density")
lines(xxx$x,xxx$y*0.1,type="l",lwd=2)	
#lines(xxx$x,xxx$y*0.0742,type="l",lwd=2)	#prob=density * x_width
sum(xxx$y*0.1) #0.99996

OWF  <- read.table("Data/WF.txt",F)	#0.1 kg bins=row 1; row1 2016 data etc
OWF[1,]
lines(OWF[1,-1],OWF[2,-1])  #*0.1) #same scale as density
sum(OWF[2,-1]) #1
#hist(Lweight,80)
#R
#plot(unlist(OWF[1,-1]),unlist(OWF[2,-1]),type="l")
# smooth the obs WF: can we get it to look like the one fish/weight version?
#	try 10% of the weight bin value
#		not right as we should use integrate over the Ls in aL^b that can
#		generate the weigth bin
 	N <-OWF[2,-1] * 60000	#generate sampel of 60000 fish
 fake   <- rep(unlist(OWF[1,-1])+.05,N)	#sample weights
 fake2  <- rnorm(fake,fake,fake*.1)	#randomise weights treating each bin as a mean
 
# intervals so that that are 0.1 kg teh same as OWF
n4density <- (max(fake2) - min(fake2) + 3*2*.05 )*10 #allow for cut * bw at extremes
fakeSmooth <- density(fake2,n=n4density,bw=0.05)
 fakeSmooth $x[2] - fakeSmooth$x[1] # 0.1002089
 lines(fakeSmooth $x,fakeSmooth $y* 0.10,col=3,lwd=2)

legend("topright",legend=c("WF: One fish/weight datum","2016 observed WF","2016 WF reverted (cv=10%)"),col=c(1,1,3),lwd=c(2,1,2))


WFin <- match(names(first$true.mWF),1:length(CONTROL$Wbin),0)
#
lines(xxx$x,xxx$y*0.1,type="l",lwd=2,lty=3)



plot(c(0,10),c(0,.07),pch=" ",las=1,xlab="Weight bin lower bound",ylab="Density")
#lines(Wbin[WFin],trueWF/sum(trueWF),type="b",lwd=2) 
lines(OWF[1,-1],OWF[2,-1],col=2,lwd=2)
#lines(OWF[1,-1],OWF[3,-1],col=3,lwd=2,lty=1)

lines(Wbin[WFin2],true.mWF/sum(true.mWF),lwd=2,col=4,lty=2)
lines(xxx$x,xxx$y*0.1,type="l",lwd=2,lty=3)

legend("topright",legend=c("True WF, relicated means from fish box","True WF, one fish/weight datum","observed, 2016"),
	lty=c(2,3,1),col=c(4,1,2),lwd=c(2,2,2))#,pch=c("o","",""))


title("True WF: 1 cm sampling width for each fish box")


#
# 0.5 cm length bins
#
# NOTE: Nlarge has been chnaged from that set above i.e., from 300K to 3M

Lbin.width <- 1  #.01

CONTROL <-  add.length.grid (len.bin,CONTROL,len.width=Lbin.width ) #default is bin widths if 1 cm

# get WF etc that represent true freq via mega sampling
first  <- sim.age.L.W(CONTROL)


Lweight<-c(first$Lweight1,first$Lweight2)	#combined sex
n4density <- (max(Lweight) - min(Lweight) + 3*2*.05 )*10 # get 0.1 bins
xxx<- density(Lweight,n=n4density,bw=0.05)
xxx$x[2]-xxx$x[1]

plot(c(0,10),c(0,.07),pch=" ",las=1,xlab="Weight bin lower bound",ylab="Density")
lines(OWF[1,-1],OWF[2,-1],col=2,lwd=2)
lines(as.numeric(names(first$true.mWF)),first$true.mWF/sum(first$true.mWF),lwd=2,col=4,lty=2)
lines(xxx$x,xxx$y*0.1,type="l",lwd=2,lty=3)

legend("topright",legend=c("True WF, relicated means from fish box","True WF, one fish/weight datum","observed, 2016"),
	lty=c(2,3,1),col=c(4,1,2),lwd=c(2,2,2))#,pch=c("o","",""))
title(paste("True WF:",Lbin.width ,"cm sampling width for each fish box"))

CONTROL $Nlarge	# 150000
CONTROL $Nlarge <- 3000000


#
# same WF
# Nlarge 3M

unit.test.Lweight <- Lweight
ccc2              <- cut(unit.test.Lweight ,CONTROL$Wbin,labels=F)    #names is lower weight of bin
init.test.WF      <- tapply(ccc2,CONTROL$Wbin[ccc2],length)
init.test.WF      <- init.test.WF /sum(init.test.WF )

plot(init.test.WF)

#another sample fo 3M
ccc2              <- cut(Lweight ,CONTROL$Wbin,labels=F)    #names is lower weight of bin
init.test.WF2      <- tapply(ccc2,CONTROL$Wbin[ccc2],length)
init.test.WF2      <- init.test.WF2 /sum(init.test.WF2 )
hist(init.test.WF2  - init.test.WF,50)  
 #range is +/- 3.2e-4; most +/- 1e-4

plot(abs(init.test.WF2  - init.test.WF ),init.test.WF)

#
# 12M = 4x = half error?
#

Lbin.width <- 1  #.01

CONTROL2 <-  add.length.grid (len.bin,CONTROL,len.width=Lbin.width ) #default is bin widths if 1 cm
CONTROL2$Nlarge <- 12e6
# get WF etc that represent true freq via mega sampling
first  <- sim.age.L.W(CONTROL)

Lweight<-c(first$Lweight1,first$Lweight2)	#combined sex

ccc2              <- cut(Lweight ,CONTROL$Wbin,labels=F)    #names is lower weight of bin
init.test.WF2.12      <- tapply(ccc2,CONTROL$Wbin[ccc2],length)
init.test.WF2.12      <- init.test.WF2.12 /sum(init.test.WF2.12 )


ccc2              <- cut(Lweight ,CONTROL$Wbin,labels=F)    #names is lower weight of bin
init.test.WF1.12      <- tapply(ccc2,CONTROL$Wbin[ccc2],length)
init.test.WF1.12      <- init.test.WF1.12 /sum(init.test.WF1.12 )



hist(init.test.WF2.12  - init.test.WF1.12,50) 
sd(init.test.WF2.12  - init.test.WF1.12)	# 5.108946e-05 expeted 4.2e-5?
sd(init.test.WF2  - init.test.WF)	# 5.986795e-05

range(init.test.WF2.12  - init.test.WF1.12)
#  -0.0002067606  0.0002405855
# 2.5e-4

hist((init.test.WF2.12  - init.test.WF1.12)/((init.test.WF2.12  + init.test.WF1.12)/2),50) 
# +/- 0.5

#
# combined two 12M for unit test
#
unit.test.final <- (init.test.WF2.12  + init.test.WF1.12)/2
unit.test.final

cat(unit.test.final)
ii <- as.numeric(names(unit.test.final))

#
# pick out 3 rows to us to verify the Casal2 code is correct
# not ~1 e-5 sim error so get larger ones
cbind(c(1:length(unit.test.final)),ii,unit.test.final)
       ii unit.test.final
           ii unit.test.final
0.6    1  0.6    6.667540e-07
0.7    2  0.7    2.500328e-06
0.8    3  0.8    1.183489e-05
0.9    4  0.9    3.800498e-05
1      5  1.0    1.121814e-04
1.1    6  1.1    2.830371e-04
1.2    7  1.2    6.079130e-04
1.3    8  1.3    1.199490e-03 <<<<<<(1)
1.4    9  1.4    2.133613e-03
1.5   10  1.5    3.541964e-03
1.6   11  1.6    5.351701e-03
1.7   12  1.7    7.742848e-03
1.8   13  1.8    1.056155e-02
1.9   14  1.9    1.368629e-02
2     15  2.0    1.704257e-02
2.1   16  2.1    2.048952e-02
2.2   17  2.2    2.398881e-02
2.3   18  2.3    2.706971e-02
2.4   19  2.4    2.986625e-02
2.5   20  2.5    3.235707e-02
2.6   21  2.6    3.408446e-02
2.7   22  2.7    3.542297e-02
2.8   23  2.8    3.623408e-02
2.9   24  2.9    3.639310e-02
3     25  3.0    3.621224e-02
3.1   26  3.1    3.559266e-02
3.2   27  3.2    3.481873e-02
3.3   28  3.3    3.380309e-02
3.4   29  3.4    3.236441e-02
3.5   30  3.5    3.084021e-02
3.6   31  3.6    2.937952e-02
3.7   32  3.7    2.783815e-02
3.8   33  3.8    2.613259e-02
3.9   34  3.9    2.455872e-02 <<<<<<<<<(2)
4     35  4.0    2.314903e-02
4.1   36  4.1    2.167184e-02
4.2   37  4.2    2.006096e-02
4.3   38  4.3    1.894865e-02
4.4   39  4.4    1.759364e-02
4.5   40  4.5    1.641732e-02
4.6   41  4.6    1.530217e-02
4.7   42  4.7    1.426654e-02
4.8   43  4.8    1.324190e-02 <<<<<<<<<<<<(3)
4.9   44  4.9    1.244796e-02
5     45  5.0    1.152918e-02
5.1   46  5.1    1.074224e-02
5.2   47  5.2    1.002081e-02
5.3   48  5.3    9.328389e-03
5.4   49  5.4    8.718809e-03
5.5   50  5.5    8.091560e-03
5.6   51  5.6    7.591494e-03
5.7   52  5.7    6.976247e-03
5.8   53  5.8    6.566860e-03
5.9   54  5.9    6.138471e-03
6     55  6.0    5.701247e-03
6.1   56  6.1    5.309196e-03
6.2   57  6.2    4.891808e-03
6.3   58  6.3    4.657277e-03
6.4   59  6.4    4.363738e-03
6.5   60  6.5    4.064366e-03
6.6   61  6.6    3.791497e-03
6.7   62  6.7    3.553299e-03
6.8   63  6.8    3.266761e-03
6.9   64  6.9    3.083571e-03
7     65  7.0    2.919716e-03
7.1   66  7.1    2.686852e-03
7.2   67  7.2    2.544500e-03
7.3   68  7.3    2.378812e-03
7.4   69  7.4    2.232959e-03
7.5   70  7.5    2.090607e-03
7.6   71  7.6    1.984927e-03
7.7   72  7.7    1.836907e-03
7.8   73  7.8    1.737061e-03
7.9   74  7.9    1.644549e-03
8     75  8.0    1.533868e-03
8.1   76  8.1    1.443689e-03
8.2   77  8.2    1.321173e-03
8.3   78  8.3    1.243830e-03
8.4   79  8.4    1.156485e-03
8.5   80  8.5    1.100644e-03
8.6   81  8.6    1.048137e-03
8.7   82  8.7    9.544584e-04
8.8   83  8.8    9.189537e-04
8.9   84  8.9    8.736145e-04
9     85  9.0    8.064390e-04
9.1   86  9.1    7.726012e-04
9.2   87  9.2    7.239282e-04
9.3   88  9.3    6.799224e-04
9.4   89  9.4    6.454179e-04
9.5   90  9.5    5.905774e-04
9.6   91  9.6    5.565729e-04
9.7   92  9.7    5.339033e-04
9.8   93  9.8    4.860637e-04
9.9   94  9.9    4.755623e-04
10    95 10.0    4.485588e-04
10.1  96 10.1    4.212219e-04
10.2  97 10.2    3.925514e-04
10.3  98 10.3    3.768827e-04
10.4  99 10.4    3.445451e-04
10.5 100 10.5    3.530462e-04
10.6 101 10.6    3.180417e-04
10.7 102 10.7    3.060401e-04
10.8 103 10.8    2.852040e-04
10.9 104 10.9    2.590339e-04
11   105 11.0    2.428651e-04
11.1 106 11.1    2.383646e-04
11.2 107 11.2    2.223625e-04
11.3 108 11.3    2.155282e-04
11.4 109 11.4    2.068604e-04
11.5 110 11.5    1.970258e-04
11.6 111 11.6    1.918585e-04
11.7 112 11.7    1.780233e-04
11.8 113 11.8    1.718558e-04
11.9 114 11.9    1.643549e-04
12   115 12.0    1.500196e-04
12.1 116 12.1    1.466859e-04
12.2 117 12.2    1.286835e-04
12.3 118 12.3    1.365179e-04
12.4 119 12.4    1.243496e-04
12.5 120 12.5    1.286835e-04
12.6 121 12.6    1.153485e-04
12.7 122 12.7    1.070140e-04
12.8 123 12.8    1.025134e-04
12.9 124 12.9    1.056805e-04
13   125 13.0    8.684470e-05
13.1 126 13.1    9.534581e-05
13.2 127 13.2    9.051187e-05
13.3 128 13.3    8.401101e-05
13.4 129 13.4    8.034386e-05
13.5 130 13.5    7.517652e-05
13.6 131 13.6    6.800890e-05
13.7 132 13.7    6.650871e-05
13.8 133 13.8    6.834229e-05
13.9 134 13.9    5.917442e-05
14   135 14.0    6.450846e-05
14.1 136 14.1    5.367370e-05
14.2 137 14.2    5.300695e-05
14.3 138 14.3    4.717284e-05
14.4 139 14.4    4.367238e-05
14.5 140 14.5    4.550596e-05
14.6 141 14.6    4.267226e-05
14.7 142 14.7    4.400577e-05
14.8 143 14.8    3.917179e-05
14.9 144 14.9    3.883842e-05
15   145 15.0    3.983855e-05
15.1 146 15.1    3.650478e-05
15.2 147 15.2    3.633810e-05
15.3 148 15.3    3.400445e-05
15.4 149 15.4    2.850374e-05
15.5 150 15.5    3.167081e-05
15.6 151 15.6    2.867042e-05
15.7 152 15.7    2.550334e-05
15.8 153 15.8    2.550334e-05
15.9 154 15.9    2.700354e-05
16   155 16.0    2.283632e-05
16.1 156 16.1    2.283632e-05
16.2 157 16.2    2.283632e-05
16.3 158 16.3    1.933586e-05
16.4 159 16.4    1.833573e-05
16.5 160 16.5    1.916918e-05
16.6 161 16.6    2.066937e-05
16.7 162 16.7    1.783568e-05
16.8 163 16.8    2.016931e-05
16.9 164 16.9    1.633547e-05
17   165 17.0    1.616878e-05
17.1 166 17.1    1.433521e-05
17.2 167 17.2    1.283502e-05
17.3 168 17.3    1.450190e-05
17.4 169 17.4    1.583541e-05
17.5 170 17.5    1.400183e-05
17.6 171 17.6    1.300170e-05
17.7 172 17.7    1.233495e-05
17.8 173 17.8    1.166820e-05
17.9 174 17.9    9.834618e-06
18   175 18.0    1.133482e-05
18.1 176 18.1    1.116813e-05
18.2 177 18.2    9.167871e-06
18.3 178 18.3    8.167739e-06
18.4 179 18.4    9.167868e-06
18.5 180 18.5    9.501242e-06
18.6 181 18.6    1.050138e-05
18.7 182 18.7    7.667674e-06
18.8 183 18.8    6.334162e-06
18.9 184 18.9    8.334425e-06
19   185 19.0    7.334293e-06
19.1 186 19.1    8.001049e-06
19.2 187 19.2    5.667409e-06
19.3 188 19.3    5.167342e-06
19.4 189 19.4    7.500983e-06
19.5 190 19.5    5.500720e-06
19.6 191 19.6    6.667540e-06
19.7 192 19.7    6.167477e-06
19.8 193 19.8    5.500720e-06
19.9 194 19.9    4.500591e-06

