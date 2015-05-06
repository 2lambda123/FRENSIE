//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_KleinNishinaPhotonScatteringDistribution.cpp
//! \author Alex Robinson
//! \brief  The Klein-Nishina photon scattering distribution definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// FRENSIE Includes
#include "MonteCarlo_KleinNishinaPhotonScatteringDistribution.hpp"
#include "Utility_UniformDistribution.hpp"
#include "Utility_PhysicalConstants.hpp"
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Default Constructor
KleinNishinaPhotonScatteringDistribution::KleinNishinaPhotonScatteringDistribution()
  : IncoherentPhotonScatteringDistribution( Teuchos::RCP<const Utility::OneDDistribution>( new Utility::UniformDistribution( 0.0, std::numeric_limits<double>::max(), 1.0 ) ), 3.0 )
{ /* ... */ }

// Constructor
/*! Above the cutoff energy Koblinger's direct sampling method will be used. 
 * Koblinger's method can only be used when the particle energy is above
 * (1 + sqrt(3))*me.
 */
KleinNishinaPhotonScatteringDistribution::KleinNishinaPhotonScatteringDistribution(
				     const double kahn_sampling_cutoff_energy )
  : IncoherentPhotonScatteringDistribution( Teuchos::RCP<const Utility::OneDDistribution>( new Utility::UniformDistribution( 0.0, std::numeric_limits<double>::max(), 1.0 ) ), 3.0 )
{ /* ... */ }

// Evaluate the integrated cross section (cm^2)
double 
KleinNishinaPhotonScatteringDistribution::evaluateIntegratedCrossSection(
						const double incoming_energy,
						const double precision ) const
{
  // Make sure the energy is valid
  testPrecondition( incoming_energy > 0.0 );
  
  const double alpha = 
    incoming_energy/Utility::PhysicalConstants::electron_rest_mass_energy;
  
  const double arg = 1.0 + 2.0*alpha;

  const double term_1 = (1.0 + alpha)/(alpha*alpha)*
    ((1.0 + arg)/arg - log(arg)/alpha);

  const double term_2 = log(arg)/(2.0*alpha);
  
  const double term_3 = -(arg + alpha)/(arg*arg);

  const double cross_section = 2.0*1e24*Utility::PhysicalConstants::pi*
    Utility::PhysicalConstants::classical_electron_radius*
    Utility::PhysicalConstants::classical_electron_radius*
    (term_1 + term_2 + term_3);

  // Make sure the cross section is valid
  testPostcondition( cross_section > 0.0 );

  return cross_section;
}

// Sample an outgoing energy and direction from the distribution
void KleinNishinaPhotonScatteringDistribution::sample( 
				     const double incoming_energy,
				     double& outgoing_energy,
				     double& scattering_angle_cosine,
				     SubshellType& shell_of_interaction ) const
{
  // Make sure the energy is valid
  testPrecondition( incoming_energy > 0.0 );
  
  unsigned trial_dummy;

  this->sampleAndRecordTrials( incoming_energy,
			       outgoing_energy,
			       scattering_angle_cosine,
			       shell_of_interaction,
			       trial_dummy );
}

// Sample an outgoing energy and direction and record the number of trials
void KleinNishinaPhotonScatteringDistribution::sampleAndRecordTrials( 
					    const double incoming_energy,
					    double& outgoing_energy,
					    double& scattering_angle_cosine,
					    SubshellType& shell_of_interaction,
					    unsigned& trials ) const
{
  // Make sure the incoming energy is valid
  testPrecondition( incoming_energy > 0.0 );
  
  this->sampleAndRecordTrialsBasicImpl( incoming_energy,
					outgoing_energy,
					scattering_angle_cosine,
					shell_of_interaction,
					trials );
}

// Randomly scatter the photon and return the shell that was interacted with
void KleinNishinaPhotonScatteringDistribution::scatterPhoton( 
				     PhotonState& photon,
				     ParticleBank& bank,
				     SubshellType& shell_of_interaction ) const
{
  double outgoing_energy, scattering_angle_cosine;

  this->sample( photon.getEnergy(),
		outgoing_energy,
		scattering_angle_cosine,
		shell_of_interaction );

  photon.setEnergy( outgoing_energy );

  photon.rotateDirection( scattering_angle_cosine, 
			  this->sampleAzimuthalAngle() );
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_KleinNishinaPhotonScatteringDistribution.cpp
//---------------------------------------------------------------------------//
