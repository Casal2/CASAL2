![Code validation Pass](https://github.com/alistairdunn1/CASAL2/actions/workflows/Casal2_testsuite_modelrunner_archive.yml/badge.svg)
[![Casal2 Windows release build](https://github.com/alistairdunn1/CASAL2/actions/workflows/BuildRelease4Windows.yml/badge.svg)](https://github.com/alistairdunn1/CASAL2/actions/workflows/BuildRelease4Windows.yml)

Casal2
======

Casal2 is an open-source age or length structured population dynamics modelling package. The Casal2 software implements generalised age- or length-structured population models that allow for a great deal of choice in specifying the population dynamics, parameter estimation, and model outputs. Casal2 is designed for flexibility. It can implement an age- or length-structured models for a single population or multiple populations using user-defined categories such as area, sex, and maturity. These structural elements are generic and not predefined, and are easily constructed. Casal2 models can be used for a single population with a single anthropogenic event (in a fish population model this would be a single fishery), or for multiple species and populations, areas, and/or anthropogenic or exploitation methods, and including predator-prey interactions.

This repository contains the required libraries and source code to compile and run Casal2. This project is covered under the GNU General Public License version 2. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

## Casal2 clone

This repository is a clone of Casal2 of https://github.com/NIWAFisheriesModelling/CASAL2. It is intended to have additional bug fixes and enhancements for potential inclusion into the original NIWA Casal2 codebase. 

## Releases

The most recent development version of Casal2 from this repository is available from https://github.com/alistairdunn1/CASAL2.  The most recent NIWA version of Casal2 is available from https://github.com/NIWAFisheriesModelling/CASAL2, but that may not contain all of the bug-fixes and enhancements found in this version.

## Auxiliary resources

An R package that will help post processing Casal2 models can be found istalled using devtools::install_github("https://github.com/alistairdunn1/CASAL2/",subdir="R-libraries/casal2"), and an add-on package to for postprocessing Casal2 output can be found here [r4Casal2](https://github.com/alistairdunn1/r4Casal2) and installed using devtools::install_github("https://github.com/alistairdunn1/r4Casal2/",subdir="r4Casal2"). It has a publicly hosted book [found here](https://alistairdunn1.github.io/r4Casal2/) which demonstrates functionality of the r4Casal2 R package. The r4Casal2 package is a clone of r4Casal2 from https://github.com/NIWAFisheriesModelling/r4Casal2. It is intended to have additional bug fixes and enhancements for potential inclusion into the original NIWA r4Casal2 codebase. 

More information about Casal2, see https://github.com/alistairdunn1/CASAL2 and https://github.com/NIWAFisheriesModelling/CASAL2
