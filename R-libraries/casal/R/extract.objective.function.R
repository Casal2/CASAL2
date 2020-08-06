# $Id: extract.objective.function.R 1845 2007-12-17 20:47:02Z adunn $
"extract.objective.function"<-
function(file,path=""){
  objective <- list()
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  if(all(casal.regexpr("Start extracting output from here",file)<0)) stop(paste("No output data found in file '",filename,"'",sep=""))
  file <- casal.get.lines(file,clip.to="Start extracting output from here")
  objective$value <- as.numeric(casal.remove.first.words(casal.get.lines(file,starts.with="Objective function :")[1],3))
  file <- casal.get.lines(file,clip.to.match="Components :",clip.from.match="Fits :")
  n.components <- length(file)
  objective$components <- data.frame(matrix(0,n.components,2))
  names(objective$components) <- c("label","value")
  for (i in 1:n.components){
    objective$components[i,1] <- casal.string.to.vector.of.words(file[i])[1]
    objective$components[i,2] <- as.numeric(casal.string.to.vector.of.words(file[i])[2])
  }
  # Pull in likelihoods from @age_size output (which were first outputted in CASAL v2)
  file <- casal.convert.to.lines(filename)
  observations <- casal.get.lines(file, clip.to = "The observations are:", clip.from = "Fits follow:")
  obs.data <- casal.get.lines(file, clip.to = "Start extracting output from here", clip.from = "Output quantities start here")

  for(i in 1:length(observations)){
         obs.name <- observations[i]
         # the observation data description is the first entry
         temp  <- casal.get.lines(obs.data, clip.to.match = obs.name, clip.from.match = "total neg_log_likelihood")
         if( casal.regexp.in(temp[1], "age-size") ){
               temp  <- casal.get.lines(temp,from = 2) # leave just the headers for the quantities
               this.data <- casal.make.table(temp)
               objective[[obs.name]] <- list()
               objective[[obs.name]]$age  <-   this.data$age
               objective[[obs.name]]$size <-   this.data$size
               objective[[obs.name]]$sex  <-   this.data$sex
               objective[[obs.name]]$value  <- this.data$"neg_log_likelihood"
         }
  }

  return(objective)
}
