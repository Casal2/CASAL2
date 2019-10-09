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
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev \
# [un]comment these out later to clear caches. package caches are useful during active dev.
 && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
 && pip install datetime \
 && R -e "install.packages(c('devtools', 'roxygen2', 'dplyr', 'ggplot2', 'here'))"

WORKDIR /r-script/casal2

RUN useradd --home-dir /r-script -U casal2

USER casal2

COPY docker.alias.txt /r-script/.alias

# RUN git clone https://github.com/NIWAFisheriesModelling/CASAL2.git casal2
COPY . .

RUN cd BuildSystem && ./doBuild.sh check

CMD ["/bin/bash"]
