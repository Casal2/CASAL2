
#include <iostream>
#include <cstdio>     // For the remove file function
#include <fstream>
#include <string>
#include <sstream>    // For integer to string function
#include "GetPot.h"  // For working with command line arguments (same file as used by Casal2)
#include <unistd.h>
#include <vector>
#include <iomanip>
#include <cctype>  // for isspace()
#include <math.h>
#include <string.h>
#include <cstdlib>

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

//#include <windows.h>
#include "../CASAL2/source/License.h"
#include "version.h"

void CopyFile(const string &input_file, const string &copy_of_file);
void CombineTwoFiles(const string &first_file, const string &second_file, const string &combined_file);
string int2string(int x);
static std::string date_of(const std::string& cvs_id);
static std::string time_of(const std::string& cvs_id);
static void most_recent_timestamp(const vector<string> all_ids, std::string& most_recent_date, std::string& most_recent_time);
static void fatal(const std::string& problem);

int main(int argc, char* argv[]){
  /////////////////////////////////////////////////////////////////////////////////////////////
  //  Read in the command line parameter values
  //////////////////////////////////////////////////////////////////////////////////////////

  GetPot cl(argc,argv);
  // Enable calcuation of elapsed time
  clock_t start_time, end_time;
  start_time = clock();
  std::string version = "1.00";
  std::string simCASAL2_header;
  simCASAL2_header += "\nsimCASAL2";
  // Print the version label, based on svn_version output, from the auto-generated header file 'version.h'
  // giving the latest modification date and time of source files
  simCASAL2_header += "v" + version + "-" + SOURCE_CONTROL_DATE + " " + SOURCE_CONTROL_TIME + " UTC (rev." + int2string(SOURCE_CONTROL_REVISION) + ")  ";
  simCASAL2_header += "(c) Copyright NIWA 2022-\n" ;
  simCASAL2_header += "Call: ";
  for (int i=0; i<argc; i++){
    simCASAL2_header += argv[i];
    simCASAL2_header += " ";
  }
  cout << simCASAL2_header << "\n\n";
  cout.flush();

  if(argc==1||cl.search("-h")||cl.search("-help")){
    cout << endl;
    cout << "simcasal2 is a casal2 add-on utility to assist with operating\nmodel/estimation model simulations with casal2.\n" << endl;
    cout << "The following command line arguments must be supplied:" << "\n\n";
    cout << " -i  input parameter file containing the free parameters from which to\n     generate the operating model simulated data" << endl;
    cout << " -s  number of sets of simulated data sets to generate" << endl;
    cout << " -o  output file name containing the results from the estimation model fits\n     to the simulated data (but cannot be supplied if using -m)" << endl << endl;
    cout << "and at least one of the -e command line arguments for the estimation\nmodel definition:" << "\n\n";
    cout << " -OM casal2 operating model configuration file name (config.csl2)" << endl;
    cout << " -EM casal2 estimation model configuration file name (config.csl2)" << endl;
    cout << " -l  Display the end-user license" << endl;
    cout << " -g  random number generator seed" << endl;
    cout << " -I  input parameter file containing the free parameters from start the estimation process on the estimation model" << endl;
    cout << " -m  use 'casal2 -m' for MCMCs rather than 'casal2 -e' to fit MPDs" << endl;
    exit(0);
  }
  if (cl.search("-l")) {
     // This section takes the text of LICENSE, and reformats by inserting carriage returns
     // so that it looks nice on a 80 character width screen
     const int window_width=80;
     std::string formated_license;
     int count_char=0, space_pos=0;
    for(int i=0;i<license.length();i++){
       formated_license.push_back(license[i]);
       if(count_char < window_width && license[i]!='\n'){
         if(isspace(license[i])) space_pos=formated_license.size();
         count_char++;
       } else if(license[i]=='\n') {
         count_char= 0;
       } else {
         formated_license[space_pos-1]='\n';
         count_char= formated_license.size()-space_pos;
       }
     }
     cout << formated_license << endl;
     exit(0);
   }
  // identify the "CASAL2 -i" component (compulsory argument)
  string mpd_file;
  string mpd_part;
  if (cl.search("-i")){
    mpd_file = cl.next("");
    if (mpd_file == "") 
      fatal("For the command line argument '-i' you haven't given the name of file containing the free parameter values.");
    mpd_part = "-i " + mpd_file + " ";
  } else 
    fatal("You must use the command line argument '-i' to provide the name of the file containing the free parameter values.");

  // identify the "CASAL2 -I" component (optional argument)
  string mpd_start;
  if (cl.search("-I")){
    mpd_start = cl.next("");
    if (mpd_start == "") fatal("For the command line argument '-I' you haven't given the name of file containing the free parameter values.");
    mpd_start = "-i " + mpd_start + " ";
  } else mpd_start = " ";

  // identify the "CASAL2 -s" component (compulsory argument)
  string boot_part;
  if (cl.search("-s")){
    int n_sims = cl.next(-1);
    if (n_sims == -1) fatal("You haven't specified the number of simulations.");
    boot_part = "-s " + int2string(n_sims) + " ";
  } else fatal("You must specify the number of simulations using the '-s' command line argument.");

  // identify the "CASAL2 -o" component (compulsory argument except if -m then not allowed)
  string param_out_file;
  if (cl.search("-o")){
    param_out_file = cl.next("");
    if (param_out_file == "") fatal("For the command line argument '-o' you haven't given the name of the file for the output of the estimation fits.");
    if (cl.search("-m")) 
      fatal("The command line argument '-o' cannot be supplied for MCMCs (i.e., if you also supply '-m').");
  } else {
    fatal("You must use the command line argument '-o' to provide the name of the file for the output of the estimation fits.");
  }

  // identify the "CASAL2 -e/-E" estimation model component (compulsory argument)
  string sim_input_files_prefix;
  string sim_input_files_suffix;
  string sim_prefix_part;
  string sim_suffix_part;
  if (cl.search("-e") || cl.search("-E")){
    if (cl.search("-e")){
      sim_input_files_prefix = cl.next("");
      if (sim_input_files_prefix == "") fatal("You haven't specified the prefix for the estimation model input casal2 files with '-e'.");
      sim_prefix_part = "-f " + sim_input_files_prefix + " ";
    }
  } else fatal("You must use the command line argument -e to provide the names of the estimation model casal2 files.");

  // identify the "CASAL2 -OM" operating model component (optional argument)
  string operating_model_config_file = "config.csl2";
  if (cl.search("-OM")){
    operating_model_config_file = cl.next("");
  }
  string estimation_model_config_file = "config.csl2";
  if (cl.search("-EM")){
    estimation_model_config_file = cl.next("");
  }

  // identify the "CASAL2 -g" random number generator component (optional argument)
  string rng_part;
  if (cl.search("-g")){
    long rng_seed = cl.next(-1);
    if (rng_seed < 0) fatal("You haven't specified the random number seed correctly with '-g'.");
    rng_part = "-g " + int2string(rng_seed) + " ";
  }


  // set exe
  string CASAL2_exe = "casal2";
  bool do_mcmc = false;

  if (cl.search("-m")) 
    do_mcmc = true


  ///////////////////////////////////////////////////////////////////////
  // Ensure that none of the filenames for the input/output are the same
  ///////////////////////////////////////////////////////////////////////

  if(operating_model_config_file == param_out_file )
    fatal("The casal2 ouput file name (-o) cannot be the same as the operating model file name (-OM).");
  if(estimation_model_config_file == param_out_file )
    fatal("The casal2 ouput file name (-o) cannot be the same as the estimation model file name (-EM).");

  ///////////////////////////////////////////////////////////////////////
  // Check the existence of the operating model files, estimation model
  // files, and the MPD file
  ///////////////////////////////////////////////////////////////////////

  { ifstream checkFile(operating_model_config_file.c_str());
    if (!checkFile) fatal("Cannot find operating model file '" + operating_model_config_file + "'");
    checkFile.close();
  }{ifstream checkFile(estimation_model_config_file.c_str());
    if (!checkFile) fatal("Cannot find estimation model file '" + estimation_model_config_file + "'");
    checkFile.close();
  }
  ///////////////////////////////////////////////////////////////////////
  // Generate the simulated observation data from the MPD point estimates
  ///////////////////////////////////////////////////////////////////////

  string run_CASAL2 = CASAL2_exe + boot_part + " " + " -c " + operating_model_config_file + " " + rng_part  + " > temp_sim.log";
  if(quiet) cerr << "\nGenerating simulated data sets\n";
  system(run_CASAL2.c_str());

  string bootstrap_filename = "temp_sim.log";
  ifstream inFile(bootstrap_filename.c_str());
  if (!inFile) fatal("Cannot open '" + bootstrap_filename + "'");

  // Load in simulated data
  string line;
  std::vector <std::string> bootstrap_files;
  int valid_line=0;
  while(getline(inFile,line)) {
    valid_line=1;
    if(line=="Bootstrap filenames:") {
      valid_line=2;
      getline(inFile,line);
      int n=atoi(line.c_str());
      for(int i=0;i<n;++i){
        getline(inFile,line);
        bootstrap_files.push_back(line);
      }
      //break;
    }
  }
  inFile.close();
  unlink(bootstrap_filename.c_str());
  if(valid_line==0) {
    fatal("Unable to locate file '" + bootstrap_filename + "'");
  } else if(valid_line==1) {
    fatal("Unable to locate the bootstrap list from '" + bootstrap_filename + "'. Please check that you are using a version of CASAL2 later than v2.07-2004/12/07");
  }

  //////////////////////////////////////////////////////////////////
  // Now do a fit for each set of simulated observation data
  ///////////////////////////////////////////////////////////////////
  // Check for the existence of the results file .. delete it if it exists
  // (but only if not continuing a previously interrupted run).
  ifstream results_inFile(param_out_file.c_str());
  int line_counter=0;
  if(continue_run==0) {
    if(results_inFile) {
      results_inFile.close();
      unlink(param_out_file.c_str());
    }
  } else {
    if (!results_inFile) {
      fatal("Cannot open '" + param_out_file + "'. Cannot continue run");
    } else {
      cerr << "Continuing previously interrupted run" << endl;
      while(getline(results_inFile,line)) line_counter++;
      line_counter=line_counter-1; // ignore header row.
    }
  }

  string run_CASAL2_stub = CASAL2_exe + " " + mpd_start;

  CopyFile(sim_pop,temp_pop);
  CopyFile(sim_out,temp_out);

  std::vector <int> CASAL2_exit_status;

  if(quiet) cerr << "Estimating model fits:  ";
  string number;
  int logfile_prefix_width;
  logfile_prefix_width = (int) floor(log10((double) bootstrap_files.size()))+1;

  for(int i=0; i<bootstrap_files.size();++i) {
    if(quiet) {
      number = "";
      for(int j=0; j<int2string(i).size(); j++) number = '\b' + number;
      cerr << number << int2string(i+1);
    }
    CombineTwoFiles(sim_est, bootstrap_files[i].c_str(), temp_est);
    std::ostringstream logfile_prefix;
    logfile_prefix << std::setw(logfile_prefix_width) << std::setfill('0') << i+1;
    string outfile = log_files_prefix + "." +  logfile_prefix.str() + ".log";
    string run_CASAL2 = run_CASAL2_stub  + " > " + outfile;
    if(i>=line_counter) CASAL2_exit_status.push_back(system(run_CASAL2.c_str()));
    else CASAL2_exit_status.push_back(-99);
    if(!keep_failures) {
      unlink(bootstrap_files[i].c_str());
    } else {
      if(keep_failures & CASAL2_exit_status[i]==0) {
        unlink(bootstrap_files[i].c_str());
      }
    }
    if(no_log_file & !keep_failures) {
      unlink(outfile.c_str());
    } else {
      if(no_log_file & keep_failures & CASAL2_exit_status[i]==0) {
        unlink(outfile.c_str());
      }
    }
  }

  unlink(temp_pop.c_str());
  unlink(temp_est.c_str());
  unlink(temp_out.c_str());
  if(quiet) cerr << "\nDone ...\n\n";

  int exit_OK=0, exit_one=0, exit_two=0, exit_error=0, exit_unsure=0, exit_system=0;
  int outfile_counter=0;

  for(int i=0; i<CASAL2_exit_status.size();++i) {
    if(CASAL2_exit_status[i]==-1) exit_system++; // System reported an error
    if(CASAL2_exit_status[i]==-99) exit_unsure++; // Continuing a prior run. Unable to determine exit status
    if(CASAL2_exit_status[i]==0) exit_OK++; //CASAL2 completed its task successfully, and if an estimation run, the minimiser reported successful convergence.
    if(CASAL2_exit_status[i]>0) {
      if(CASAL2_exit_status[i]==1) exit_one++; //CASAL2 completed an estimation run, but the minimiser reported that it was unable to determine if the minimiser had converged.
      if(CASAL2_exit_status[i]==2) exit_two++; //CASAL2 completed an estimation run, but the minimiser reported that it failed to obtain convergence.
      if(CASAL2_exit_status[i]>=10) exit_error++; //CASAL2 halted because of a �Fatal error� or a �Betadiff error�
      if(outfile_counter==0) {
        cout << "bootstrap_file exit_status\n";
        outfile_counter=1;
      }
      cout << bootstrap_files[i] << " " << CASAL2_exit_status[i] << "\n";
    }
  }
  cout << "\n";

  cerr << "CASAL2 exit status summary\n" ;
  cerr << "---------------------------------------\n";
  cerr << "  Successfully completed          " << exit_OK << "\n";
  cerr << "  Completed (convergence unclear) " << exit_one << "\n";
  cerr << "  Completed (failed to converge)  " << exit_two << "\n";
  cerr << "  Betadiff/fatal error            " << exit_error << "\n";
  if(continue_run) cerr << "  Continued from a previous run   " << exit_unsure << "\n";
  if(exit_system>0) cerr << "  Unidentified errors           " << exit_system << "\n";
  cerr << "  Total simulations               " << CASAL2_exit_status.size() << "\n";
  cerr << "---------------------------------------\n\n";

  // Calculate Elasped time)
  end_time = clock();
  double elapsed_time = static_cast<double>(end_time-start_time);
  elapsed_time = elapsed_time * 1.0/CLOCKS_PER_SEC/3600;
  int P = (int) floor(log10(elapsed_time))+4;
  cerr << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time==1?" hour":" hours") << ".\n";
  // And exit nicely
  return 0;
  }

