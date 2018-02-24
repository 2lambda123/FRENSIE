//---------------------------------------------------------------------------//
//!
//! \file   tstNuclideProperties.cpp
//! \author Alex Robinson
//! \brief  Nuclide properties unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// FRENSIE Includes
#include "Data_NuclideProperties.hpp"
#include "Utility_UnitTestHarnessWithMain.hpp"
#include "TestNuclearDataProperties.hpp"
#include "TestAtomicDataProperties.hpp"
#include "ArchiveTestHelpers.hpp"

//---------------------------------------------------------------------------//
// Testing Types
//---------------------------------------------------------------------------//

using boost::units::si::kelvin;
using Utility::Units::MeV;
using Utility::Units::amu;

typedef std::tuple<
  std::tuple<boost::archive::xml_oarchive,boost::archive::xml_iarchive>,
  std::tuple<boost::archive::text_oarchive,boost::archive::text_iarchive>,
  std::tuple<boost::archive::binary_oarchive,boost::archive::binary_iarchive>,
  std::tuple<Utility::HDF5OArchive,Utility::HDF5IArchive>,
  std::tuple<boost::archive::polymorphic_oarchive*,boost::archive::polymorphic_iarchive*>
  > TestArchives;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the constructor will throw an exception if the data is invalid
FRENSIE_UNIT_TEST( NuclideProperties, constructor )
{
  FRENSIE_CHECK_NO_THROW( Data::NuclideProperties( 1001, 1.0 ) );
  FRENSIE_CHECK_NO_THROW( Data::NuclideProperties( 2004, 4.0 ) );

  FRENSIE_CHECK_THROW( Data::NuclideProperties( 1001, 0.0 ),
                       Data::InvalidScatteringCenterPropertiesData );
  FRENSIE_CHECK_THROW( Data::NuclideProperties( 2004, 0.0 ),
                       Data::InvalidScatteringCenterPropertiesData );

  FRENSIE_CHECK_THROW( Data::NuclideProperties( 1001, -1.0 ),
                       Data::InvalidScatteringCenterPropertiesData );
  FRENSIE_CHECK_THROW( Data::NuclideProperties( 2004, -1.0 ),
                       Data::InvalidScatteringCenterPropertiesData );
}

//---------------------------------------------------------------------------//
// Check that the properties correspond to a nuclide
FRENSIE_UNIT_TEST( NuclideProperties, isNuclide )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  FRENSIE_CHECK( nuclide_properties.isNuclide() );

  std::unique_ptr<Data::AtomProperties> base_properties(
                                    new Data::NuclideProperties( 2004, 4.0 ) );

  FRENSIE_CHECK( base_properties->isNuclide() );
}

//---------------------------------------------------------------------------//
// Check that the zaid can be returned
FRENSIE_UNIT_TEST( NuclideProperties, zaid )
{
  Data::NuclideProperties h1_properties( 1001, 1.0 );

  FRENSIE_CHECK_EQUAL( h1_properties.zaid(), Data::ZAID(1001) );

  Data::NuclideProperties he4_properties( 2004, 4.0 );

  FRENSIE_CHECK_EQUAL( he4_properties.zaid(), Data::ZAID(2004) );
}

//---------------------------------------------------------------------------//
// Check that the atomic weight can be returned
FRENSIE_UNIT_TEST( NuclideProperties, atomicWeight )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  FRENSIE_CHECK_EQUAL( nuclide_properties.atomicWeight(),
                       1.0*Utility::PhysicalConstants::neutron_rest_mass_amu_q );
}

//---------------------------------------------------------------------------//
// Check that the atomic weight ratio can be returned
FRENSIE_UNIT_TEST( NuclideProperties, atomicWeightRatio )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  FRENSIE_CHECK_EQUAL( nuclide_properties.atomicWeightRatio(), 1.0 );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data properties can be set
FRENSIE_UNIT_TEST( NuclideProperties, setNuclearDataProperties )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE ) );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );
    
    nuclide_properties.setNuclearDataProperties( test_data );
  }

  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0 ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 0.0*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 1e-7*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 2.1543E-07*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 2.499961566412709544e+03*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 0, 1e-6*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1 ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 0.0*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 1e-7*MeV ) );
  FRENSIE_CHECK( nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 2.1543E-07*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 1, 1e-6*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.nuclearDataAvailable( Data::NuclearDataProperties::ACE_FILE, 2 ) );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data file types can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getNuclearDataFileTypes )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );
  }

  std::set<Data::NuclearDataProperties::FileType> file_types =
    nuclide_properties.getNuclearDataFileTypes();

  FRENSIE_CHECK_EQUAL( file_types.size(), 1 );
  FRENSIE_CHECK( file_types.count( Data::NuclearDataProperties::ACE_FILE ) );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data file versions can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataFileVersions_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );
  }

  std::set<unsigned> versions = nuclide_properties.getDataFileVersions(
                                       Data::NuclearDataProperties::ACE_FILE );

  FRENSIE_CHECK_EQUAL( versions.size(), 2 );
  FRENSIE_CHECK( versions.count( 0 ) );
  FRENSIE_CHECK( versions.count( 1 ) );
}

