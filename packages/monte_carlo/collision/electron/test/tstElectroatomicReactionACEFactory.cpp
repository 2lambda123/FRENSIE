//---------------------------------------------------------------------------//
//!
//! \file   tstElectroatomicReactionACEFactory.cpp
//! \author Luke Kersting
//! \brief  Electroatomic reaction factory using ACE data unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_VerboseObject.hpp>
#include <Teuchos_RCP.hpp>

// FRENSIE Includes
#include "MonteCarlo_ElectroatomicReactionACEFactory.hpp"
#include "MonteCarlo_BremsstrahlungAngularDistributionType.hpp"
#include "Data_ACEFileHandler.hpp"
#include "Data_XSSEPRDataExtractor.hpp"
#include "Utility_StandardHashBasedGridSearcher.hpp"
#include "Utility_UnitTestHarnessExtensions.hpp"

//---------------------------------------------------------------------------//
// Testing Variables
//---------------------------------------------------------------------------//

MonteCarlo::BremsstrahlungAngularDistributionType photon_distribution_function;
Teuchos::RCP<Data::XSSEPRDataExtractor> xss_data_extractor;
Teuchos::ArrayRCP<double> energy_grid;
Teuchos::RCP<Utility::HashBasedGridSearcher> grid_searcher;
std::shared_ptr<MonteCarlo::ElectroatomicReaction> reaction;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that an elastic reaction can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createCutoffElasticReaction )
{
  MonteCarlo::ElectroatomicReactionACEFactory::createCutoffElasticReaction(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reaction );

  // Test reaction properties
  TEST_EQUALITY_CONST( reaction->getReactionType(),
                       MonteCarlo::CUTOFF_ELASTIC_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test that the stored cross section is correct
  double energy = 1e-5;
  double cross_section =
    reaction->getCrossSection( energy );

  TEST_FLOATING_EQUALITY( cross_section, 2.48924e+9, 1e-12 );

  energy = 4e-4;
  cross_section = reaction->getCrossSection( energy );

  TEST_FLOATING_EQUALITY( cross_section, 4.436635458458e+8, 1e-12 );

  energy = 1e+5;
  cross_section = reaction->getCrossSection( energy );

  TEST_FLOATING_EQUALITY( cross_section, 8.83051e-2, 1e-12 );

  // Clear the reaction
  reaction.reset();
}

//---------------------------------------------------------------------------//
// Check that an atomic excitation reaction can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createAtomicExcitationReaction )
{
  MonteCarlo::ElectroatomicReactionACEFactory::createAtomicExcitationReaction(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reaction );

  // Test reaction properties
  TEST_EQUALITY_CONST( reaction->getReactionType(),
                       MonteCarlo::ATOMIC_EXCITATION_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test that the stored cross section is correct
  double cross_section =
    reaction->getCrossSection( 1e-5 );

  TEST_FLOATING_EQUALITY( cross_section, 8.75755e+6, 1e-12 );

  cross_section = reaction->getCrossSection( 4e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 6.22682e+8, 1e-12 );

  cross_section = reaction->getCrossSection( 1e+5 );

  TEST_FLOATING_EQUALITY( cross_section, 1.57861e+6, 1e-12 );

  // Clear the reaction
  reaction.reset();
}

//---------------------------------------------------------------------------//
// Check that the total electroionization reaction can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createElectroionizationReaction )
{
  std::shared_ptr<MonteCarlo::ElectroatomicReaction> reaction;

  MonteCarlo::ElectroatomicReactionACEFactory::createTotalElectroionizationReaction(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reaction );

  // Test the first shell's reaction properties
  TEST_EQUALITY_CONST(
            reaction->getReactionType(),
            MonteCarlo::TOTAL_ELECTROIONIZATION_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test the total cross section is correct
  double cross_section = reaction->getCrossSection( 1e-5 );
  TEST_FLOATING_EQUALITY( cross_section, 1.310577E+08, 1e-12 );

  cross_section = reaction->getCrossSection( 8.9754e-2 );
  TEST_FLOATING_EQUALITY( cross_section, 3.59749583408E+06, 1e-12 );

  cross_section = reaction->getCrossSection( 1e-1 );
  TEST_FLOATING_EQUALITY( cross_section, 3.345696097519E+06, 1e-12 );

  cross_section = reaction->getCrossSection( 1.58489e-1 );
  TEST_FLOATING_EQUALITY( cross_section, 2.510192579238E+06, 1e-12 );

  cross_section = reaction->getCrossSection( 1e+5 );
  TEST_FLOATING_EQUALITY( cross_section, 1.2648388779E+06, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that the electroionization subshell reactions can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createSubshellElectroionizationReactions )
{
  std::vector<std::shared_ptr<MonteCarlo::ElectroatomicReaction> > reactions;

  MonteCarlo::ElectroatomicReactionACEFactory::createSubshellElectroionizationReactions(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reactions );

  TEST_EQUALITY_CONST( reactions.size(), 24 );

  // Test the first shell's reaction properties
  TEST_EQUALITY_CONST(
            reactions.front()->getReactionType(),
            MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reactions.front()->getThresholdEnergy(), 8.9754e-2 );

  // Test the first shell's stored cross section is correct
  double cross_section =
    reactions.front()->getCrossSection( 8.9754e-2 );

  TEST_FLOATING_EQUALITY( cross_section, 1.250673571307e-1, 1e-12 );

  cross_section =
    reactions.front()->getCrossSection( 1e-1 );

  TEST_FLOATING_EQUALITY( cross_section, 9.2835e-1, 1e-12 );

  cross_section =
    reactions.front()->getCrossSection( 1.58489e-1 );

  TEST_FLOATING_EQUALITY( cross_section, 3.51034535975, 1e-12 );

  cross_section =
    reactions.front()->getCrossSection( 1e+5 );

  TEST_FLOATING_EQUALITY( cross_section, 3.64919e+1, 1e-12 );

  // Check the last shell's reaction properties
  TEST_EQUALITY_CONST(
                  reactions.back()->getReactionType(),
                  MonteCarlo::P3_SUBSHELL_ELECTROIONIZATION_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reactions.back()->getThresholdEnergy(), 1e-5 );

  cross_section =
    reactions.back()->getCrossSection( 1e-5 );

  TEST_FLOATING_EQUALITY( cross_section, 1.0653e+8, 1e-12 );

  cross_section =
    reactions.back()->getCrossSection( 2e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 1.123672769743e+8, 1e-12 );

  cross_section =
    reactions.back()->getCrossSection( 6.57156e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 4.612644761466e+7, 1e-12 );

  cross_section =
    reactions.back()->getCrossSection( 1e+5 );

  TEST_FLOATING_EQUALITY( cross_section, 1.82234e+5, 1e-12 );
}

//---------------------------------------------------------------------------//
// Check that a basic (dipole distribution) bremsstrahlung reaction can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createBremsstrahlungReaction_dipole )
{
  photon_distribution_function = MonteCarlo::DIPOLE_DISTRIBUTION;

  MonteCarlo::ElectroatomicReactionACEFactory::createBremsstrahlungReaction(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reaction,
                photon_distribution_function );

  // Test reaction properties
  TEST_EQUALITY_CONST( reaction->getReactionType(),
                       MonteCarlo::BREMSSTRAHLUNG_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test that the stored cross section is correct
  double cross_section =
    reaction->getCrossSection( reaction->getThresholdEnergy() );

  TEST_FLOATING_EQUALITY( cross_section, 4.8698e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 7.363889022643e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1.79008e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 8.026497035136e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1e+5 );

  TEST_EQUALITY_CONST( cross_section, 1.95417e+3 );

  // Clear the reaction
  reaction.reset();
}

//---------------------------------------------------------------------------//
/* Check that a electroatom with detailed 2BS photon angular distribution
 * data can be created
 */
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createBremsstrahlungReaction_2bs )
{
  photon_distribution_function = MonteCarlo::TWOBS_DISTRIBUTION;

  MonteCarlo::ElectroatomicReactionACEFactory::createBremsstrahlungReaction(
                *xss_data_extractor,
                energy_grid,
                grid_searcher,
                reaction,
                photon_distribution_function );

  // Test reaction properties
  TEST_EQUALITY_CONST( reaction->getReactionType(),
                       MonteCarlo::BREMSSTRAHLUNG_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test that the stored cross section is correct
  double cross_section =
    reaction->getCrossSection( reaction->getThresholdEnergy() );

  TEST_FLOATING_EQUALITY( cross_section, 4.8698e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 7.363889022643e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1.79008e-4 );

  TEST_FLOATING_EQUALITY( cross_section, 8.026497035136e+3, 1e-12 );

  cross_section = reaction->getCrossSection( 1e+5 );

  TEST_EQUALITY_CONST( cross_section, 1.95417e+3 );

  // Clear the reaction
  reaction.reset();
}



//---------------------------------------------------------------------------//
// Check that a void absorption reaction can be created
TEUCHOS_UNIT_TEST( ElectroatomicReactionACEFactory,
                   createVoidAbsorptionReaction )
{
  MonteCarlo::ElectroatomicReactionACEFactory::createVoidAbsorptionReaction(
                                                reaction );

  // Test reaction properties
  TEST_EQUALITY_CONST( reaction->getReactionType(),
                       MonteCarlo::TOTAL_ABSORPTION_ELECTROATOMIC_REACTION );
  TEST_EQUALITY_CONST( reaction->getThresholdEnergy(), 1e-5 );

  // Test that the stored cross section is correct
  double cross_section =
    reaction->getCrossSection( reaction->getThresholdEnergy() );

  TEST_EQUALITY_CONST( cross_section, 0.0);

  cross_section = reaction->getCrossSection( 1e-4 );

  TEST_EQUALITY_CONST( cross_section, 0.0);

  cross_section = reaction->getCrossSection( 1.79008e-4 );

  TEST_EQUALITY_CONST( cross_section, 0.0);

  cross_section = reaction->getCrossSection( 1e+5 );

  TEST_EQUALITY_CONST( cross_section, 0.0);

  // Clear the reaction
  reaction.reset();
}

//---------------------------------------------------------------------------//
// Custom setup
//---------------------------------------------------------------------------//
FRENSIE_CUSTOM_UNIT_TEST_SETUP_BEGIN();

std::string test_ace_file_name, test_ace_table_name;

FRENSIE_CUSTOM_UNIT_TEST_COMMAND_LINE_OPTIONS()
{
  clp().setOption( "test_ace_file",
                   &test_ace_file_name,
                   "Test ACE file name" );
  clp().setOption( "test_ace_table",
                   &test_ace_table_name,
                   "Test ACE table name" );
}

FRENSIE_CUSTOM_UNIT_TEST_INIT()
{
  // Create a file handler and data extractor
  Teuchos::RCP<Data::ACEFileHandler> ace_file_handler(
        new Data::ACEFileHandler( test_ace_file_name,
                                  test_ace_table_name,
                                  1u ) );
  xss_data_extractor.reset(
        new Data::XSSEPRDataExtractor( ace_file_handler->getTableNXSArray(),
                                       ace_file_handler->getTableJXSArray(),
                                       ace_file_handler->getTableXSSArray() ) );

  // Extract the common energy grid
  energy_grid.deepCopy( xss_data_extractor->extractElectronEnergyGrid() );

  // Create the hash-based grid searcher
  grid_searcher.reset( new Utility::StandardHashBasedGridSearcher<Teuchos::ArrayRCP<const double>,false>(
                energy_grid,
                energy_grid[0],
                energy_grid[energy_grid.size()-1],
                100 ) );
}

FRENSIE_CUSTOM_UNIT_TEST_SETUP_END();

//---------------------------------------------------------------------------//
// end tstElectroatomicReactionACEFactory.cpp
//---------------------------------------------------------------------------//