//////////////////////////////////////////////////////////////
// Auxiliary functions
////////////////////////////////////////////////////////////////

void CopyFile(const string &input_file, const string &copy_of_file)
{
    // Copying a file: input_file is copied to copy_of_file
    // If copy_of_file already exists it will be overwritten

    char ch;
    ifstream iFile(input_file.c_str());
    ofstream oFile(copy_of_file.c_str());

    if(!iFile) fatal("Cannot open file " + input_file);
    if(!oFile) fatal("Cannot create file " + copy_of_file);

    while (iFile.get(ch))
       oFile.put(ch);
    oFile.flush();

    oFile.close();
    iFile.close();
}

void CombineTwoFiles( const string& first_file, const string& second_file, const string& combined_file)
{
  // Add the second_file onto end of first_file, calling the resultant file combined_file
  // If combined_file already exists it will be overwritten.

  char ch;
  ifstream iFile_first(first_file.c_str());
  ifstream iFile_second(second_file.c_str());
  ofstream oFile_combined(combined_file.c_str());

  if(!iFile_first) fatal("Cannot open file " + first_file);
  if(!iFile_second) fatal("Cannot open file " + second_file);
  if(!oFile_combined) fatal("Cannot create file " + combined_file);

  while(iFile_first.get(ch))
    oFile_combined.put(ch);
  while(iFile_second.get(ch))
    oFile_combined.put(ch);
  oFile_combined.flush();

  iFile_first.close();
  iFile_second.close();
  oFile_combined.close();
}

