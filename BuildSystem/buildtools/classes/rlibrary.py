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
    library_name = 'Casal2'
    ## CHG Dir to R library
    os.chdir("../R-libraries/")  
    ## Run the oxygen
    os.system("R --vanilla < run-roxygen.R")   
    ## Build Namespace
    os.system("R --vanilla < CASAL2_make_version.R")
    ## build package
    os.system("R CMD build --force casal2")
    os.system("R CMD check casal2")
    return True
