# Get iSAM Version
VERSION<-system("iSAM.exe -v",intern=TRUE)
VERSION<-substring(VERSION,1,regexpr(" ",VERSION)-1)
version.number<-"1.0" #substring(VERSION,1,regexpr("-",VERSION)-1)
version.date<- "2015-06-10" #substring(VERSION,regexpr("-",VERSION)+1)

# Build DESCRIPTION file
filename<-"iSAM/DESCRIPTION"
cat("Package: iSAM\nTitle: iSAM extract package\nVersion: ",file=filename)
cat(version.number,file=filename,append=TRUE)
cat("\nDate: ",file=filename,append=TRUE)
cat(version.date,file=filename,append=TRUE)
cat("\n",file=filename,append=TRUE)
cat("Author: D. Fu\n",file=filename,append=TRUE)
cat("Description: A set of R functions for extracting from iSAM output files.\n",file=filename,append=TRUE)
cat("Maintainer: D Fu <D.Fu@niwa.co.nz>\n",file=filename,append=TRUE)
cat("License: CPL v1.0. See the iSAM User Manual for license details.\n",file=filename,append=TRUE)
cat("URL: http://www.niwa.co.nz\n",file=filename,append=TRUE)
cat("Copyright: National Institute of Water & Atmospheric Research (NIWA), New Zealand Ministry for Primary Industries.\n",file=filename,append=TRUE)


# Create R function to return version number
filename<-"iSAM/R/iSAM.binary.version.R"
cat("\"iSAM.binary.version\"<-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("return(\"",VERSION,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Write a .html file to report version number for the Wiki
cat(paste("Version",VERSION),file="iSAM.html")

# Exit
q()

