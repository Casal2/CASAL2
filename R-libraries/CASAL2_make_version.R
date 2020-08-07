# Get CASAL2 Version
# the release_betadiff executable should already exist
VERSION <- ""
if (Sys.info()['sysname'] == 'Windows') {
  VERSION<-system("../BuildSystem/bin/windows/release_betadiff/casal2.exe -v",intern=TRUE)
} else {
  VERSION<-system("../BuildSystem/bin/linux/release_betadiff/casal2 -v",intern=TRUE)
}

VERSION<-substring(VERSION,1,regexpr(" ",VERSION)-1)
version.number<-"1.0" #substring(VERSION,1,regexpr("-",VERSION)-1)
version.date<- "2020-06-30" #substring(VERSION,regexpr("-",VERSION)+1)

# Build DESCRIPTION file
filename<-"casal2/DESCRIPTION"
cat("Package: casal2\nTitle: casal2 extract package\nVersion: ",file=filename)
cat(version.number,file=filename,append=TRUE)
cat("\nDate: ",file=filename,append=TRUE)
cat(version.date,file=filename,append=TRUE)
cat("\n",file=filename,append=TRUE)
cat("Author: D. Fu and C. Marsh\n",file=filename,append=TRUE)
cat("Description: A set of R functions for extracting and plotting from Casal2 output files.\n",file=filename,append=TRUE)
cat("Maintainer: Casal2 development team <casal2@niwa.co.nz>\n",file=filename,append=TRUE)
cat("License: GPL-2\n",file=filename,append=TRUE)
cat("URL: https://www.niwa.co.nz\n",file=filename,append=TRUE)
cat("Copyright: National Institute of Water & Atmospheric Research, Ltd. (NIWA), New Zealand Ministry for Primary Industries\n",file=filename,append=TRUE)
cat("LazyData: true\n",file=filename,append=TRUE)
cat("BugReports: https://github.com/NIWAFisheriesModelling/CASAL2/issues\n",file=filename,append=TRUE)


# Create R function to return version number
filename<-"casal2/R/CASAL2.binary.version.R"
cat("\"CASAL2.binary.version\"<-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("return(\"",VERSION,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Create R function to return R library version number
filename<-"casal2/R/Version.R"
cat("\"Version\"<-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("return(\"",version.date,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Write a .html file to report version number for the Wiki
cat(paste("Version",VERSION),file="casal2.html")

# Exit
q()

