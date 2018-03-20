//---------------------------------------------------------------------------//
//!
//! \file   tstParticleState.cpp
//! \author Alex Robinson
//! \brief  Particle state unit tests.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/base_object.hpp>

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>

// FRENSIE Includes
#include "MonteCarlo_ParticleState.hpp"
#include "Geometry_InfiniteMediumModel.hpp"
#include "Utility_UnitTestHarnessExtensions.hpp"
#include "Utility_PhysicalConstants.hpp"

//---------------------------------------------------------------------------//
// Testing Structs.
//---------------------------------------------------------------------------//
class TestParticleState : public MonteCarlo::ParticleState
{
public:

  TestParticleState()
  { /* ... */ }

  TestParticleState( const unsigned long long history_number )
  : MonteCarlo::ParticleState( history_number, MonteCarlo::PHOTON, 1 )
  { /* ... */ }

  TestParticleState( const MonteCarlo::ParticleState& existing_base_state,
                     const bool increment_generation_number = false,
                     const bool reset_collision_number = false )
    : MonteCarlo::ParticleState( existing_base_state,
                                 MonteCarlo::PHOTON,
                                 1,
                                 increment_generation_number,
                                 reset_collision_number )
  { /* ... */ }

  ~TestParticleState()
  { /* ... */ }

  double getSpeed() const
  { return 1.0; }

  MonteCarlo::ParticleState::timeType calculateTraversalTime(
                                                  const double distance ) const
  { return 1.0; }

  MonteCarlo::ParticleState* clone() const
  { return NULL; }

  void print( std::ostream& os ) const
  { printImplementation<TestParticleState>( os ); }

private:
  TestParticleState( const TestParticleState& state );

  // Save the state to an archive
  template<typename Archive>
  void serialize( Archive& ar, const unsigned version )
  {
    ar & boost::serialization::make_nvp("ParticleState",boost::serialization::base_object<MonteCarlo::ParticleState>(*this));
  }

  // Declare the boost serialization access object as a friend
  friend class boost::serialization::access;
};

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Get the history number
TEUCHOS_UNIT_TEST( ParticleState, getHistoryNumber )
{
  TestParticleState particle( 1ull );

  TEST_EQUALITY_CONST( particle.getHistoryNumber(), 1ull );
}

//---------------------------------------------------------------------------//
// Set/get the source id that created the particle (history)
TEUCHOS_UNIT_TEST( ParticleState, setgetSourceId )
{
  TestParticleState particle( 1ull );

  particle.setSourceId( 2 );

  TEST_EQUALITY_CONST( particle.getSourceId(), 2 );
}

//---------------------------------------------------------------------------//
// Set/get the source cell where the particle (history) started
TEUCHOS_UNIT_TEST( ParticleState, setgetSourceCell )
{
  TestParticleState particle( 1ull );

  particle.setSourceCell( 1 );

  TEST_EQUALITY_CONST( particle.getSourceCell(), 1 );
}

//---------------------------------------------------------------------------//
// Set/get the position of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetPosition )
{
  TestParticleState particle( 1ull );

  double position[3] = {1.0, 1.0, 1.0};

  particle.setPosition( position[0], position[1], position[2] );

  TEST_EQUALITY_CONST( particle.getXPosition(), 1.0 );
  TEST_EQUALITY_CONST( particle.getYPosition(), 1.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), 1.0 );

  const double* particle_position = particle.getPosition();

  TEST_EQUALITY_CONST( particle_position[0], 1.0 );
  TEST_EQUALITY_CONST( particle_position[1], 1.0 );
  TEST_EQUALITY_CONST( particle_position[2], 1.0 );

  position[0] = 2.0;

  particle.setPosition( position );

  TEST_EQUALITY_CONST( particle.getXPosition(), 2.0 );
  TEST_EQUALITY_CONST( particle.getYPosition(), 1.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), 1.0 );

  particle_position = particle.getPosition();

  TEST_EQUALITY_CONST( particle_position[0], 2.0 );
  TEST_EQUALITY_CONST( particle_position[1], 1.0 );
  TEST_EQUALITY_CONST( particle_position[2], 1.0 );
}

