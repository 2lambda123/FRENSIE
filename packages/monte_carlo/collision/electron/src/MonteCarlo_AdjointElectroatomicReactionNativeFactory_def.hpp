//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_AdjointElectroatomicReactionNativeFactory_def.hpp
//! \author Luke Kersting
//! \brief  The adjoint electroatomic reaction Native data factory template definition
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_ADJOINT_ELECTROATOMIC_REACTION_NATIVE_FACTORY_DEF_HPP
#define MONTE_CARLO_ADJOINT_ELECTROATOMIC_REACTION_NATIVE_FACTORY_DEF_HPP

// Std Lib Includes
#include <algorithm>

// FRENSIE Includes
#include "MonteCarlo_CoupledElasticAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_DecoupledElasticAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_HybridElasticAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_CutoffElasticAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_MomentPreservingElasticAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_BremsstrahlungAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_ElectroionizationSubshellAdjointElectroatomicReaction.hpp"
#include "MonteCarlo_AbsorptionElectroatomicReaction.hpp"
#include "Data_SubshellType.hpp"
#include "Utility_TabularDistribution.hpp"
#include "Utility_SortAlgorithms.hpp"
#include "Utility_DesignByContract.hpp"

namespace MonteCarlo{

// Create the coupled elastic scattering adjoint electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createCoupledElasticReaction(
        const Data::AdjointElectronPhotonRelaxationDataContainer&
            raw_adjoint_electroatom_data,
        const std::shared_ptr<const std::vector<double> >& energy_grid,
        const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
        std::shared_ptr<const AdjointElectroatomicReaction>& elastic_reaction,
        const CoupledElasticSamplingMethod& sampling_method,
        const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().back() >=
                    energy_grid->back() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Cutoff elastic cross section
  std::shared_ptr<std::vector<double> >
    cutoff_cross_section( new std::vector<double> );
  cutoff_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().end() );

  // Total elastic cross section
  std::shared_ptr<std::vector<double> >
    total_cross_section( new std::vector<double> );
  total_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSection().end() );

  // Create the coupled elastic scattering distribution
  std::shared_ptr<const CoupledElasticElectronScatteringDistribution> distribution;
  ElasticFactory::createCoupledElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
    distribution,
    energy_grid,
    cutoff_cross_section,
    total_cross_section,
    raw_adjoint_electroatom_data,
    sampling_method,
    evaluation_tol );

  elastic_reaction.reset(
    new CoupledElasticAdjointElectroatomicReaction<Utility::LinLin>(
      energy_grid,
      total_cross_section,
      raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSectionThresholdEnergyIndex(),
      grid_searcher,
      distribution ) );
}

// Create the decoupled elastic scattering adjoint electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createDecoupledElasticReaction(
        const Data::AdjointElectronPhotonRelaxationDataContainer&
            raw_adjoint_electroatom_data,
        const std::shared_ptr<const std::vector<double> >& energy_grid,
        const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
        std::shared_ptr<const AdjointElectroatomicReaction>& elastic_reaction,
        const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Cutoff elastic cross section
  std::shared_ptr<std::vector<double> >
    cutoff_cross_section( new std::vector<double> );
  cutoff_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().end() );

  // Total elastic cross section
  std::shared_ptr<std::vector<double> >
    total_cross_section( new std::vector<double> );
  
  total_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSection().end() );

  // Total elastic cross section threshold energy bin index
  size_t threshold_energy_index =
    raw_adjoint_electroatom_data.getAdjointTotalElasticCrossSectionThresholdEnergyIndex();

  // Calculate sampling ratios
  std::shared_ptr<std::vector<double> >
    sampling_ratios( new std::vector<double>( total_cross_section->size() ) );
  
  for( unsigned i = 0; i < sampling_ratios->size(); ++i )
  {
    (*sampling_ratios)[i] =
      (*cutoff_cross_section)[i]/(*total_cross_section)[i];
  }

  // Create the tabular cutoff elastic scattering distribution
  std::shared_ptr<const CutoffElasticElectronScatteringDistribution> tabular_distribution;
  ElasticFactory::createCutoffElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
    tabular_distribution,
    raw_adjoint_electroatom_data,
    MonteCarlo::ElasticElectronTraits::mu_peak,
    evaluation_tol );

  // Create the analytical screened Rutherford elastic scattering distribution
  std::shared_ptr<const ScreenedRutherfordElasticElectronScatteringDistribution> analytical_distribution;
  ElasticFactory::createScreenedRutherfordElasticDistribution(
    analytical_distribution,
    raw_adjoint_electroatom_data.getAtomicNumber() );


  elastic_reaction.reset(
    new DecoupledElasticAdjointElectroatomicReaction<Utility::LinLin>(
                                        energy_grid,
                                        total_cross_section,
                                        sampling_ratios,
                                        threshold_energy_index,
                                        grid_searcher,
                                        tabular_distribution,
                                        analytical_distribution ) );
}

