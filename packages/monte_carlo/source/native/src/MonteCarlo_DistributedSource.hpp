//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_DistributedSource.hpp
//! \author Alex Robinson
//! \brief  Distributed source class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_DISTRIBUTED_SOURCE_HPP
#define MONTE_CARLO_DISTRIBUTED_SOURCE_HPP

// Std Lib Includes
#include <memory>

// Trilinos Includes
#include <Teuchos_ScalarTraits.hpp>

// MONTE_CARLO Includes
#include "MonteCarlo_ParticleSource.hpp"
#include "Utility_OneDDistribution.hpp"
#include "Utility_SpatialDistribution.hpp"
#include "Utility_DirectionalDistribution.hpp"
#include "MonteCarlo_ParticleType.hpp"
#include "MonteCarlo_ParticleBank.hpp"
#include "Geometry_ModuleTraits.hpp"
#include "Geometry_PointLocation.hpp"

namespace MonteCarlo{

//! The distributed source class
class DistributedSource : public ParticleSource
{

private:
  
  // Typedef for scalar traits
  typedef Teuchos::ScalarTraits<double> ST;


public:

  //! Typedef for get particle location geometry module interface function
  typedef Geometry::PointLocation (*getLocationFunction)(const Geometry::Ray&, Geometry::ModuleTraits::InternalCellHandle );
  
  //! Constructor
  DistributedSource( 
     const unsigned id,
     const std::shared_ptr<Utility::SpatialDistribution>& spatial_distribution,
     const std::shared_ptr<Utility::DirectionalDistribution>& 
     directional_distribution,
     const std::shared_ptr<Utility::OneDDistribution>& 
     energy_distribution,
     const std::shared_ptr<Utility::OneDDistribution>& 
     time_distribution,
     const ParticleType particle_type,
     getLocationFunction get_particle_location_func );
  
  //! Destructor
  ~DistributedSource()
  { /* ... */ }

  //! Set the spatial importance distribution
  void setSpatialImportanceDistribution( 
   const std::shared_ptr<Utility::SpatialDistribution>& spatial_distribution );

  //! Set the directional importance distribution
  void setDirectionalImportanceDistribution(
                       const std::shared_ptr<Utility::DirectionalDistribution>&
                       directional_distribution );

  //! Set the energy importance distribution
  void setEnergyImportanceDistribution(
       const std::shared_ptr<Utility::OneDDistribution>& energy_distribution );

  //! Set the time importance distribution
  void setTimeImportanceDistribution(
	 const std::shared_ptr<Utility::OneDDistribution>& time_distribution );

  //! Set the rejection cell
  void setRejectionCell( 
		      const Geometry::ModuleTraits::InternalCellHandle& cell );

  //! Sample a particle state from the source
  void sampleParticleState( ParticleBank& bank,
			    const unsigned long long history );

  //! Get the sampling efficiency from the source distribution
  double getSamplingEfficiency() const;

  //! Get the source id
  unsigned getId() const;

private:

  // Sample the particle position
  void sampleParticlePosition( ParticleState& particle );

  // Sample the particle direction
  void sampleParticleDirection( ParticleState& particle );

  // Sample the particle energy
  void sampleParticleEnergy( ParticleState& particle );
  
  // Sample the particle time
  void sampleParticleTime( ParticleState& particle );

  // The spatial distribution of the source 
  std::shared_ptr<Utility::SpatialDistribution> d_spatial_distribution;
  
  // The true spatial distribution of the source
  std::shared_ptr<Utility::SpatialDistribution> 
  d_spatial_importance_distribution;

  // The directional distribution
  std::shared_ptr<Utility::DirectionalDistribution> d_directional_distribution;

  // The true directional distribution of the source
  std::shared_ptr<Utility::DirectionalDistribution> 
  d_directional_importance_distribution;

  // The energy distribution
  std::shared_ptr<Utility::OneDDistribution> d_energy_distribution;

  // The true energy distribution of the source
  std::shared_ptr<Utility::OneDDistribution> d_energy_importance_distribution;

  // The time distribution
  std::shared_ptr<Utility::OneDDistribution> d_time_distribution;

  // The true time distribution of the source
  std::shared_ptr<Utility::OneDDistribution> d_time_importance_distribution;

  // The source id
  unsigned d_id;

  // The type of particle emitted
  ParticleType d_particle_type;

  // The cell handle of the cell used for rejection sampling of the position
  Geometry::ModuleTraits::InternalCellHandle d_rejection_cell;

  // The number of trials
  unsigned d_number_of_trials;

  // The number of valid samples
  unsigned d_number_of_samples;

  // A pointer to the desired getParticleLocation geometry module function
  getLocationFunction d_get_particle_location_func;
};

} // end MonteCarlo namespace

#endif // end MONTE_CARLO_DISTRIBUTED_SOURCE_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_DistributedSource.hpp
//---------------------------------------------------------------------------//
