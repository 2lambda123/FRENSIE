//---------------------------------------------------------------------------//
//!
//! \file   CellCollisionFluxEstimator.hpp
//! \author Alex Robinson
//! \brief  Cell track length flux estimator class declaration.
//!
//---------------------------------------------------------------------------//

#ifndef CELL_COLLISION_FLUX_ESTIMATOR_HPP
#define CELL_COLLISION_FLUX_ESTIMATOR_HPP

// FACEMC Includes
#include "StandardCellEstimator.hpp"
#include "EstimatorContributionMultiplierPolicy.hpp"

namespace FACEMC{

//! The cell collision flux estimator class
template<typename ContributionMultiplierPolicy = WeightMultiplier>
class CellCollisionFluxEstimator : public StandardCellEstimator
{

public:

  //! Constructor
  CellCollisionFluxEstimator( 
	     const Estimator::idType id,
	     const double multiplier,
	     const Teuchos::Array<StandardCellEstimator::cellIdType>& cell_ids,
	     const Teuchos::Array<double>& cell_volumes );

  //! Destructor
  ~CellCollisionFluxEstimator()
  { /* ... */ }

  //! Add estimator contribution from a portion of the current history
  void addPartialHistoryContribution( 
		     const ParticleState& particle,
		     const StandardCellEstimator::cellIdType cell_of_collision,
		     const double inverse_total_cross_section,
		     const double angle_cosine = 0.0);
  
  //! Print the estimator data
  void print( std::ostream& os ) const;
};

} // end FACEMC namespace

//---------------------------------------------------------------------------//
// Template Includes
//---------------------------------------------------------------------------//

#include "CellCollisionFluxEstimator_def.hpp"

//---------------------------------------------------------------------------//

#endif // end CELL_COLLISION_FLUX_ESTIMATOR_HPP

//---------------------------------------------------------------------------//
// end CellCollisionFluxEstimator.hpp
//---------------------------------------------------------------------------//
