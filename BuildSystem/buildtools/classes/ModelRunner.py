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
  Start the documentation builder
  """
  def start(self):
    binary_name = 'isam'
    if Globals.operating_system_ == 'win32':
      binary_name += '.exe'
    if not os.path.exists('bin/' + Globals.operating_system_ + '/debug/' + binary_name):
      print 'Looking for bin/' + Globals.operating_system_ + '/debug/' + binary_name
      print 'iSAM binary was not found. Can not continue'
      print 'Please complete a debug binary build before running the models'
      return False

    print ''
    success_count = 0
    fail_count = 0
    dir_list = os.listdir("../TestModels/")
    cwd = os.path.normpath(os.getcwd())  
    for folder in dir_list:
      if folder.startswith("."):
        continue
      os.chdir("../TestModels/" + folder)
      start = time.time()
      if os.system("isam -r > run.log 2>&1") != EX_OK:
        elapsed = time.time() - start
        print '[FAILED] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds'
        fail_count += 1
      else:
        elapsed = time.time() - start
        print '[OK] - ' + folder + ' in ' + str(round(elapsed, 2)) + ' seconds'
        success_count += 1
      os.chdir(cwd) 

    print ''
    print 'Total Models: ' + str(success_count + fail_count)
    print 'Failed Models: ' + str(fail_count)
    if fail_count > 0:
      print 'Please check the run.log file in each of the failed model directories'
    return True
