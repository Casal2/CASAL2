

/*

Bloodshed Dev-C++ 

Test code to get fraction of fish in specified length classes


 Main routines:
 Cum_normal: calculates the probability of lengths being less than or equal to given length boundary
 				uses an approximation to the cumulative normal curve
 Get_age_length: loops through all ages and generates the age-length key (calls Cum_normal)
 
 (1) Test Cum_normal function on one age
 (2) Get length frequency for a test age structure and growth means-at-age
 (3) Write results out and compare to R-code tested results (file "obs_length_results.txt")


Results are identical to the R-code version which were verified using brute force random sampling
except for plus-group in (2) above where the 35+ class using the C++ code gives 0.0545121, but the R-code gives 0.0545122.

*/


// m2 length intervals
// m3 length minimums (either m2 is a plus-group or m2+1 for a cap on the last length interval
// returns an array of probabilities of fish to be in each length interval for a particular age

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

// length class minimums, there are m2 intervals, m2+1 element caps last class if plus_gp = F
								//    m2 intervals if plus_gp = T 
								//    ith class contains lengths that are greater than or equal to class_mins[i] 
								//    and less than, but not including class_mins[i + 1]
class class_mins_t {
	int im3;   //im2 im3 not really needed??
	int im2;


	
	public:
		bool plus_gp ;         // is there a plus group?
		vector <double> vlow_length_boundary; //low length boundary, but is no plue-group then last one is the cap upper length
											// it might be neater to have the cap length recorded seperately
											// and leave vlow_length_boundary pure
		bool is_log =0;			// if lognoramls distribution = true
		vector <double> vlog_low_length_boundary ;
		
		class_mins_t(int i1,int i2,bool b1,vector <double> vec){
			im3 = i1;
			im2 = i2;
			plus_gp = b1;
			vlow_length_boundary = vec ;
		}
		class_mins_t(int i1,int i2,bool b1,vector <double> vec,int b2){//lognormal version
			im3 = i1;
			im2 = i2;
			plus_gp = b1;
			is_log = b2;
			vlow_length_boundary = vec ;
			if(is_log){
				vlog_low_length_boundary.resize(vlow_length_boundary.size());
					// need to trap
				if(vlow_length_boundary[0] < 0.0001) vlog_low_length_boundary[0] = log(0.0001) ; else vlog_low_length_boundary[0] = log(vlow_length_boundary[0]);
  				if(vlow_length_boundary.size() > 1) for (int h =1; h < vlow_length_boundary.size() ; h++) 	vlog_low_length_boundary[h] = log(vlow_length_boundary[h]);
  			 
  				 }   
			}
        };