//---------------------------------------------------------------------------//
// Check that the recommended nuclear data file major version can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getRecommendedDataFileVersion_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     2 ) );

    nuclide_properties.setNuclearDataProperties( test_data );
  }

  FRENSIE_CHECK_EQUAL( nuclide_properties.getRecommendedDataFileVersion( Data::NuclearDataProperties::ACE_FILE ), 2 );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data evaluation temperatures can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataEvaluationTempsInMeV_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );
    
    nuclide_properties.setNuclearDataProperties( test_data );
  }

  std::vector<Data::NuclearDataProperties::Energy> data_evaluation_temps =
    nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::NuclearDataProperties::ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::NuclearDataProperties::Energy>({0.0*MeV, 2.5301e-08*MeV, 2.1543E-07*MeV}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::NuclearDataProperties::ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::NuclearDataProperties::Energy>({0.0*MeV, 2.1543E-07*MeV}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::NuclearDataProperties::ACE_FILE, 2 );

  FRENSIE_CHECK( data_evaluation_temps.empty() );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data evaluation temperatures can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataEvaluationTemps_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );
    
    nuclide_properties.setNuclearDataProperties( test_data );
  }

  std::vector<Data::NuclearDataProperties::Temperature> data_evaluation_temps =
    nuclide_properties.getDataEvaluationTemps(
                                    Data::NuclearDataProperties::ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::NuclearDataProperties::Temperature>({0.0*kelvin, 2.936059397103837227e+02*kelvin, 2.499961566412709544e+03*kelvin}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTemps(
                                    Data::NuclearDataProperties::ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::NuclearDataProperties::Temperature>({0.0*kelvin, 2.499961566412709544e+03*kelvin}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTemps(
                                    Data::NuclearDataProperties::ACE_FILE, 2 );

  FRENSIE_CHECK( data_evaluation_temps.empty() );
}

//---------------------------------------------------------------------------//
// Check that the nuclear data evaluation properties can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getNuclearDataProperties )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::NuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     0.0*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     0 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.5301e-08*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );

    nuclide_properties.setNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::NuclearDataProperties>(
                                     1001,
                                     1.0,
                                     2.1543E-07*MeV,
                                     Data::NuclearDataProperties::ACE_FILE,
                                     1 ) );
    
    nuclide_properties.setNuclearDataProperties( test_data );
  }

  // Get the properties evaluated at 0.0*MeV
  const Data::NuclearDataProperties* nuclear_properties =
    &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         0.0*MeV,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         0.0*kelvin,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the properties with an evaluation that is closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.26e-8*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.26e-8*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.4621694163672723e2*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the properties with an evaluation that is closest to 1.27e-8 MeV
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.27e-8*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.4737739355447903e2*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties evaluated at 2.5301e-08*MeV
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         2.5301e-08*MeV,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         2.936059397103837227e+02*kelvin,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties with an evaluation that is closest to 1.20e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.20e-7*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.20e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.3925423013021639e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties with an evaluation that is closest to 1.21e-7 MeV
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.21e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         1.4041468204796822e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties evaluated at 2.1543e-7*MeV 
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         2.1543e-7*MeV,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         2.4999615664127095e3*kelvin,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closes to 3e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         3e-7*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         3e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         0,
                                         3.4813557532554096e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closest to 0.0*MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         0.0*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         0.0*MeV,
                                         false );
  
  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         0.0*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties evaluated at 2.5301e-08*MeV
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         2.5301e-08*MeV,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         2.936059397103837227e+02*kelvin,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties with an evaluation that is closest to 1.20e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         1.20e-7*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         1.20e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         1.3925423013021639e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties with an evaluation that is closest to 1.21e-7 MeV
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         1.21e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         1.4041468204796822e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties evaluated at 2.1543e-7*MeV 
  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         2.1543e-7*MeV,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         2.4999615664127095e3*kelvin,
                                         true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closes to 3e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         3e-7*MeV,
                                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         3e-7*MeV,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getNuclearDataProperties(
                                         Data::NuclearDataProperties::ACE_FILE,
                                         1,
                                         3.4813557532554096e3*kelvin,
                                         false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::NuclearDataProperties::ACE_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );
}

