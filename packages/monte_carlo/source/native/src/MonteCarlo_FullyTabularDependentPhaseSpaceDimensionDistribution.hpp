//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_FullyTabularDependentPhaseSpaceDimensionDistribution.hpp
//! \author Alex Robinson
//! \brief  Fully tabular dependent phase space dimension dist. class decl.
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_FULLY_TABULAR_DEPENDENT_PHASE_SPACE_DIMENSION_DISTRIBUTION_HPP
#define MONTE_CARLO_FULLY_TABULAR_DEPENDENT_PHASE_SPACE_DIMENSION_DISTRIBUTION_HPP

// FRENSIE Includes
#include "MonteCarlo_StandardDependentPhaseSpaceDimensionDistribution.hpp"
#include "Utility_FullyTabularTwoDDistribution.hpp"

namespace MonteCarlo{

//! The fully-tabular dependent phase space dimension distribution
template<PhaseSpaceDimension parent_dimension,PhaseSpaceDimension dimension> using FullyTabularDependentPhaseSpaceDimensionDistribution =
  StandardDependentPhaseSpaceDimensionDistribution<parent_dimension,dimension,Utility::FullyTabularTwoDDistribution>;
  
} // end MonteCarlo namespace

#endif // end MONTE_CARLO_FULLY_TABULAR_DEPENDENT_PHASE_SPACE_DIMENSION_DISTRIBUTION_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_FullyTabularDependentPhaseSpaceDimensionDistribution.hpp
//---------------------------------------------------------------------------//