//---------------------------------------------------------------------------//
// Set/get the direction of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetDirection )
{
  TestParticleState particle( 1ull );

  double direction[3] =
    {0.5773502691896258, 0.5773502691896258, 0.5773502691896258};

  particle.setDirection( direction[0], direction[1], direction[2] );

  TEST_EQUALITY_CONST( particle.getXDirection(), 0.5773502691896258 );
  TEST_EQUALITY_CONST( particle.getYDirection(), 0.5773502691896258 );
  TEST_EQUALITY_CONST( particle.getZDirection(), 0.5773502691896258 );

  const double* particle_direction = particle.getDirection();

  TEST_EQUALITY_CONST( direction[0], 0.5773502691896258 );
  TEST_EQUALITY_CONST( direction[1], 0.5773502691896258 );
  TEST_EQUALITY_CONST( direction[2], 0.5773502691896258 );

  particle.setDirection( direction );

  TEST_EQUALITY_CONST( particle.getXDirection(), 0.5773502691896258 );
  TEST_EQUALITY_CONST( particle.getYDirection(), 0.5773502691896258 );
  TEST_EQUALITY_CONST( particle.getZDirection(), 0.5773502691896258 );

  particle_direction = particle.getDirection();

  TEST_EQUALITY_CONST( direction[0], 0.5773502691896258 );
  TEST_EQUALITY_CONST( direction[1], 0.5773502691896258 );
  TEST_EQUALITY_CONST( direction[2], 0.5773502691896258 );
}