// Create a hybrid elastic scattering adjoint electroatomic reaction
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createHybridElasticReaction(
    const Data::AdjointElectronPhotonRelaxationDataContainer&
                raw_adjoint_electroatom_data,
    const std::shared_ptr<const std::vector<double> >& energy_grid,
    const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
    std::shared_ptr<const AdjointElectroatomicReaction>& elastic_reaction,
    const double cutoff_angle_cosine,
    const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().back() >=
                    energy_grid->back() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Cutoff elastic cross section
  std::shared_ptr<std::vector<double> >
    cutoff_cross_section( new std::vector<double> );
  
  cutoff_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().end() );

  // Cutoff elastic cross section threshold energy bin index
  size_t cutoff_threshold_energy_index =
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSectionThresholdEnergyIndex();

  // Moment preserving elastic cross section
  std::shared_ptr<std::vector<double> >
    mp_cross_section( new std::vector<double> );
  
  size_t mp_threshold_energy_index;
  ElasticFactory::calculateMomentPreservingCrossSections<TwoDInterpPolicy,TwoDGridPolicy>(
                                                  *mp_cross_section,
                                                  mp_threshold_energy_index,
                                                  raw_adjoint_electroatom_data,
                                                  energy_grid,
                                                  evaluation_tol );
  // Create the hybrid elastic scattering distribution
  std::shared_ptr<const HybridElasticElectronScatteringDistribution> distribution;

  ElasticFactory::createHybridElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
                                                  distribution,
                                                  energy_grid,
                                                  cutoff_cross_section,
                                                  mp_cross_section,
                                                  raw_adjoint_electroatom_data,
                                                  cutoff_angle_cosine,
                                                  evaluation_tol );

  // Create the cutoff elastic scattering distribution
  std::shared_ptr<const CutoffElasticElectronScatteringDistribution> cutoff_distribution;
  ElasticFactory::createCutoffElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
            cutoff_distribution,
            raw_adjoint_electroatom_data,
            cutoff_angle_cosine,
            evaluation_tol );

  // Calculate the hybrid cross section
  size_t hybrid_threshold_energy_index =
    std::min( mp_threshold_energy_index, cutoff_threshold_energy_index );

  size_t mp_threshold_diff =
    mp_threshold_energy_index - hybrid_threshold_energy_index;
  size_t cutoff_threshold_diff =
    cutoff_threshold_energy_index - hybrid_threshold_energy_index;

  std::vector<double> combined_cross_section(
                           energy_grid->size() - hybrid_threshold_energy_index );

  for (size_t i = 0; i < combined_cross_section.size(); ++i )
  {
    double energy = (*energy_grid)[i + hybrid_threshold_energy_index];
    double reduced_cutoff_ratio =
                cutoff_distribution->evaluateCutoffCrossSectionRatio( energy );

    if ( i < mp_threshold_diff )
    {
      combined_cross_section[i] =
        (*cutoff_cross_section)[i]*reduced_cutoff_ratio;
    }
    else if ( i < cutoff_threshold_diff )
    {
      combined_cross_section[i] = (*mp_cross_section)[i];
    }
    else
    {
      combined_cross_section[i] =
        (*cutoff_cross_section)[i-cutoff_threshold_diff]*reduced_cutoff_ratio +
        (*mp_cross_section)[i-mp_threshold_diff];
    }
  }

  std::shared_ptr<std::vector<double> >
    hybrid_cross_section( new std::vector<double> );
  
  hybrid_cross_section->assign( combined_cross_section.begin(),
                                combined_cross_section.end() );


  // Create the hybrid elastic reaction
  elastic_reaction.reset(
    new HybridElasticAdjointElectroatomicReaction<Utility::LinLin>(
            energy_grid,
            hybrid_cross_section,
            hybrid_threshold_energy_index,
            grid_searcher,
            cutoff_angle_cosine,
            distribution ) );
}

