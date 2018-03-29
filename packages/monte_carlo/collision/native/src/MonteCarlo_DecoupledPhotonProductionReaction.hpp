//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_DecoupledPhotonProductionReaction.hpp
//! \author Alex Robinson, Eli Moll
//! \brief  The decoupled photon production reaction base class declaration
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_DECOUPLED_PHOTON_PRODUCTION_REACTION_HPP
#define MONTE_CARLO_DECOUPLED_PHOTON_PRODUCTION_REACTION_HPP

// Std Lib Includes
#include <memory>

// FRENSIE Includes
#include "MonteCarlo_NuclearScatteringDistribution.hpp"
#include "MonteCarlo_NuclearReaction.hpp"
#include "MonteCarlo_NeutronState.hpp"
#include "MonteCarlo_PhotonState.hpp"
#include "MonteCarlo_ParticleBank.hpp"
#include "Utility_OneDDistribution.hpp"
#include "Utility_TabularDistribution.hpp"
#include "Utility_Vector.hpp"
#include "Utility_QuantityTraits.hpp"

namespace MonteCarlo{

//! The photon production (resulting from neutron absorption) reaction
class DecoupledPhotonProductionReaction
{
  // Typedef for QuantityTraits
  typedef Utility::QuantityTraits<double> QT;

public:

  //! The scattering distribution type
  typedef NuclearScatteringDistribution<NeutronState,PhotonState> ScatteringDistribution;

  //! Constructor
  DecoupledPhotonProductionReaction(
   const NuclearReactionType base_reaction_type,
   const unsigned photon_production_id,
   const double temperature,
   const std::shared_ptr<const ScatteringDistribution>&
   photon_production_distribution,
   const std::shared_ptr<const NuclearReaction>& total_reaction,
   const std::vector<std::shared_ptr<const Utility::OneDDistribution> >& total_mt_yield_array );

  //! Destructor
  virtual ~DecoupledPhotonProductionReaction()
  { /* ... */ }

  //! Return the base nuclear reaction type
  NuclearReactionType getBaseReactionType() const;

  //! Return the photon production id for the base reaction type
  unsigned getPhotonProductionId() const;

  //! Return the photon production reaction id
  unsigned getPhotonProductionReactionId() const;

  //! Return the temperature (in MeV) at which the reaction occurs
  double getTemperature() const;

  //! Return the total neutron cross section
  double getTotalCrossSection( const double energy ) const;

  //! Return the total yield
  double getTotalYield( const double energy ) const;

  //! Return the threshold energy
  virtual double getThresholdEnergy() const = 0;

  //! Return the base reaction cross section at a given energy
  virtual double getBaseReactionCrossSection( const double energy ) const = 0;

  //! Return the cross section at a given energy
  virtual double getCrossSection( const double energy ) const = 0;

  //! Simulate the reaction
  virtual void react( const NeutronState& neutron,
                      ParticleBank& bank,
                      double total_photon_production_cross_section ) const;

private:

  // The base nuclear reaction type
  NuclearReactionType d_base_reaction_type;

  // The array of all yield distributions
  std::vector<std::shared_ptr<const Utility::OneDDistribution> >
    d_total_mt_yield_array;

  // The photon production id
  unsigned d_photon_production_id;

  // The temperature at which the reaction occurs
  double d_temperature;

  // The photon production distribution (energy and angle)
  std::shared_ptr<const NuclearScatteringDistribution<NeutronState,PhotonState> >
  d_photon_production_distribution;

  // The total reaction class
  std::shared_ptr<const NuclearReaction> d_total_neutron_reaction;
};

// Return the base nuclear reaction type
inline NuclearReactionType
DecoupledPhotonProductionReaction::getBaseReactionType() const
{
  return d_base_reaction_type;
}

// Return the photon production id for the base reaction type
inline unsigned DecoupledPhotonProductionReaction::getPhotonProductionId() const
{
  return d_photon_production_id/1000u;
}

} // end MonteCarlo namespace

#endif // end MONTE_CARLO_DECOUPLED_PHOTON_PRODUCTION_REACTION_HP

//---------------------------------------------------------------------------//
// end MonteCarlo_DecoupledPhotonProductionReaction.hpp
//---------------------------------------------------------------------------//