//---------------------------------------------------------------------------//
// Rotate the direction using a polar and azimuthal angle
TEUCHOS_UNIT_TEST( ParticleState, rotateDirection )
{
  TestParticleState particle( 1ull );

  // Rotate x direction to neg. x direction
  particle.setDirection( 1.0, 0.0, 0.0 );
  particle.rotateDirection( -1.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate x direction to y direction
  particle.setDirection( 1.0, 0.0, 0.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate x direction to neg. y direction
  particle.setDirection( 1.0, 0.0, 0.0 );
  particle.rotateDirection( 0.0, 3*Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate x direction to z direction
  particle.setDirection( 1.0, 0.0, 0.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 1.0, 1e-15 );

  // Rotate x direction to neg. z direction
  particle.setDirection( 1.0, 0.0, 0.0 );
  particle.rotateDirection( 0.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), -1.0, 1e-15 );

  // Rotate y direction to neg. y direction
  particle.setDirection( 0.0, 1.0, 0.0 );
  particle.rotateDirection( -1.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate y direction to x direction
  particle.setDirection( 0.0, 1.0, 0.0 );
  particle.rotateDirection( 0.0, 3*Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate y direction to neg. x direction
  particle.setDirection( 0.0, 1.0, 0.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate the y direction to z direction
  particle.setDirection( 0.0, 1.0, 0.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 1.0, 1e-15 );

  // Rotate the y direction to neg. z direction
  particle.setDirection( 0.0, 1.0, 0.0 );
  particle.rotateDirection( 0.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), -1.0, 1e-15 );

  // Rotate the z direction to neg. z direction
  particle.setDirection( 0.0, 0.0, 1.0 );
  particle.rotateDirection( -1.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), -1.0, 1e-15 );

  // Rotate z direction to x direction
  particle.setDirection( 0.0, 0.0, 1.0 );
  particle.rotateDirection( 0.0, 0.0 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate z direction to neg. x direction
  particle.setDirection( 0.0, 0.0, 1.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate z direction to y direction
  particle.setDirection( 0.0, 0.0, 1.0 );
  particle.rotateDirection( 0.0, Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), 1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );

  // Rotate z direction to neg. y direction
  particle.setDirection( 0.0, 0.0, 1.0 );
  particle.rotateDirection( 0.0, 3*Utility::PhysicalConstants::pi/2 );

  UTILITY_TEST_FLOATING_EQUALITY( particle.getXDirection(), 0.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getYDirection(), -1.0, 1e-15 );
  UTILITY_TEST_FLOATING_EQUALITY( particle.getZDirection(), 0.0, 1e-15 );
}

//---------------------------------------------------------------------------//
// Set/get the source energy of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetSourceEnergy )
{
  TestParticleState particle( 1ull );

  particle.setSourceEnergy( 1.0 );

  TEST_EQUALITY_CONST( particle.getSourceEnergy(), 1.0 );
}

//---------------------------------------------------------------------------//
// Set/get the energy of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetEnergy )
{
  TestParticleState particle( 1ull );

  particle.setEnergy( 1.0 );

  TEST_EQUALITY_CONST( particle.getEnergy(), 1.0 );
}

//---------------------------------------------------------------------------//
// Get the charge of a particle
TEUCHOS_UNIT_TEST( ParticleState, getCharge )
{
  TestParticleState particle( 1ull );

  TEST_EQUALITY_CONST( particle.getCharge(), 1 );
}

//---------------------------------------------------------------------------//
// Set/get the source time of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetSourceTime )
{
  TestParticleState particle( 1ull );

  particle.setSourceTime( 1.0 );

  TEST_EQUALITY_CONST( particle.getSourceTime(), 1.0 );
}

//---------------------------------------------------------------------------//
// Set/get the time state of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetTime )
{
  TestParticleState particle( 1ull );

  particle.setTime( 1.0 );

  TEST_EQUALITY_CONST( particle.getTime(), 1.0 );
}

//---------------------------------------------------------------------------//
// Increment/get the collision number of the particle
TEUCHOS_UNIT_TEST( ParticleState, incrementgetCollisionNumber )
{
  TestParticleState particle( 1ull );

  particle.incrementCollisionNumber();
  particle.incrementCollisionNumber();

  TEST_EQUALITY_CONST( particle.getCollisionNumber(), 2u );
}

//---------------------------------------------------------------------------//
// Get the generation number of the particle
TEUCHOS_UNIT_TEST( ParticleState, getGenerationNumber )
{
  TestParticleState particle( 1ull );

  TEST_EQUALITY_CONST( particle.getGenerationNumber(), 0u );
}

//---------------------------------------------------------------------------//
// Set/get the source weight of the particle
TEUCHOS_UNIT_TEST( ParticleState, setgetSourceWeight )
{
  TestParticleState particle( 1ull );

  particle.setSourceWeight( 1.0 );

  TEST_EQUALITY_CONST( particle.getSourceWeight(), 1.0 );
}

//---------------------------------------------------------------------------//
// Set/get the weight of a particle
TEUCHOS_UNIT_TEST( ParticleState, setgetWeight )
{
  TestParticleState particle( 1ull );

  particle.setWeight( 1.0 );

  TEST_EQUALITY_CONST( particle.getWeight(), 1.0 );
}

//---------------------------------------------------------------------------//
// Multiply the weight of a particle
TEUCHOS_UNIT_TEST( ParticleState, multiplyWeight )
{
  TestParticleState particle( 1ull );

  particle.setWeight( 1.0 );
  particle.multiplyWeight( 0.5 );

  TEST_EQUALITY_CONST( particle.getWeight(), 0.5 );
}

//---------------------------------------------------------------------------//
// Test if a particle is lost
TEUCHOS_UNIT_TEST( ParticleState, lost )
{
  TestParticleState particle( 1ull );

  TEST_ASSERT( !particle.isLost() );

  particle.setAsLost();

  TEST_ASSERT( particle.isLost() );
}

//---------------------------------------------------------------------------//
// Test if a particle is gone
TEUCHOS_UNIT_TEST( ParticleState, gone )
{
  TestParticleState particle( 1ull );

  TEST_ASSERT( !particle.isGone() );

  particle.setAsGone();

  TEST_ASSERT( particle.isGone() );
}

//---------------------------------------------------------------------------//
// Test if the navigator can be returned
TEUCHOS_UNIT_TEST( ParticleState, navigator )
{
  TestParticleState particle( 1ull );

  particle.setPosition( 0.1, -10.0, sqrt(2.0) );
  particle.setDirection( 0.0, -1.0/sqrt(2.0), 1.0/sqrt(2.0) );

  // Get the particle location and direction from the navigator
  const Geometry::Navigator& const_particle_navigator =
    dynamic_cast<const MonteCarlo::ParticleState&>( particle ).navigator();

  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayPosition()[0],
                       0.1 );
  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayPosition()[1],
                       -10.0 );
  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayPosition()[2],
                       sqrt(2.0) );
  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayDirection()[0],
                       0.0 );
  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayDirection()[1],
                       -1.0/sqrt(2.0) );
  TEST_EQUALITY_CONST( const_particle_navigator.getInternalRayDirection()[2],
                       1.0/sqrt(2.0) );

  // Change the particle location and direction from the navigator
  Geometry::Navigator& particle_navigator = particle.navigator();

  particle_navigator.advanceInternalRayBySubstep( 1.0 );

  TEST_EQUALITY_CONST( particle.getXPosition(), 0.1 );
  TEST_FLOATING_EQUALITY( particle.getYPosition(), -10.0-1.0/sqrt(2.0), 1e-15 );
  TEST_FLOATING_EQUALITY( particle.getZPosition(), 3.0/sqrt(2.0), 1e-15 );

  particle_navigator.changeInternalRayDirection( 1.0, 0.0, 0.0 );

  TEST_EQUALITY_CONST( particle.getXDirection(), 1.0 );
  TEST_EQUALITY_CONST( particle.getYDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getZDirection(), 0.0 );
}

