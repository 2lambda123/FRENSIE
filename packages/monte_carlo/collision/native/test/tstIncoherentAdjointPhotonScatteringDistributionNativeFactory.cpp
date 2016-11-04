//---------------------------------------------------------------------------//
//!
//! \file   tstIncoherentAdjointPhotonScatteringDistributionNativeFactory.cpp
//! \author Alex Robinson
//! \brief  Incoherent adjoint photon scattering distribution native factory
//!         unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <memory>

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_VerboseObject.hpp>

// FRENSIE Includes
#include "MonteCarlo_IncoherentAdjointPhotonScatteringDistributionNativeFactory.hpp"
#include "Data_AdjointElectronPhotonRelaxationDataContainer.hpp"
#include "Utility_RandomNumberGenerator.hpp"
#include "Utility_UnitTestHarnessExtensions.hpp"

//---------------------------------------------------------------------------//
// Testing Variables
//---------------------------------------------------------------------------//

std::shared_ptr<const Data::AdjointElectronPhotonRelaxationDataContainer>
  data_container;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that a Klein-Nishina distribution can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createKleinNishinaDistribution )
{
  std::shared_ptr<MonteCarlo::IncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createDistribution(
                                       *data_container,
                                       distribution,
                                       MonteCarlo::KN_INCOHERENT_ADJOINT_MODEL,
                                       20.0 );

  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
 
  TEST_FLOATING_EQUALITY( dist_value, 0.498938241600864918, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.4989344050883251, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );

  TEST_FLOATING_EQUALITY( dist_value, 4.818399835538855, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );

  TEST_FLOATING_EQUALITY( dist_value, 0.4634138962142929, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.4989344050883251, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.4989344050883251, 1e-15 );
}

//---------------------------------------------------------------------------//
// Check that a Waller-Hartree incoherent distribution can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createWallerHartreeDistribution )
{
  std::shared_ptr<MonteCarlo::IncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createDistribution(
                                       *data_container,
                                       distribution,
                                       MonteCarlo::WH_INCOHERENT_ADJOINT_MODEL,
                                       20.0 );
  
  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
 
  TEST_FLOATING_EQUALITY( dist_value, 0.853837503637913553, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );
  
  TEST_FLOATING_EQUALITY( dist_value, 67.4575976975439602, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );
  
  TEST_FLOATING_EQUALITY( dist_value, 6.48779098171434399, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );
}