//---------------------------------------------------------------------------//
// Check that the thermal nuclear data properties can be set
FRENSIE_UNIT_TEST( NuclideProperties, setThermalNuclearProperties )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O" ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "D2O" ) );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O" ) );
  
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ) );
  
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 0.0*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 0.0*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 1e-7*MeV ) );
    
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 0.0*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 0.0*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 1e-7*MeV ) );

  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ) );
  
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0 ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 0.0*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 0.0*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0, 1e-7*MeV ) );

  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O" ) );
  
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ) );
  
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 0.0*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 0.0*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0, 1e-7*MeV ) );
    
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 0.0*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 0.0*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.thermalNuclearDataAvailable( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1, 1e-7*MeV ) );
}

//---------------------------------------------------------------------------//
// Check that the thermal nuclear data names can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getThermalNuclearDataNames )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  std::set<std::string> data_names =
    nuclide_properties.getThermalNuclearDataNames();

  FRENSIE_CHECK_EQUAL( data_names.size(), 2 );
  FRENSIE_CHECK( data_names.count( "H2O" ) );
  FRENSIE_CHECK( data_names.count( "D2O" ) );
}

//---------------------------------------------------------------------------//
// Check that the data file types can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getThermalNuclearDataFileTypes )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  std::set<Data::ThermalNuclearDataProperties::FileType> file_types = 
    nuclide_properties.getThermalNuclearDataFileTypes( "H2O" );

  FRENSIE_CHECK_EQUAL( file_types.size(), 2 );
  FRENSIE_CHECK( file_types.count( Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ) );
  FRENSIE_CHECK( file_types.count( Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ) );

  file_types = nuclide_properties.getThermalNuclearDataFileTypes( "D2O" );

  FRENSIE_CHECK_EQUAL( file_types.size(), 1 );
  FRENSIE_CHECK( file_types.count( Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ) );
}

