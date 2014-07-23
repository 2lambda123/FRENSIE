//---------------------------------------------------------------------------//
//! 
//! \file   Facemc_AceLaw3NeutronScatteringEnergyDistribution.cpp
//! \author Alex Robinson, Alex Bennett
//! \brief  The neutron inelastic level scattering energy distribution class
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// FRENSIE Includes
#include "Facemc_AceLaw3NeutronScatteringEnergyDistribution.hpp"
#include "Utility_ContractException.hpp"

namespace Facemc{

// Constructor
/*! \details param_a = (A+1)/A * |Q| and param_b = (A/(A+1))^2
 */
AceLaw3NeutronScatteringEnergyDistribution::AceLaw3NeutronScatteringEnergyDistribution(
						         const double param_a,
							 const double param_b )
  : NeutronScatteringEnergyDistribution( 3u ),
    d_param_a( param_a ),
    d_param_b( param_b )
{
  // Check to make sure param_a and param b are greater then 0
  testPrecondition( param_a > 0 );
  testPrecondition( param_b > 0 );
}

// Sample a scattering energy
double AceLaw3NeutronScatteringEnergyDistribution::sampleEnergy( 
						    const double energy ) const
{
  // Make sure the energy is valid
  testPrecondition( energy > 0 );
  testPrecondition( energy < std::numeric_limits<double>::infinity() );
  
  return d_param_b * ( energy - d_param_a);
}

} // end Facemc namespace

//---------------------------------------------------------------------------//
// end Facemc_AceLaw3NeutronScatteringEnergyDistribution.cpp
//---------------------------------------------------------------------------//


