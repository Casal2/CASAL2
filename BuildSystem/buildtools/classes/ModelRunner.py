import os
import sys
import subprocess
import os.path
import fileinput
import re
import time
import shutil
import Globals

EX_OK = getattr(os, "EX_OK", 0)

class ModelRunner:
  """
  Start the modelrunner builder
  """
  def start(self):
    binary_name = 'casal2'
    if Globals.operating_system_ == 'windows':
      binary_name += '.exe'
    exe_path = f"Casal2/{binary_name}"

    if not os.path.exists(exe_path):
      print(f"Looking for {exe_path}")
      print('The Casal2 binary (' + binary_name + ') was not found. Cannot continue')
      print('Please complete building the complete end-user application before running the models (i.e., doBuild archive')
      return False

    print('')
    success_count = 0
    fail_count = 0
    ##estimation_dir_list = {"Simple", "SBW"}    # requires a config.csl2 file (uses whatever minimiser is defined)
    estimation_betadiff_dir_list = {"TwoSex", "SBW", "Simple","ComplexTag", "SexedLengthBased"}  # requires a config-betadiff.csl2 file
    estimation_gammadiff_dir_list = {"TwoSex", "SBW", "Simple", "SexedLengthBased"} # requires a config-gammadiff.csl2 file
    estimation_adolc_dir_list = {"TwoSex", "SBW", "Simple", "SexedLengthBased"}     # requires a config-adolc.csl2 file
    simulate_dash_i_dir_list = {"ORH3B"} # if you change this you will need to formulate the report or python code below, not very general.
    run_dash_i_dir_list = {"Complex_input","TwoSex_input"}
    resume_mcmc_from_mpd_dir_list = {"mcmc_start_mpd_mcmc_fixed","mcmc_start_mpd"}
    resume_mcmc_dir_list = {"mcmc_resume"}
    run_dash_I_dir_list = {"SingleSexTagByLength_input"}
    dir_list = os.listdir("../TestModels/")
    cwd = os.path.normpath(os.getcwd())  

    exe_path = f"{cwd}/{exe_path}"
    print(f"--> Full Casal2 path for model runner: {exe_path}")

    # test -r functionality with full/different models
    for folder in dir_list:
      if folder in estimation_betadiff_dir_list:
      	continue
      if folder in estimation_gammadiff_dir_list:
      	continue
      if folder in estimation_adolc_dir_list:
      	continue
      if folder in resume_mcmc_from_mpd_dir_list:
      	continue
      if folder in resume_mcmc_dir_list:
      	continue
      if folder in simulate_dash_i_dir_list:
        continue
      if folder in run_dash_i_dir_list:
        continue
      if folder in run_dash_I_dir_list:
        continue
      if folder.startswith("."):
        continue
      if folder.startswith("DO NOT"):
        continue
		
      os.chdir("../TestModels/" + folder)
      
      start = time.time()
      
      result = False 
      if os.system(f"{exe_path} -r > run.log 2> run.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds')
        print("--> Printing last 20 lines of run.err")
        os.system("tail -n20 run.err")
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd)   
    # test -i functionality 
    for folder in run_dash_i_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -r -i pars.out > run.log 2> run.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -i run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of run.log")
        #os.system("tail -n20 run.log")        
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -i run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 
    # test -I functionality 
    for folder in run_dash_I_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -r -I pars.out > run.log 2> run.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -I run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of run.log")
        #os.system("tail -n20 run.log")
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -I run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 
    # test -s functionality 
    for folder in simulate_dash_i_dir_list:
      os.chdir("../TestModels/" + folder)
	  # create sim directory casal will fail if this doesn't exist
      if not os.path.exists("sim"):
        os.mkdir("sim")	  
      ## first delete any previous simulated observations from previous model runners
      for filename in os.listdir('sim'):
        file_path = os.path.join('sim', filename)
        os.remove(file_path)
      if os.system(f"{exe_path} -s 10 -i samples.1  > multi_sim.log 2> multi_sim.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -s run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of multi_sim.log")
        #os.system("tail -n20 multi_sim.out")        
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -s run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      # check the correct files were generated
      if not os.path.exists("sim/CPUEandes.1_01"):
        print('[FAILED] - ' + folder + ' -s run in ' + str(round(elapsed, 2)) + ' expected simulated file sim/CPUEandes.1_01')
        fail_count += 1
      if not os.path.exists("sim/CPUEandes.9_10"):
        print('[FAILED] - ' + folder + ' -s run in ' + str(round(elapsed, 2)) + ' expected simulated file sim/CPUEandes.9_10')
        fail_count += 1		
      if not os.path.exists("sim/Obs_Andes_LF.3_05"):
        print('[FAILED] - ' + folder + ' -s run in ' + str(round(elapsed, 2)) + ' expected simulated file sim/Obs_Andes_LF.3_05')
        fail_count += 1			
      os.chdir(cwd)
    # test -M mpd.log functionality 
    for folder in resume_mcmc_from_mpd_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -E mpd.log > estimate.log 2> esimate.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -E mpd.log run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of run.log")
        #os.system("tail -n20 mcmc.log")        
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -E mpd.log run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      if os.system(f"{exe_path} -M mpd.log > mcmc.log 2> mcmc.err ") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -M mpd.log run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of run.log")
        #os.system("tail -n20 mcmc.log")        
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -M mpd.log run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd)
    # test -R mpd.log 
    for folder in resume_mcmc_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -R mpd.log --objective-file objectives.1 --sample-file samples.1 > mcmc.log 2>&1") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -R run in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of run.log")
        #os.system("tail -n20 mcmc.log")        
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -R run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd)
    # test -e functionality
    for folder in estimation_betadiff_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -e -g 20 -c config_betadiff.csl2 > estimate_betadiff.log 2> estimate_betadiff.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' betadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of estimate_betadiff.log")
        #os.system("tail -n20 estimate_betadiff.log")       
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' betadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 	  
    for folder in estimation_gammadiff_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -e -g 20 -c config_gammadiff.csl2 > estimate_gammadiff.log 2> estimate_gammadiff.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' gammadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of estimate_gammadiff.log")
        #os.system("tail -n20 estimate_gammadiff.log")       
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' gammadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 	  
    for folder in estimation_adolc_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system(f"{exe_path} -e -g 20 --config config_adolc.csl2 > estimate_adolc.log 2> estimate_adolc.err") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' adolc estimation in ' + str(round(elapsed, 2)) + ' seconds')
        #print("--> Printing last 20 lines of estimate_adolc.log")
        #os.system("tail -n20 estimate_adolc.log")       
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' adolc estimation in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd)  
    print('')
    print('Total Models: ' + str(success_count + fail_count))
    print('Failed Models: ' + str(fail_count))
    if fail_count > 0:
      print('Please check the run.log or estimate.log file in each of the failed model directories')
      return False
    return True

class UnitTests:
  def start(self):
    binary_name = 'casal2'
    if Globals.operating_system_ == 'windows':
      binary_name += '.exe'

    exe_path = f"bin/{Globals.operating_system_}_{Globals.compiler_}/test/{binary_name}"
    cwd = os.path.normpath(os.getcwd())  
    exe_path = f"{cwd}/{exe_path}"

    print(exe_path)
    if not os.path.exists(exe_path):
      print(f"Looking for {exe_path}")
      print('CASAL2 binary was not found. Can not continue')
      print('Please complete a release test binary build before running the models')
      return False
  
    print('')
    start = time.time()
      
    if os.system(f"{exe_path} > run.log 2>&1") != EX_OK:
    #if os.system(f"{exe_path}") != EX_OK:
      elapsed = time.time() - start
      print('[FAILED] in ' + str(round(elapsed, 2)) + ' seconds')
    else:
      elapsed = time.time() - start
      print('[COMPLETED] in ' + str(round(elapsed, 2)) + ' seconds')
    
    os.chdir(cwd)
    return True
    
