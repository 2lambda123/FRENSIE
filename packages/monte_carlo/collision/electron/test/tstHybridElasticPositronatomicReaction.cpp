//---------------------------------------------------------------------------//
//!
//! \file   tstHybridElasticPositronatomicReaction.cpp
//! \author Luke Kersting
//! \brief  Hybrid Elastic positron-atomic reaction unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// FRENSIE Includes
#include "Data_ElectronPhotonRelaxationDataContainer.hpp"
#include "MonteCarlo_HybridElasticPositronatomicReaction.hpp"
#include "MonteCarlo_HybridElasticElectronScatteringDistribution.hpp"
#include "MonteCarlo_ElasticElectronScatteringDistributionNativeFactory.hpp"
#include "Utility_RandomNumberGenerator.hpp"
#include "Utility_HistogramDistribution.hpp"
#include "Utility_TabularDistribution.hpp"
#include "Utility_UnitTestHarnessWithMain.hpp"

//---------------------------------------------------------------------------//
// Testing Variables.
//---------------------------------------------------------------------------//

std::shared_ptr<MonteCarlo::HybridElasticPositronatomicReaction<Utility::LogLog> >
    hybrid_elastic_reaction;

//---------------------------------------------------------------------------//
// Tests
//---------------------------------------------------------------------------//
// Check that the reaction type can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, getReactionType )
{
  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getReactionType(),
                       MonteCarlo::HYBRID_ELASTIC_POSITRONATOMIC_REACTION );
}

//---------------------------------------------------------------------------//
// Check that the threshold energy can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, getThresholdEnergy )
{
  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getThresholdEnergy(),
                       1.0e-5 );
}

//---------------------------------------------------------------------------//
// Check that the number of electrons emitted from the rxn can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, getNumberOfEmittedElectrons )
{
  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedElectrons(1e-3),
                       0u );

  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedElectrons(20.0),
                       0u );
}

//---------------------------------------------------------------------------//
// Check that the number of photons emitted from the rxn can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, getNumberOfEmittedPhotons )
{
  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedPhotons(1e-3),
                       0u );

  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedPhotons(20.0),
                       0u );
}

//---------------------------------------------------------------------------//
// Check that the number of positrons emitted from the rxn can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, getNumberOfEmittedPositrons )
{
  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedPositrons(1e-3),
                       1u );

  FRENSIE_CHECK_EQUAL( hybrid_elastic_reaction->getNumberOfEmittedPositrons(20.0),
                       1u );
}

//---------------------------------------------------------------------------//
// Check that the hybrid cross section can be returned
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction,
                   getCrossSection )
{

  double ratio = 9.500004750002375431e-01;
  double cross_section = hybrid_elastic_reaction->getCrossSection( 1.0E-05 );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 3.62586e+09*ratio + 1.611494138359356821e+08, 1e-12 );

  cross_section = hybrid_elastic_reaction->getCrossSection( 1.0E-03 );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 7.5860420097891897e+07, 1e-12 );

  ratio = 8.6868172466733646e-06;
  cross_section = hybrid_elastic_reaction->getCrossSection( 1.0E+05 );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.2177e-03*ratio + 6.8080603251349155e-05, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the elastic reaction can be simulated
FRENSIE_UNIT_TEST( HybridElasticPositronatomicReaction, react )
{
  MonteCarlo::PositronState positron( 0 );
  positron.setEnergy( 20.0 );
  positron.setDirection( 0.0, 0.0, 1.0 );

  MonteCarlo::ParticleBank bank;

  Data::SubshellType shell_of_interaction;

  hybrid_elastic_reaction->react( positron, bank, shell_of_interaction );

  FRENSIE_CHECK_EQUAL( positron.getEnergy(), 20.0 );
  FRENSIE_CHECK( positron.getZDirection() < 2.0 );
  FRENSIE_CHECK( positron.getZDirection() > 0.0 );
  FRENSIE_CHECK( bank.isEmpty() );
  FRENSIE_CHECK_EQUAL( shell_of_interaction, Data::UNKNOWN_SUBSHELL );
}

//---------------------------------------------------------------------------//
// Custom setup
//---------------------------------------------------------------------------//
FRENSIE_CUSTOM_UNIT_TEST_SETUP_BEGIN();

std::string test_native_file_name;