//---------------------------------------------------------------------------//
// Test if the particle can be embeded inside of a geometry model
TEUCHOS_UNIT_TEST( ParticleState, embed )
{
  TestParticleState particle( 1ull );

  particle.setPosition( 0.1, -10.0, sqrt(2.0) );
  particle.setDirection( 0.0, -1.0/sqrt(2.0), 1.0/sqrt(2.0) );

  TEST_EQUALITY_CONST( particle.getCell(), 0 );

  // Embed the particle in a model
  std::shared_ptr<Geometry::InfiniteMediumModel>
    model( new Geometry::InfiniteMediumModel( 2 ) );

  particle.embedInModel( model );

  // Verify that the particle position and direction are unchanged
  TEST_EQUALITY_CONST( particle.getXPosition(), 0.1 );
  TEST_EQUALITY_CONST( particle.getYPosition(), -10.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), sqrt(2.0) );
  TEST_EQUALITY_CONST( particle.getXDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getYDirection(), -1.0/sqrt(2.0) );
  TEST_EQUALITY_CONST( particle.getZDirection(), 1.0/sqrt(2.0) );

  // Verify that the particle is in the correct cell in the new model
  TEST_EQUALITY_CONST( particle.getCell(), 2 );

  // Extract the particle from the model
  particle.extractFromModel();

  TEST_EQUALITY_CONST( particle.getCell(), 0 );

  // Embed the particle in a model with the cell specified
  model.reset( new Geometry::InfiniteMediumModel( 3 ) );

  particle.embedInModel( model, 3 );

  // Verify that the particle position and direction are unchanged
  TEST_EQUALITY_CONST( particle.getXPosition(), 0.1 );
  TEST_EQUALITY_CONST( particle.getYPosition(), -10.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), sqrt(2.0) );
  TEST_EQUALITY_CONST( particle.getXDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getYDirection(), -1.0/sqrt(2.0) );
  TEST_EQUALITY_CONST( particle.getZDirection(), 1.0/sqrt(2.0) );

  // Verify that the particle is in the correct cell in the new model
  TEST_EQUALITY_CONST( particle.getCell(), 3 );

  // Extract the particle from the model
  particle.extractFromModel();

  // Embed the particle in a model at a new location and with a new direction
  model.reset( new Geometry::InfiniteMediumModel( 4 ) );

  particle.embedInModel( model,
                         (const double[3]){-1.0, 2.0, -0.3},
                         (const double[3]){1.0, 0.0, 0.0} );

  // Verify that the particle position and direction are correct
  TEST_EQUALITY_CONST( particle.getXPosition(), -1.0 );
  TEST_EQUALITY_CONST( particle.getYPosition(), 2.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), -0.3 );
  TEST_EQUALITY_CONST( particle.getXDirection(), 1.0 );
  TEST_EQUALITY_CONST( particle.getYDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getZDirection(), 0.0 );

  // Verify that the particle is in the correct cell in the new model
  TEST_EQUALITY_CONST( particle.getCell(), 4 );

  // Extract the particle from the model
  particle.extractFromModel();

  // Embed the particle in a model at a new location and with a new direction
  model.reset( new Geometry::InfiniteMediumModel( 5 ) );

  particle.embedInModel( model,
                         (const double[3]){1.0, -2.0, 0.3},
                         (const double[3]){0.0, 0.0, 1.0},
                         5 );

  // Verify that the particle position and direction are correct
  TEST_EQUALITY_CONST( particle.getXPosition(), 1.0 );
  TEST_EQUALITY_CONST( particle.getYPosition(), -2.0 );
  TEST_EQUALITY_CONST( particle.getZPosition(), 0.3 );
  TEST_EQUALITY_CONST( particle.getXDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getYDirection(), 0.0 );
  TEST_EQUALITY_CONST( particle.getZDirection(), 1.0 );

  // Verify that the particle is in the correct cell in the new model
  TEST_EQUALITY_CONST( particle.getCell(), 5 );
}