//------------------------------------------------------------------------------------------
void Cum_normal(
	double          mu, 		    //mean of length distribution
	double          sigma, 		    // standard deviation of length distribution
	string          distribution,   // "normal" or "lognormal"  REDUNANT NOW use class_mins_t.is_log
	class_mins_t    *class_mins,    //lower bounds of length classes
	vector<double>  *vprop_in_length // fraction of fish in each length class
 								  
	)
	{
	
 // est proportion of age class that are in each length interval
  
  vector <double> *class_length = &class_mins->vlow_length_boundary; //points to correct boundaries log or untransformed version
  if(distribution == "lognormal"){ // if lognormal, convert growth parameters to log space; use class_mins->is_log
  double cv     = sigma / mu;
  double  Lvar  = log(cv*cv + 1.0) ;      //variance in log space
  mu    		= log(mu) - Lvar / 2.0 ; //mean in log space
  sigma 		= sqrt(Lvar);
 
    class_length = &class_mins->vlog_low_length_boundary; // point to lognormal version opf boundaries
  }
 
  
 // z = abs( ( class_mins - mu)/sigma)    //normal unit var for values greater then the mean
 vector <double> z    = (*class_length);  //not really needed since it can inside loop below
  std::vector<double>::iterator j;
  cout<<"mu sigma is "<<mu<<" "<<sigma<<endl;
 for(j=z.begin(); j!=z.end(); ++j){
 	
 		*j = abs(*j - mu)/sigma;
 	
 }
 
 
 // tt   = 1.0/(1.0 + 0.2316419 * z)         // variable for expression

  std::vector<int>::size_type sz = z.size(); //array length
  const double pi = 3.141592653589793;
  
  vector <double> tt  (sz);
  for (unsigned k=0; k<sz; k++) tt[k] = 1.0/(1.0 + 0.2316419 * z[k]);
  
 // norm = 1.0/sqrt(2 * pi)  * exp( -z * z/2.0) //normal variate
 vector <double> norm  (sz);
  for (unsigned k=0; k<sz; k++) norm[k] = 1.0/sqrt(2 * pi)  * exp( -0.5*z[k] * z[k]);///sigma; //use erf() instead?
  
  /*
  for( unsigned k=0; k<norm.size(); ++k){ 
        cout << z[k] << " " <<tt[k] <<" "<<norm[k]<<endl ;
    	}
    	cout << endl;
*/

// vector <double> ttt  = (tt);   		// accumulate powers if tt, which is used up to 5th power
 
  //tmp  = 0.319381530 * ttt        // tt, accumulate total expression in tmp
  //cumN 		= 1.0 - norm * tmp final calc for propability is length is larger than mu
// vector <double> tmp  (sz);
 double tmp, ttt;			// tmp acculate total expr., xxx accumulate powers if tt, which is used up to 5th power
 vector <double> cumN (sz);// hold cummulative normal values
 
 for (unsigned k=0; k<sz; k++){
 
 	ttt  = tt[k];
 	tmp  = 0.319381530 * ttt ;
 	
 	ttt  = ttt * tt[k];
 	tmp  = tmp - 0.356563782 * ttt;          // tt^2
 	ttt  = ttt * tt[k];
 	tmp  = tmp + 1.781477937 * ttt ;       // tt^3
 	ttt  = ttt * tt[k];
 	tmp  = tmp - 1.821255978 * ttt;        // tt^4
 	ttt  = ttt * tt[k];
 	tmp  = tmp + 1.330274429 * ttt ;      // tt^5
 
 	cumN[k] = 1.0 - norm[k]*tmp;

	//get values for lengths below the mean could save a subtraction here
	if( (*class_length)[k] < mu)	cumN[k] = 1.0 - cumN[k];
}


 // proportion in length interval
 
 vprop_in_length->resize(sz);
 double sumN = 0.0;  //sum the propbabilities
 for (unsigned k=1; k<sz; k++) {   					// note index starts at 1 not 0
 	(*vprop_in_length)[k-1] = cumN[k] - cumN[k-1];  //get proportions for interval
 	sumN += (*vprop_in_length)[k - 1];
 }
 
 
 if(class_mins->plus_gp){
     (*vprop_in_length)[sz - 1] = 1.0 - sumN - cumN[0] ;//plus_gp is what is left on the RHS; also need to account for lengths below the first boundary
     } else vprop_in_length->resize(sz - 1);			// remove last array member as it is not needed
	 
 //cout<<mu<<"  "<<sigma<<" "<<sumN<<endl;
 return ;
 }

 //------------------------------------
 
 void Get_age_length(
	vector <int> age,          // ages in model
	class_mins_t *class_mins,   // length class minimums details there are m2 intervals, m2+1 element caps last class
								//    ith class contains lengths that are greater than or equal to class_mins[i] 
								//    and less than, but not including class_mins[i + 1]
	vector <double> mu,			// mean length by are (=same length as age)
	vector <double> sigma,		// stardard deviations by age
	string distribution,		// "normal" or "lognomal" for growth distribution REDUNANT use class_mins_t.is_log
	vector <vector <double> > *age_length	// columns are length classes, rows are ages
	)
	{ 
//get age-length conversion matrix

unsigned N_age = mu.size(); //number of ages
if(sigma.size() != N_age ) cout<<"Error: growth sigma nto same size as mu"<<endl;
if(age.size()   != N_age ) cout<<"Error: number of ages not the same size as mu"<<endl;

//age_length->resize((size_t) N_age) ;  //age by row (ist index)


for(unsigned iage=0; iage<N_age; iage++){ 
	
	vector <double> freq;  //EMPTY length freq for an age
	Cum_normal(mu[iage], sigma[iage], distribution, class_mins, &freq);  //#replace iage-row in matrix with length distn for age[iage]

	(*age_length).push_back(freq);  //cummulate rows
}

/*
 for(unsigned k=0; k <(*age_length).size() ; k++){
 	cout<<"k is "<<k<<" ";
 	for(unsigned j=0;j <(*age_length)[k].size() ; j++)cout<<"  "<<(*age_length)[k][j];
 	cout<<endl;
 }
 */

}
//------------------------------------------------------------------------------------------------
	double sum(vector <double> v)
		{			//function for apply; sum values in vector
			double tmp = 0; //accumulate result
			for(unsigned k = 0; k<v.size(); k++) tmp += v[k];
			return tmp;	
		}
		
