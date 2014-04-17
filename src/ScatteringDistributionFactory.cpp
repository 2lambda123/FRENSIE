//---------------------------------------------------------------------------//
//!
//! \file   ScatteringDistributionFactor.cpp
//! \author Alex Robinson
//! \brief  Scattering distribution factory class definition
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <sstream>
#include <stdexcept>

// Trilinos Includes
#include "Teuchos_ArrayView.hpp"

// FACEMC Includes
#include "ScatteringDistributionFactory.hpp"
#include "UniformDistribution.hpp"
#include "32EquiprobableBinDistribution.hpp"
#include "ContractException.hpp"

namespace FACEMC{

// Initialize the static member data
Teuchos::RCP<OneDDistribution> 
ScatteringDistributionFactory::isotropic_angle_cosine_dist(
			   new FACEMC::UniformDistribution( -1.0, 1.0, 1.0 ) );

// Constructor
ScatteringDistributionFactory::ScatteringDistributionFactory( 
					     const std::string& table_name,
					     const double atomic_weight_ratio )
  : d_table_name( table_name ),
    d_atomic_weight_ratio( atomic_weight_ratio )
{ /* ... */ }

// Create a scattering distribution
Teuchos::RCP<ScatteringDistribution> 
ScatteringDistributionFactory::createElasticScatteringDistribution(
		       const Teuchos::Array<double>& raw_angular_distribution )
{
  unsigned number_of_tabulated_energies = raw_angular_distribution[0];

  // Get the energy grid
  Teuchos::ArrayView<double> energy_grid = 
    raw_angular_distribution( 1, number_of_tabulated_energies );

  // Get the location of the angular distribution for each energy
  Teuchos::ArrayView<double> distribution_indices = 
    raw_angular_distribution( number_of_tabulated_energies + 1,
				number_of_tabulated_energies );

  // Initialize the angular distribution array
  Teuchos::Array<Pair<double,Teuchos::RCP<OneDDistribution> > >
    angular_distribution( number_of_tabulated_energies );

  for( unsigned i = 0; i < energy_grid.size(); ++i )
  {
    angular_distribution[i].first = energy_grid[i];

    unsigned distribution_index = 
      static_cast<unsigned>( distribution_indices[i] );

    // 32 equiprobable bin distribution
    if( distribution_indices[i] > 0 )
    {
      Teuchos::ArrayView<double> bin_boundaries = 
	raw_angular_distribution( distribution_index, 33 );

      angular_distribution[i].second.reset( 
			 new 32EquiprobableBinDistribution( bin_boundaries ) );
    }

    // Tabular distribution
    else if( distribution_indices[i] < 0 )
    {
      unsigned interpolation_flag = 
	raw_angular_distribution[distribution_index];

      unsigned number_of_points_in_dist = 
	raw_angular_distribution[distribution_index + 1];

      Teuchos::ArrayView<double> scattering_angle_cosine_grid = 
	raw_angular_distribution( distribution_index + 2,
				    number_of_points_in_dist );

      switch( interpolation_flag )
      {
      case 1u: // histogram interpolation
	
	// Ignore the last evaluated point in the PDF
	Teuchos::ArrayView<double> pdf = raw_angular_distribution( 
			     distribution_index + 2 + number_of_points_in_dist,
			     number_of_points_in_dist-1 );
	
	angular_distribution[i].second.reset(
	      new HistogramDistribution( scattering_angle_cosine_grid, pdf ) );
	break;
	
      case 2u: // Linear-Linear interpolation

	angular_distribution[i].second.reset( 
	 new TabularDistribution<LinLin>( scattering_angle_cosine_grid, pdf ));
						 
	break;
	
      default:
	std::stringstream ss;
	ss << "Unknown interpolation flag found in table " << d_table_name
	   << " for angular distribution of MT = 2: " << interpolation_flag;
	
	throw std::runtime_error( ss.str() );
      }
    }
    
    // Isotropic distribution
    else
    {
      angular_distribution[i].second = 
	ScatteringDistributionFactory::isotropic_angle_cosine_dist;
    }
  }

  // Create the elastic scattering distribution
  return Teuchos::RCP<ScatteringDistribution>( 
		   new ElasticScatteringDistribution( d_atomic_weight_ratio,
					              angular_distribution ) );
}

// Create a scattering distribution
Teuchos::RCP<ScatteringDistribution> 
ScatteringDistributionFactory::createDistribution(
		      const Teuchos::Array<double>& raw_angular_distribution,
		      const Teuchos::Array<double>& raw_energy_distribution )
{
  
}
								   

} // end FACEMC namespace

//---------------------------------------------------------------------------//
// end ScatteringDistributionFactory.cpp
//---------------------------------------------------------------------------//