//---------------------------------------------------------------------------//
// Check that the thermal nuclear data file versions can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataFileVersions_thermal_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  FRENSIE_CHECK( nuclide_properties.getDataFileVersions( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ).empty() );
  FRENSIE_CHECK( nuclide_properties.getDataFileVersions( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ).empty() );
  FRENSIE_CHECK( nuclide_properties.getDataFileVersions( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ).empty() );
  FRENSIE_CHECK( nuclide_properties.getDataFileVersions( "D2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ).empty() );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  std::set<unsigned> versions = nuclide_properties.getDataFileVersions(
                "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );

  FRENSIE_CHECK_EQUAL( versions.size(), 2 );
  FRENSIE_CHECK( versions.count( 0 ) );
  FRENSIE_CHECK( versions.count( 1 ) );

  versions = nuclide_properties.getDataFileVersions(
                   "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );

  FRENSIE_CHECK_EQUAL( versions.size(), 1 );
  FRENSIE_CHECK( versions.count( 0 ) );

  versions = nuclide_properties.getDataFileVersions(
                "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );

  FRENSIE_CHECK_EQUAL( versions.size(), 2 );
  FRENSIE_CHECK( versions.count( 0 ) );
  FRENSIE_CHECK( versions.count( 1 ) );

  versions = nuclide_properties.getDataFileVersions(
                   "D2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );

  FRENSIE_CHECK( versions.empty() );
}

//---------------------------------------------------------------------------//
// Check that the recommended thermal nuclear data file version can be returned
FRENSIE_UNIT_TEST( NuclideProperties,
                   getRecommendedDataFileVersion_thermal_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }
  
  FRENSIE_CHECK_EQUAL( nuclide_properties.getRecommendedDataFileVersion( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ), 1 );
  FRENSIE_CHECK_EQUAL( nuclide_properties.getRecommendedDataFileVersion( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ), 0 );
  FRENSIE_CHECK_EQUAL( nuclide_properties.getRecommendedDataFileVersion( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE ), 1 );
  FRENSIE_CHECK_THROW( nuclide_properties.getRecommendedDataFileVersion( "D2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE ),
                       Data::InvalidScatteringCenterPropertiesRequest );
}

//---------------------------------------------------------------------------//
// Check that thermal nuclear data evaluation temps can be returned
FRENSIE_UNIT_TEST( NuclideProperties,
                   getDataEvaluationTempsInMeV_thermal_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  std::vector<Data::NuclideProperties::Energy> evaluation_temps =
    nuclide_properties.getDataEvaluationTempsInMeV( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Energy>({0.0*MeV, 2.5301e-08*MeV}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Energy>({2.5301e-08*MeV}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Energy>({0.0*MeV, 2.5301e-08*MeV}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Energy>({0.0*MeV, 2.5301e-08*MeV}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Energy>({0.0*MeV}) );
}

//---------------------------------------------------------------------------//
// Check that thermal nuclear data evaluation temps can be returned
FRENSIE_UNIT_TEST( NuclideProperties,
                   getDataEvaluationTemps_thermal_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1002, 1.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  std::vector<Data::NuclideProperties::Temperature> evaluation_temps =
    nuclide_properties.getDataEvaluationTemps( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Temperature>({0.0*kelvin, 2.936059397103837227e+02*kelvin}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTemps( "H2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Temperature>({2.936059397103837227e+02*kelvin}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTemps( "H2O", Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Temperature>({0.0*kelvin, 2.936059397103837227e+02*kelvin}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTemps( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 0 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Temperature>({0.0*kelvin, 2.936059397103837227e+02*kelvin}) );

  evaluation_temps = nuclide_properties.getDataEvaluationTemps( "D2O", Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE, 1 );

  FRENSIE_CHECK_EQUAL( evaluation_temps, std::vector<Data::NuclideProperties::Temperature>({0.0*kelvin}) );
}

//---------------------------------------------------------------------------//
// Check that thremal nuclear data properties can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getThermalNuclearDataProperties )
{
  Data::NuclideProperties nuclide_properties( 1002, 2.0 );

  {
    std::shared_ptr<const Data::ThermalNuclearDataProperties> test_data(
      new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "H2O",
                         {1001, 1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            0.0*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                            "H2O",
                            {1001, 1002, 8016},
                            2.5301e-08*MeV,
                            Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                            0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         0.0*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );
    
    nuclide_properties.setThermalNuclearDataProperties( test_data );

    test_data.reset(
       new Data::TestThermalNuclearDataProperties<Data::ThermalNuclearDataProperties>(
                         "D2O",
                         {1002, 8016},
                         2.5301e-08*MeV,
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1 ) );

    nuclide_properties.setThermalNuclearDataProperties( test_data );
  }

  // Get the H2O standard ace file v0 properties evaluated at 0.0*MeV
  const Data::ThermalNuclearDataProperties* thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         0.0*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         0.0*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O standard ace file v0 properties with an evaluation that is
  // closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.4621694163672723e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O standard ace file v0 properties with an evaluation that is
  // closest to 1.27e-8 MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.27e-8*MeV,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.4737739355447903e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the H2O standard ace file v0 properties evaluated at 2.5301e-08*MeV,
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         2.5301e-08*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         2.936059397103837227e+02*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the H2O standard ace file v1 properties evaluated at 0.0*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         0.0*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         0.0*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O standard ace file v1 properties with an evaluation that is
  // closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.26e-8*kelvin,
                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.26e-8*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.4621694163672723e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O standard ace file v1 properties with an evaluation that is
  // closest to 1.27e-8 MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.27e-8*MeV,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.4737739355447903e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the H2O standard ace file v1 properties evaluated at 2.5301e-08*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         2.5301e-08*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         2.936059397103837227e+02*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the H2O mcnp6 ace file v0 properties evaluated at 0.0*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         0.0*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         0.0*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O mcnp6 ace file v0 properties with an evaluation that is
  // closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         1.4621694163672723e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the H2O mcnp6 ace file v0 properties with an evaluation that is
  // closest to 1.27e-8 MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         1.27e-8*MeV,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         1.4737739355447903e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the H2O mcnp6 ace file v0 properties evaluated at 2.5301e-08*MeV,
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         2.5301e-08*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "H2O",
                         Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE,
                         0,
                         2.936059397103837227e+02*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "H2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::MCNP6_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the D2O standard ace file v0 properties evaluated at 0.0*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         0.0*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         0.0*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the D2O standard ace file v0 properties with an evaluation that is
  // closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.26e-8*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.4621694163672723e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the D2O standard ace file v0 properties with an evaluation that is
  // closest to 1.27e-8 MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.27e-8*MeV,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         1.4737739355447903e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the D2O standard ace file v0 properties evaluated at 2.5301e-08*MeV,
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         2.5301e-08*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         0,
                         2.936059397103837227e+02*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the D2O standard ace file v1 properties evaluated at 0.0*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         0.0*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         0.0*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the D2O standard ace file v1 properties with an evaluation that is
  // closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.26e-8*kelvin,
                         true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.26e-8*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.4621694163672723e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the D2O standard ace file v1 properties with an evaluation that is
  // closest to 1.27e-8 MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.27e-8*MeV,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         1.4737739355447903e2*kelvin,
                         false );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the D2O standard ace file v1 properties evaluated at 2.5301e-08*MeV
  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         2.5301e-08*MeV,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  thermal_nuclear_properties =
    &nuclide_properties.getThermalNuclearDataProperties(
                         "D2O",
                         Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE,
                         1,
                         2.936059397103837227e+02*kelvin,
                         true );

  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->name(), "D2O" );
  FRENSIE_CHECK( thermal_nuclear_properties->hasDataForZAID( nuclide_properties.zaid() ) );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileType(),
                       Data::ThermalNuclearDataProperties::STANDARD_ACE_FILE );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( thermal_nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );
}

//---------------------------------------------------------------------------//
// Check that the adjoint nuclear data properties can be set
FRENSIE_UNIT_TEST( NuclideProperties, setAdjointNuclearDataProperties )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE ) );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0 ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 0.0*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 2.936059397103837227e+02*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 1e-7*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 2.1543E-07*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 2.499961566412709544e+03*kelvin ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 0, 1e-6*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1 ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 0.0*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 1e-9*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 2.5301e-08*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 1e-7*MeV ) );
  FRENSIE_CHECK( nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 2.1543E-07*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 1, 1e-6*MeV ) );
  FRENSIE_CHECK( !nuclide_properties.adjointNuclearDataAvailable( Data::AdjointNuclearDataProperties::Native_FILE, 2 ) );
}

//---------------------------------------------------------------------------//
// Check that the adjoint nuclear data file types can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getAdjointNuclearDataFileTypes )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  std::set<Data::AdjointNuclearDataProperties::FileType> file_types =
    nuclide_properties.getAdjointNuclearDataFileTypes();

  FRENSIE_CHECK_EQUAL( file_types.size(), 1 );
  FRENSIE_CHECK( file_types.count( Data::AdjointNuclearDataProperties::Native_FILE ) );
}

//---------------------------------------------------------------------------//
// Check that the adjoint anuclear data file versions can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataFileVersions_adjoint_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  std::set<unsigned> versions = nuclide_properties.getDataFileVersions(
                                       Data::AdjointNuclearDataProperties::Native_FILE );

  FRENSIE_CHECK_EQUAL( versions.size(), 2 );
  FRENSIE_CHECK( versions.count( 0 ) );
  FRENSIE_CHECK( versions.count( 1 ) );
}

