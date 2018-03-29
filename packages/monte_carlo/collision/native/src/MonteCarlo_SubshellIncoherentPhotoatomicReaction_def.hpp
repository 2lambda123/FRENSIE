//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_SubshellIncoherentPhotoatomicReaction_def.hpp
//! \author Alex Robinson
//! \brief  The subshell incoherent photoatomic reaction class decl.
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_SUBSHELL_INCOHERENT_PHOTOATOMIC_REACTION_DEF_HPP
#define MONTE_CARLO_SUBSHELL_INCOHERENT_PHOTOATOMIC_REACTION_DEF_HPP

// FRENSIE Includes
#include "Utility_SortAlgorithms.hpp"
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Basic constructor
template<typename InterpPolicy, bool processed_cross_section>
SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::SubshellIncoherentPhotoatomicReaction(
   const std::shared_ptr<const std::vector<double> >& incoming_energy_grid,
   const std::shared_ptr<const std::vector<double> >& cross_section,
   const unsigned threshold_energy_index,
   const std::shared_ptr<const SubshellIncoherentPhotonScatteringDistribution>&
      scattering_distribution )
  : BaseType( incoming_energy_grid,
              cross_section,
              threshold_energy_index ),
    d_scattering_distribution( scattering_distribution ),
    d_reaction_type()
{
  // Make sure the scattering distribution is valid
  testPrecondition( scattering_distribution.get() );

  d_reaction_type = convertSubshellEnumToIncoherentPhotoatomicReactionEnum(
				    d_scattering_distribution->getSubshell() );
}

// Constructor
template<typename InterpPolicy, bool processed_cross_section>
SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::SubshellIncoherentPhotoatomicReaction(
   const std::shared_ptr<const std::vector<double> >& incoming_energy_grid,
   const std::shared_ptr<const std::vector<double> >& cross_section,
   const unsigned threshold_energy_index,
   const std::shared_ptr<const Utility::HashBasedGridSearcher>& grid_searcher,
   const std::shared_ptr<const SubshellIncoherentPhotonScatteringDistribution>&
   scattering_distribution )
  : BaseType( incoming_energy_grid,
              cross_section,
              threshold_energy_index,
              grid_searcher ),
    d_scattering_distribution( scattering_distribution ),
    d_reaction_type()
{
  // Make sure the scattering distribution is valid
  testPrecondition( scattering_distribution.get() );
  // Make sure the grid searcher is valid
  testPrecondition( grid_searcher.get() );

  d_reaction_type = convertSubshellEnumToIncoherentPhotoatomicReactionEnum(
				    d_scattering_distribution->getSubshell() );
}

// Return the number of photons emitted from the rxn at the given energy
template<typename InterpPolicy, bool processed_cross_section>
unsigned SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedPhotons(
						    const double energy ) const
{
  if( energy >= this->getThresholdEnergy() )
    return 1u;
  else
    return 0u;
}

// Return the number of electrons emitted from the rxn at the given energy
template<typename InterpPolicy, bool processed_cross_section>
unsigned SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedElectrons(
						    const double energy ) const
{
  if( energy >= this->getThresholdEnergy() )
    return 1u;
  else
    return 0u;
}

// Return the reaction type
template<typename InterpPolicy, bool processed_cross_section>
PhotoatomicReactionType SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::getReactionType() const
{
  return d_reaction_type;
}

// Simulate the reaction
template<typename InterpPolicy, bool processed_cross_section>
void SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::react(
				     PhotonState& photon,
				     ParticleBank& bank,
				     Data::SubshellType& shell_of_interaction ) const
{
  d_scattering_distribution->scatterPhoton(photon, bank, shell_of_interaction);

  photon.incrementCollisionNumber();
}

// Get the interaction subshell (non-standard interface)
template<typename InterpPolicy, bool processed_cross_section>
inline Data::SubshellType SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::getSubshell() const
{
  return d_scattering_distribution->getSubshell();
}

// Get the subshell binding energy (non-standard interface)
template<typename InterpPolicy, bool processed_cross_section>
inline double SubshellIncoherentPhotoatomicReaction<InterpPolicy,processed_cross_section>::getSubshellBindingEnergy() const
{
  return d_scattering_distribution->getSubshellBindingEnergy();
}

} // end MonteCarlo namespace

#endif // end MONTE_CARLO_SUBSHELL_INCOHERENT_PHOTOATOMIC_REACTION_DEF_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_SubshellIncoherentPhotoatomicReaction_def.hpp
//---------------------------------------------------------------------------//
