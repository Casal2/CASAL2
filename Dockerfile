# FROM rocker/r-ver:3.6.1
# FROM rocker/tidyverse:3.6.1
FROM rocker/verse:3.6.1

ARG R_VERSION=3.6.1

LABEL maintainer="Ryan Darby <ryan.darby@niwa.co.nz>"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git-svn openssh-client nano vim curl procps time \
                                               python-pip p7zip cmake build-essential cpp gcc \
                                               clang bibtool python-dateutil clang-tidy doxygen-latex \
                                               fontforge-extras texlive-font-utils texlive-latex-extra pkg-config \
# [un]comment these out later to clear caches. package caches are useful during active dev.
 && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
 && pip install datetime \
 && R -e "install.packages(c('roxygen2', 'here', 'Hmisc'))"

WORKDIR /r-script/casal2

RUN useradd --home-dir /r-script -U casal2

USER casal2

COPY alias.txt /r-script/.alias

# RUN git clone https://github.com/NIWAFisheriesModelling/CASAL2.git casal2
COPY . .

RUN cd BuildSystem && ./doBuild.sh check && ./doBuild.sh thirdparty \
                   && ./doBuild.sh release adolc && ./doBuild.sh release betadiff && ./doBuild.sh release cppad && ./doBuild.sh test \
                   && ./doBuild.sh rlibrary && ./doBuild.sh documentation && ./doBuild.sh frontend

CMD ["/bin/bash"]
