//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_CutoffElasticElectroatomicReaction_def.hpp
//! \author Luke Kersting
//! \brief  The cutoff scattering elastic electroatomic reaction class def.
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_CUTOFF_ELASTIC_ELECTROATOMIC_REACTION_DEF_HPP
#define MONTE_CARLO_CUTOFF_ELASTIC_ELECTROATOMIC_REACTION_DEF_HPP

// FRENSIE Includes
#include "Utility_SortAlgorithms.hpp"
#include "Utility_ContractException.hpp"

namespace MonteCarlo{

// Basic Constructor
template<typename InterpPolicy, bool processed_cross_section>
CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::CutoffElasticElectroatomicReaction(
       const std::shared_ptr<const std::vector<double> >& incoming_energy_grid,
       const std::shared_ptr<const std::vector<double> >& cross_section,
       const unsigned threshold_energy_index,
       const std::shared_ptr<const CutoffElasticElectronScatteringDistribution>&
         scattering_distribution )
  : BaseType( incoming_energy_grid,
              cross_section,
              threshold_energy_index ),
    d_scattering_distribution( scattering_distribution )
{
  // Make sure the incoming energy grid is valid
  testPrecondition( incoming_energy_grid.size() > 0 );
  testPrecondition( Utility::Sort::isSortedAscending(
                        incoming_energy_grid.begin(),
                        incoming_energy_grid.end() ) );
  // Make sure the cross section is valid
  testPrecondition( cross_section.size() > 0 );
  testPrecondition( cross_section.size() ==
                    incoming_energy_grid.size() - threshold_energy_index );
  // Make sure the threshold energy is valid
  testPrecondition( threshold_energy_index < incoming_energy_grid.size() );
  // Make sure scattering distribution is valid
  testPrecondition( scattering_distribution.use_count() > 0 );
}

// Constructor
template<typename InterpPolicy, bool processed_cross_section>
CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::CutoffElasticElectroatomicReaction(
       const std::shared_ptr<const std::vector<double> >& incoming_energy_grid,
       const std::shared_ptr<const std::vector<double> >& cross_section,
       const unsigned threshold_energy_index,
       const std::shared_ptr<const Utility::HashBasedGridSearcher>& grid_searcher,
       const std::shared_ptr<const CutoffElasticElectronScatteringDistribution>&
         scattering_distribution )
  : BaseType( incoming_energy_grid,
              cross_section,
              threshold_energy_index,
              grid_searcher ),
    d_scattering_distribution( scattering_distribution )
{
  // Make sure the incoming energy grid is valid
  testPrecondition( incoming_energy_grid.size() > 0 );
  testPrecondition( Utility::Sort::isSortedAscending(
                        incoming_energy_grid.begin(),
                        incoming_energy_grid.end() ) );
  // Make sure the cross section is valid
  testPrecondition( cross_section.size() > 0 );
  testPrecondition( cross_section.size() ==
                    incoming_energy_grid.size() - threshold_energy_index );
  // Make sure the threshold energy is valid
  testPrecondition( threshold_energy_index < incoming_energy_grid.size() );
  // Make sure scattering distribution is valid
  testPrecondition( scattering_distribution.use_count() > 0 );
  // Make sure the grid searcher is valid
  testPrecondition( !grid_searcher.is_null() );
}

// Return the number of photons emitted from the rxn at the given energy
/*! \details This does not include photons from atomic relaxation.
 */
template<typename InterpPolicy, bool processed_cross_section>
unsigned CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedPhotons( const double energy ) const
{
  return 0u;
}

// Return the number of electrons emitted from the rxn at the given energy
template<typename InterpPolicy, bool processed_cross_section>
unsigned CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getNumberOfEmittedElectrons( const double energy ) const
{
  return 0u;
}

// Return the reaction type
template<typename InterpPolicy, bool processed_cross_section>
ElectroatomicReactionType CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getReactionType() const
{
  return CUTOFF_ELASTIC_ELECTROATOMIC_REACTION;
}

// Return the cross section at the given energy
template<typename InterpPolicy, bool processed_cross_section>
double CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getCrossSection(
    const double energy ) const
{
  // Make sure the energy is valid
  testPrecondition( this->isEnergyWithinEnergyGrid( energy ) );

  // Get the cross section ratio for the cutoff angle cosine
  double cross_section_ratio =
    d_scattering_distribution->evaluateCutoffCrossSectionRatio( energy );

  double cross_section;

  cross_section =
    StandardGenericAtomicReaction<ElectroatomicReaction,InterpPolicy,processed_cross_section>::getCrossSection(
    energy );

  // Make sure the cross section ratio is valid
  testPostcondition( cross_section_ratio >= 0.0 );
  testPostcondition( cross_section_ratio <= 1.0 );
  // Make sure the cross section is valid
  testPostcondition( cross_section >= 0.0 );

  return cross_section*cross_section_ratio;
}

// Return the cross section at the given energy (efficient)
template<typename InterpPolicy, bool processed_cross_section>
double CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getCrossSection(
    const double energy,
    const unsigned bin_index ) const
{
  // Get the cross section ratio for the cutoff angle cosine
  double cross_section_ratio =
    d_scattering_distribution->evaluateCutoffCrossSectionRatio( energy );

  double cross_section;

  cross_section =
    StandardGenericAtomicReaction<ElectroatomicReaction,InterpPolicy,processed_cross_section>::getCrossSection(
    energy,
    bin_index );

  // Make sure the cross section ratio is valid
  testPostcondition( cross_section_ratio >= 0.0 );
  testPostcondition( cross_section_ratio <= 1.0 );

  return cross_section*cross_section_ratio;
}

// Return the differential cross section
template<typename InterpPolicy, bool processed_cross_section>
double CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::getDifferentialCrossSection(
            const double incoming_energy,
            const double scattering_angle_cosine ) const
{
  // Get the PDF
  double pdf =
    d_scattering_distribution->evaluatePDF( incoming_energy,
                                            scattering_angle_cosine );

  // Get the cross section
  double cross_section = this->getCrossSection( incoming_energy );

  return pdf*cross_section;
}

// Simulate the reaction
template<typename InterpPolicy, bool processed_cross_section>
void CutoffElasticElectroatomicReaction<InterpPolicy,processed_cross_section>::react(
                    ElectronState& electron,
                    ParticleBank& bank,
                    Data::SubshellType& shell_of_interaction ) const
{
  d_scattering_distribution->scatterElectron( electron,
                                              bank,
                                              shell_of_interaction);

  electron.incrementCollisionNumber();

  // The shell of interaction is currently ignored
  shell_of_interaction =Data::UNKNOWN_SUBSHELL;
}

} // end MonteCarlo namespace

#endif // end MONTE_CARLO_CUTOFF_ELASTIC_ELECTROATOMIC_REACTION_DEF_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_CutoffElasticElectroatomicReaction_def.hpp
//---------------------------------------------------------------------------//
