FROM r-base:latest as developer
LABEL maintainer="Ryan Darby <ryan.darby@niwa.co.nz>"
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get upgrade -y \
  && apt-get -y --no-install-recommends install git git-svn openssh-client nano ca-certificates curl procps time \
  libgeos++ libgeos-dev libgdal-dev python-pip gfortran texlive \
  p7zip cmake build-essential cpp g++ gcc gfortran
# TODO uncomment these later to clear caches. package caches are useful during active dev.
#  && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*
RUN R -e "install.packages('sp')"
RUN R -e "install.packages('Rcpp')"
RUN R -e "install.packages('rgeos')"
RUN R -e "install.packages('rgdal')"
RUN R -e "install.packages('RPostgreSQL')"
RUN R -e "install.packages('DBI')"
RUN R -e "library(sp)"
RUN R -e "library(Rcpp)"
RUN R -e "library(rgeos)"
RUN R -e "library(rgdal)"
WORKDIR /r-script
