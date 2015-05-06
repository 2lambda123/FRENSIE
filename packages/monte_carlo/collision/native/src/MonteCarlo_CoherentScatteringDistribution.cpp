//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_CoherentScatteringDistribution.cpp
//! \author Luke Kersting
//! \brief  The coherent photon scattering distribution definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// Boost Includes
#include <boost/function.hpp>
#include <boost/bind.hpp>

// FRENSIE Includes
#include "MonteCarlo_CoherentScatteringDistribution.hpp"
#include "Utility_PhysicalConstants.hpp"
#include "Utility_GaussKronrodQuadratureKernel.hpp"
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Constructor
CoherentScatteringDistribution::CoherentScatteringDistribution(
		    const Teuchos::RCP<const Utility::TabularOneDDistribution>&
		    form_factor_function_squared )
  : PhotonScatteringDistribution(),
    d_form_factor_function_squared( form_factor_function_squared )
{
  // Make sure the array is valid
  testPrecondition( !form_factor_function_squared.is_null() );
}

// Evaluate the distribution
/*! The cross section (b) differential in the scattering angle cosine is
 * returned from this function.
 */
double CoherentScatteringDistribution::evaluate( 
			           const double incoming_energy,
			           const double scattering_angle_cosine ) const
{
  // Make sure the incoming energy is valid
  testPrecondition( incoming_energy > 0.0 );
  // Make sure the scattering angle cosine is valid
  testPrecondition( scattering_angle_cosine >= -1.0 );
  testPrecondition( scattering_angle_cosine <= 1.0 );

  const double mult = Utility::PhysicalConstants::pi*
    Utility::PhysicalConstants::classical_electron_radius*
    Utility::PhysicalConstants::classical_electron_radius;

  const double form_factor_squared = 
    this->evaluateFormFactorSquared( incoming_energy, scattering_angle_cosine);

  return mult*1e24*(1.0 + scattering_angle_cosine*scattering_angle_cosine)*
    form_factor_squared;    
}

// Evaluate the PDF
double CoherentScatteringDistribution::evaluatePDF( 
				   const double incoming_energy,
				   const double scattering_angle_cosine ) const
{
  // Make sure the incoming energy is valid
  testPrecondition( incoming_energy > 0.0 );
  // Make sure the scattering angle cosine is valid
  testPrecondition( scattering_angle_cosine >= -1.0 );
  testPrecondition( scattering_angle_cosine <= 1.0 );

  return this->evaluate( incoming_energy, scattering_angle_cosine )/
    this->evaluateIntegratedCrossSection( incoming_energy, 1e-3 );
}

// Evaluate the integrated cross section (b)
double CoherentScatteringDistribution::evaluateIntegratedCrossSection( 
					         const double incoming_energy,
					         const double precision ) const
{
  // Make sure the incoming energy is valid
  testPrecondition( incoming_energy > 0.0 );

  // Evaluate the integrated cross section
  boost::function<double (double x)> diff_cs_wrapper = 
    boost::bind<double>( &CoherentScatteringDistribution::evaluate,
			 boost::cref( *this ),
			 incoming_energy,
			 _1 );

  double abs_error, integrated_cs;

  Utility::GaussKronrodQuadratureKernel quadrature_kernel( precision );

  quadrature_kernel.integrateAdaptively<15>( diff_cs_wrapper,
					     -1.0,
					     1.0,
					     integrated_cs,
					     abs_error );

  // Make sure the integrated cross section is valid
  testPostcondition( integrated_cs > 0.0 );

  return integrated_cs;
}

// Randomly scatter the photon
void CoherentScatteringDistribution::scatterPhoton( 
				     PhotonState& photon,
				     ParticleBank& bank,
				     SubshellType& shell_of_interaction ) const
{
  double outgoing_energy, scattering_angle_cosine;

  // Sample an outgoing direction
  this->sample( photon.getEnergy(),
		outgoing_energy,
		scattering_angle_cosine,
		shell_of_interaction );

  // Set the new direction
  photon.rotateDirection( scattering_angle_cosine, 
			  this->sampleAzimuthalAngle() );
}

// Evaluate the form factor squared
double CoherentScatteringDistribution::evaluateFormFactorSquared( 
				  const double incoming_energy,
				  const double scattering_angle_cosine ) const
{
  // The inverse wavelength of the photon (1/cm)
  const double inverse_wavelength = incoming_energy/
    (Utility::PhysicalConstants::planck_constant*
     Utility::PhysicalConstants::speed_of_light);

  // The squared form factor argument
  const double form_factor_arg_squared = ((1.0 - scattering_angle_cosine)/2.0)*
    inverse_wavelength*inverse_wavelength;

  // Make sure the squared form factor argument is valid
  testPostcondition( form_factor_arg_squared >= 0.0 );

  return d_form_factor_function_squared->evaluate( form_factor_arg_squared );
}

// Basic sampling implementation
void CoherentScatteringDistribution::sampleAndRecordTrialsBasicImpl( 
					    const double incoming_energy,
					    double& outgoing_energy,
					    double& scattering_angle_cosine,
					    SubshellType& shell_of_interaction,
					    unsigned& trials ) const
{
  // Make sure the incoming energy is valid
  testPrecondition( incoming_energy > 0.0 );

  // Increment the number of trials
  ++trials;

  // Use the probability mixing technique to sample an outgoing angle
  double random_number_1 = 
    Utility::RandomNumberGenerator::getRandomNumber<double>();
  double random_number_2 = 
    Utility::RandomNumberGenerator::getRandomNumber<double>();
    
  if( random_number_1 <= 0.75 )
    scattering_angle_cosine = 2*random_number_2 - 1.0;
  else
  {
    scattering_angle_cosine = pow( fabs(2*random_number_2 - 1.0), 1.0/3.0 );

    if( random_number_2 < 0.5 )
      scattering_angle_cosine *= -1.0;
  }

  // Check for roundoff error
  if( fabs( scattering_angle_cosine ) > 1.0 )
    scattering_angle_cosine = copysign( 1.0, scattering_angle_cosine );

  // There is no change in energy for Thompson scattering
  outgoing_energy = incoming_energy;
  
  // The shell is not relevant for free electron scattering
  shell_of_interaction = UNKNOWN_SUBSHELL;

  // Make sure the outgoing energy is valid
  testPostcondition( outgoing_energy == incoming_energy );
  // Make sure the scattering angle cosine is valid
  testPostcondition( scattering_angle_cosine >= -1.0 );
  testPostcondition( scattering_angle_cosine <= 1.0 );
}

// Return the form factor function squared distribution
const Teuchos::RCP<const Utility::TabularOneDDistribution>&
CoherentScatteringDistribution::getFormFactorSquaredDistribution() const
{
  return d_form_factor_function_squared;
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_IncoherentScatteringDistribution.cpp
//---------------------------------------------------------------------------//
