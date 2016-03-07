//---------------------------------------------------------------------------//
//!
//! \file   tstParticleCollidingInCellEventlocalDispatcher.cpp
//! \author Alex Robinson
//! \brief  Particle colliding in cell event local dispatcher unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <memory>

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_Array.hpp>
#include <Teuchos_RCP.hpp>

// FRENSIE Includes
#include "MonteCarlo_CellCollisionFluxEstimator.hpp"
#include "MonteCarlo_ParticleCollidingInCellEventLocalDispatcher.hpp"
#include "MonteCarlo_PhotonState.hpp"
#include "Geometry_ModuleTraits.hpp"

//---------------------------------------------------------------------------//
// Testing Variables
//---------------------------------------------------------------------------//
std::shared_ptr<MonteCarlo::CellCollisionFluxEstimator<MonteCarlo::WeightMultiplier> >
    estimator_1;

std::shared_ptr<MonteCarlo::CellCollisionFluxEstimator<MonteCarlo::WeightAndEnergyMultiplier> >
    estimator_2;

std::shared_ptr<MonteCarlo::ParticleCollidingInCellEventLocalDispatcher> 
  dispatcher( new MonteCarlo::ParticleCollidingInCellEventLocalDispatcher( 0 ) );

//---------------------------------------------------------------------------//
// Testing Functions.
//---------------------------------------------------------------------------//
// Initialize the estimator
template<typename CellCollisionFluxEstimator>
void initializeCellCollisionFluxEstimator( 
			  const unsigned estimator_id,
			  std::shared_ptr<CellCollisionFluxEstimator>& estimator )
{  
  // Set the entity ids
  Teuchos::Array<Geometry::ModuleTraits::InternalCellHandle> 
    cell_ids( 2 );
  cell_ids[0] = 0;
  cell_ids[1] = 1;

  Teuchos::Array<double> cell_volumes( 2 );
  cell_volumes[0] = 1.0;
  cell_volumes[1] = 2.0;

  // Set the estimator multiplier
  double estimator_multiplier = 10.0;

  estimator.reset( new CellCollisionFluxEstimator( estimator_id,
						   estimator_multiplier,
						   cell_ids,
						   cell_volumes ) );

  Teuchos::Array<MonteCarlo::ParticleType> particle_types( 1 );
  particle_types[0] = MonteCarlo::PHOTON;
  
  estimator->setParticleTypes( particle_types );
}

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that the cell id associated with the dispatcher can be returned
TEUCHOS_UNIT_TEST( ParticleCollidingInCellEventDispatcher, getCellId )
{
  TEST_EQUALITY_CONST( dispatcher->getId(), 0 );
}

//---------------------------------------------------------------------------//
// Check that the number of observers attached to the disp. can be returned
TEUCHOS_UNIT_TEST( ParticleCollidingInCellEventDispatcher, 
		   getNumberOfObservers )
{
  TEST_EQUALITY_CONST( dispatcher->getNumberOfObservers(), 0 );
}

//---------------------------------------------------------------------------//
// Check that an observer can be attached to the dispatcher
TEUCHOS_UNIT_TEST( ParticleCollidingInCellEventDispatcher, attachObserver )
{
  initializeCellCollisionFluxEstimator( 0u, estimator_1 );
  initializeCellCollisionFluxEstimator( 1u, estimator_2 );

  std::shared_ptr<MonteCarlo::ParticleCollidingInCellEventObserver> observer_1 =
    std::dynamic_pointer_cast<MonteCarlo::ParticleCollidingInCellEventObserver>( 
								 estimator_1 );
  std::shared_ptr<MonteCarlo::ParticleCollidingInCellEventObserver> observer_2 =
    std::dynamic_pointer_cast<MonteCarlo::ParticleCollidingInCellEventObserver>(
								 estimator_2 );
  
  dispatcher->attachObserver( estimator_1->getId(), observer_1 );
  dispatcher->attachObserver( estimator_2->getId(), observer_2 );
  
  observer_1.reset();
  observer_2.reset();

  TEST_EQUALITY_CONST( estimator_1.use_count(), 2 );
  TEST_EQUALITY_CONST( estimator_2.use_count(), 2 );
  TEST_EQUALITY_CONST( dispatcher->getNumberOfObservers(), 2 );
}

//---------------------------------------------------------------------------//
// Check that a collision event can be dispatched
TEUCHOS_UNIT_TEST( ParticleCollidingInCellEventDispatcher, 
		   dispatchParticleCollidingInCellEvent )
{
  MonteCarlo::PhotonState particle( 0ull );
  particle.setWeight( 1.0 );
  particle.setEnergy( 1.0 );

  TEST_ASSERT( !estimator_1->hasUncommittedHistoryContribution() );
  TEST_ASSERT( !estimator_2->hasUncommittedHistoryContribution() );

  dispatcher->dispatchParticleCollidingInCellEvent( particle, 0, 1.0 );

  TEST_ASSERT( estimator_1->hasUncommittedHistoryContribution() );
  TEST_ASSERT( estimator_2->hasUncommittedHistoryContribution() );
}

//---------------------------------------------------------------------------//
// Check that an observer can be detached from the dispatcher
TEUCHOS_UNIT_TEST( ParticleCollidingInCellEventDispatcher, detachObserver )
{
  dispatcher->detachObserver( 0u );

  TEST_EQUALITY_CONST( estimator_1.use_count(), 1 );
  TEST_EQUALITY_CONST( estimator_2.use_count(), 2 );
  TEST_EQUALITY_CONST( dispatcher->getNumberOfObservers(), 1 );

  dispatcher->detachObserver( 1u );

  TEST_EQUALITY_CONST( estimator_1.use_count(), 1 );
  TEST_EQUALITY_CONST( estimator_2.use_count(), 1 );
  TEST_EQUALITY_CONST( dispatcher->getNumberOfObservers(), 0 );  

  // Remove nonexistent estimator
  dispatcher->detachObserver( 2u );

  TEST_EQUALITY_CONST( estimator_1.use_count(), 1 );
  TEST_EQUALITY_CONST( estimator_2.use_count(), 1 );
  TEST_EQUALITY_CONST( dispatcher->getNumberOfObservers(), 0 );  
}

//---------------------------------------------------------------------------//
// end tstParticleCollidingInCellEventLocalDispatcher.cpp
//---------------------------------------------------------------------------//
