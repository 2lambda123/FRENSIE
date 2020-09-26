//---------------------------------------------------------------------------//
//!
//! \file   Utility_PQLAQuadrature.cpp
//! \author Philip Britt
//! \brief  PQLA quadrature discretization definition
//!
//---------------------------------------------------------------------------//

// FRENSIE Includes
#include "Utility_PQLAQuadrature.hpp"
#include "Utility_DesignByContract.hpp"
#include "Utility_3DCartesianVectorHelpers.hpp"

// std includes
#include <cmath>
#include <iostream>

namespace Utility{

// Constructor
PQLAQuadrature::PQLAQuadrature(unsigned quadrature_order)
{
  testPrecondition(quadrature_order > 0);
  d_quadrature_order = quadrature_order;

  // Form the triangle vertices

  // Initialize triangle vertex indices at lower left corner of positive domain octahedron face
  // This point is always either at lower left end of right-side-up triangle or upper left of up-side-down triangle
  double i_x = static_cast<double>(quadrature_order);
  double i_y = 0.0;
  double i_z = 0.0;

  // Used for iteration purposes (to tell where you are in use of conditionals)
  unsigned x_index = quadrature_order;
  unsigned y_index = 0;
  unsigned z_index = 0;

  // Triangle row index (used for row iteration)
  unsigned row_index = quadrature_order;

  for(size_t triangle = 0; triangle < this->getNumberOfTriangles(); ++triangle)
  {
    // Array vector (for triangle formation later)
    std::vector<std::array<double, 3>> vertex_vector;

    double quad_order_divisor = static_cast<double>(quadrature_order);
    vertex_vector.push_back({i_x/quad_order_divisor, i_y/quad_order_divisor, i_z/quad_order_divisor});
    if(triangle % 2 == 0)
    {
      // Always go counter-clockwise with vertices around triangle (start lower left vertex)
      vertex_vector.push_back({(i_x-1.0)/quad_order_divisor, (i_y+1.0)/quad_order_divisor, i_z/quad_order_divisor});
      vertex_vector.push_back({(i_x-1.0)/quad_order_divisor, i_y/quad_order_divisor, (i_z+1.0)/quad_order_divisor});

      // Set info for next triangle (not used in calculations below if blocks)
      if(x_index > 0)
      {
        x_index = x_index - 1;
        z_index = z_index + 1;
        // y _index doesn't change;

        i_x = vertex_vector[2][0];
        i_z = vertex_vector[2][1];
      }
      else // reset to next row
      {
        row_index = row_index - 1;
        x_index = row_index;
        y_index = quadrature_order - row_index;
        z_index = 0;

        i_x = static_cast<double>(x_index);
        i_y = static_cast<double>(y_index);
        i_z = 0.0;
      }
    }
    else
    {
      // Always go counter-clockwise with vertices around triangle (start upper left vertex)
      vertex_vector.push_back({i_x/quad_order_divisor, (i_y+1.0)/quad_order_divisor, (i_z-1.0)/quad_order_divisor});
      vertex_vector.push_back({(i_x-1.0)/quad_order_divisor, (i_y+1.0)/quad_order_divisor, i_z/quad_order_divisor});

      // Do nothing for next triangle if triangle is pointing down (same vertex starting point for next triangle)
    }
    // normalize vectors to 2-norm (currently in 1-norm)

    for(size_t i = 0; i < 3; ++i)
    {
      normalizeVector(vertex_vector[i].data());
    }


    
    // calculate cosine of length of side of spherical triangle opposite from respective vertex (for use later, not kept as member data)
    std::vector<double> opposite_cos_vector {calculateCosineOfAngleBetweenUnitVectors(vertex_vector[1].data(), vertex_vector[2].data()),
                                             calculateCosineOfAngleBetweenUnitVectors(vertex_vector[0].data(), vertex_vector[2].data()),
                                             calculateCosineOfAngleBetweenUnitVectors(vertex_vector[0].data(), vertex_vector[1].data())};

    // calculate length of side of spherical triangle opposite from respective vertex (in radians b/c unit sphere)
    std::vector<double> opposite_side_length_vector{acos(opposite_cos_vector[0]), acos(opposite_cos_vector[1]), acos(opposite_cos_vector[2])};

    std::vector<double> angle_vector{acos((opposite_cos_vector[0] - opposite_cos_vector[1]*opposite_cos_vector[2])/(sin(opposite_side_length_vector[1])*sin(opposite_side_length_vector[2]))),
                                     acos((opposite_cos_vector[1] - opposite_cos_vector[0]*opposite_cos_vector[2])/(sin(opposite_side_length_vector[0])*sin(opposite_side_length_vector[2]))),
                                     acos((opposite_cos_vector[2] - opposite_cos_vector[0]*opposite_cos_vector[1])/(sin(opposite_side_length_vector[0])*sin(opposite_side_length_vector[1])))};

    // Store triangle area
    double triangle_area = opposite_side_length_vector[0]+opposite_side_length_vector[1]+opposite_side_length_vector[2]-M_PI;

    SphericalTriangle local_triangle;

    for(size_t i = 0; i < 3; ++i)
    {
      local_triangle.triangle_parameter_vector.push_back( std::make_tuple(vertex_vector[i], opposite_side_length_vector[i], angle_vector[i]));
    }

    local_triangle.area = triangle_area;
    // Store triangle info
    d_spherical_triangle_vector.push_back(local_triangle);
    // if triangle is pointing up
  }
}

// Find which triangle bin a direction vector is in
unsigned PQLAQuadrature::findTriangleBin(const std::array<double, 3>& direction) const
{

  // Convert direction vector to 1-norm vector
  std::array<double, 3> direction_normalized_1_norm;
  
  normalizeVectorToOneNorm(direction,
                           direction_normalized_1_norm);

  return this->calculatePositiveTriangleBinIndex(static_cast<int>(fabs(direction_normalized_1_norm[0])*d_quadrature_order),
                                                 static_cast<int>(fabs(direction_normalized_1_norm[1])*d_quadrature_order),
                                                 static_cast<int>(fabs(direction_normalized_1_norm[2])*d_quadrature_order))
         +this->findSecondaryIndex(std::signbit(direction_normalized_1_norm[0]), 
                                    std::signbit(direction_normalized_1_norm[1]),
                                    std::signbit(direction_normalized_1_norm[2]))*std::pow(d_quadrature_order, 2);
}  

// Find which triangle bin a direction vector is in (takes 2-norm vector)
unsigned PQLAQuadrature::findTriangleBin( const double x_direction, const double y_direction, const double z_direction) const
{
  std::array<double, 3> direction_array {x_direction, y_direction, z_direction};

  return this->findTriangleBin(direction_array);
}

// Return the order of the quadrature
unsigned PQLAQuadrature::getQuadratureOrder() const
{
  return d_quadrature_order;
}

// Return the total number of triangles
size_t PQLAQuadrature::getNumberOfTriangles() const
{
  return 8*pow(d_quadrature_order,2);
}

// Return the area of a triangle
double PQLAQuadrature::getTriangleArea(size_t triangle_index) const
{
  testPrecondition(triangle_index >= 0 && triangle_index <= this->getNumberOfTriangles()-1);
  // Convert to positive domain triangle index
  if(triangle_index > pow(d_quadrature_order, 2)-1)
  {
    triangle_index = triangle_index%(d_quadrature_order*d_quadrature_order);
  }

  return d_spherical_triangle_vector[triangle_index].area;
}

// Return a random direction from within a spherical triangle
void sampleIsotropicallyFromTriangle(std::array<double, 3>& direction_vector)
{
  
}

// Converts direction vector to 1-norm normalized vector
void PQLAQuadrature::normalizeVectorToOneNorm( const std::array<double, 3>& direction_normalized_2_norm,
                                               std::array<double, 3>& direction_normalized_1_norm ) const
{
  double normalization_constant = fabs(direction_normalized_2_norm[0]) + fabs(direction_normalized_2_norm[1]) + fabs(direction_normalized_2_norm[2]);

  direction_normalized_1_norm[0] = direction_normalized_2_norm[0]/normalization_constant;
  direction_normalized_1_norm[1] = direction_normalized_2_norm[1]/normalization_constant;
  direction_normalized_1_norm[2] = direction_normalized_2_norm[2]/normalization_constant;
}

// Converts direction vector to 1-norm normalized vector
void PQLAQuadrature::normalizeVectorToOneNorm(  const double x_direction,
                                                const double y_direction, 
                                                const double z_direction,
                                                std::array<double, 3>& direction_normalized_1_norm) const
{
  std::array<double, 3> direction_normalized_2_norm {x_direction, y_direction, z_direction};

  this->normalizeVectorToOneNorm( direction_normalized_2_norm,
                                  direction_normalized_1_norm );
}

// Take lower bounding plane indices of direction vector to form triangle index
unsigned PQLAQuadrature::calculatePositiveTriangleBinIndex(const unsigned i_x, const unsigned i_y, const unsigned i_z) const
{

  unsigned sum = 0;
  /* If not on the first row (first i_x plane), first row has 2N-1 triangles, and 2 less with every row from there.
    Calculate the sum of these elements until the relevant row is found*/
  for(unsigned i = 0; i<i_x; i++)
  {
    sum = sum + 2*(d_quadrature_order - i) - 1;
  }
  // Add the basic equation for calculating the triangle index
  sum = sum + d_quadrature_order + i_z - i_x - i_y - 1;

  // handle edge cases, default to lower plane index on edge cases (i_z is added, i_y is subtracted, making the below logic correct)
  if( i_x + i_y + i_z == d_quadrature_order)
  {
    if( i_z > 0 )
    {
      sum = sum - 1;
    }else if (i_x != d_quadrature_order)
    {
      sum = sum + 1;
    }
  }
  return sum;
}

// Returns the index for the octant that a direction is in
unsigned PQLAQuadrature::findSecondaryIndex(const bool x_sign, const bool y_sign, const bool z_sign) const
{
  unsigned secondary_index = 0;

  if (x_sign) secondary_index += 1;
  if (y_sign) secondary_index += 2;
  if (z_sign) secondary_index += 4;

  return secondary_index;
  
}

} // end Utility namespace

//---------------------------------------------------------------------------//
// end Utility_PQLAQuadrature.cpp
//---------------------------------------------------------------------------//