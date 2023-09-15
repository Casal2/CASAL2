![Validation Pass](https://github.com/NIWAFisheriesModelling/CASAL2/actions/workflows/Casal2_testsuite_modelrunner_archive.yml/badge.svg)

Casal2
======

Casal2 is an open-source age or length structured population dynamics modelling package. The Casal2 software implements generalised age- or length-structured population models that allow for a great deal of choice in specifying the population dynamics, parameter estimation, and model outputs. Casal2 is designed for flexibility. It can implement an age- or length-structured models for a single population or multiple populations using user-defined categories such as area, sex, and maturity. These structural elements are generic and not predefined, and are easily constructed. Casal2 models can be used for a single population with a single anthropogenic event (in a fish population model this would be a single fishery), or for multiple species and populations, areas, and/or anthropogenic or exploitation methods, and including predator-prey interactions.

This repository contains the required libraries and source code to compile and run Casal2. This project is covered under the GNU General Public License version 2. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

## Releases

The most recent production version of Casal2 is available [here](https://github.com/NIWAFisheriesModelling/CASAL2/releases/tag/v22.08). The production version has been checked by comparing the 7 sets of Test Case MPD and MCMC results with CASAL and generating [Test Case reports](https://github.com/NIWAFisheriesModelling/Casal2-supporting-information/tree/main/TestCases/primary). We anticipate production versions being released once or twice a year, with validation of new key functionality and fixes.

The most recent development version of Casal2 is available [here](https://github.com/NIWAFisheriesModelling/CASAL2/releases/tag/dev.2207). The development version may include updates to the Test Cases reports, and may not have undergone the same level of testing as the production versions. The development versions will made available more frequently than the production versions, as fixes and other updates are made between production releases.

## Auxiliary resources

An R package that will help post processing Casal2 models can be found here: [r4Casal2](https://github.com/NIWAFisheriesModelling/r4Casal2). It has a publicly hosted book [found here](https://niwafisheriesmodelling.github.io/r4Casal2/) which demonstrates functionality of the r4Casal2 R package.

More information about Casal2, including release notes and project status, is available at: https://casal2.github.io.

For more information about Casal2, contact the Casal2 Development Team at: email:casal2@niwa.co.nz