//------------------------------------------------------------------------------------------------------

void apply(vector <vector <double> > *vec,  // 2D array; ist index is rows
	int side,								//1 apply over rows; 2 down columns
	vector <double> *v_result,				// result to return
	double (*FN)(vector <double> v) = &sum	// function to apply; default is sum
)
{
	// apply function to row or column
	//return a vector <double) 


	
	if(side == 1){   // apply FN to each row
		v_result->resize((*vec).size()) ;  //same length as no. rows
		for(unsigned k=0; k< (*vec).size(); k++) (*v_result)[k] = (*FN)((*vec)[k]);
	} else {		// else apply FN to each column
		v_result->resize((*vec)[0].size()) ;  //same length as no. cols; assume all same length<<<
		
	//	cout<<"apply   <"<<(*vec)[0].size()<<"  "<< (*vec).size()<<endl;
		
		for(unsigned k=0; k< (*vec)[0].size(); k++) {  // k = col = length class
			vector <double> tmp ((*vec).size(), 0.0);  // build vector for FN
			for(unsigned j=0; j<(*vec).size(); j++) tmp[j] = (*vec)[j][k]; // loop over ages; within length class
  			(*v_result)[k] = (*FN)(tmp);
		}
	}
	
	
}
//------------------------------------------------------------------------------------------------

