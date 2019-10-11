  # see https://www.rocker-project.org/images/

 # FROM rocker/r-ver:3.6.1
 FROM rocker/tidyverse:3.6.1
 # FROM rocker/verse:3.6.1

 ARG R_VERSION=3.6.1

 ENV DEBIAN_FRONTEND=noninteractive

 RUN apt-get update && apt-get upgrade -y \
  && apt-get -y --no-install-recommends install git-svn nano curl cmake procps time \
                                                python-pip p7zip build-essential \
                                                clang bibtool python-dateutil clang-tidy \
                                                texlive-full texlive-font-utils texlive-latex-recommended texlive-latex-extra \
                                                doxygen-latex fontforge-extras \
 # [un]comment these out later to clear caches. package caches are useful during active dev.
  && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
  && pip install datetime \
  && R -e "install.packages(c('roxygen2', 'here', 'Hmisc'))"

 WORKDIR /r-script/casal2

 RUN useradd --home-dir /r-script -U casal2

 USER casal2

 COPY --chown=casal2:casal2 alias.txt /r-script/.alias

 # RUN git clone https://git.niwa.co.nz/fisheries/modelling/casal2-development.git casal2
 COPY --chown=casal2:casal2 . .

 ENV DOCKER='T'

 RUN cd BuildSystem && ./doBuild.sh check

 # RUN ./doBuild.sh thirdparty \
 #  && ./doBuild.sh release adolc && ./doBuild.sh release betadiff && ./doBuild.sh release cppad && ./doBuild.sh test
 #  && ./doBuild.sh rlibrary && ./doBuild.sh documentation && ./doBuild.sh frontend

 CMD ["/bin/bash"]
