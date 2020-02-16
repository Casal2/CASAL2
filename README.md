Casal2
======

The purpose of this repository is to mirror the Casal2 code base in the NIWA internal developement repository.

Casal2 is an open source age structured population dynamics model. 
It can implement a range of a statistical catch-at-age 
population dynamics models, using a discrete time-step 
area-space model that represents a cohort-based 
population age structure. 

This repository is standalone containing all thirdparty 
libraries and source code necessary to compile Casal2. 
This project is covered under the GNU General Public 
License version 2. This program is distributed in the 
hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.

Casal2 is in an alpha testing phase, with some modules functioning as intended and others in development phase. 

For more information about Casal2 contact the Casal2 Development Team at casal2@niwa.co.nz

## Docker

The Dockerfile currently defines the development environment:  

To start the docker container and keep it running in background:  
`docker-compose up -d`

To get into the running container and test further developerment:  
`docker-compose exec r bash`

To stop the running container and start again:  
`docker-compose down`

To make sure that you build the full local system:  
`docker build . -t local:casal2`
Add more `FROM` clauses in the Dockerfile to create artefacts instead of the development environment