FRENSIE_CUSTOM_UNIT_TEST_COMMAND_LINE_OPTIONS()
{
  ADD_STANDARD_OPTION_AND_ASSIGN_VALUE( "test_native_file",
                                        test_native_file_name, "",
                                        "Test Native file name" );
}

FRENSIE_CUSTOM_UNIT_TEST_INIT()
{
  double evaluation_tol = 1e-15;

  // Get native data container
  Data::ElectronPhotonRelaxationDataContainer data_container =
        Data::ElectronPhotonRelaxationDataContainer( test_native_file_name );

  std::shared_ptr<const std::vector<double> > energy_grid(
           new std::vector<double>( data_container.getElectronEnergyGrid() ) );

  // Construct the grid searcher
  std::shared_ptr<Utility::HashBasedGridSearcher<double> > grid_searcher(
        new Utility::StandardHashBasedGridSearcher<std::vector<double>,false>(
                    energy_grid,
                    energy_grid->front(),
                    energy_grid->back(),
                    energy_grid->size()/10+1 ) );

  std::shared_ptr<const std::vector<double> > cutoff_cross_section(
     new std::vector<double>( data_container.getCutoffElasticCrossSection() ) );

  // Moment preserving elastic cross section
  std::vector<double> moment_preserving_cross_sections;
  size_t mp_threshold_energy_index;
  MonteCarlo::ElasticElectronScatteringDistributionNativeFactory::calculateMomentPreservingCrossSections<Utility::LogLogCosLog,Utility::Correlated>(
                               moment_preserving_cross_sections,
                               mp_threshold_energy_index,
                               data_container,
                               energy_grid,
                               evaluation_tol );

  std::shared_ptr<const std::vector<double> > mp_cross_section(
                 new std::vector<double>( moment_preserving_cross_sections ) );

  // Calculate the hybrid cross section
  size_t hybrid_threshold_energy_index =
    std::min( mp_threshold_energy_index,
              (size_t)data_container.getCutoffElasticCrossSectionThresholdEnergyIndex() );

  size_t mp_threshold_diff =
    mp_threshold_energy_index - hybrid_threshold_energy_index;
  
  size_t cutoff_threshold_diff =
    data_container.getCutoffElasticCrossSectionThresholdEnergyIndex() -
    hybrid_threshold_energy_index;

  std::vector<double> combined_cross_section(
                         energy_grid->size() - hybrid_threshold_energy_index );

  std::shared_ptr<const MonteCarlo::CutoffElasticElectronScatteringDistribution>
        cutoff_elastic_distribution;

  MonteCarlo::ElasticElectronScatteringDistributionNativeFactory::createCutoffElasticDistribution<Utility::LogLogCosLog,Utility::Correlated>(
        cutoff_elastic_distribution,
        data_container,
        data_container.getCutoffAngleCosine(),
        evaluation_tol );


  for (unsigned i = 0; i < combined_cross_section.size(); ++i )
  {
    double energy = (*energy_grid)[i + hybrid_threshold_energy_index];
    double reduced_cutoff_ratio =
        cutoff_elastic_distribution->evaluateCutoffCrossSectionRatio( energy );
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

  std::shared_ptr<const std::vector<double> > hybrid_cross_section(
                           new std::vector<double>( combined_cross_section ) );

  // Create hybrid distribution
  std::shared_ptr<const MonteCarlo::HybridElasticElectronScatteringDistribution>
        hybrid_elastic_distribution;

  MonteCarlo::ElasticElectronScatteringDistributionNativeFactory::createHybridElasticDistribution<Utility::LogLogCosLog,Utility::Correlated>(
        hybrid_elastic_distribution,
        energy_grid,
        cutoff_cross_section,
        mp_cross_section,
        data_container,
        data_container.getCutoffAngleCosine(),
        evaluation_tol );

  // Create the reaction
  hybrid_elastic_reaction.reset(
    new MonteCarlo::HybridElasticPositronatomicReaction<Utility::LogLog>(
            energy_grid,
            hybrid_cross_section,
            hybrid_threshold_energy_index,
            grid_searcher,
            data_container.getCutoffAngleCosine(),
            hybrid_elastic_distribution ) );

  // Initialize the random number generator
  Utility::RandomNumberGenerator::createStreams();
}

FRENSIE_CUSTOM_UNIT_TEST_SETUP_END();

//---------------------------------------------------------------------------//
// end tstHybridElasticPositronatomicReaction.cpp
//---------------------------------------------------------------------------//
