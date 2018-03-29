//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_CollisionHandler.cpp
//! \author Alex Robinson
//! \brief  Collision handler class definition.
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <stdexcept>

// FRENSIE Includes
#include "MonteCarlo_CollisionHandler.hpp"
#include "Utility_RandomNumberGenerator.hpp"
#include "Utility_ExceptionTestMacros.hpp"
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Constructor
CollisionHandler::CollisionHandler( const bool analogue_collisions )
  : NeutronCollisionHandler( analogue_collisions ),
    PhotonCollisionHandler( analogue_collisions ),
    AdjointPhotonCollisionHandler( analogue_collisions ),
    ElectronCollisionHandler( analogue_collisions ),
    AdjointElectronCollisionHandler( analogue_collisions ),
    PositronCollisionHandler( analogue_collisions )
{ /* ... */ }

// Add a material to the collision handler (neutron-photon mode)
void CollisionHandler::addMaterial(
          const std::shared_ptr<const NeutronMaterial>& neutron_material,
          const std::shared_ptr<const PhotonMaterial>& photon_material,
          const std::vector<Geometry::ModuleTraits::InternalCellHandle>&
          cells_containing_material )
{
  // Make sure the material pointers are valid
  testPrecondition( !neutron_material.is_null() );
  testPrecondition( !photon_material.is_null() );
  // Make sure the cells are valid
  testPrecondition( cells_containing_material.size() > 0 );

  NeutronCollisionHandler::addMaterial(
                                 neutron_material, cells_containing_material );

  PhotonCollisionHandler::addMaterial(
                                  photon_material, cells_containing_material );
}

// Add a material to the collision handler (photon-electron mode)
void CollisionHandler::addMaterial(
              const std::shared_ptr<const PhotonMaterial>& photon_material,
              const std::shared_ptr<const ElectronMaterial>& electron_material,
              const std::shared_ptr<const PositronMaterial>& positron_material,
              const std::vector<Geometry::ModuleTraits::InternalCellHandle>&
              cells_containing_material )
{
  // Make sure the material pointers are valid
  testPrecondition( !photon_material.is_null() );
  testPrecondition( !electron_material.is_null() );
  testPrecondition( !positron_material.is_null() );
  // Make sure the cells are valid
  testPrecondition( cells_containing_material.size() > 0 );

  PhotonCollisionHandler::addMaterial(
                                  photon_material, cells_containing_material );

  ElectronCollisionHandler::addMaterial(
                                electron_material, cells_containing_material );

  PositronCollisionHandler::addMaterial(
                                positron_material, cells_containing_material );
}

// Add a material to the collision handler (neutron-photon-electron mode)
void CollisionHandler::addMaterial(
              const std::shared_ptr<const NeutronMaterial>& neutron_material,
              const std::shared_ptr<const PhotonMaterial>& photon_material,
              const std::shared_ptr<const ElectronMaterial>& electron_material,
              const std::shared_ptr<const PositronMaterial>& positron_material,
              const std::vector<Geometry::ModuleTraits::InternalCellHandle>&
              cells_containing_material )
{
  // Make sure the material pointers are valid
  testPrecondition( !neutron_material.is_null() );
  testPrecondition( !photon_material.is_null() );
  testPrecondition( !electron_material.is_null() );
  testPrecondition( !positron_material.is_null() );
  // Make sure the cells are valid
  testPrecondition( cells_containing_material.size() > 0 );

  NeutronCollisionHandler::addMaterial(
                                 neutron_material, cells_containing_material );

  PhotonCollisionHandler::addMaterial(
                                  photon_material, cells_containing_material );

  ElectronCollisionHandler::addMaterial(
                                electron_material, cells_containing_material );

  PositronCollisionHandler::addMaterial(
                                positron_material, cells_containing_material );
}

// Check if a cell is void
bool CollisionHandler::isCellVoid(
             const Geometry::ModuleTraits::InternalCellHandle cell,
             const MonteCarlo::ParticleType particle_type )
{
  switch( particle_type )
  {
  case NEUTRON:
    return NeutronCollisionHandler::isCellVoid( cell );
  case PHOTON:
    return PhotonCollisionHandler::isCellVoid( cell );
  case ADJOINT_PHOTON:
    return AdjointPhotonCollisionHandler::isCellVoid( cell );
  case ELECTRON:
    return ElectronCollisionHandler::isCellVoid( cell );
  case ADJOINT_ELECTRON:
    return AdjointElectronCollisionHandler::isCellVoid( cell );
  case POSITRON:
    return PositronCollisionHandler::isCellVoid( cell );
  default:
    THROW_EXCEPTION( std::logic_error,
                     "Particle type " << particle_type <<
                     " is not recognized by the collision handler!" );
  }
}

// Sample the optical path length traveled by a particle before a collision
double CollisionHandler::sampleOpticalPathLength() const
{
  return -std::log( Utility::RandomNumberGenerator::getRandomNumber<double>() );
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_CollisionHandler.cpp
//---------------------------------------------------------------------------//