int main()
{
	
	
//
// test Cum_normal on one age
// example of fraction of fish for age with no plus-group
//

//class mins for example
vector <double> vtmp = {0,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
int  i = vtmp.size();
class_mins_t L_min1 (i,i - 1,0,vtmp,1); // add log normals as well since it uses same structure; needs last ",1"

vector <double> prop_length_age3 ((size_t) i);
Cum_normal(35.4985776819425 ,  3.54985776819426, "normal", &L_min1,&prop_length_age3);  



vector <double> prop_length_age3log ((size_t) i);
Cum_normal(35.4985776819425 ,  3.54985776819426, "lognormal", &L_min1,&prop_length_age3log);
    	
//
// test full LF from 10 ages with plus-group from 35 onwards 
//

// 

// ages and number of fish by age
vector <double> vage    = {1,2,3,4,5,6,7,8,9,10};
vector <double> vNatAge = {904.83741803596, 0, 740.818220681718, 0, 606.530659712633, 0, 496.585303791409, 0, 406.569659740599, 0}; //freq for fish by age
 										
								

 //growth @age_length e.g type= von_bertalanffy
 vector <int> iage = {1,2,3,4,5,6,7,8,9,10};
vector <double> mu = {16.4982193,20.7014,23.9748374,26.52419,28.5096355,30.0559,31.2601296,32.19799,32.9283886,33.49723}; // length means by age
vector <double> sigma = mu;
for(unsigned k=0; k<mu.size(); k++) sigma[k] *= 0.1; // 10% cv convert to standard deviation

//for(unsigned j=0;j<vage.size();++j){
//	vNatAge[j] *= 1000.0 * exp(-0.2 * vage[j]);   // natural moratily of 10% with recruits at 1000
//	}

//class mins with  plus-group

vector <double> vtmp2 = {0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35};

int  i2 = vtmp2.size();
class_mins_t L_min2 ={i2,i2 - 1,1,vtmp2};

vector <vector <double> > age_length; //age length key matrix; age are rows

 Get_age_length(iage, &L_min2, mu, sigma, "normal",&age_length); // returns age-length key
 
 //
 //now get proportions oF fish in each age-length cell using vNatAge (sums to 1 here, but it would sum to population size in CAasal2)
 //
 
 vector <vector <double> > pop_age_length = age_length;
 
 for(unsigned k=0; k <age_length.size() ; k++){
 	double tmp2 = vNatAge[k];
 	for(unsigned j=0;j <age_length[k].size() ; j++) pop_age_length[k][j] *= tmp2;
 	}
 
 
 // add within length class to get LF
 
 vector <double> length_freq;
 apply(&pop_age_length,2,&length_freq);  //sum over columns= within length class = pop by length class
 
 double sum_pop = sum(length_freq); //sum up to get frequency

 for(unsigned k=0; k <length_freq.size() ; k++) length_freq[k] /= sum_pop;  // sums to 1 i.e., a frequency
 
 //
 // (1) length_freq can now be put into the likelihood with the observed length frequency
 // (2) pop_age_length can have some length based process applied (like selectivity) and
 //       then results added along rows (age) to get the resultant population by age
 //       to be used in an age frequency observation or abundance observation, or shifted into another 
 //       partition (e.g., migration, maturation), or used in a derived variable calculation.
 //

 
 
 
 ///
 // report results
 //
  
	ofstream report;
    report.open("obs_length_results.txt");
    report << endl<<"LF for age 39, no plus-group "<<endl;
	report <<"lower boundary ----- C++ LF ------- R code LF"<<endl;
	
	vector <double> R_freq_age3 = 	{3.87135989932119e-09,1.59602436822226e-08,7.42237110662458e-08,3.1902009711704e-07,1.26726413218758e-06,4.65254798021064e-06,1.57866304445475e-05,4.95065258897531e-05,0.000143485744269256,0.000384349717516996,0.000951510394033628,0.00217704263860841,0.00460345520851202,0.00899625855523312,0.0162479702921859,0.0271203213695711,0.0418359735963918,0.0596439364349164,0.0785861889422703,0.0956951456376467,0.107694917259926,0.112012167809337,0.107670770510254,0.0956522357149099,0.0785333373186126,0.0595904596632406,0.041789090954766,0.027083854716181,0.0162224840640299,0.00898013290693078,0.00459417312498966,0.00217216582268553,0.000949166040997707};
    vector <double> R_freq_age3log =   { 0, 9.99200722162641e-16,1.13908882326541e-13,6.90780765921772e-12,2.48631892851847e-10,5.68088787122889e-09,8.71922248846246e-08,9.42697598094178e-07,7.47452744942478e-06,4.49824995761849e-05,0.00021163782147382,0.000798629648607996,0.00247155801717358,0.00639629717310153,0.0140816369376855,0.0267735595455451,0.0445555823085991,0.0656767367983564,0.086665295092453,0.103353266194502,0.11234200996379,0.112157064654696,0.103555181952736,0.0889789559049539,0.0715523628199436,0.054126561342811,0.0386965094084698,0.0262574405418146,0.0169760331592279,0.0104945579643262,0.00622370060274335,0.00355129426114842,0.00195510620993899};
    for( unsigned k=0; k<prop_length_age3.size(); ++k){ 
        report  << L_min1.vlow_length_boundary[k] << " " <<prop_length_age3[k] <<"  "<<R_freq_age3[k]<<endl ;
    	}
    	report  << endl<< endl<< endl;
        report << endl<<"LF for age 39, no plus-group Log Normal Distribution "<<endl;
	report <<"lower boundary (log ; untransformed)----- C++ LF ------- R code LF"<<endl;	
    	for( unsigned k=0; k<prop_length_age3.size(); ++k){ 
        report  << L_min1.vlog_low_length_boundary[k]<<":"<<L_min1.vlow_length_boundary[k] << " " <<prop_length_age3log[k] <<"  "<<R_freq_age3log[k]<<endl ;
    	}
    		report  << endl<< endl<< endl;
		// results from R code for multi-age example
 vector <double> R_freq = 	{1.175016e-05,0.000111631,0.0007945247,0.003954113,0.01376653,
0.03354821,0.05727003,0.06862156,0.05819813,0.03648005,
0.02089097,0.01831591,0.02527293,0.0357812,0.0450688,
0.05050628,0.05215012,0.05206139,0.05231438,0.05338999,
0.05421289,0.05332755,0.04993315,0.04414566,0.03671602,
 0.02864407,0.05451215};
   
    	
		
		report << length_freq.size()<<"  "<<R_freq.size()<<endl;
		report << " Combined length frequency for 10 ages, with plus-group example "<<endl;
    	report << "Length lower boundary -- C++ freq -- R freq "<<endl;
    	
     for( unsigned k=0; k<length_freq.size(); ++k){ 
        report << L_min2.vlow_length_boundary[k] << " " <<length_freq[k] << " "<<R_freq[k] <<" " <<endl ;
    	}
    	report << endl;
    	report.close();
	return 0;
}

