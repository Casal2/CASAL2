# Get CASAL2 Version (see 'doBuild version')
source("Version.R")

# Build DESCRIPTION file
filename<-"Casal2/DESCRIPTION"
cat("Package: Casal2\nTitle: Casal2 extract package\nVersion: ",file=filename)
cat(VersionNumber,file=filename,append=TRUE)
cat("\nDate: ",file=filename,append=TRUE)
cat(SourceControlDateDoc,file=filename,append=TRUE)
cat("\n",file=filename,append=TRUE)
cat("Author: Casal2 Development Team\n",file=filename,append=TRUE)
cat("Description: A set of R functions for extracting and plotting from Casal2 configuration input files, reports, and other associated files.\n",file=filename,append=TRUE)
cat("Maintainer: Casal2 Development Team <casal2@niwa.co.nz>\n",file=filename,append=TRUE)
cat("License: GPL-2\n",file=filename,append=TRUE)
cat("URL: https://www.niwa.co.nz\n",file=filename,append=TRUE)
cat("Copyright: National Institute of Water & Atmospheric Research, Ltd. (NIWA), New Zealand Ministry for Primary Industries\n",file=filename,append=TRUE)
cat("LazyData: true\n",file=filename,append=TRUE)
cat("BugReports: https://github.com/NIWAFisheriesModelling/CASAL2/issues\n",file=filename,append=TRUE)


# Create R function to return version number
filename<-"Casal2/R/Casal2.binary.version.R"
cat("\"Casal2.binary.version\"<-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("return(\"",VersionNumber,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Create R function to return R library version number
filename<-"Casal2/R/Version.R"
cat("\"Version\" <-\n",file=filename)
cat("function() {\n",file=filename,append=T)
cat(paste("  return(\"",VersionNumber,"\")\n",sep=""),file=filename,append=T)
cat("}\n",file=filename,append=T)

# Exit
q()
