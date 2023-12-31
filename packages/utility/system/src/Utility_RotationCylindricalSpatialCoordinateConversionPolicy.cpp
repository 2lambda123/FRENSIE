//---------------------------------------------------------------------------//
//!
//! \file   Utility_RotationCylindricalSpatialCoordinateConversionPolicy.cpp
//! \author Alex Robinson
//! \brief  Rotation cylindrical spatial coordinate conversion policy def.
//!
//---------------------------------------------------------------------------//

// FRENSIE Includes
#include "FRENSIE_Archives.hpp"
#include "Utility_RotationCylindricalSpatialCoordinateConversionPolicy.hpp"
#include "Utility_3DCartesianVectorHelpers.hpp"
#include "Utility_DesignByContract.hpp"

namespace Utility{

// Constructor
/*! \details The axis parameter is the z-axis of the local coordinate system
 * w.r.t. the global Cartesian coordinate system. It does not need to be a 
 * unit vector as it will be normalized by the constructor. 
 */ 
RotationCylindricalSpatialCoordinateConversionPolicy::RotationCylindricalSpatialCoordinateConversionPolicy(
                                                         const double axis[3] )
  : d_axis{ axis[0], axis[1], axis[2] }
{
  // Normalize the axis vector
  normalizeVector( d_axis );
}

// Convert the spatial coordinates to cartesian coordinates
/*! \details The primary coordinate is the local r coordinate. The secondary
 * coordinate is the local theta (azimuthal) coordinate. The tertiary
 * coordinate is the local z coordinate. They will be converted to coordinates 
 * in the global Cartesian coordinate system.
 */
void RotationCylindricalSpatialCoordinateConversionPolicy::convertToCartesianSpatialCoordinates(
                                          const double primary_spatial_coord,
                                          const double secondary_spatial_coord,
                                          const double tertiary_spatial_coord,
                                          double& x_spatial_coord,
                                          double& y_spatial_coord,
                                          double& z_spatial_coord ) const
{
  // Make sure that the radial spatial coordinate is valid
  testPrecondition( primary_spatial_coord >= 0.0 );

  // Convert the cylindrical coordinates to local cartesian coordinates
  double local_x_spatial_coord, local_y_spatial_coord, local_z_spatial_coord;

  this->convertToCartesianPosition( primary_spatial_coord,
                                    secondary_spatial_coord,
                                    tertiary_spatial_coord,
                                    local_x_spatial_coord,
                                    local_y_spatial_coord,
                                    local_z_spatial_coord );
  
  // Convert the local Cartesian coordinates to global Cartesian coordinates
  convertLocalVectorToGlobalVector( local_x_spatial_coord,
                                    local_y_spatial_coord,
                                    local_z_spatial_coord,
                                    d_axis,
                                    x_spatial_coord,
                                    y_spatial_coord,
                                    z_spatial_coord );

  // Clear rounding errors
  clearVectorOfRoundingErrors( x_spatial_coord,
                               y_spatial_coord,
                               z_spatial_coord,
                               1e-15 );
}

// Convert the cartesian coordinates to the spatial coordinate system
/*! \details The primary coordinate is the local r coordinate. The secondary
 * coordinate is the local theta (azimuthal) coordinate. The tertiary
 * coordinate is the local z coordinate. The global Cartesian coordinates will 
 * be converted to these local coordinates.
 */
void RotationCylindricalSpatialCoordinateConversionPolicy::convertFromCartesianSpatialCoordinates(
                                         const double x_spatial_coord,
                                         const double y_spatial_coord,
                                         const double z_spatial_coord,
                                         double& primary_spatial_coord,
                                         double& secondary_spatial_coord,
                                         double& tertiary_spatial_coord ) const
{
  // Convert the global cartesian coordinates to local cartesian coordinates
  double local_x_spatial_coord, local_y_spatial_coord, local_z_spatial_coord;

  convertGlobalVectorToLocalVector( x_spatial_coord,
                                    y_spatial_coord,
                                    z_spatial_coord,
                                    d_axis,
                                    local_x_spatial_coord,
                                    local_y_spatial_coord,
                                    local_z_spatial_coord );

  // Clear rounding errors
  clearVectorOfRoundingErrors( local_x_spatial_coord,
                               local_y_spatial_coord,
                               local_z_spatial_coord,
                               1e-15 );

  // Convert the local Cartesian coordinates to cylindrical coordinates
  this->convertFromCartesianPosition( local_x_spatial_coord,
                                      local_y_spatial_coord,
                                      local_z_spatial_coord,
                                      primary_spatial_coord,
                                      secondary_spatial_coord,
                                      tertiary_spatial_coord );
}

EXPLICIT_CLASS_SAVE_LOAD_INST( RotationCylindricalSpatialCoordinateConversionPolicy );

} // end Utility namespace

BOOST_CLASS_EXPORT_IMPLEMENT( Utility::RotationCylindricalSpatialCoordinateConversionPolicy );

//---------------------------------------------------------------------------//
// end Utility_RotationCylindricalSpatialCoordinateConversionPolicy.cpp
//---------------------------------------------------------------------------//