// Create the cutoff elastic scattering adjoint electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createCutoffElasticReaction(
        const Data::AdjointElectronPhotonRelaxationDataContainer&
            raw_adjoint_electroatom_data,
        const std::shared_ptr<const std::vector<double> >& energy_grid,
        const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
        std::shared_ptr<const AdjointElectroatomicReaction>& elastic_reaction,
        const double cutoff_angle_cosine,
        const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().back() >=
                    energy_grid->back() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Create the cutoff elastic scattering distribution using the cutoff angle cosine
  std::shared_ptr<const CutoffElasticElectronScatteringDistribution> distribution;

  ElasticFactory::createCutoffElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
    distribution,
    raw_adjoint_electroatom_data,
    cutoff_angle_cosine,
    evaluation_tol );

  // Cutoff elastic cross section
  std::shared_ptr<std::vector<double> >
    elastic_cross_section( new std::vector<double> );
  elastic_cross_section->assign(
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().begin(),
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSection().end() );

  // Cutoff elastic cross section threshold energy bin index
  size_t threshold_energy_index =
    raw_adjoint_electroatom_data.getAdjointCutoffElasticCrossSectionThresholdEnergyIndex();

  elastic_reaction.reset(
    new CutoffElasticAdjointElectroatomicReaction<Utility::LinLin>(
                          energy_grid,
                          elastic_cross_section,
                          threshold_energy_index,
                          grid_searcher,
                          distribution ) );
}

// Create the moment preserving elastic scattering adjoint electroatomic reaction
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createMomentPreservingElasticReaction(
        const Data::AdjointElectronPhotonRelaxationDataContainer&
            raw_adjoint_electroatom_data,
        const std::shared_ptr<const std::vector<double> >& energy_grid,
        const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
        std::shared_ptr<const AdjointElectroatomicReaction>& elastic_reaction,
        const double cutoff_angle_cosine,
        const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().back() >=
                    energy_grid->back() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Create the moment preserving elastic scattering distribution
  std::shared_ptr<const MomentPreservingElasticElectronScatteringDistribution>
    distribution;

  ElasticFactory::createMomentPreservingElasticDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
    distribution,
    raw_adjoint_electroatom_data,
    cutoff_angle_cosine,
    evaluation_tol );

  // Moment preserving elastic cross section
  std::vector<double> moment_preserving_cross_sections;
  size_t threshold_energy_index;
  ElasticFactory::calculateMomentPreservingCrossSections<TwoDInterpPolicy,TwoDGridPolicy>(
                                moment_preserving_cross_sections,
                                threshold_energy_index,
                                raw_adjoint_electroatom_data,
                                energy_grid,
                                evaluation_tol );

  std::shared_ptr<std::vector<double> >
    elastic_cross_section( new std::vector<double> );
  
  elastic_cross_section->assign(
    moment_preserving_cross_sections.begin(),
    moment_preserving_cross_sections.end() );

  elastic_reaction.reset(
    new MomentPreservingElasticAdjointElectroatomicReaction<Utility::LinLin>(
                          energy_grid,
                          elastic_cross_section,
                          threshold_energy_index,
                          grid_searcher,
                          distribution ) );
}

