# $Id: extract.fits.R 4636 2012-03-19 23:48:49Z Dunn $
"extract.fits" <-
function(file,path=""){
  fits <- list()
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  file <- casal.convert.to.lines(filename)
  if(all(casal.regexpr("Start extracting output from here",file)<0)) stop(paste("No output data found in file '",filename,"'",sep=""))
  file <- casal.get.lines(file,clip.to="Start extracting output from here")
  file <- casal.get.lines(file,clip.to="The observations are:")
  observations <- casal.get.lines(file,clip.from="Fits follow:")
  file <- casal.get.lines(file,clip.to="Fits follow:")
#  For version v2 of CASAL the output for @age_size does not include fits.
#  Therefore the age-size observation(s) should be excluded when fits are extracted
#  Note that there can be more than one @age_size block
  which.is.age.size.v2 <- NULL
  for(i in 1:length(observations)){
    obs.description <- casal.get.lines(file, clip.to.match = observations[i])
    if(casal.regexp.in(obs.description[1], "age-size")){
      which.is.age.size.v2 <- rbind(which.is.age.size.v2,i)
    }
  }
  if(mode(which.is.age.size.v2) != "NULL"){
    for(i in length(which.is.age.size.v2):1){
      observations <- observations[ observations != observations[which.is.age.size.v2[i]] ]
    }
  }
  for(i in 1:length(observations)){
    this.observation <- observations[i]
    fits[[this.observation]] <- list()
    file <- casal.get.lines(file,clip.to.match=this.observation)
    abundance <- casal.regexp.in(file[1],"abundance") # format is different for abundance than all other observations so far
    age.size <- casal.regexp.in(file[1],"age / size") # format is different for abundance than all other observations so far
    tag.recapture <- casal.regexp.in(file[1],"tag-recapture") # format is different for tag-recapture than all other observations so far
    if(i<length(observations)) {
      if(!age.size && !tag.recapture) temp <- casal.get.lines(file,from=2,clip.from.match=observations[i+1])
      else temp <- casal.get.lines(file,from=2,clip.from.match="objective function:")
    } else {
      temp <- casal.get.lines(file,from=2,to=-1)
      if(!abundance && !age.size && !tag.recapture){
        temp2 <- casal.get.lines(temp,clip.to.match="obs",clip.from.match="fits")
        n.lines <- nrow(casal.make.table(temp2))
        temp <- casal.get.lines(file,from=2,to=casal.pos.match(file,"objective function:")+n.lines-1)
      }
    }
    if(abundance){
      temp2 <- casal.get.lines(temp,from=1,clip.from.match="objective function:")
      temp3 <- casal.get.lines(temp,clip.to.match="objective function:")
      data <- casal.make.table(temp2)
      fits[[this.observation]]$year <- data$year
      fits[[this.observation]]$obs <- data$obs
      fits[[this.observation]]$fits <- data$fits
      if(casal.regexp.in(names(data),"resids")){
        fits[[this.observation]]$resids <- data$resids}
      if(casal.regexp.in(names(data),"Pearson_resids")){
        fits[[this.observation]]$pearson.resids <- data[["Pearson_resids"]]}
      if(casal.regexp.in(names(data),"Normalised_resids")){
        fits[[this.observation]]$normalised.resids <- data[["Normalised_resids"]]}
      if(casal.regexp.in(names(data),"Normalised_resids")){
        fits[[this.observation]]$normalised.resids <- data[["Normalised_resids"]]}
      fits[[this.observation]]$error.value <- casal.string.to.vector.of.numbers(temp3[1:length(data$year)])
    } else if(age.size){
      temp2 <- casal.get.lines(temp,clip.to.match="obs",clip.from.match="fits")
      fits[[this.observation]]$obs <- casal.string.to.vector.of.numbers(temp2)
      temp2 <- casal.get.lines(temp,clip.to.match="fits",clip.from.match="resids")
      fits[[this.observation]]$fits <- casal.string.to.vector.of.numbers(temp2)
      if(casal.regexp.in(temp,"^resids")){
        temp2 <- casal.get.lines(temp,clip.to.match="^resids",clip.from.match="resids")
        fits[[this.observation]]$resids <- casal.string.to.vector.of.numbers(temp2)
      }
      if(casal.regexp.in(temp,"pearson_resids")){
        temp2 <- casal.get.lines(temp,clip.to.match="pearson_resids",clip.from.match="resids")
        fits[[this.observation]]$pearson.resids <- casal.string.to.vector.of.numbers(temp2)
      }
      if(casal.regexp.in(temp,"normalised_resids")){
        temp2 <- casal.get.lines(temp,clip.to.match="normalised_resids")
        fits[[this.observation]]$normalised.resids <- casal.string.to.vector.of.numbers(temp2)
      }
    } else if(tag.recapture){
      if(i==length(observations)) temp2<-casal.get.lines(temp,clip.from="Output quantities start here")
      else temp2 <- casal.get.lines(temp,clip.from.match=observations[i+1])
      is.sexed<-substring(temp2[2],1,3)=="sex"
      tag.index<-(1:length(temp2))[casal.regexpr("total neg_log_likelihood:",temp2)>0]
      fits[[this.observation]]<-list()
      for(j in 1:length(tag.index)) {
        temp3 <- casal.get.lines(temp2,from=ifelse(j>1,tag.index[j-1]+1,1),to=ifelse(j>1,tag.index[j]-tag.index[j-1],tag.index[j]))
        tag.year<-as.character(as.numeric(substring(temp3[1],5)))
        if(is.sexed) {
          temp4<-casal.string.to.vector.of.words(temp3[-c(1,2,length(temp3))])
          temp4<-temp4[-seq(1,length(temp4),by=6)]
          sex<-substring(temp3[-c(1,2,length(temp3))],1,1)
          temp4<-t(matrix(as.numeric(temp4),ncol=5,byrow=T))
          dimnames(temp4)<-list(casal.string.to.vector.of.words(temp3[2])[-1],temp4[1,])
          temp4<-temp4[-1,]
          male<-temp4[,sex=="M"]
          female<-temp4[,sex=="F"]
          fits[[this.observation]][[tag.year]]<-list("male"=male,"female"=female)
        } else {
          temp4<-casal.string.to.vector.of.numbers(temp3[-c(1,2,length(temp3))])
          temp4<-t(matrix(temp4,ncol=5,byrow=T))
          dimnames(temp4)<-list(casal.string.to.vector.of.words(temp3[2]),temp4[1,])
          temp4<-temp4[-1,]
          fits[[this.observation]][[tag.year]]<-temp4
        }
      }
    } else if(!abundance && !age.size && !tag.recapture){
      temp2 <- casal.get.lines(temp,clip.to.match="obs",clip.from.match="fits")
      columns<-temp2[1]
      obs.table <- casal.make.table(temp2)
      fits[[this.observation]]$year <- obs.table[,1]
      fits[[this.observation]]$obs <- data.frame(obs.table[,-1])
      row.names(fits[[this.observation]]$obs) <- obs.table[,1]
      if(casal.regexp.in(temp, "resids")) temp2 <- casal.get.lines(temp, clip.to.match = "fits", clip.from.match = "resids")
      else temp2 <- casal.get.lines(temp, clip.to.match = "fits", clip.from.match = "objective")
      fits.table <- casal.make.table(temp2)
      fits[[this.observation]]$fits <- data.frame(fits.table[,-1])
      row.names(fits[[this.observation]]$fits) <- obs.table[,1]
      if(casal.regexp.in(temp,"^resids")){
        temp2 <- casal.get.lines(temp,clip.to.match="^resids",clip.from.match="resids")
        if(length(temp2)!=nrow(fits[[this.observation]]$obs))
          temp2 <- temp2[1:(nrow(fits[[this.observation]]$obs)+1)]
        resids.table <- casal.make.table(temp2)
        fits[[this.observation]]$resids <- data.frame(resids.table[,-1])
        row.names(fits[[this.observation]]$resids) <- resids.table[,1]
      }
      if(casal.regexp.in(temp,"pearson_resids")){
        if(casal.regexp.in(temp,"normalised_resids"))
          temp2 <- casal.get.lines(temp,clip.to.match="pearson_resids",clip.from.match="resids")
        else temp2 <- casal.get.lines(temp,clip.to.match="pearson_resids",clip.from.match="objective")
        pearson.resids.table <- casal.make.table(temp2)
        fits[[this.observation]]$pearson.resids <- data.frame(pearson.resids.table[,-1])
        row.names(fits[[this.observation]]$pearson.resids) <- pearson.resids.table[,1]
      }
      if(casal.regexp.in(temp,"normalised_resids")){
        temp2 <- casal.get.lines(temp,clip.to.match="normalised_resids",clip.from.match="objective")
        normalised.resids.table <- casal.make.table(temp2)
        fits[[this.observation]]$normalised.resids <- data.frame(normalised.resids.table[,-1])
        row.names(fits[[this.observation]]$normalised.resids) <- normalised.resids.table[,1]
      }
      if(casal.regexp.in(temp,"objective")){
        temp2 <- casal.get.lines(temp,clip.to.match="objective")
        if(!casal.regexp.in(temp,"user-supplied likelihood")){
          temp2 <- temp2[1:(nrow(fits[[this.observation]]$obs))]
          columns <- paste(casal.string.to.vector.of.words(columns)[-1],collapse = " ")
          error.value.table <- casal.make.table(c(columns,temp2))
          fits[[this.observation]]$error.value <-  data.frame(error.value.table)
          row.names(fits[[this.observation]]$error.value) <- obs.table[, 1]
        }
      }
    }
  }
  return(fits)
}