//---------------------------------------------------------------------------//
// Check that the recommended adjoint nuclear data file major version can be
// returned
FRENSIE_UNIT_TEST( NuclideProperties, getRecommendedDataFileVersion_adjoint_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               2 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  FRENSIE_CHECK_EQUAL( nuclide_properties.getRecommendedDataFileVersion( Data::AdjointNuclearDataProperties::Native_FILE ), 2 );
}

//---------------------------------------------------------------------------//
// Check that the adjoint nuclear data evaluation temperatures can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataEvaluationTempsInMeV_adjoint_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  std::vector<Data::AdjointNuclearDataProperties::Energy> data_evaluation_temps =
    nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::AdjointNuclearDataProperties::Native_FILE, 0 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::AdjointNuclearDataProperties::Energy>({0.0*MeV, 2.5301e-08*MeV, 2.1543E-07*MeV}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::AdjointNuclearDataProperties::Native_FILE, 1 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::AdjointNuclearDataProperties::Energy>({0.0*MeV, 2.1543E-07*MeV}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTempsInMeV(
                                    Data::AdjointNuclearDataProperties::Native_FILE, 2 );

  FRENSIE_CHECK( data_evaluation_temps.empty() );
}

//---------------------------------------------------------------------------//
// Check that the adjoint nuclear data evaluation temperatures can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getDataEvaluationTemps_adjoint_nuclear )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  std::vector<Data::AdjointNuclearDataProperties::Temperature> data_evaluation_temps =
    nuclide_properties.getDataEvaluationTemps(
                          Data::AdjointNuclearDataProperties::Native_FILE, 0 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::AdjointNuclearDataProperties::Temperature>({0.0*kelvin, 2.936059397103837227e+02*kelvin, 2.499961566412709544e+03*kelvin}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTemps(
                          Data::AdjointNuclearDataProperties::Native_FILE, 1 );

  FRENSIE_CHECK_EQUAL( data_evaluation_temps, std::vector<Data::AdjointNuclearDataProperties::Temperature>({0.0*kelvin, 2.499961566412709544e+03*kelvin}) );

  data_evaluation_temps = nuclide_properties.getDataEvaluationTemps(
                          Data::AdjointNuclearDataProperties::Native_FILE, 2 );

  FRENSIE_CHECK( data_evaluation_temps.empty() );
}

//---------------------------------------------------------------------------//
// Check that the adjoint nuclear data evaluation properties can be returned
FRENSIE_UNIT_TEST( NuclideProperties, getAdjointNuclearDataProperties )
{
  Data::NuclideProperties nuclide_properties( 1001, 1.0 );

  {
    std::shared_ptr<const Data::AdjointNuclearDataProperties> test_data(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               0.0*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.5301e-08*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );

    nuclide_properties.setAdjointNuclearDataProperties( test_data );

    test_data.reset(
              new Data::TestNuclearDataProperties<Data::AdjointNuclearDataProperties>(
                               1001,
                               1.0,
                               2.1543E-07*MeV,
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1 ) );
    
    nuclide_properties.setAdjointNuclearDataProperties( test_data );
  }

  // Get the properties evaluated at 0.0*MeV
  const Data::AdjointNuclearDataProperties* nuclear_properties =
    &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               0.0*MeV,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               0.0*kelvin,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the properties with an evaluation that is closest to 1.26e-8 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.26e-8*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.26e-8*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.4621694163672723e2*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       0.0*MeV );

  // Get the properties with an evaluation that is closest to 1.27e-8 MeV
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.27e-8*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.4737739355447903e2*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties evaluated at 2.5301e-08*MeV
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               2.5301e-08*MeV,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               2.936059397103837227e+02*kelvin,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties with an evaluation that is closest to 1.20e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.20e-7*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.20e-7*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.3925423013021639e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties with an evaluation that is closest to 1.21e-7 MeV
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.21e-7*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               1.4041468204796822e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties evaluated at 2.1543e-7*MeV 
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               2.1543e-7*MeV,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               2.4999615664127095e3*kelvin,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closes to 3e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               3e-7*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               3e-7*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               0,
                               3.4813557532554096e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 0 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closest to 0.0*MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               0.0*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               0.0*MeV,
                               false );
  
  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               0.0*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties evaluated at 2.5301e-08*MeV
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               2.5301e-08*MeV,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               2.936059397103837227e+02*kelvin,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-08*MeV );

  // Get the properties with an evaluation that is closest to 1.20e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               1.20e-7*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               1.20e-7*MeV,
                               false );
  
  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               1.3925423013021639e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.5301e-8*MeV );

  // Get the properties with an evaluation that is closest to 1.21e-7 MeV
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               1.21e-7*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               1.4041468204796822e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties evaluated at 2.1543e-7*MeV 
  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               2.1543e-7*MeV,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               2.4999615664127095e3*kelvin,
                               true );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  // Get the properties with an evaluation that is closes to 3e-7 MeV
  FRENSIE_CHECK_THROW( nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               3e-7*MeV,
                               true ),
                       Data::InvalidScatteringCenterPropertiesRequest );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               3e-7*MeV,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );

  nuclear_properties = &nuclide_properties.getAdjointNuclearDataProperties(
                               Data::AdjointNuclearDataProperties::Native_FILE,
                               1,
                               3.4813557532554096e3*kelvin,
                               false );

  FRENSIE_CHECK_EQUAL( nuclear_properties->zaid(),
                       nuclide_properties.zaid() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->atomicWeightRatio(),
                       nuclide_properties.atomicWeightRatio() );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileType(),
                       Data::AdjointNuclearDataProperties::Native_FILE );
  FRENSIE_CHECK_EQUAL( nuclear_properties->fileVersion(), 1 );
  FRENSIE_CHECK_EQUAL( nuclear_properties->evaluationTemperatureInMeV(),
                       2.1543e-7*MeV );
}

//---------------------------------------------------------------------------//
// end tstNuclideProperties.cpp
//---------------------------------------------------------------------------//
