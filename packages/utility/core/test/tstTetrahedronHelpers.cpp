//---------------------------------------------------------------------------//
//!
//! \file   tstTetrahedronHelpers.cpp
//! \author Alex Robinson, Eli Moll
//! \brief  Tetrahedron helper functions
//!
//---------------------------------------------------------------------------//

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_Array.hpp>
#include <Teuchos_Tuple.hpp>

// Moab Includes
#include <moab/Matrix3.hpp>
#include <moab/CartVect.hpp>

// FRENSIE Includes
#include "Utility_TetrahedronHelpers.hpp"

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the volume of a tet can be calculated
TEUCHOS_UNIT_TEST( TetrahedronHelpers, calculateTetrahedronVolume_array )
{
  double vertex_a[3] = {0.0, 0.0, 0.0};
  double vertex_b[3] = {1.0, 0.0, 0.0};
  double vertex_c[3] = {0.0, 1.0, 0.0};
  double vertex_d[3] = {0.0, 0.0, 1.0};

  double volume = Utility::calculateTetrahedronVolume( vertex_a,
						       vertex_b,
						       vertex_c,
						       vertex_d );

  TEST_EQUALITY_CONST( volume, 1.0/6.0 );
}

//---------------------------------------------------------------------------//
// Check that the volume of a tet can be calculated
TEUCHOS_UNIT_TEST( TetrahedronHelpers, calculateTetrahedronVolume_cartvect )
{
  moab::CartVect vertex_a( 0.0, 0.0, 0.0 );
  moab::CartVect vertex_b( 1.0, 0.0, 0.0 );
  moab::CartVect vertex_c( 0.0, 1.0, 0.0 );
  moab::CartVect vertex_d( 0.0, 0.0, 1.0 );
  
  double volume = Utility::calculateTetrahedronVolume( vertex_a,
						       vertex_b,
						       vertex_c,
						       vertex_d );
  
  TEST_EQUALITY_CONST( volume, 1.0/6.0 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated with Matrix3
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                   calculateBarycentricTransformMatrix_array_Matrix3 )
{
  double vertex_a[3] = { 0.0, 0.0, 0.0 };
  double vertex_b[3] = { 1.0, 0.0, 0.0 };
  double vertex_c[3] = { 0.0, 1.0, 0.0 };
  double vertex_d[3] = { 0.0, 0.0, 1.0 };
  moab::Matrix3 transform_matrix;
  
  // Usage Test Passing Matrix3
  Utility::calculateBarycentricTransformMatrix(
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 transform_matrix[0] );
					 
  Teuchos::Tuple<double,9> transform_matrix_tuple = 
    Teuchos::tuple<double>( transform_matrix( 0, 0), transform_matrix( 0, 1),
                            transform_matrix( 0, 2), transform_matrix( 1, 0),
                            transform_matrix( 1, 1), transform_matrix( 1, 2),
                            transform_matrix( 2, 0), transform_matrix( 2, 1),
                            transform_matrix( 2, 2) );
                         
    
  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  TEST_COMPARE_FLOATING_ARRAYS( transform_matrix_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated with Matrix3
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                   calculateBarycentricTransformMatrix_cartvect_Matrix3 )
{
  moab::CartVect vertex_a( 0.0, 0.0, 0.0 );
  moab::CartVect vertex_b( 1.0, 0.0, 0.0 );
  moab::CartVect vertex_c( 0.0, 1.0, 0.0 );
  moab::CartVect vertex_d( 0.0, 0.0, 1.0 );
  moab::Matrix3 transform_matrix;
  
  // Usage Test Passing Matrix3
  Utility::calculateBarycentricTransformMatrix(
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 transform_matrix[0] );
					 
  Teuchos::Tuple<double,9> transform_matrix_tuple = 
    Teuchos::tuple<double>( transform_matrix( 0, 0), transform_matrix( 0, 1),
                            transform_matrix( 0, 2), transform_matrix( 1, 0),
                            transform_matrix( 1, 1), transform_matrix( 1, 2),
                            transform_matrix( 2, 0), transform_matrix( 2, 1),
                            transform_matrix( 2, 2) );
                         
    
  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  TEST_COMPARE_FLOATING_ARRAYS( transform_matrix_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated with Teuchos
//   SerialDenseMatrix
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                   calculateBarycentricTransformMatrix_array_SerialDenseMatrix )
{
  double vertex_a[3] = { 0.0, 0.0, 0.0 };
  double vertex_b[3] = { 1.0, 0.0, 0.0 };
  double vertex_c[3] = { 0.0, 1.0, 0.0 };
  double vertex_d[3] = { 0.0, 0.0, 1.0 };
  Teuchos::SerialDenseMatrix<int, double> transform_matrix( 3, 3 );
  
  // Usage Test Passing Matrix3
  Utility::calculateBarycentricTransformMatrix(
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 transform_matrix );
					 
  Teuchos::Tuple<double,9> transform_matrix_tuple = 
    Teuchos::tuple<double>( transform_matrix( 0, 0), transform_matrix( 0, 1),
                            transform_matrix( 0, 2), transform_matrix( 1, 0),
                            transform_matrix( 1, 1), transform_matrix( 1, 2),
                            transform_matrix( 2, 0), transform_matrix( 2, 1),
                            transform_matrix( 2, 2) );
                         
    
  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  TEST_COMPARE_FLOATING_ARRAYS( transform_matrix_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated with Teuchos
//   SerialDenseMatrix
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                calculateBarycentricTransformMatrix_cartvect_SerialDenseMatrix )
{
  moab::CartVect vertex_a( 0.0, 0.0, 0.0 );
  moab::CartVect vertex_b( 1.0, 0.0, 0.0 );
  moab::CartVect vertex_c( 0.0, 1.0, 0.0 );
  moab::CartVect vertex_d( 0.0, 0.0, 1.0 );
  Teuchos::SerialDenseMatrix<int, double> transform_matrix( 3, 3 );
  
  // Usage Test Passing Matrix3
  Utility::calculateBarycentricTransformMatrix(
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 transform_matrix );
					 
  Teuchos::Tuple<double,9> transform_matrix_tuple = 
    Teuchos::tuple<double>( transform_matrix( 0, 0), transform_matrix( 0, 1),
                            transform_matrix( 0, 2), transform_matrix( 1, 0),
                            transform_matrix( 1, 1), transform_matrix( 1, 2),
                            transform_matrix( 2, 0), transform_matrix( 2, 1),
                            transform_matrix( 2, 2) );
                         
    
  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  TEST_COMPARE_FLOATING_ARRAYS( transform_matrix_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated with arrays
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                   calculateBarycentricTransformMatrix_array_array )
{
  double vertex_a[3] = { 0.0, 0.0, 0.0 };
  double vertex_b[3] = { 1.0, 0.0, 0.0 };
  double vertex_c[3] = { 0.0, 1.0, 0.0 };
  double vertex_d[3] = { 0.0, 0.0, 1.0 };
  double transform_arrays[9];
  Teuchos::Array<double> teuchos_transform_array( 9 );
  Teuchos::Tuple<double,9> teuchos_transform_tuple;

  // Usage A Test (safe)
  Utility::calculateBarycentricTransformMatrix( vertex_a,
						vertex_b,
						vertex_c,
						vertex_d,
						transform_arrays );
  
  TEST_FLOATING_EQUALITY( transform_arrays[0], -1.0, 1e-12 );

  // Usage B Test (safe)
  Utility::calculateBarycentricTransformMatrix( 
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 teuchos_transform_array.getRawPtr() );

  TEST_FLOATING_EQUALITY( teuchos_transform_array[0], -1.0, 1e-12 );

  // Usage C Test (safest)
  Utility::calculateBarycentricTransformMatrix( 
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 teuchos_transform_tuple.getRawPtr() );

  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  
  TEST_COMPARE_FLOATING_ARRAYS( teuchos_transform_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the barycentric transform matrix can be calculated
TEUCHOS_UNIT_TEST( TetrahedronHelpers, 
                   calculateBarycentricTransformMatrix_cartvect_array )
{
  moab::CartVect vertex_a( 0.0, 0.0, 0.0 );
  moab::CartVect vertex_b( 1.0, 0.0, 0.0 );
  moab::CartVect vertex_c( 0.0, 1.0, 0.0 );
  moab::CartVect vertex_d( 0.0, 0.0, 1.0 );
  double transform_arrays[9];
  Teuchos::Array<double> teuchos_transform_array( 9 );
  Teuchos::Tuple<double,9> teuchos_transform_tuple;

  // Usage A Test (safe)
  Utility::calculateBarycentricTransformMatrix( vertex_a,
						vertex_b,
						vertex_c,
						vertex_d,
						transform_arrays );
  
  TEST_FLOATING_EQUALITY( transform_arrays[0], -1.0, 1e-12 );

  // Usage B Test (safe)
  Utility::calculateBarycentricTransformMatrix( 
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 teuchos_transform_array.getRawPtr() );

  TEST_FLOATING_EQUALITY( teuchos_transform_array[0], -1.0, 1e-12 );

  // Usage C Test (safest)
  Utility::calculateBarycentricTransformMatrix( 
					 vertex_a,
					 vertex_b,
					 vertex_c,
					 vertex_d,
					 teuchos_transform_tuple.getRawPtr() );

  Teuchos::Tuple<double,9> expected_transform_tuple = 
    Teuchos::tuple<double>( -1.0, -1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  
  TEST_COMPARE_FLOATING_ARRAYS( teuchos_transform_tuple, 
				expected_transform_tuple, 
				1e-12 );
}

//---------------------------------------------------------------------------//
// end tstTetrahedronHelpers.cpp
//---------------------------------------------------------------------------//
