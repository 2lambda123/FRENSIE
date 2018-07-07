//---------------------------------------------------------------------------//
//!
//! \file   tstSimulationGeneralProperties.cpp
//! \author Alex Robinson, Luke Kersting
//! \brief  Simulation general properties unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// FRENSIE Includes
#include "MonteCarlo_SimulationGeneralProperties.hpp"
#include "Utility_UnitTestHarnessWithMain.hpp"
#include "ArchiveTestHelpers.hpp"

//---------------------------------------------------------------------------//
// Testing Types
//---------------------------------------------------------------------------//

typedef TestArchiveHelper::TestArchives TestArchives;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Test the simulation properties defaults
FRENSIE_UNIT_TEST( SimulationGeneralProperties, defaults )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::NEUTRON_MODE );
  FRENSIE_CHECK_EQUAL( properties.getNumberOfHistories(), 0 );
  FRENSIE_CHECK_EQUAL( properties.getSurfaceFluxEstimatorAngleCosineCutoff(),
                       0.001 );
  FRENSIE_CHECK( properties.displayWarnings() );
  FRENSIE_CHECK( !properties.isImplicitCaptureModeOn() );
}

//---------------------------------------------------------------------------//
// Test that the particle mode can be set
FRENSIE_UNIT_TEST( SimulationGeneralProperties, setParticleMode )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setParticleMode( MonteCarlo::NEUTRON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::NEUTRON_MODE );

  properties.setParticleMode( MonteCarlo::PHOTON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(), MonteCarlo::PHOTON_MODE );

  properties.setParticleMode( MonteCarlo::ELECTRON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::ELECTRON_MODE );

  properties.setParticleMode( MonteCarlo::NEUTRON_PHOTON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::NEUTRON_PHOTON_MODE );

  properties.setParticleMode( MonteCarlo::PHOTON_ELECTRON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::PHOTON_ELECTRON_MODE );

  properties.setParticleMode( MonteCarlo::NEUTRON_PHOTON_ELECTRON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::NEUTRON_PHOTON_ELECTRON_MODE );

  properties.setParticleMode( MonteCarlo::ADJOINT_PHOTON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::ADJOINT_PHOTON_MODE );

  properties.setParticleMode( MonteCarlo::ADJOINT_ELECTRON_MODE );

  FRENSIE_CHECK_EQUAL( properties.getParticleMode(),
                       MonteCarlo::ADJOINT_ELECTRON_MODE );
}

//---------------------------------------------------------------------------//
// Test that the number of histories to run can be set
FRENSIE_UNIT_TEST( SimulationGeneralProperties, setNumberOfHistories )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setNumberOfHistories( 1000000000 );

  FRENSIE_CHECK_EQUAL(properties.getNumberOfHistories(), 1000000000 );
}

//---------------------------------------------------------------------------//
// Test that the surface flux angle cosine cutoff can be set
FRENSIE_UNIT_TEST( SimulationGeneralProperties,
                   setSurfaceFluxEstimatorAngleCosineCutoff )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setSurfaceFluxEstimatorAngleCosineCutoff( 0.1 );

  FRENSIE_CHECK_EQUAL( properties.getSurfaceFluxEstimatorAngleCosineCutoff(),
                       0.1 );
}

//---------------------------------------------------------------------------//
// Test that warnings can be disabled/enabled
FRENSIE_UNIT_TEST( SimulationGeneralProperties, setWarningsOnOff )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setWarningsOff();

  FRENSIE_CHECK( !properties.displayWarnings() );

  properties.setWarningsOn();

  FRENSIE_CHECK( properties.displayWarnings() );
}

//---------------------------------------------------------------------------//
// Test that implicit capture mode can be turned on/off
FRENSIE_UNIT_TEST( SimulationGeneralProperties, setImplicitCaptureModeOnOff )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setImplicitCaptureModeOn();

  FRENSIE_CHECK( properties.isImplicitCaptureModeOn() );

  properties.setAnalogueCaptureModeOn();

  FRENSIE_CHECK( !properties.isImplicitCaptureModeOn() );
}