string int2string(int x)   // Converts an integer to a c++ string
{
  stringstream ss;
  ss << x;
  string s;
  ss >> s;
  return s;
}

static std::string date_of(const std::string& cvs_id){
  // grabs the date out of a CVS $Id field
  istringstream i(cvs_id.c_str());
  std::string id, name, version, date;
  i >> id >> name >> version >> date;
  return date;
}

static std::string time_of(const std::string& cvs_id){
  // grabs the time out of a CVS $Id field
  istringstream i(cvs_id.c_str());
  std::string id, name, version, date, time;
  i >> id >> name >> version >> date >> time;
  return time;
}

static void most_recent_timestamp(const vector<string> all_ids, std::string& most_recent_date, std::string& most_recent_time){
  // all_ids -> vector with cvs_id for each file in the program. This vector is read through and the most recent
  // date and time are returned in the variables most_recent_date, most_recent_time
  most_recent_date = date_of(all_ids[0]);
  most_recent_time = time_of(all_ids[0]);
  string current_date;
  string current_time;
  for (int i=1; i<=all_ids.size()-1; i++){
    current_date = date_of(all_ids[i]);
    current_time = time_of(all_ids[i]);
    if (current_date > most_recent_date){
      most_recent_date = current_date;
      most_recent_time = current_time;
    }
    if (current_date == most_recent_date && current_time > most_recent_time){
      most_recent_time = current_time;
    }
  }
}

static void fatal(const std::string& problem){
 cerr << "Error: " << problem << '\n';
 exit(1);
}
