//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_AbsorptionAdjointElectroatomicReaction_def.hpp
//! \author Luke Kersting
//! \brief  The absorption adjoint electroatomic reaction class definition
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_ABSORPTION_ADJOINT_ELECTROATOMIC_REACTION_DEF_HPP
#define MONTE_CARLO_ABSORPTION_ADJOINT_ELECTROATOMIC_REACTION_DEF_HPP

// FRENSIE Includes
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Basic constructor
template<typename InterpPolicy, bool processed_cross_section>
AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::AbsorptionAdjointElectroatomicReaction(
                   const Teuchos::ArrayRCP<const double>& incoming_energy_grid,
                   const Teuchos::ArrayRCP<const double>& cross_section,
                   const unsigned threshold_energy_index,
                   const AdjointElectroatomicReactionType reaction )
  : BaseType( incoming_energy_grid, cross_section, threshold_energy_index ),
    d_reaction( reaction )
{ /* ... */ }

// Constructor
template<typename InterpPolicy, bool processed_cross_section>
AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::AbsorptionAdjointElectroatomicReaction(
       const Teuchos::ArrayRCP<const double>& incoming_energy_grid,
       const Teuchos::ArrayRCP<const double>& cross_section,
       const unsigned threshold_energy_index,
       const Teuchos::RCP<const Utility::HashBasedGridSearcher>& grid_searcher,
       const AdjointElectroatomicReactionType reaction )
  : BaseType( incoming_energy_grid,
              cross_section,
              threshold_energy_index,
              grid_searcher ),
    d_reaction( reaction )
{ /* ... */ }

// Return the number of adjoint photons emitted from the rxn at the given energy
template<typename InterpPolicy, bool processed_cross_section>
unsigned AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedPhotons( const double energy ) const
{
  return 0u;
}

// Return the number of adjoint electrons emitted from the rxn at the given energy
template<typename InterpPolicy, bool processed_cross_section>
unsigned AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedElectrons( const double energy ) const
{
  return 0u;
}

// Return the reaction type
template<typename InterpPolicy, bool processed_cross_section>
AdjointElectroatomicReactionType AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::getReactionType() const
{
  return d_reaction;
}

// Simulate the reaction
template<typename InterpPolicy, bool processed_cross_section>
void AbsorptionAdjointElectroatomicReaction<InterpPolicy,processed_cross_section>::react(
                               AdjointElectronState& adjoint_electron,
                               ParticleBank& bank,
                               Data::SubshellType& shell_of_interaction ) const
{
  adjoint_electron.setAsGone();

  shell_of_interaction = Data::UNKNOWN_SUBSHELL;
}
  
} // end MonteCarlo namespace

#endif // end MONTE_CARLO_ABSORPTION_ADJOINT_ELECTROATOMIC_REACTION_DEF_HPP


//---------------------------------------------------------------------------//
// end MonteCarlo_AbsorptionAdjointElectroatomicReaction_def.hpp
//---------------------------------------------------------------------------//