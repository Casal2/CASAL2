Docker images
============

## Docker

The Dockerfile currently defines the development environment:

To start the docker container and keep it running in background:
`docker-compose up -d`

To get into the running container and test further development:
`docker-compose exec r bash`

To stop the running container and start again:
`docker-compose down`

To make sure that you build the full local system:
`docker build . -t local:casal2`
Add more `FROM` clauses in the Dockerfile to create artefacts instead of the development environment


