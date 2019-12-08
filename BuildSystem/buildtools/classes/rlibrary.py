import os
import sys
import subprocess
import os.path
import fileinput
import re
import time

import Globals

EX_OK = getattr(os, "EX_OK", 0)

class Rlibrary:
  """
  Build the R library
  """
  def start(self):
    library_name = 'casal2'
    binary_name = 'casal2'
    if Globals.operating_system_ == 'windows':
      binary_name += '.exe'
   
    ## Check Casal2 is in your path. The R script needs it to get versions and that sort of information. Betadiff should be in the system because it is required for
    ## modelrunner class
    if not os.path.exists('bin/' + Globals.operating_system_ + '/release_betadiff/' + binary_name):
      print('Looking for bin/' + Globals.operating_system_ + '/release_betadiff/' + binary_name)
      print('CASAL2 binary was not found. Can not continue')
      print('Please complete a release betadiff binary build before running the models')
      return False
    ## CHG Dir to R library
    os.chdir("../R-libraries/")  
    ## Run the oxygen
    os.system("R --vanilla < run-roxygen.R")   
    ## Build Namespace
    os.system("R --vanilla < CASAL2_make_version.R")
    ## build package
    os.system("R CMD build --force casal2")
    os.system("R CMD INSTALL --build casal2")
    os.system("R CMD check casal2")
    os.system("del casal2.html")
    return True