//---------------------------------------------------------------------------//
// Test that the number of batches per processor can be set
FRENSIE_UNIT_TEST( SimulationGeneralProperties, setNumberOfBatchesPerProcessor )
{
  MonteCarlo::SimulationGeneralProperties properties;
  
  properties.setNumberOfBatchesPerProcessor( 25 );

  FRENSIE_CHECK_EQUAL( properties.getNumberOfBatchesPerProcessor(), 25 );
}

//---------------------------------------------------------------------------//
// Check that the properties can be archived
FRENSIE_UNIT_TEST_TEMPLATE_EXPAND( SimulationGeneralProperties,
                                   archive,
                                   TestArchives )
{
  FETCH_TEMPLATE_PARAM( 0, RawOArchive );
  FETCH_TEMPLATE_PARAM( 1, RawIArchive );

  typedef typename std::remove_pointer<RawOArchive>::type OArchive;
  typedef typename std::remove_pointer<RawIArchive>::type IArchive;

  std::string archive_base_name( "test_simulation_general_props" );
  std::ostringstream archive_ostream;

  {
    std::unique_ptr<OArchive> oarchive;

    createOArchive( archive_base_name, archive_ostream, oarchive );

    MonteCarlo::SimulationGeneralProperties default_properties;

    MonteCarlo::SimulationGeneralProperties custom_properties;
    custom_properties.setParticleMode( MonteCarlo::NEUTRON_PHOTON_MODE );
    custom_properties.setNumberOfHistories( 1000000000 );
    custom_properties.setSurfaceFluxEstimatorAngleCosineCutoff( 0.1 );
    custom_properties.setWarningsOff();
    custom_properties.setImplicitCaptureModeOn();
    custom_properties.setNumberOfBatchesPerProcessor( 25 );

    FRENSIE_REQUIRE_NO_THROW( (*oarchive) << BOOST_SERIALIZATION_NVP( default_properties ) );
    FRENSIE_REQUIRE_NO_THROW( (*oarchive) << BOOST_SERIALIZATION_NVP( custom_properties ) );
  }

  // Copy the archive ostream to an istream
  std::istringstream archive_istream( archive_ostream.str() );

  // Load the archived distributions
  std::unique_ptr<IArchive> iarchive;

  createIArchive( archive_istream, iarchive );

  MonteCarlo::SimulationGeneralProperties default_properties;

  FRENSIE_REQUIRE_NO_THROW( (*iarchive) >> BOOST_SERIALIZATION_NVP( default_properties ) );

  FRENSIE_CHECK_EQUAL( default_properties.getParticleMode(),
                       MonteCarlo::NEUTRON_MODE );
  FRENSIE_CHECK_EQUAL( default_properties.getNumberOfHistories(), 0 );
  FRENSIE_CHECK_EQUAL( default_properties.getSurfaceFluxEstimatorAngleCosineCutoff(),
                       0.001 );
  FRENSIE_CHECK( default_properties.displayWarnings() );
  FRENSIE_CHECK( !default_properties.isImplicitCaptureModeOn() );

  MonteCarlo::SimulationGeneralProperties custom_properties;

  FRENSIE_REQUIRE_NO_THROW( (*iarchive) >> BOOST_SERIALIZATION_NVP( custom_properties ) );

  FRENSIE_CHECK_EQUAL( custom_properties.getParticleMode(),
                       MonteCarlo::NEUTRON_PHOTON_MODE );
  FRENSIE_CHECK_EQUAL( custom_properties.getNumberOfHistories(), 1000000000 );
  FRENSIE_CHECK_EQUAL( custom_properties.getSurfaceFluxEstimatorAngleCosineCutoff(),
                       0.1 );
  FRENSIE_CHECK( !custom_properties.displayWarnings() );
  FRENSIE_CHECK( custom_properties.isImplicitCaptureModeOn() );
}

//---------------------------------------------------------------------------//
// end tstSimulationGeneralProperties.cpp
//---------------------------------------------------------------------------//