//---------------------------------------------------------------------------//
// Check that a subshell incoherent distribution can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createSubshellDistribution_base )
{
  std::shared_ptr<MonteCarlo::IncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createDistribution(
                                  *data_container,
                                  distribution,
                                  MonteCarlo::IMPULSE_INCOHERENT_ADJOINT_MODEL,
                                  20.0,
                                  Data::K_SUBSHELL );
  
  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
    
  TEST_FLOATING_EQUALITY( dist_value, 7.16614513178088125e-05, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );
  
  TEST_FLOATING_EQUALITY( dist_value, 4.8183422986505029, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.926433751496189117, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Check that a sample can be made
  std::vector<double> fake_stream( 26 );
  
  fake_stream[0] = 0.15; // branch 1
  fake_stream[1] = 0.4721647344828152; // select x = 0.9
  fake_stream[2] = 0.55; // reject
  fake_stream[3] = 0.15; // branch 1
  fake_stream[4] = 0.22986680137273696; // select x = 0.95
  fake_stream[5] = 0.245; // accept x
  fake_stream[6] = 0.32; // reject mu
  fake_stream[7] = 0.15; // branch 1
  fake_stream[8] = 0.22986680137273696; // select x = 0.95
  fake_stream[9] = 0.245; // accept x
  fake_stream[10] = 0.31; // accept mu

  Utility::RandomNumberGenerator::setFakeStream( fake_stream );

  double outgoing_energy, scattering_angle_cosine;
  
  distribution->sample(
                    Utility::PhysicalConstants::electron_rest_mass_energy/10.0,
                    outgoing_energy,
                    scattering_angle_cosine );

  TEST_FLOATING_EQUALITY( outgoing_energy, 0.053789358961052636, 1e-15 );
  TEST_FLOATING_EQUALITY( scattering_angle_cosine, 0.5, 1e-15 );

  Utility::RandomNumberGenerator::unsetFakeStream();
}

//---------------------------------------------------------------------------//
// Check that a subshell incoherent distribution can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createSubshellDistribution )
{
  std::shared_ptr<MonteCarlo::SubshellIncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createSubshellDistribution(
                                  *data_container,
                                  distribution,
                                  MonteCarlo::IMPULSE_INCOHERENT_ADJOINT_MODEL,
                                  20.0,
                                  Data::K_SUBSHELL );
  
  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  TEST_EQUALITY_CONST( distribution->getSubshell(), Data::K_SUBSHELL );
  TEST_EQUALITY_CONST( distribution->getSubshellOccupancy(), 2 );
  TEST_EQUALITY_CONST( distribution->getSubshellBindingEnergy(), 1.8285e-3 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
    
  TEST_FLOATING_EQUALITY( dist_value, 7.16614513178088125e-05, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );
  
  TEST_FLOATING_EQUALITY( dist_value, 4.8183422986505029, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.926433751496189117, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Check that a sample can be made
  std::vector<double> fake_stream( 26 );
  
  fake_stream[0] = 0.15; // branch 1
  fake_stream[1] = 0.4721647344828152; // select x = 0.9
  fake_stream[2] = 0.55; // reject
  fake_stream[3] = 0.15; // branch 1
  fake_stream[4] = 0.22986680137273696; // select x = 0.95
  fake_stream[5] = 0.245; // accept x
  fake_stream[6] = 0.32; // reject mu
  fake_stream[7] = 0.15; // branch 1
  fake_stream[8] = 0.22986680137273696; // select x = 0.95
  fake_stream[9] = 0.245; // accept x
  fake_stream[10] = 0.31; // accept mu

  Utility::RandomNumberGenerator::setFakeStream( fake_stream );

  double outgoing_energy, scattering_angle_cosine;
  
  distribution->sample(
                    Utility::PhysicalConstants::electron_rest_mass_energy/10.0,
                    outgoing_energy,
                    scattering_angle_cosine );

  TEST_FLOATING_EQUALITY( outgoing_energy, 0.053789358961052636, 1e-15 );
  TEST_FLOATING_EQUALITY( scattering_angle_cosine, 0.5, 1e-15 );

  Utility::RandomNumberGenerator::unsetFakeStream();
}

//---------------------------------------------------------------------------//
// Check that a Doppler broadened subshell incoherent dist. can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createDopplerBroadenedSubshellDistribution_base )
{
  std::shared_ptr<MonteCarlo::IncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createDistribution(
                               *data_container,
                               distribution,
                               MonteCarlo::DB_IMPULSE_INCOHERENT_ADJOINT_MODEL,
                               20.0,
                               Data::K_SUBSHELL );
  
  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
    
  TEST_FLOATING_EQUALITY( dist_value, 7.16614513178088125e-05, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );
  
  TEST_FLOATING_EQUALITY( dist_value, 4.8183422986505029, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.926433751496189117, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Check that a sample can be made
  std::vector<double> fake_stream( 30 );
  
  fake_stream[0] = 0.15; // branch 1
  fake_stream[1] = 0.4721647344828152; // select x = 0.9
  fake_stream[2] = 0.55; // reject
  fake_stream[3] = 0.15; // branch 1
  fake_stream[4] = 0.22986680137273696; // select x = 0.95
  fake_stream[5] = 0.245; // accept x
  fake_stream[6] = 0.32; // reject mu
  fake_stream[7] = 0.15; // branch 1
  fake_stream[8] = 0.22986680137273696; // select x = 0.95
  fake_stream[9] = 0.245; // accept x
  fake_stream[10] = 0.31; // accept mu
  fake_stream[11] = 0.5; // pz = -0.04935286011127609

  Utility::RandomNumberGenerator::setFakeStream( fake_stream );

  double outgoing_energy, scattering_angle_cosine;

  distribution->sample(
                    Utility::PhysicalConstants::electron_rest_mass_energy/10.0,
                    outgoing_energy,
                    scattering_angle_cosine );
  
  TEST_FLOATING_EQUALITY( outgoing_energy, 0.057653411316070699, 1e-15 );
  TEST_FLOATING_EQUALITY( scattering_angle_cosine, 0.5, 1e-15 );

  Utility::RandomNumberGenerator::unsetFakeStream();
}

//---------------------------------------------------------------------------//
// Check that a Doppler broadened subshell incoherent dist. can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createDopplerBroadenedSubshellDistribution )
{
  std::shared_ptr<MonteCarlo::IncoherentAdjointPhotonScatteringDistribution>
    distribution;

  MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createDistribution(
                               *data_container,
                               distribution,
                               MonteCarlo::DB_IMPULSE_INCOHERENT_ADJOINT_MODEL,
                               20.0,
                               Data::K_SUBSHELL );
  
  // Test the distribution
  TEST_ASSERT( distribution.get() );
  TEST_EQUALITY_CONST( distribution->getMaxEnergy(), 20.0 );
  
  // Evaluate the distribution at the min energy (E = 1e-3 MeV)
  double dist_value = distribution->evaluate( 1e-3, -1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 7.16614513178088125e-05, 1e-15 );

  dist_value = distribution->evaluate( 1e-3, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at E = 1.0 MeV
  dist_value = distribution->evaluate( 1.0, 0.5145510353765 );
  
  TEST_FLOATING_EQUALITY( dist_value, 4.8183422986505029, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 0.9 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.926433751496189117, 1e-15 );

  dist_value = distribution->evaluate( 1.0, 1.0 );

  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Evaluate the distribution at the max energy (E = 20.0 MeV)
  dist_value = distribution->evaluate( 20.0, 1.0 );
  
  TEST_FLOATING_EQUALITY( dist_value, 0.0, 1e-15 );

  // Check that a sample can be made
  std::vector<double> fake_stream( 30 );
  
  fake_stream[0] = 0.15; // branch 1
  fake_stream[1] = 0.4721647344828152; // select x = 0.9
  fake_stream[2] = 0.55; // reject
  fake_stream[3] = 0.15; // branch 1
  fake_stream[4] = 0.22986680137273696; // select x = 0.95
  fake_stream[5] = 0.245; // accept x
  fake_stream[6] = 0.32; // reject mu
  fake_stream[7] = 0.15; // branch 1
  fake_stream[8] = 0.22986680137273696; // select x = 0.95
  fake_stream[9] = 0.245; // accept x
  fake_stream[10] = 0.31; // accept mu
  fake_stream[11] = 0.5; // pz = -0.04935286011127609

  Utility::RandomNumberGenerator::setFakeStream( fake_stream );

  double outgoing_energy, scattering_angle_cosine;

  distribution->sample(
                    Utility::PhysicalConstants::electron_rest_mass_energy/10.0,
                    outgoing_energy,
                    scattering_angle_cosine );
  
  TEST_FLOATING_EQUALITY( outgoing_energy, 0.057653411316070699, 1e-15 );
  TEST_FLOATING_EQUALITY( scattering_angle_cosine, 0.5, 1e-15 );

  Utility::RandomNumberGenerator::unsetFakeStream();
}

//---------------------------------------------------------------------------//
// Check that a Doppler broadened subshell incoherent dist. can be created
TEUCHOS_UNIT_TEST( IncoherentAdjointPhotonScatteringDistributionNativeFactory,
                   createSubshellDistribution_exception_handling )
{
  std::shared_ptr<MonteCarlo::SubshellIncoherentAdjointPhotonScatteringDistribution>
    distribution;

  TEST_THROW( MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createSubshellDistribution(
                               *data_container,
                               distribution,
                               MonteCarlo::KN_INCOHERENT_ADJOINT_MODEL,
                               20.0,
                               Data::K_SUBSHELL ),
              std::logic_error );

  TEST_THROW( MonteCarlo::IncoherentAdjointPhotonScatteringDistributionNativeFactory::createSubshellDistribution(
                               *data_container,
                               distribution,
                               MonteCarlo::WH_INCOHERENT_ADJOINT_MODEL,
                               20.0,
                               Data::K_SUBSHELL ),
              std::logic_error );

}

//---------------------------------------------------------------------------//
// Custom setup
//---------------------------------------------------------------------------//
UTILITY_CUSTOM_TEUCHOS_UNIT_TEST_SETUP_BEGIN();

std::string test_native_file_name;

UTILITY_CUSTOM_TEUCHOS_UNIT_TEST_COMMAND_LINE_OPTIONS()
{
  clp().setOption( "test_native_file",
                   &test_native_file_name,
                   "Test Native file name" );
}

UTILITY_CUSTOM_TEUCHOS_UNIT_TEST_DATA_INITIALIZATION()
{
  // Create the native data file container
  data_container.reset( new Data::AdjointElectronPhotonRelaxationDataContainer(
                                                     test_native_file_name ) );

  // Initialize the random number generator
  Utility::RandomNumberGenerator::createStreams();
}

UTILITY_CUSTOM_TEUCHOS_UNIT_TEST_SETUP_END();

//---------------------------------------------------------------------------//
// end tstIncoherentAdjointPhotonScatteringDistributionNativeFactory.cpp
//---------------------------------------------------------------------------//
