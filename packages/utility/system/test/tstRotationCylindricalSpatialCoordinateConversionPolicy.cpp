//---------------------------------------------------------------------------//
//!
//! \file   tstRotationCylindricalSpatialCoordinateConversionPolicy.cpp
//! \author Alex Robinson
//! \brief  Rotation cylindrical spatial coordinate conversion policy tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <memory>

// FRENSIE Includes
#include "Utility_RotationCylindricalSpatialCoordinateConversionPolicy.hpp"
#include "Utility_3DCartesianVectorHelpers.hpp"
#include "Utility_Vector.hpp"
#include "Utility_Array.hpp"
#include "Utility_QuantityTraits.hpp"
#include "Utility_PhysicalConstants.hpp"
#include "Utility_UnitTestHarnessWithMain.hpp"
#include "ArchiveTestHelpers.hpp"

//---------------------------------------------------------------------------//
// Testing Types
//---------------------------------------------------------------------------//

typedef TestArchiveHelper::TestArchives TestArchives;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the local cylindrical spatial coordinates can be converted to
// global Cartesian spatial coordinates
FRENSIE_UNIT_TEST( RotationCylindricalSpatialCoordinateConversionPolicy,
                   convertToCartesianSpatialCoordinates )
{
  std::shared_ptr<const Utility::SpatialCoordinateConversionPolicy> policy(
            new Utility::RotationCylindricalSpatialCoordinateConversionPolicy(
                               std::vector<double>({1.0, 1.0, 1.0}).data() ) );

  // Local z-axis
  std::array<double,3> local_cylindrical_position = {0.0, 0.0, 2.0};

  std::array<double,3> global_cartesian_position;
  std::array<double,3> ref_global_cartesian_position =
    {2.0/sqrt(3.0), 2.0/sqrt(3.0), 2.0/sqrt(3.0)};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );

  // Local neg. z-axis
  local_cylindrical_position = {0.0, 0.0, -2.0};
  ref_global_cartesian_position =
    {-2.0/sqrt(3.0), -2.0/sqrt(3.0), -2.0/sqrt(3.0)};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );

  // Local y-axis
  local_cylindrical_position = {2.0, Utility::PhysicalConstants::pi/2, 0.0};
  ref_global_cartesian_position = {-sqrt(2.0), sqrt(2.0), 0.0};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );

  // Local neg. y-axis
  local_cylindrical_position = {2.0, 3*Utility::PhysicalConstants::pi/2, 0.0};
  ref_global_cartesian_position = {sqrt(2.0), -sqrt(2.0), 0.0};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );

  // Local x-axis
  local_cylindrical_position = {2.0, 0.0, 0.0};
  ref_global_cartesian_position =
    {2.0/sqrt(6.0), 2.0/sqrt(6.0), -sqrt(8.0/3.0)};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );

  // Local neg. x-axis
  local_cylindrical_position = {2.0, Utility::PhysicalConstants::pi, 0.0};
  ref_global_cartesian_position =
    {-2.0/sqrt(6.0), -2.0/sqrt(6.0), sqrt(8.0/3.0)};

  policy->convertToCartesianSpatialCoordinates(
                                       local_cylindrical_position.data(),
                                       global_cartesian_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( global_cartesian_position,
                                   ref_global_cartesian_position,
                                   1e-15 );  
}

//---------------------------------------------------------------------------//
// Check that the global Cartesian spatial coordinates can be converted to
// local cylindrical spatial coordinates
FRENSIE_UNIT_TEST( RotationCylindricalSpatialCoordinateConversionPolicy,
                   convertFromCartesianSpatialCoordinates )
{
  std::shared_ptr<const Utility::SpatialCoordinateConversionPolicy> policy(
            new Utility::RotationCylindricalSpatialCoordinateConversionPolicy(
                               std::vector<double>({1.0, 1.0, 1.0}).data() ) );

  // Local z-axis
  std::array<double,3> global_cartesian_position =
    {2.0/sqrt(3.0), 2.0/sqrt(3.0), 2.0/sqrt(3.0)};

  std::array<double,3> local_cylindrical_position;
  std::array<double,3> ref_local_cylindrical_position = {0.0, 0.0, 2.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );

  // Local neg. z-axis
  global_cartesian_position = {-2.0/sqrt(3.0), -2.0/sqrt(3.0), -2.0/sqrt(3.0)};
  ref_local_cylindrical_position = {0.0, 0.0, -2.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );

  // Local y-axis
  global_cartesian_position = {-sqrt(2.0), sqrt(2.0), 0.0};
  ref_local_cylindrical_position =
    {2.0, Utility::PhysicalConstants::pi/2, 0.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );

  // Local neg. y-axis
  global_cartesian_position = {sqrt(2.0), -sqrt(2.0), 0.0};
  ref_local_cylindrical_position =
    {2.0, 3*Utility::PhysicalConstants::pi/2, 0.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );

  // Local x-axis
  global_cartesian_position = {2.0/sqrt(6.0), 2.0/sqrt(6.0), -sqrt(8.0/3.0)};
  ref_local_cylindrical_position = {2.0, 0.0, 0.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );
  
  // Local neg. x-axis
  global_cartesian_position = {-2.0/sqrt(6.0), -2.0/sqrt(6.0), sqrt(8.0/3.0)};
  ref_local_cylindrical_position = {2.0, Utility::PhysicalConstants::pi, 0.0};

  policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
  FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                   ref_local_cylindrical_position,
                                   1e-15 );
}

