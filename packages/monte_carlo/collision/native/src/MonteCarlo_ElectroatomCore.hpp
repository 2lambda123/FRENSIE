//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_ElectroatomCore.hpp
//! \author Luke Kersting
//! \brief  The electroatom core class declaration
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_ELECTROATOM_CORE_HPP
#define MONTE_CARLO_ELECTROATOM_CORE_HPP

// Std Lib Includes
#include <memory>

// Boost Includes
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

// FRENSIE Includes
#include "MonteCarlo_ElectroatomicReactionType.hpp"
#include "MonteCarlo_ElectroatomicReaction.hpp"
#include "MonteCarlo_AtomicRelaxationModel.hpp"
#include "Utility_HashBasedGridSearcher.hpp"
#include "Utility_Vector.hpp"

namespace MonteCarlo{

/*! The electroatom core class for storing electroatomic reactions
 * \details This class can be used to store all reactions and the atomic
 * relaxation model associated with a electroatom. Exposing this object (e.g.
 * get method or export method) is safe since it only allows access to
 * the underlying member data in a way that prohibits modification of that
 * data. This class was created to address the issue that arises when dealing
 * with electro-nuclear data - electro-nuclide's that share the same atomic number
 * need the same electroatomic data. This class allows each electro-nuclide to
 * share the electroatomic data without copying that data (even if each
 * electro-nuclide has its own copy of the electroatom core object).
 */
class ElectroatomCore
{

public:

  //! Typedef for the particle state type
  typedef ElectronState ParticleStateType;

  //! Typedef for the reaction map
  typedef boost::unordered_map<ElectroatomicReactionType,
                   std::shared_ptr<ElectroatomicReaction> >
  ReactionMap;

  //! Typedef for the const reaction map
  typedef boost::unordered_map<ElectroatomicReactionType,
                   std::shared_ptr<const ElectroatomicReaction> >
  ConstReactionMap;

  // Reactions that should be treated as scattering
  static const boost::unordered_set<ElectroatomicReactionType>
  scattering_reaction_types;

  // Reactions that should be treated as void
  static const boost::unordered_set<ElectroatomicReactionType>
  void_reaction_types;

  //! Default constructor
  ElectroatomCore();

  //! Basic constructor
  template<typename InterpPolicy>
  ElectroatomCore(
    const std::shared_ptr<std::vector<double> >& energy_grid,
    const std::shared_ptr<const Utility::HashBasedGridSearcher>& grid_searcher,
    const ReactionMap& standard_scattering_reactions,
    const ReactionMap& standard_absorption_reactions,
    const std::shared_ptr<const AtomicRelaxationModel>& relaxation_model,
    const bool processed_atomic_cross_sections,
    const InterpPolicy policy );

  //! Advanced constructor
  ElectroatomCore(
    const std::shared_ptr<const ElectroatomicReaction>& total_reaction,
    const std::shared_ptr<const ElectroatomicReaction>& total_absorption_reaction,
    const ConstReactionMap& scattering_reactions,
    const ConstReactionMap& absorption_reactions,
    const ConstReactionMap& miscellaneous_reactions,
    const std::shared_ptr<const AtomicRelaxationModel> relaxation_model,
    const std::shared_ptr<const Utility::HashBasedGridSearcher>& grid_searcher );

  //! Copy constructor
  ElectroatomCore( const ElectroatomCore& instance );

  //! Assignment operator
  ElectroatomCore& operator=( const ElectroatomCore& instance );

  //! Destructor
  ~ElectroatomCore()
  { /* ... */ }

  //! Return the total reaction
  const ElectroatomicReaction& getTotalReaction() const;

  //! Return the total absorption reaction
  const ElectroatomicReaction& getTotalAbsorptionReaction() const;

  //! Return the scattering reactions
  const ConstReactionMap& getScatteringReactions() const;

  //! Return the absorption reactions
  const ConstReactionMap& getAbsorptionReactions() const;

  //! Return the miscellaneous non scattering reactions
  const ConstReactionMap& getMiscReactions() const;

