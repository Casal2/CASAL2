import os
import sys
import subprocess
import os.path
import fileinput
import re
import time

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

    if not os.path.exists('bin/' + Globals.operating_system_ + '/release_betadiff/' + binary_name):
      print('Looking for bin/' + Globals.operating_system_ + '/release_betadiff/' + binary_name)
      print('CASAL2 binary was not found. Can not continue')
      print('Please complete a release betadiff binary build before running the models')
      return False
  

    print('')
    success_count = 0
    fail_count = 0
    estimation_dir_list = {"Simple", "TwoSex", "SBW"}
    dash_i_dir_list = {"Complex_input","TwoSex_input","SingleSexTagByLength_input"}
    dir_list = os.listdir("../TestModels/")
    cwd = os.path.normpath(os.getcwd())  
    # test -r functionality with full/different models
    for folder in dir_list:
      if folder in estimation_dir_list:
      	continue
      if folder in dash_i_dir_list:
        continue
      if folder.startswith("."):
        continue
      if folder.startswith("DO NOT"):
        continue
      os.chdir("../TestModels/" + folder)
      
      start = time.time()
      
      result = False;
      if os.system("casal2 -r --loglevel=trace> run.log 2>&1") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds')
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd)   
    # test -i functionality 
    for folder in dash_i_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system("casal2 -r -i pars.out > run.log 2>&1") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' -i run in ' + str(round(elapsed, 2)) + ' seconds')
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' -i run in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 
    # test -e functionality
    for folder in estimation_dir_list:
      os.chdir("../TestModels/" + folder)
      if os.system("casal2 -e -g 20 > estimate.log 2>&1") != EX_OK:
        elapsed = time.time() - start
        print('[FAILED] - ' + folder + ' betadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        fail_count += 1
      else:
        elapsed = time.time() - start
        print('[OK] - ' + folder + ' betadiff estimation in ' + str(round(elapsed, 2)) + ' seconds')
        success_count += 1
      os.chdir(cwd) 
      
    print('')
    print('Total Models: ' + str(success_count + fail_count))
    print('Failed Models: ' + str(fail_count))
    if fail_count > 0:
      print('Please check the run.log or estimate.log file in each of the failed model directories')
      return False
    return True
