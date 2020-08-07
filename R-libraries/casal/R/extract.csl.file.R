# $Id: extract.csl.file.R 1830 2007-11-30 01:37:18Z adunn $
"extract.csl.file"<-
function(file, path="") {
  csl.commands<-csl.commands
  csl.commands$count<-rep(0,length(csl.commands$command))
  set.class <- function(object, new.class) {
    # use in the form
    # object <- set.class(object,"new class")
    attributes(object)$class <- c(new.class, attributes(object)$class[attributes(object)$class != new.class])
    object
  }
  if(missing(path)) path<-""
  filename<-casal.make.filename(path=path,file=file)
  res <- casal.convert.to.lines(filename)
  # remove any initial spaces from any lines
  while(any(casal.regexpr(" ",res)==1)) {
   index<-casal.regexpr(" ",res)==1
   res<-ifelse(index,substring(res,2),res)
   }
  # remove any lines starting with a comment(i.e., '#')
  res<-res[substring(res,1,1)!="#"]
  # remove any commentblock commands
  index1<-ifelse(substring(res,1,1)=="{",1:length(res),0)
  index2<-ifelse(substring(res,1,1)=="}",1:length(res),0)
  index1<-index1[index1!=0]
  index2<-index2[index2!=0]
  if(length(index1)!=length(index2))
    stop(paste("Error in the file ",filename,". Cannot find a matching '{' or '}'",sep=""))
  if(length(index1)>0 || length(index2)>0) {
    index<-unlist(apply(cbind(index1,index2),1,function(x) seq(x[1],x[2])))
    res<-res[!casal.isin(1:length(res),index)]
  }
  # remove text after any inline comments (i.e., '#')
  res<-ifelse(casal.regexpr("#",res)>0,substring(res,1,casal.regexpr("#",res)-1),res)
  # remove any 'blank' lines
  res<-res[res!=""]
  # check that the first character is a command block
  if(substring(res[1],1,1)!="@")
    stop(paste("Error in the file ",filename,". Cannot find a '@' at the begining of the file",sep=""))
  # convert '\t' to 'space'
  res<-as.vector(tapply(res,1:length(res),function(x) {
    tmp<-unlist(casal.unpaste(x,sep="\t"))
    tmp<-tmp[!casal.isin(tmp,c(""," ","\t"))]
    return(as.vector(paste(tmp,collapse=" ")))
    }))
  # Strip white space from end and double spaces in throughout
  res<-as.vector(tapply(res,1:length(res),function(x) {
    tmp<-unlist(casal.unpaste(x,sep=" "))
    tmp<-tmp[!casal.isin(tmp,c(""," ","\t"))]
    return(as.vector(paste(tmp,collapse=" ")))
    }))
  ans<-list()
  print(paste("The 'csl' input parameter file has",length(res[substring(res,1,1)=="@"]),"commands, and",length(res),"lines"))
  CommandCount<-0
  for(i in 1:length(res)) {
    temp<-casal.string.to.vector.of.words(res[i])
    # If an '@', then this is a command
    if(substring(temp[1],1,1)=="@") {
      CommandCount<-CommandCount+1
      Command<-substring(temp[1],2)
      if(length(csl.commands$type[csl.commands$command==Command])>0) {
        if(csl.commands$type[csl.commands$command==Command][1]=="autonumber") {
          counter<-csl.commands$count[csl.commands$command==Command]+1
          csl.commands$count[csl.commands$command==Command]<-counter
          ans[[paste(Command,"[",counter,"]",sep="")]]<-list("command"=Command,"value"=temp[-1])
        } else if(csl.commands$type[csl.commands$command==Command][1]=="argument") {
          ans[[Command]]<-list("command"=Command,"value"=temp[-1])
        } else if(csl.commands$type[csl.commands$command==Command][1]=="label") {
          ans[[paste(Command,"[",temp[2],"]",sep="")]]<-list("command"=Command,"value"=temp[-1])
        } else if(csl.commands$type[csl.commands$command==Command][1]=="labelifrepeated") {
          if(length(temp)>1) ans[[paste(Command,"[",temp[2],"]",sep="")]]<-list("command"=Command,"value"=temp[-1])
          else ans[[Command]]<-list("command"=Command,"value"=temp[-1])
        } else if(csl.commands$type[csl.commands$command==Command][1]=="nolabel") {
          ans[[Command]]<-list("command"=Command,"value"=temp[-1])
        } else {
          print(paste("Warning: Unknown CASAL command '",Command,"' found in '",filename,"'\n",sep=""))
          ans[[Command]]<-list("command"=Command,"value"=temp[-1])
        }
      } else {
        print(paste("Warning: Unknown CASAL command '",Command,"' found in '",filename,"'\n",sep=""))
        ans[[Command]]<-list("command"=Command,"value"=temp[-1])
      }
    } else { # must be a subcommand
      ans[[CommandCount]][[temp[1]]]<-temp[-1]
    }
  }
  ans <- set.class(ans, "casalCSLfile")
  return(ans)
}
