/**
 * @file GrowthBased.cpp
 * @author Craig Marsh
 * @github https://github.com/Zaita
 * @date 12/06/2015
 * @section LICENSE
 *
 * Copyright NIWA Science ©2015 - www.niwa.co.nz
 *
 */

// headers
#include "GrowthBased.h"

#include <iostream>
#include <iomanip>

#include "Model/Model.h"
#include "LengthWeights/Manager.h"
#include "Utilities/DoubleCompare.h"
#include "Utilities/To.h"

// namespaces
namespace niwa {
namespace agelengthkeys {

/**
 * Default constructor
 *
 * Bind any parameters that are allowed to be loaded from the configuration files.
 * Set bounds on registered parameters
 * Register any parameters that can be an estimated or utilised in other run modes (e.g profiling, yields, projections etc)
 * Set some initial values
 *
 * Note: The constructor is parsed to generate Latex for the documentation.
 */
GrowthBased::GrowthBased() {
  parameters_.Bind<Double>(PARAM_CLASS_MINIMUMS, &class_mins_, "Length Class Minimums", "")->set_lower_bound(0.0, false);
  parameters_.Bind<string>(PARAM_DISTRIBUTION, &distribution_, "Distribution around each Age length mean","")->set_allowed_values({ PARAM_NORMAL, PARAM_LOGNORMAL });
}


/** Calculate a vector of length frequencies based on a distribution
 * @parm mu , cv = from @age_length
 * @parm *vprop_in_length   = is the output vector that is changed by reference
 * @parm class_mins  is a vector of length minimums feed in from the config file
 * @parm distribution is the distribution around each length at age
 * @parm Class_min_temp is the temporary vector used if transformations are needed e.g log class_mins
 *
 */
void GrowthBased::CummulativeNormal(
	Double          mu, 		    //mean of length distribution
	Double          cv, 		    // cv from @age_length
	vector<Double>  *vprop_in_length,  // fraction of fish in each length class return
	vector<Double>  class_mins,
	string          distribution,
	vector<Double>  Class_min_temp,
	bool 			plus_grp
	)
	{

 // est proportion of age class that are in each length interval
  Class_min_temp = class_mins; 	// Temp vec is a vector that all the work gets done on.
  Double sigma = cv * mu;
  if(distribution == "lognormal"){ // if lognormal, convert growth parameters to log space; use class_mins->is_log
  Double cv     = sigma / mu;
  Double  Lvar  = log(cv*cv + 1.0) ;      //variance in log space
  mu    		= log(mu) - Lvar / 2.0 ; //mean in log space
  sigma 		= sqrt(Lvar);

  if(class_mins[0] < 0.0001) Class_min_temp[0] = log(0.0001) ; else Class_min_temp[0] = log(class_mins[0]);
    if(class_mins.size() > 1) for (unsigned h =1; h < class_mins.size() ; h++)   Class_min_temp[h] = log(class_mins[h]);

  }


 Double z, tt, norm, ttt, tmp,CumN_0,  CumN, CumN_prev, sumN ;
 const Double pi = 3.141592653589793;
 std::vector<int>::size_type sz = class_mins.size();
 vprop_in_length->resize(sz);

 for(unsigned j = 0; j < sz; ++j)
 {
  z = (Class_min_temp[j] - mu) / sigma;
  tt = 1.0/(1.0 + 0.2316419 * z);
  norm = 1.0/sqrt(2 * pi)  * exp( -0.5*z * z);
  ttt  = tt;
  tmp  = 0.319381530 * ttt ;
  ttt  = ttt * tt;
  tmp  = tmp - 0.356563782 * ttt;          // tt^2
  ttt  = ttt * tt;
  tmp  = tmp + 1.781477937 * ttt ;       // tt^3
  ttt  = ttt * tt;
  tmp  = tmp - 1.821255978 * ttt;        // tt^4
  ttt  = ttt * tt;
  tmp  = tmp + 1.330274429 * ttt ;      // tt^5

  CumN = 1.0 - norm*tmp;
  if (Class_min_temp[j] < mu)	CumN = 1.0 - CumN;


  if( (j = 0) ){
  CumN_0 = CumN;
  CumN_prev = CumN;
  } else {

  (*vprop_in_length)[j-1] = CumN - CumN_prev;
  sumN += (*vprop_in_length)[j - 1];
 }
	}
 if(plus_grp){
     (*vprop_in_length)[sz - 1] = 1.0 - sumN - CumN_0 ;    //plus_gp is what is left on the RHS; also need to account for lengths below the first boundary
     } else vprop_in_length->resize(sz - 1);			   // remove last array member as it is not needed

 return ;
 }



/**
 * Do the conversion of the partition structure from age to length
 *
 * @param category The current category to convert
 */

void GrowthBased::DoAgeToLengthConversion(std::shared_ptr<partition::Category> category ) {

  unsigned min_a = Model::Instance()->min_age();
  unsigned max_a = Model::Instance()->max_age();


  for(unsigned age = min_a; age < max_a ; ++age){

  category->UpdateMeanLengthData();


  Double cv = category->age_length()->cv();

  //
  bool plus_grp = Model::Instance()->age_plus();
  CummulativeNormal(category->length_per_[age], cv, &Age_freq_ , class_mins_ , distribution_, Temp_vec, plus_grp);

  // Loop through the length bins and multiple the partition of the current age to go from
  // length frequencies to age length numbers
  for(unsigned i = 0; i < class_mins_.size(); ++i){
  category->length_data_[age][i] =  category->data_[age] * Age_freq_[i];


  }
 }
}

} /* namespace agelengthkeys */
} /* namespace niwa */
