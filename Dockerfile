FROM rocker/r-ver:3.6.1

ARG R_VERSION=3.6.1

LABEL maintainer="Ryan Darby <ryan.darby@niwa.co.nz>"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git git-svn openssh-client nano vim ca-certificates curl procps time \
                                               python-pip gfortran texlive-full p7zip cmake build-essential cpp g++ gcc gfortran \
                                               clang unzip bibtool python-dateutil clang-tidy doxygen-latex \
                                               fontforge-extras texlive-font-utils texlive-latex-extra bash-completion pkg-config \
# for devtools dependencies
                                               libxml2-dev libudunits2-dev \
                                               # libcurl4-openssl-dev libssl-dev zlib1g-dev libgit2-dev \
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev

RUN pip install datetime

# TODO uncomment these later to clear caches. package caches are useful during active dev.
#  && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*
RUN R -e "install.packages(c('devtools', 'roxygen2', 'Rcpp', 'ggplot2'))"

WORKDIR /r-script/casal2

COPY docker.alias.txt /r-script/.alias

# RUN uname -a

# RUN git clone https://github.com/NIWAFisheriesModelling/CASAL2.git casal2

COPY [a-zA-Z]* ./

RUN cd BuildSystem && ./doBuild.sh check

