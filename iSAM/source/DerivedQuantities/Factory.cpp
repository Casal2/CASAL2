/**
 * @file Factory.cpp
 * @author  Scott Rasmussen (scott.rasmussen@zaita.com)
 * @date 6/06/2013
 * @section LICENSE
 *
 * Copyright NIWA Science ©2013 - www.niwa.co.nz
 *
 */

// headers
#include "Factory.h"

#include "DerivedQuantities/Manager.h"
#include "DerivedQuantities/Children/Abundance.h"
#include "DerivedQuantities/Children/Biomass.h"

// namespaces
namespace isam {
namespace derivedquantities {

/**
 *
 */
DerivedQuantityPtr Factory::Create(const string& block_type, const string& derived_quantity_type) {

  DerivedQuantityPtr derived_quantity;

  if (block_type == PARAM_DERIVED_QUANTITY && derived_quantity_type == PARAM_ABUNDANCE) {
    derived_quantity = DerivedQuantityPtr(new Abundance());

  } else if (block_type == PARAM_DERIVED_QUANTITY && derived_quantity_type == PARAM_BIOMASS) {
    derived_quantity = DerivedQuantityPtr(new Biomass());

  }

  if (derived_quantity)
    isam::derivedquantities::Manager::Instance().AddObject(derived_quantity);

  return derived_quantity;
}


} /* namespace derivedquantities */
} /* namespace isam */
