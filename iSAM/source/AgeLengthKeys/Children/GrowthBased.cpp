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

/**
 * Do the conversion of the partition structure from age to length
 *
 * @param category The current category to convert
 */
void GrowthBased::DoAgeToLengthConversion(std::shared_ptr<partition::Category> category) {

  /**
   * Age Partition structure is in:
   */
//  category->data_;

  // Age length object is in
//  category->age_length_;

  // Length/Age Matrix
//  category->length_data_;

  /**
   * Goal of this method is to take data from category->data_ and load category->length_data_;
   */
 }

/**
 *
 */
//Double GrowthBased::CummulativeNormal(
//	Double          mu, 		    //mean of length distribution
//	Double          sigma, 		    // standard deviation of length distribution
//	vector<Double>  *vprop_in_length  // fraction of fish in each length class return
//
//	)
//	{
//
// // est proportion of age class that are in each length interval
//  vector <Double> *class_length = &class_mins->vlow_length_boundary; //points to correct boundaries log or untransformed version
//  if(distribution == "lognormal"){ // if lognormal, convert growth parameters to log space; use class_mins->is_log
//  Double cv     = sigma / mu;
//  Double  Lvar  = log(cv*cv + 1.0) ;      //variance in log space
//  mu    		= log(mu) - Lvar / 2.0 ; //mean in log space
//  sigma 		= sqrt(Lvar);
//
//    class_length = &class_mins->vlog_low_length_boundary; // point to lognormal version opf boundaries
//  }
//
//
// // z = abs( ( class_mins - mu)/sigma)    //normal unit var for values greater then the mean
// vector <Double> z    = (*class_length);  //not really needed since it can inside loop below
//  std::vector<Double>::iterator j;
// // cout<<"mu sigma is "<<mu<<" "<<sigma<<endl;
// for(j=z.begin(); j!=z.end(); ++j){
//
// 		*j = abs(*j - mu)/sigma;
//
// }
//
//  std::vector<int>::size_type sz = z.size(); //array length
//  const Double pi = 3.141592653589793;  // This will be defined somewhere surely
//
//  vector <Double> tt  (sz);
//  for (unsigned k=0; k<sz; k++) tt[k] = 1.0/(1.0 + 0.2316419 * z[k]);
//
//
// vector <Double> norm  (sz);
//  for (unsigned k=0; k<sz; k++) norm[k] = 1.0/sqrt(2 * pi)  * exp( -0.5*z[k] * z[k]);
//
//
// Double tmp, ttt;			// tmp acculate total expr., xxx accumulate powers if tt, which is used up to 5th power
// vector <Double> cumN (sz);// hold cummulative normal values
//
// for (unsigned k=0; k<sz; k++){
//
// 	ttt  = tt[k];
// 	tmp  = 0.319381530 * ttt ;
//
// 	ttt  = ttt * tt[k];
// 	tmp  = tmp - 0.356563782 * ttt;          // tt^2
// 	ttt  = ttt * tt[k];
// 	tmp  = tmp + 1.781477937 * ttt ;       // tt^3
// 	ttt  = ttt * tt[k];
// 	tmp  = tmp - 1.821255978 * ttt;        // tt^4
// 	ttt  = ttt * tt[k];
// 	tmp  = tmp + 1.330274429 * ttt ;      // tt^5
//
// 	cumN[k] = 1.0 - norm[k]*tmp;
//
//	//get values for lengths below the mean could save a subtraction here
//	if( (*class_length)[k] < mu)	cumN[k] = 1.0 - cumN[k];
//}
//
//
// // proportion in length interval
//
// vprop_in_length->resize(sz);
// Double sumN = 0.0;  //sum the propbabilities
// for (unsigned k=1; k<sz; k++) {   					// note index starts at 1 not 0
// 	(*vprop_in_length)[k-1] = cumN[k] - cumN[k-1];  //get proportions for interval
// 	sumN += (*vprop_in_length)[k - 1];
// }
//
//
// if(class_mins->plus_gp){
//     (*vprop_in_length)[sz - 1] = 1.0 - sumN - cumN[0] ;//plus_gp is what is left on the RHS; also need to account for lengths below the first boundary
//     } else vprop_in_length->resize(sz - 1);			// remove last array member as it is not needed
//
// //cout<<mu<<"  "<<sigma<<" "<<sumN<<endl;
// return ;
// }
//
//
//// Now I need to obtain the ages that the exist for the category/partition
//// I need to call in @age_length which will output mean and cv of length for that age
//// This would be how you evaluate minimum age
//
//for(unsigned k = Model::Instance()->min_age(); k < Model::Instance()->max_age() + 1 ; ++k){
//	@age_lenth(k)		// We need a mean and standard deviation out of this for each age class
//}
//
//// ALK
//// Set up a matrix that is length(age) by length(length) .... note there could be a plus group account for that
//// Then we populate the matrix by iterating through the age classes applying the Cum normal function
//// For that given mu and cv from @age_length
//
//if(Plus_grp_)
//	Im2_ = Im2_ + 1;
//
//	// Build a matrix of age_ (rows) by Im2_(cols)
//	for(unsigned i=0; i < max_age ; ++i){
//		mat[iage,] = Cum_Normal(mu, sigma, distribution, class_mins, plus_grp) // This should in theory return a vector of proportionat length
//	}
//
//
//
//
//
//
//
//
//
//
//
//
//// - ------------------------- Same as Data.cpp from here on
///**
// * Build any objects that will need to be utilised by this object.
// * Obtain smart_pointers to any objects that will be used by this object.
//
//void Data::DoBuild() {
//  if (!data_table_)
//    LOG_CODE_ERROR() << "!data_table_";
//
//  unsigned length_bins_count = Model::Instance()->length_bins().size();
//
//
//   * Build our data_by_year map so we can fill the gaps
//   * and use it in the model
//   */
//  vector<vector<string>>& data = data_table_->data();
//  for (vector<string> row : data) {
//    if (row.size() != length_bins_count + 1) {
//      LOG_ERROR_P(PARAM_DATA) << "row data count (" << row.size() << ") must be <age> <length> <length n> for a total of " << length_bins_count + 1 << " columns";
//      return;
//    }
//
//    unsigned age = utilities::ToInline<string, unsigned>(row[0]);
//    for (unsigned i = 1; i < row.size(); ++i) {
//      lookup_table_[age][i - 1] = utilities::ToInline<string, Double>(row[i]);
//    }
//  }
//
//  // rescale the data to ensure it's a proportion
//  unsigned row_index = 0;
//  for (auto row : lookup_table_) {
//    ++row_index;
//    Double total = 0.0;
//    for (auto iter : row.second)
//      total += iter.second;
//
//    if (!utilities::doublecompare::IsOne(total)) {
//      LOG_WARNING() << location() << "row " << row_index << " in data table sums to " << total << ". This should sum to 1.0. Auto re-scaling";
//      for (auto& iter : row.second)
//        iter.second /= total;
//    }
//  }
//}

} /* namespace agelengthkeys */
} /* namespace niwa */
