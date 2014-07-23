//---------------------------------------------------------------------------//
//!
//! \file   Facemc_NeutronScatteringReaction.cpp
//! \author Alex Robinson
//! \brief  The neutron-in, neutron-out reaction class definition
//!
//---------------------------------------------------------------------------//

// FRENSIE Includes
#include "Facemc_NeutronScatteringReaction.hpp"
#include "Facemc_SimulationProperties.hpp"
#include "Utility_RandomNumberGenerator.hpp"
#include "Utility_ContractException.hpp"

namespace Facemc{

// Constructor 
NeutronScatteringReaction::NeutronScatteringReaction( 
		   const NuclearReactionType reaction_type,
		   const double temperature,
		   const double q_value,
		   const unsigned multiplicity,
		   const unsigned threshold_energy_index,
	           const Teuchos::ArrayRCP<const double>& incoming_energy_grid,
		   const Teuchos::ArrayRCP<const double>& cross_section,
		   const Teuchos::RCP<NeutronScatteringDistribution>& 
		   scattering_distribution )
  : NuclearReaction( reaction_type, 
		     temperature, 
		     q_value,
		     threshold_energy_index,
		     incoming_energy_grid,
		     cross_section ),
    d_multiplicity( multiplicity ),
    d_scattering_distribution( scattering_distribution )
{
  // Make sure the multiplicity is valid
  testPrecondition( multiplicity > 0 );
  // Make sure the scattering distribution is valid
  testPrecondition( scattering_distribution.get() != NULL );
}

// Return the number of neutrons emitted from the rxn at the given energy
unsigned NeutronScatteringReaction::getNumberOfEmittedNeutrons(
						    const double energy ) const
{
  return d_multiplicity;
}

// Simulate the reaction
void NeutronScatteringReaction::react( NeutronState& neutron, 
				       ParticleBank& bank ) const
{
  neutron.incrementCollisionNumber();
  
  // There should always be at least one outgoing neutron (>= 0 additional)
  unsigned num_additional_neutrons = 
    this->getNumberOfEmittedNeutrons( neutron.getEnergy() ) - 1u;

  // Create the additional neutrons (multiplicity - 1)
  for( unsigned i = 0; i < num_additional_neutrons; ++i )
  {
    Teuchos::RCP<NeutronState> new_neutron(
				   new NeutronState( neutron, true, false ) );
					   
    d_scattering_distribution->scatterNeutron( *new_neutron, 
					       this->getTemperature() );

    // Add the new neutron to the bank
    bank.push( new_neutron, this->getReactionType() );
  }
  
  // Scatter the "original" neutron
  d_scattering_distribution->scatterNeutron( neutron,
					     this->getTemperature() );
}

} // end Facemc namespace

//---------------------------------------------------------------------------//
// end Facemc_NeutronScatteringReaction.cpp
//---------------------------------------------------------------------------//