//---------------------------------------------------------------------------//
// Check that a policy can be archived
FRENSIE_UNIT_TEST_TEMPLATE_EXPAND( RotationCylindricalSpatialCoordinateConversionPolicy,
                                   archive,
                                   TestArchives )
{
  FETCH_TEMPLATE_PARAM( 0, RawOArchive );
  FETCH_TEMPLATE_PARAM( 1, RawIArchive );

  typedef typename std::remove_pointer<RawOArchive>::type OArchive;
  typedef typename std::remove_pointer<RawIArchive>::type IArchive;
  
  std::string archive_base_name( "test_rotation_cylindrical_spatial_coordinate_conversion_policy" );
  std::ostringstream archive_ostream;

  // Create and archive some uniform distributions
  {
    std::unique_ptr<OArchive> oarchive;

    createOArchive( archive_base_name, archive_ostream, oarchive );

    Utility::RotationCylindricalSpatialCoordinateConversionPolicy
      concrete_policy( std::vector<double>({1.0, 1.0, 1.0}).data() );

    std::shared_ptr<const Utility::SpatialCoordinateConversionPolicy>
      shared_policy(
            new Utility::RotationCylindricalSpatialCoordinateConversionPolicy(
                               std::vector<double>({1.0, 1.0, 1.0}).data() ) );

    FRENSIE_REQUIRE_NO_THROW( (*oarchive) << BOOST_SERIALIZATION_NVP( concrete_policy ) );
    FRENSIE_REQUIRE_NO_THROW( (*oarchive) << BOOST_SERIALIZATION_NVP( shared_policy ) );
  }

  // Copy the archive ostream to an istream
  std::istringstream archive_istream( archive_ostream.str() );

  // Load the archived distributions
  std::unique_ptr<IArchive> iarchive;

  createIArchive( archive_istream, iarchive );

  Utility::RotationCylindricalSpatialCoordinateConversionPolicy
    concrete_policy( std::vector<double>({0.0, 0.0, 0.0}).data() );
  
  std::shared_ptr<const Utility::SpatialCoordinateConversionPolicy>
    shared_policy;

  FRENSIE_REQUIRE_NO_THROW( (*iarchive) >> BOOST_SERIALIZATION_NVP( concrete_policy ) );
  FRENSIE_REQUIRE_NO_THROW( (*iarchive) >> BOOST_SERIALIZATION_NVP( shared_policy ) );

  iarchive.reset();

  {
    // Local z-axis
    std::array<double,3> global_cartesian_position =
      {2.0/sqrt(3.0), 2.0/sqrt(3.0), 2.0/sqrt(3.0)};

    std::array<double,3> local_cylindrical_position;
    std::array<double,3> ref_local_cylindrical_position = {0.0, 0.0, 2.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local neg. z-axis
    global_cartesian_position = {-2.0/sqrt(3.0), -2.0/sqrt(3.0), -2.0/sqrt(3.0)};
    ref_local_cylindrical_position = {0.0, 0.0, -2.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local y-axis
    global_cartesian_position = {-sqrt(2.0), sqrt(2.0), 0.0};
    ref_local_cylindrical_position =
      {2.0, Utility::PhysicalConstants::pi/2, 0.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local neg. y-axis
    global_cartesian_position = {sqrt(2.0), -sqrt(2.0), 0.0};
    ref_local_cylindrical_position =
      {2.0, 3*Utility::PhysicalConstants::pi/2, 0.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local x-axis
    global_cartesian_position = {2.0/sqrt(6.0), 2.0/sqrt(6.0), -sqrt(8.0/3.0)};
    ref_local_cylindrical_position = {2.0, 0.0, 0.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );
  
    // Local neg. x-axis
    global_cartesian_position = {-2.0/sqrt(6.0), -2.0/sqrt(6.0), sqrt(8.0/3.0)};
    ref_local_cylindrical_position = {2.0, Utility::PhysicalConstants::pi, 0.0};

    concrete_policy.convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );
  }

  {
    // Local z-axis
    std::array<double,3> global_cartesian_position =
      {2.0/sqrt(3.0), 2.0/sqrt(3.0), 2.0/sqrt(3.0)};

    std::array<double,3> local_cylindrical_position;
    std::array<double,3> ref_local_cylindrical_position = {0.0, 0.0, 2.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local neg. z-axis
    global_cartesian_position = {-2.0/sqrt(3.0), -2.0/sqrt(3.0), -2.0/sqrt(3.0)};
    ref_local_cylindrical_position = {0.0, 0.0, -2.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local y-axis
    global_cartesian_position = {-sqrt(2.0), sqrt(2.0), 0.0};
    ref_local_cylindrical_position =
      {2.0, Utility::PhysicalConstants::pi/2, 0.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local neg. y-axis
    global_cartesian_position = {sqrt(2.0), -sqrt(2.0), 0.0};
    ref_local_cylindrical_position =
      {2.0, 3*Utility::PhysicalConstants::pi/2, 0.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );

    // Local x-axis
    global_cartesian_position = {2.0/sqrt(6.0), 2.0/sqrt(6.0), -sqrt(8.0/3.0)};
    ref_local_cylindrical_position = {2.0, 0.0, 0.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );
  
    // Local neg. x-axis
    global_cartesian_position = {-2.0/sqrt(6.0), -2.0/sqrt(6.0), sqrt(8.0/3.0)};
    ref_local_cylindrical_position = {2.0, Utility::PhysicalConstants::pi, 0.0};

    shared_policy->convertFromCartesianSpatialCoordinates(
                                      global_cartesian_position.data(),
                                      local_cylindrical_position.data() );
  
    FRENSIE_CHECK_FLOATING_EQUALITY( local_cylindrical_position,
                                     ref_local_cylindrical_position,
                                     1e-15 );
  }
}

//---------------------------------------------------------------------------//
// end tstRotationCylindricalSpatialCoordinateConversionPolicy.cpp
//---------------------------------------------------------------------------//