  //! Return the atomic relaxation model
  const AtomicRelaxationModel& getAtomicRelaxationModel() const;

  //! Return the hash-based grid searcher
  const Utility::HashBasedGridSearcher& getGridSearcher() const;

  //! Test if all of the reactions share a common energy grid
  bool hasSharedEnergyGrid() const;

private:

  // Set the default scattering reaction types
  static boost::unordered_set<ElectroatomicReactionType>
  setDefaultScatteringReactionTypes();

  // Create the total absorption reaction
  template<typename InterpPolicy>
  static void createTotalAbsorptionReaction(
        const std::shared_ptr<std::vector<double> >& energy_grid,
        const ConstReactionMap& absorption_reactions,
        std::shared_ptr<ElectroatomicReaction>& total_absorption_reaction );

  // Create the processed total absorption reaction
  template<typename InterpPolicy>
  static void createProcessedTotalAbsorptionReaction(
        const std::shared_ptr<std::vector<double> >& energy_grid,
        const ConstReactionMap& absorption_reactions,
        std::shared_ptr<ElectroatomicReaction>& total_absorption_reaction );

  // Create the total reaction
  template<typename InterpPolicy>
  static void createTotalReaction(
      const std::shared_ptr<std::vector<double> >& energy_grid,
      const ConstReactionMap& scattering_reactions,
      const std::shared_ptr<const ElectroatomicReaction>& total_absorption_reaction,
      std::shared_ptr<ElectroatomicReaction>& total_reaction );

  // Calculate the processed total absorption cross section
  template<typename InterpPolicy>
  static void createProcessedTotalReaction(
      const std::shared_ptr<std::vector<double> >& energy_grid,
      const ConstReactionMap& scattering_reactions,
      const std::shared_ptr<const ElectroatomicReaction>& total_absorption_reaction,
      std::shared_ptr<ElectroatomicReaction>& total_reaction );

  // The total reaction
  std::shared_ptr<const ElectroatomicReaction> d_total_reaction;

  // The total absorption reaction
  std::shared_ptr<const ElectroatomicReaction> d_total_absorption_reaction;

  // The scattering reactions
  ConstReactionMap d_scattering_reactions;

  // The absorption reactions
  ConstReactionMap d_absorption_reactions;

  // The miscellaneous reactions
  ConstReactionMap d_miscellaneous_reactions;

  // The atomic relaxation model
  std::shared_ptr<const AtomicRelaxationModel> d_relaxation_model;

  // The hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> d_grid_searcher;
};

// Return the total reaction
inline const ElectroatomicReaction& ElectroatomCore::getTotalReaction() const
{
  return *d_total_reaction;
}

// Return the total absorption reaction
inline const ElectroatomicReaction&
ElectroatomCore::getTotalAbsorptionReaction() const
{
  return *d_total_absorption_reaction;
}

// Return the scattering reactions
inline const ElectroatomCore::ConstReactionMap&
ElectroatomCore::getScatteringReactions() const
{
  return d_scattering_reactions;
}

// Return the absorption reactions
inline const ElectroatomCore::ConstReactionMap&
ElectroatomCore::getAbsorptionReactions() const
{
  return d_absorption_reactions;
}

// Return the miscellaneous reactions
inline const ElectroatomCore::ConstReactionMap&
ElectroatomCore::getMiscReactions() const
{
  return d_miscellaneous_reactions;
}

// Return the atomic relaxation model
inline const AtomicRelaxationModel&
ElectroatomCore::getAtomicRelaxationModel() const
{
  return *d_relaxation_model;
}

// Return the hash-based grid searcher
inline const Utility::HashBasedGridSearcher& ElectroatomCore::getGridSearcher() const
{
  return *d_grid_searcher;
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// Template Includes
//---------------------------------------------------------------------------//

#include "MonteCarlo_ElectroatomCore_def.hpp"

//---------------------------------------------------------------------------//

#endif // end MONTE_CARLO_ELECTROATOM_CORE_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_ElectroatomCore.hpp
//---------------------------------------------------------------------------//