// Create the subshell electroionization electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createSubshellElectroionizationReaction(
    const Data::AdjointElectronPhotonRelaxationDataContainer& raw_adjoint_electroatom_data,
    const std::shared_ptr<const std::vector<double> >& energy_grid,
    const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
    const unsigned subshell,
    std::shared_ptr<const AdjointElectroatomicReaction>& electroionization_subshell_reaction,
    const double evaluation_tol )
{
  // Convert subshell number to enum
  Data::SubshellType subshell_type =
    Data::convertENDFDesignatorToSubshellEnum( subshell );

  // Electroionization cross section
  std::shared_ptr<std::vector<double> >
    subshell_cross_section( new std::vector<double> );
  subshell_cross_section->assign(
      raw_adjoint_electroatom_data.getAdjointElectroionizationCrossSection( subshell ).begin(),
      raw_adjoint_electroatom_data.getAdjointElectroionizationCrossSection( subshell ).end() );

  // Electroionization cross section threshold energy bin index
  size_t threshold_energy_index =
      raw_adjoint_electroatom_data.getAdjointElectroionizationCrossSectionThresholdEnergyIndex(
      subshell );

  // The electroionization subshell distribution
  std::shared_ptr<const ElectroionizationSubshellAdjointElectronScatteringDistribution>
      electroionization_subshell_distribution;

  // Create the electroionization subshell distribution
  ElectroionizationFactory::createElectroionizationSubshellDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
        raw_adjoint_electroatom_data,
        subshell,
        raw_adjoint_electroatom_data.getSubshellBindingEnergy( subshell ),
        electroionization_subshell_distribution,
        evaluation_tol );


  // Create the subshell electroelectric reaction
  electroionization_subshell_reaction.reset(
      new ElectroionizationSubshellAdjointElectroatomicReaction<Utility::LinLin>(
              energy_grid,
              subshell_cross_section,
              threshold_energy_index,
              grid_searcher,
              subshell_type,
              electroionization_subshell_distribution ) );
}

// Create the subshell electroionization electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createSubshellElectroionizationReactions(
    const Data::AdjointElectronPhotonRelaxationDataContainer& raw_adjoint_electroatom_data,
    const std::shared_ptr<const std::vector<double> >& energy_grid,
    const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
    std::vector<std::shared_ptr<const AdjointElectroatomicReaction> >&
        electroionization_subshell_reactions,
    const double evaluation_tol )
{
  electroionization_subshell_reactions.clear();

  // Extract the subshell information
  std::set<unsigned> subshells = raw_adjoint_electroatom_data.getSubshells();

  std::shared_ptr<const AdjointElectroatomicReaction>
    electroionization_subshell_reaction;

  std::set<unsigned>::iterator shell = subshells.begin();

  for( shell; shell != subshells.end(); ++shell )
  {
    ThisType::createSubshellElectroionizationReaction<TwoDInterpPolicy,TwoDGridPolicy>(
        raw_adjoint_electroatom_data,
        energy_grid,
        grid_searcher,
        *shell,
        electroionization_subshell_reaction,
        evaluation_tol );

    electroionization_subshell_reactions.push_back(
                      electroionization_subshell_reaction );
  }

  // Make sure the subshell electroelectric reactions have been created
  testPostcondition( electroionization_subshell_reactions.size() > 0 );
}

