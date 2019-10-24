# $Id: casal.get.lines.R 1747 2007-11-12 21:01:38Z adunn $
"casal.get.lines"<-
function(lines, from = -1, to = -1, contains = "", starts.with = "", clip.to = "", clip.from = "", clip.to.match = "", clip.from.match = "", ...)
{
  result <- lines
  if(from > 0) {
    result <- result[(1:length(result)) >= from]
  }
  if(to > 0) {
    result <- result[(1:length(result)) <= to]
  }
  if(clip.to != "") {
    result <- result[(casal.pos(result, clip.to) + 1):length(result)]
  }
  if(clip.from != "") {
    result <- result[1:(casal.pos(result, clip.from) - 1)]
  }
  if(clip.to.match != "") {
    if(casal.regexp.in(result, clip.to.match)) {
      result <- result[(casal.pos.match(result, clip.to.match) + 1):length(result)]
    }
  }
  if(clip.from.match != "") {
    if(casal.regexp.in(result, clip.from.match)) {
      result <- result[1:(casal.pos.match(result, clip.from.match) - 1)]
    }
  }
  if(contains != "") {
    result <- result[casal.regexpr(contains, result) > 0]
  }
  if(starts.with != "") {
    result <- result[casal.regexpr(paste("^", starts.with, sep = ""), result) > 0]
  }
  return(result)
}
