# Selectivities.r
# @author C.Marsh
# @date 18/02/2017
# @description
# A R script with all the selectivity function available in CASAL2.

# Common parameter to all functions
# @param: age_or_length, A vector of ages or lengths to calculate the selectivity value for.
# @param: alpha, A scalar which is multiplied to the selectivity. Used to move the max from 1.

print("I haven't yet checked these functions are exact. I have jsut converted the C++ code to R code.")

## Utility functions
pow = function(x,exponent) {return(x^exponent)}


## Selectivities
DoubleNormal = function(age_or_length,alpha = 1,mu,sigma_l,sigma_r) {
	selectivity = vector();
	for(i in 1:length(age_or_length)) {
		if(age_or_length[i] < mu) {
			selectivity[i] = pow(2.0, -((age_or_length[i] - mu) / sigma_l * (age_or_length[i] - mu) / sigma_l)) * alpha;
		} else {
			selectivity[i] = pow(2.0, -((age_or_length[i] - mu) / sigma_r * (age_or_length[i] - mu) / sigma_r)) * alpha;		
		}		
	}
	return(selectivity);
}

InverseLogistic = function(age_or_length,alpha = 1,a50,ato95) {
	selectivity = vector();
	for (i in 1:length(age_or_length)) {
		threshold = (a50 - age_or_length[i]) / ato95;
		if (threshold > 5) {
			selectivity[i] = alpha;
		} else if (threshold < -5){
			selectivity[i] = 0.0
		} else {
			selectivity[i] = alpha - (alpha / (1.0 + pow(19.0, threshold)));
		}		
	}
	return(selectivity);
}

LogisticProducing = function(age_or_length,alpha = 1,a50,ato95,high,low) {
	selectivity = vector();
	for (i in 1:length(age_or_length)) {
		if (age_or_length[i] > low) {
			selectivity[i] = 0.0;
		} else if (age_or_length[i] >= high){
			selectivity[i] = alpha
		} else if (age_or_length[i] == low){
			selectivity[i] = 1.0 / (1.0 + pow(19.0, (a50 - age_or_length[i]) / ato95)) * alpha;		
		} else {
			lambda2 = 1.0 / (1.0 + pow(19.0, (a50 - (age_or_length[i] - 1)) / ato95));
      lambda1 = 1.0 / (1.0 + pow(19.0, (a50 - age_or_length[i]) / ato95));
      selectivity[i] = (lambda1 - lambda2) / (1.0 - lambda2) * alpha;			
		}		
	}
	return(selectivity);
}

Logistic = function(age_or_length,alpha = 1,a50,ato95) {
	selectivity = vector();
	for (i in 1:length(age_or_length)) {
		threshold = (a50 - age_or_length[i]) / ato95;
		if (threshold > 5) {
			selectivity[i] = 0.0;
		} else if (threshold < -5){
			selectivity[i] = alpha
		} else {
			selectivity[i] = alpha / (1.0 + pow(19.0, threshold));
		}		
	}
	return(selectivity);
}


DoubleExponential = function(age_or_length,x0,x1,x2,y0,y1,y2,alpha = 1) {
	selectivity = vector();
	for(i in 1:length(age_or_length)) {
		if(age_or_length[i] <= x0) {
		  selectivity[i] = alpha * y0 * pow((y1 / y0), (age_or_length[i] - x0)/(x1 - x0));
		} else if (age_or_length[i] > x0 & age_or_length[i] <= x2) {
			selectivity[i] = alpha * y0 * pow((y2 / y0), (age_or_length[i] - x0)/(x2 - x0));
		} else {
			selectivity[i] = y2;
		}
	}
	return(selectivity);
}


AllValues = function(age_or_length, v,alpha = 1) {
	if(length(age_or_length) != length(v)) 
	 stop("This function will only work if v is the same length as the age_or_length vector")
	return(v * alpha);
}

Increasing = function(age_or_length, alpha = 1,v,low,high) {
	if(length(age_or_length) != length(v)) 
	 stop("This function will only work if v is the same length as the age_or_length vector")
	
	for(i in 1:length(age_or_length)) {
		if(age_or_length[i] < low) {
			selectivity[i] = 0.0;
		} else if (age_or_length[i] > high){
			selectivity[i] = v[length(v)] * alpha;
		} else {
			value = v[1];
			j = low + 1
			while(j < i) {
			  if (j > high || value >= alpha)
			  	break;
			  value = value + (alpha - value) * v[j - low]; ## might need to check this
			}
			selectivity[i] = value;
		}
	}
	return(selectivity);
}


KnifeEdge = function(age_or_length, alpha = 1, Edge) {
	selectivity = vector();
	for (i in 1:length(age_or_length)) {
		if (age_or_length[i] >= Edge) {
			selectivity[i] = alpha;
		} else {
			selectivity[i] = 0.0;
		}
	}
	return(selectivity);
}


## Generate the plot for the UserManual
Age = seq(1,30,by = 0.02);
par(mfrow = c(2,4), mar = c(0.3,0.3,2,0.2),oma = c(4,4,1,1),xaxt = "n",yaxt = "n")
## constant
plot(Age,rep(1,length(Age)),type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Constant",yaxt = "s")
text(3,0.9,"C = 1")
## knife edge
sel = KnifeEdge(age_or_length = Age, Edge = 8)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Knife-edge")
text(3,0.9,"E = 8")
## logisitic
sel = Logistic(age_or_length = Age, a50 = 13,ato95 = 3)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Logisitic")
legend('topleft',legend = c("a50 = 13","ato95 = 3"))
## Inverse-logisitic
sel = InverseLogistic(age_or_length = Age, a50 = 13,ato95 = 3)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Inverse logisitic")
legend('topright',legend = c("a50 = 13","ato95 = 3"))
## Logistic Producing
#sel = LogisticProducing(age_or_length = Age, a50 = 10,ato95 = 4,low = 7, high = 12)
#plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Logistic producing")
#legend('topleft',legend = c("a50 = 10","ato95 = 4","low = 7", "high = 12"))
## logisitic-capped alpha set to something other than 1
sel = Logistic(age_or_length = Age, a50 = 13,ato95 = 3,alpha = 0.8)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Logisitic-capped",xaxt = "s",yaxt = "s")
legend('topleft',legend = c("a50 = 13","ato95 = 3","alpha = 0.8"))
## Double normal
sel = DoubleNormal(age_or_length = Age, mu = 13,sigma_l = 3,sigma_r = 7)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Double Normal",xaxt = "s")
legend('topright',legend = c("mu = 13","sigma_l = 3","sigma_r = 7"),cex = 0.8)
## Double exponential
sel = DoubleExponential(age_or_length = Age, x0 = 15,x1 = 1,x2 = 30,y0 = 0.1,y1 = 0.5,y2 = 0.8)
plot(Age,sel,type = "l", xlim = c(1,30), ylim = c(0,1.05),main = "Double Exponential",xaxt = "s")
legend('topleft',legend = c("x0 = 15","x1 = 1","x2 = 30","y0 = 0.1","y1 = 0.5","y2 = 0.8"))
##
mtext(side = 1,adj = 0.5, outer = T,"Age",line = 2)