// Create a bremsstrahlung electroatomic reactions
template<typename TwoDInterpPolicy, template<typename> class TwoDGridPolicy>
void AdjointElectroatomicReactionNativeFactory::createBremsstrahlungReaction(
        const Data::AdjointElectronPhotonRelaxationDataContainer& raw_adjoint_electroatom_data,
        const std::shared_ptr<const std::vector<double> >& energy_grid,
        const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
        std::shared_ptr<const AdjointElectroatomicReaction>& bremsstrahlung_reaction,
        const double evaluation_tol )
{
  // Make sure the energy grid is valid
  testPrecondition( raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid().size() ==
                    energy_grid->size() );
  testPrecondition( Utility::Sort::isSortedAscending( energy_grid->begin(),
                                                      energy_grid->end() ) );

  // Bremsstrahlung cross section
  std::shared_ptr<std::vector<double> >
    bremsstrahlung_cross_section( new std::vector<double> );
  bremsstrahlung_cross_section->assign(
   raw_adjoint_electroatom_data.getAdjointBremsstrahlungElectronCrossSection().begin(),
   raw_adjoint_electroatom_data.getAdjointBremsstrahlungElectronCrossSection().end() );

  // Index of first non zero cross section in the energy grid
  size_t threshold_energy_index =
    raw_adjoint_electroatom_data.getAdjointBremsstrahlungElectronCrossSectionThresholdEnergyIndex();

  // Create bremsstrahlung scattering distribution
  std::shared_ptr<const BremsstrahlungAdjointElectronScatteringDistribution>
    bremsstrahlung_distribution;

  BremsstrahlungFactory::createBremsstrahlungDistribution<TwoDInterpPolicy,TwoDGridPolicy>(
        raw_adjoint_electroatom_data,
        raw_adjoint_electroatom_data.getAdjointElectronEnergyGrid(),
        bremsstrahlung_distribution,
        evaluation_tol );

  // Create the bremsstrahlung reaction
  bremsstrahlung_reaction.reset(
         new BremsstrahlungAdjointElectroatomicReaction<Utility::LinLin>(
                          energy_grid,
                          bremsstrahlung_cross_section,
                          threshold_energy_index,
                          grid_searcher,
                          bremsstrahlung_distribution ) );
}

// Create the forward total reaction (only used to get the cross section)
template<typename ReactionType>
void AdjointElectroatomicReactionNativeFactory::createTotalForwardReaction(
      const Data::AdjointElectronPhotonRelaxationDataContainer&
        raw_adjoint_electroatom_data,
      const std::shared_ptr<const std::vector<double> >& energy_grid,
      const std::shared_ptr<const Utility::HashBasedGridSearcher<double>>& grid_searcher,
      const std::shared_ptr<const ReactionType>& elastic_reaction,
      std::shared_ptr<const ElectroatomicReaction>& total_forward_reaction )
{
  // Make sure the elastic reaction is valid
  testPrecondition( elastic_reaction.use_count() > 0 );

  // Get the inelastic cross sections
  std::vector<double> inelastic_cross_section =
    raw_adjoint_electroatom_data.getForwardInelasticElectronCrossSection();

  // Add the inelastic and elastic cross section together
  std::shared_ptr<std::vector<double> > total_forward_cross_section(
                              new std::vector<double>( energy_grid->size() ) );
  
  for( size_t i = 0; i < energy_grid->size(); ++i )
  {
    (*total_forward_cross_section)[i] = inelastic_cross_section[i] +
      elastic_reaction->getCrossSection( (*energy_grid)[i] );
  }

  // Create the total forward reaction
  total_forward_reaction.reset(
     new AbsorptionElectroatomicReaction<Utility::LinLin,false>(
                                               energy_grid,
                                               total_forward_cross_section,
                                               0u,
                                               grid_searcher,
                                               TOTAL_ELECTROATOMIC_REACTION ) );
}

} // end MontCarlo namespace

#endif // end MONTE_CARLO_ADJOINT_ELECTROATOMIC_REACTION_NATIVE_FACTORY_DEF_HPP