//---------------------------------------------------------------------------//
// Archive a particle state
TEUCHOS_UNIT_TEST( ParticleState, archive )
{
  // Create and archive a particle
  {
    TestParticleState particle( 1ull );
    particle.setSourceId( 10 );
    particle.setSourceCell( 1 );
     particle.setPosition( 1.0, 1.0, 1.0 );
    particle.setDirection( 0.0, 0.0, 1.0 );
    particle.setSourceEnergy( 2.0 );
    particle.setEnergy( 1.0 );
    particle.setSourceTime( 0.0 );
    particle.setTime( 0.5 );
    particle.incrementCollisionNumber();
    particle.setSourceWeight( 1.0 );
    particle.setWeight( 0.25 );

    std::ofstream ofs( "test_particle_state_archive.xml" );

    boost::archive::xml_oarchive ar(ofs);
    ar << BOOST_SERIALIZATION_NVP( particle );
  }

  // Load the archived particle
  TestParticleState loaded_particle;

  std::ifstream ifs( "test_particle_state_archive.xml" );

  boost::archive::xml_iarchive ar(ifs);
  ar >> boost::serialization::make_nvp( "particle", loaded_particle );

  TEST_EQUALITY_CONST( loaded_particle.getSourceId(), 10 );
  TEST_EQUALITY_CONST( loaded_particle.getSourceCell(), 1 );
  TEST_EQUALITY_CONST( loaded_particle.getCell(), 0 );
  TEST_EQUALITY_CONST( loaded_particle.getXPosition(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getYPosition(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getZPosition(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getXDirection(), 0.0 );
  TEST_EQUALITY_CONST( loaded_particle.getYDirection(), 0.0 );
  TEST_EQUALITY_CONST( loaded_particle.getZDirection(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getSourceEnergy(), 2.0 );
  TEST_EQUALITY_CONST( loaded_particle.getEnergy(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getCharge(), 1 );
  TEST_EQUALITY_CONST( loaded_particle.getSourceTime(), 0.0 );
  TEST_EQUALITY_CONST( loaded_particle.getTime(), 0.5 );
  TEST_EQUALITY_CONST( loaded_particle.getCollisionNumber(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getGenerationNumber(), 0.0 );
  TEST_EQUALITY_CONST( loaded_particle.getSourceWeight(), 1.0 );
  TEST_EQUALITY_CONST( loaded_particle.getWeight(), 0.25 );
  TEST_EQUALITY_CONST( loaded_particle.getHistoryNumber(), 1ull );
}

//---------------------------------------------------------------------------//
// Create new particles
TEUCHOS_UNIT_TEST( ParticleState, copy_constructor )
{
  TestParticleState particle_gen_a( 1ull );
  particle_gen_a.setSourceId( 10 );
  particle_gen_a.setSourceCell( 1 );
  particle_gen_a.setPosition( 1.0, 1.0, 1.0 );
  particle_gen_a.setDirection( 0.0, 0.0, 1.0 );
  particle_gen_a.setSourceEnergy( 2.0 );
  particle_gen_a.setEnergy( 1.0 );
  particle_gen_a.setSourceTime( 0.0 );
  particle_gen_a.setTime( 1.0 );
  particle_gen_a.incrementCollisionNumber();
  particle_gen_a.setSourceWeight( 1.0 );
  particle_gen_a.setWeight( 0.5 );

  // Create a second generation particle with the same collision number
  TestParticleState particle_gen_b( particle_gen_a, true );

  TEST_EQUALITY( particle_gen_b.getSourceId(),
                 particle_gen_a.getSourceId() );
  TEST_EQUALITY( particle_gen_b.getXPosition(),
                 particle_gen_a.getXPosition() );
  TEST_EQUALITY( particle_gen_b.getYPosition(),
                 particle_gen_a.getYPosition() );
  TEST_EQUALITY( particle_gen_b.getZPosition(),
                 particle_gen_a.getZPosition() );
  TEST_EQUALITY( particle_gen_b.getXDirection(),
                 particle_gen_a.getXDirection() );
  TEST_EQUALITY( particle_gen_b.getYDirection(),
                 particle_gen_a.getYDirection() );
  TEST_EQUALITY( particle_gen_b.getZDirection(),
                 particle_gen_a.getZDirection() );
  TEST_EQUALITY( particle_gen_b.getSourceEnergy(),
                 particle_gen_a.getSourceEnergy() );
  TEST_EQUALITY( particle_gen_b.getEnergy(),
                 particle_gen_a.getEnergy() );
  TEST_EQUALITY( particle_gen_b.getCharge(),
                 particle_gen_a.getCharge() );
  TEST_EQUALITY( particle_gen_b.getSourceTime(),
                 particle_gen_a.getSourceTime() );
  TEST_EQUALITY( particle_gen_b.getTime(),
                 particle_gen_a.getTime() );
  TEST_EQUALITY( particle_gen_b.getCollisionNumber(),
                 particle_gen_a.getCollisionNumber() );
  TEST_EQUALITY( particle_gen_b.getGenerationNumber(),
                 particle_gen_a.getGenerationNumber()+1u );
  TEST_EQUALITY( particle_gen_b.getSourceWeight(),
                 particle_gen_a.getSourceWeight() );
  TEST_EQUALITY( particle_gen_b.getWeight(),
                 particle_gen_a.getWeight() );
  TEST_EQUALITY( particle_gen_b.getSourceCell(),
                 particle_gen_a.getSourceCell() );
  TEST_EQUALITY( particle_gen_b.getCell(),
                 particle_gen_a.getCell() );

  // Create a third generation particle and reset the collision counter
  TestParticleState particle_gen_c( particle_gen_b, true, true );

  TEST_EQUALITY( particle_gen_c.getSourceId(),
                 particle_gen_b.getSourceId() );
  TEST_EQUALITY( particle_gen_c.getXPosition(),
                 particle_gen_b.getXPosition() );
  TEST_EQUALITY( particle_gen_c.getYPosition(),
                 particle_gen_b.getYPosition() );
  TEST_EQUALITY( particle_gen_c.getZPosition(),
                 particle_gen_b.getZPosition() );
  TEST_EQUALITY( particle_gen_c.getXDirection(),
                 particle_gen_b.getXDirection() );
  TEST_EQUALITY( particle_gen_c.getYDirection(),
                 particle_gen_b.getYDirection() );
  TEST_EQUALITY( particle_gen_c.getZDirection(),
                 particle_gen_b.getZDirection() );
  TEST_EQUALITY( particle_gen_c.getSourceEnergy(),
                 particle_gen_b.getSourceEnergy() );
  TEST_EQUALITY( particle_gen_c.getEnergy(),
                 particle_gen_b.getEnergy() );
  TEST_EQUALITY( particle_gen_c.getCharge(),
                 particle_gen_b.getCharge() );
  TEST_EQUALITY( particle_gen_c.getSourceTime(),
                 particle_gen_b.getSourceTime() );
  TEST_EQUALITY( particle_gen_c.getTime(),
                 particle_gen_b.getTime() );
  TEST_EQUALITY_CONST( particle_gen_c.getCollisionNumber(), 0u );
  TEST_EQUALITY( particle_gen_c.getGenerationNumber(),
                 particle_gen_b.getGenerationNumber()+1u );
  TEST_EQUALITY( particle_gen_c.getSourceWeight(),
                 particle_gen_b.getSourceWeight() );
  TEST_EQUALITY( particle_gen_c.getWeight(),
                 particle_gen_b.getWeight() );
  TEST_EQUALITY( particle_gen_c.getSourceCell(),
                 particle_gen_b.getSourceCell() );
  TEST_EQUALITY( particle_gen_c.getCell(),
                 particle_gen_b.getCell() );

  // Create a second third generation particle after the second gen particle
  // has been set to gone
  particle_gen_b.setAsGone();

  TestParticleState particle_gen_2c( particle_gen_b, true );

  TEST_ASSERT( !particle_gen_2c.isGone() );
  TEST_ASSERT( !particle_gen_2c.isLost() );

  // Create a second second generation particle after the first gen particle
  // has been set to lost
  particle_gen_a.setAsLost();

  TestParticleState particle_gen_2b( particle_gen_a, true );

  TEST_ASSERT( !particle_gen_2b.isGone() );
  TEST_ASSERT( !particle_gen_2b.isLost() );
}

//---------------------------------------------------------------------------//
// end tstParticleState.cpp
//---------------------------------------------------------------------------//
