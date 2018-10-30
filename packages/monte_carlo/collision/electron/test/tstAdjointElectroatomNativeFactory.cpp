//---------------------------------------------------------------------------//
//!
//! \file   tstAdjointElectroatomNativeFactory.cpp
//! \author Luke Kersting
//! \brief  Adjoint electroatom factory using Native data unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Boost Includes
#include <boost/unordered_set.hpp>

// FRENSIE Includes
#include "MonteCarlo_AdjointElectroatomNativeFactory.hpp"
#include "MonteCarlo_AdjointElectroatomicReactionType.hpp"
#include "MonteCarlo_AtomicRelaxationModelFactory.hpp"
#include "MonteCarlo_BremsstrahlungAngularDistributionType.hpp"
#include "MonteCarlo_CutoffElasticElectronScatteringDistribution.hpp"
#include "MonteCarlo_ElasticElectronScatteringDistributionNativeFactory.hpp"
#include "MonteCarlo_SimulationProperties.hpp"
#include "MonteCarlo_SimulationAdjointElectronProperties.hpp"
#include "Data_AdjointElectronPhotonRelaxationDataContainer.hpp"
#include "Utility_InterpolationPolicy.hpp"
#include "Utility_PhysicalConstants.hpp"
#include "Utility_UnitTestHarnessWithMain.hpp"

//---------------------------------------------------------------------------//
// Testing Variables
//---------------------------------------------------------------------------//

std::unique_ptr<Data::AdjointElectronPhotonRelaxationDataContainer> data_container;
std::string electroatom_name;
double atomic_weight;

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check that an adjoint electroatom can be created
FRENSIE_UNIT_TEST( AdjointElectroatomNativeFactory,
                   createAdjointElectroatomCore )
{
  double cutoff_angle_cosine = 1.0;
  double evaluation_tol = 1e-7;

  MonteCarlo::SimulationProperties properties;

  {
    std::vector<double> user_critical_line_energies( 1 );
    user_critical_line_energies[0] = 20.0;

    properties.setAdjointBremsstrahlungAngularDistributionFunction( MonteCarlo::DIPOLE_DISTRIBUTION );
    properties.setAdjointElasticCutoffAngleCosine( cutoff_angle_cosine );
    properties.setAdjointElectronEvaluationTolerance( evaluation_tol );
    properties.setNumberOfAdjointElectronHashGridBins( 100 );
    properties.setCriticalAdjointElectronLineEnergies( user_critical_line_energies );
  }

  std::shared_ptr<const MonteCarlo::AdjointElectroatomCore> atom_core;

  MonteCarlo::AdjointElectroatomNativeFactory::createAdjointElectroatomCore(
        *data_container,
        properties,
        atom_core );

  // Check that the grid searcher was constructed correctly
  FRENSIE_CHECK( !atom_core->getGridSearcher().isValueWithinGridBounds( 9.9e-6 ) );
  FRENSIE_CHECK( atom_core->getGridSearcher().isValueWithinGridBounds( 1e-5 ) );
  FRENSIE_CHECK( atom_core->getGridSearcher().isValueWithinGridBounds( 20.0 ) );
  FRENSIE_CHECK( !atom_core->getGridSearcher().isValueWithinGridBounds( 20.1 ) );
  FRENSIE_CHECK( atom_core->hasSharedEnergyGrid() );

  // Check that the critical line energies were constructed correctly
  FRENSIE_CHECK_EQUAL( atom_core->getCriticalLineEnergies().size(), 1 );
  FRENSIE_CHECK_EQUAL( atom_core->getCriticalLineEnergies()[0], 20.0 );

  // Check that the total forward reaction was constructed correctly
  FRENSIE_CHECK_EQUAL( atom_core->getTotalForwardReaction().getThresholdEnergy(),
                       1e-5 );
  FRENSIE_CHECK( atom_core->getTotalForwardReaction().getCrossSection( 20.0 ) > 0.0 );

  // Check that the scattering reactions were constructed correctly
  const MonteCarlo::AdjointElectroatomCore::ConstReactionMap&
    scattering_reactions = atom_core->getScatteringReactions();

  FRENSIE_CHECK_EQUAL( scattering_reactions.size(), 4 );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( !scattering_reactions.count( MonteCarlo::L1_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::ATOMIC_EXCITATION_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::COUPLED_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION ) );

  {
    std::shared_ptr<const MonteCarlo::AdjointElectroatomicReaction> reaction =
      scattering_reactions.find( MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );

    std::shared_ptr<const MonteCarlo::ElectroionizationSubshellAdjointElectroatomicReaction<Utility::LogLog> > subshell_reaction =
      std::dynamic_pointer_cast<const MonteCarlo::ElectroionizationSubshellAdjointElectroatomicReaction<Utility::LogLog> >( reaction );

    FRENSIE_CHECK_EQUAL( subshell_reaction->getCriticalLineEnergies(),
                         atom_core->getCriticalLineEnergies() );

    reaction = scattering_reactions.find( MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );

    std::shared_ptr<const MonteCarlo::BremsstrahlungAdjointElectroatomicReaction<Utility::LogLog> > brem_reaction =
      std::dynamic_pointer_cast<const MonteCarlo::BremsstrahlungAdjointElectroatomicReaction<Utility::LogLog> >( reaction );

    FRENSIE_CHECK_EQUAL( brem_reaction->getCriticalLineEnergies(),
                         atom_core->getCriticalLineEnergies() );
  }

  // Check that the absorption reactions were constructed correctly
  const MonteCarlo::AdjointElectroatomCore::ConstReactionMap&
    absorption_reactions = atom_core->getAbsorptionReactions();

  FRENSIE_CHECK_EQUAL( absorption_reactions.size(), 0 );
}

//---------------------------------------------------------------------------//
// Check that a adjoint electroatom with a higher cutoff angle can be created
FRENSIE_UNIT_TEST( AdjointElectroatomNativeFactory,
                   createAdjointElectroatomCore_cutoff )
{
  double cutoff_angle_cosine = 0.9;
  double evaluation_tol = 1e-7;

  MonteCarlo::SimulationProperties properties;

  {
    std::vector<double> user_critical_line_energies( 1 );
    user_critical_line_energies[0] = 20.0;

    properties.setAdjointBremsstrahlungAngularDistributionFunction( MonteCarlo::DIPOLE_DISTRIBUTION );
    properties.setAdjointElasticCutoffAngleCosine( cutoff_angle_cosine );
    properties.setAdjointElasticElectronDistributionMode( MonteCarlo::HYBRID_DISTRIBUTION );
    properties.setAdjointElectronEvaluationTolerance( evaluation_tol );
    properties.setNumberOfAdjointElectronHashGridBins( 100 );
    properties.setCriticalAdjointElectronLineEnergies( user_critical_line_energies );
  }

  std::shared_ptr<const MonteCarlo::AdjointElectroatomCore> atom_core;

  MonteCarlo::AdjointElectroatomNativeFactory::createAdjointElectroatomCore(
        *data_container,
        properties,
        atom_core );

  // Check that the grid searcher was constructed correctly
  FRENSIE_CHECK( !atom_core->getGridSearcher().isValueWithinGridBounds( 9.9e-6 ) );
  FRENSIE_CHECK( atom_core->getGridSearcher().isValueWithinGridBounds( 1e-5 ) );
  FRENSIE_CHECK( atom_core->getGridSearcher().isValueWithinGridBounds( 20.0 ) );
  FRENSIE_CHECK( !atom_core->getGridSearcher().isValueWithinGridBounds( 20.1 ) );
  FRENSIE_CHECK( atom_core->hasSharedEnergyGrid() );

  // Check that the critical line energies were constructed correctly
  FRENSIE_CHECK_EQUAL( atom_core->getCriticalLineEnergies().size(), 1 );
  FRENSIE_CHECK_EQUAL( atom_core->getCriticalLineEnergies()[0], 20.0 );

  // Check that the total forward reaction was constructed correctly
  FRENSIE_CHECK_EQUAL( atom_core->getTotalForwardReaction().getThresholdEnergy(),
                       1e-5 );
  FRENSIE_CHECK( atom_core->getTotalForwardReaction().getCrossSection( 20.0 ) > 0.0 );

  // Check that the scattering reactions were constructed correctly
  const MonteCarlo::AdjointElectroatomCore::ConstReactionMap&
    scattering_reactions = atom_core->getScatteringReactions();

  FRENSIE_CHECK_EQUAL( scattering_reactions.size(), 4 );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::ATOMIC_EXCITATION_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION ) );
  FRENSIE_CHECK( scattering_reactions.count( MonteCarlo::HYBRID_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION ) );

  {
    std::shared_ptr<const MonteCarlo::AdjointElectroatomicReaction> reaction =
      scattering_reactions.find( MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );

    std::shared_ptr<const MonteCarlo::BremsstrahlungAdjointElectroatomicReaction<Utility::LogLog> > brem_reaction =
      std::dynamic_pointer_cast<const MonteCarlo::BremsstrahlungAdjointElectroatomicReaction<Utility::LogLog> >( reaction );

    FRENSIE_CHECK_EQUAL( brem_reaction->getCriticalLineEnergies(),
                         atom_core->getCriticalLineEnergies() );

    reaction = scattering_reactions.find( MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );

    std::shared_ptr<const MonteCarlo::ElectroionizationSubshellAdjointElectroatomicReaction<Utility::LogLog> > k_reaction =
      std::dynamic_pointer_cast<const MonteCarlo::ElectroionizationSubshellAdjointElectroatomicReaction<Utility::LogLog> >( reaction );

    FRENSIE_CHECK_EQUAL( k_reaction->getCriticalLineEnergies(),
                         atom_core->getCriticalLineEnergies() );

    reaction = scattering_reactions.find( MonteCarlo::ATOMIC_EXCITATION_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );

    reaction = scattering_reactions.find( MonteCarlo::HYBRID_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION )->second;

    FRENSIE_CHECK_EQUAL( reaction->getThresholdEnergy(), 1e-5 );
    FRENSIE_CHECK_EQUAL( reaction->getMaxEnergy(), 20.0 );
  }

  // Check that the absorption reactions were constructed correctly
  const MonteCarlo::AdjointElectroatomCore::ConstReactionMap&
    absorption_reactions = atom_core->getAbsorptionReactions();

  FRENSIE_CHECK_EQUAL( absorption_reactions.size(), 0 );
}

//---------------------------------------------------------------------------//
// Check that an adjoint electroatom can be created
FRENSIE_UNIT_TEST( AdjointElectroatomNativeFactory,
                   createAdjointElectroatom )
{
  double cutoff_angle_cosine = 1.0;
  double evaluation_tol = 1e-7;

  MonteCarlo::SimulationAdjointElectronProperties properties;

  {
    std::vector<double> user_critical_line_energies( 1 );
    user_critical_line_energies[0] = 20.0;

    properties.setAdjointBremsstrahlungAngularDistributionFunction( MonteCarlo::DIPOLE_DISTRIBUTION );
    properties.setAdjointElasticCutoffAngleCosine( cutoff_angle_cosine );
    properties.setAdjointElectronEvaluationTolerance( evaluation_tol );
    properties.setNumberOfAdjointElectronHashGridBins( 100 );
    properties.setCriticalAdjointElectronLineEnergies( user_critical_line_energies );
  }

  std::shared_ptr<const MonteCarlo::AdjointElectroatom> atom;

  MonteCarlo::AdjointElectroatomNativeFactory::createAdjointElectroatom(
        *data_container,
        electroatom_name,
        atomic_weight,
        properties,
        atom );


  // Test the adjoint electroatom properties
  FRENSIE_CHECK_EQUAL( atom->getAtomName(), "H-Native" );
  FRENSIE_CHECK_EQUAL( atom->getAtomicNumber(), 1 );
  FRENSIE_CHECK_FLOATING_EQUALITY( atom->getAtomicWeight(), 1.00794, 1e-12 );

  // Test that the total cross section can be returned
  double energy = 1e-5;
  double cross_section = atom->getTotalCrossSection( energy );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.665353570771184540e+10, 1e-12 );

  energy = 1e-3;
  cross_section = atom->getTotalCrossSection( energy );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.926416226694965735e+07, 1e-12 );

  energy = 20.0;
  cross_section = atom->getTotalCrossSection( energy );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.785097370823987876e+05, 1e-12 );


  // Test that the absorption cross section can be returned
  cross_section = atom->getAbsorptionCrossSection( 1.0E-02 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 2.0E-03 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 4.0E-04 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 9.0E-05 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  MonteCarlo::AdjointElectroatomicReactionType reaction;

  // Test that the atomic excitation cross section can be returned
  reaction = MonteCarlo::ATOMIC_EXCITATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 6.1243057898416743e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.050234737111856416e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 8.1829299836129925e+04, 1e-12 );


  // Test that the bremsstrahlung cross section can be returned
  reaction = MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.420906922056859401e+01, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.557600066977331110e+01, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.873816755338521323e-01, 1e-12 );

  // Test that the coupled elastic cross section can be returned
  reaction = MonteCarlo::COUPLED_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.74896e+08, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.804290802376420237e+06, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.3022122514987041e+04, 1e-12 );


  // Test that the decoupled elastic cross section can be returned
  reaction = MonteCarlo::DECOUPLED_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that there is no hybrid elastic cross section
  reaction = MonteCarlo::HYBRID_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that there is no total electroionization
  reaction = MonteCarlo::TOTAL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that the K subshell electroionization cross section can be returned
  reaction = MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.631739660560436249e+10, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.595750851745400578e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 8.365802734960628732e+04, 1e-12 );


  // Test that the L1 subshell electroionization cross section can be returned
  reaction = MonteCarlo::L1_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );
}

//---------------------------------------------------------------------------//
// Check that a adjoint electroatom with a higher cutoff angle can be created
FRENSIE_UNIT_TEST( AdjointElectroatomNativeFactory,
                   createAdjointElectroatom_cutoff )
{
  double cutoff_angle_cosine = 0.9;
  double evaluation_tol = 1e-7;

  MonteCarlo::SimulationAdjointElectronProperties properties;
  {
    std::vector<double> user_critical_line_energies( 1 );
    user_critical_line_energies[0] = 20.0;

    properties.setAdjointBremsstrahlungAngularDistributionFunction( MonteCarlo::DIPOLE_DISTRIBUTION );
    properties.setAdjointElasticCutoffAngleCosine( cutoff_angle_cosine );
    properties.setAdjointElasticElectronDistributionMode( MonteCarlo::HYBRID_DISTRIBUTION );
    properties.setAdjointElectronEvaluationTolerance( evaluation_tol );
    properties.setNumberOfAdjointElectronHashGridBins( 100 );
    properties.setCriticalAdjointElectronLineEnergies( user_critical_line_energies );
  }

  std::shared_ptr<const MonteCarlo::AdjointElectroatom> atom;

  MonteCarlo::AdjointElectroatomNativeFactory::createAdjointElectroatom(
        *data_container,
        electroatom_name,
        atomic_weight,
        properties,
        atom );

  // Test the adjoint electroatom properties
  FRENSIE_CHECK_EQUAL( atom->getAtomName(), "H-Native" );
  FRENSIE_CHECK_EQUAL( atom->getAtomicNumber(), 1 );
  FRENSIE_CHECK_FLOATING_EQUALITY( atom->getAtomicWeight(), 1.00794, 1e-12 );

  // Test that the total cross section can be returned
  double cross_section = atom->getTotalCrossSection( 1e-5);
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.665200864439084625e+10, 1e-12 );

  cross_section = atom->getTotalCrossSection( 1e-3 );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.841091382164378092e+07, 1e-12 );

  cross_section = atom->getTotalCrossSection( 20.0 );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.654896684976172401e+05, 1e-12 );


  // Test that the absorption cross section can be returned
  cross_section = atom->getAbsorptionCrossSection( 1.0E-02 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 2.0E-03 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 4.0E-04 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getAbsorptionCrossSection( 9.0E-05 );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  MonteCarlo::AdjointElectroatomicReactionType reaction;

  // Test that the atomic excitation cross section can be returned
  reaction = MonteCarlo::ATOMIC_EXCITATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 6.1243057898416743e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.050234737111856416e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 8.1829299836129925e+04, 1e-12 );


  // Test that the bremsstrahlung cross section can be returned
  reaction = MonteCarlo::BREMSSTRAHLUNG_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.420906922056859401e+01, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.557600066977331110e+01, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.873816755338521323e-01, 1e-12 );


  // Test that the coupled elastic cross section can be returned
  reaction = MonteCarlo::DECOUPLED_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that the coupled elastic cross section can be returned
  reaction = MonteCarlo::COUPLED_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that there is no hybrid elastic cross section
  reaction = MonteCarlo::HYBRID_ELASTIC_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.7336893667900169e+08, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.951042357070542872e+06, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 2.053930205497564110, 1e-12 );


  // Test that there is no total electroionization
  reaction = MonteCarlo::TOTAL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );


  // Test that the K subshell electroionization cross section can be returned
  reaction = MonteCarlo::K_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 4.631739660560436249e+10, 1e-12 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 1.595750851745400578e+07, 1e-12 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_FLOATING_EQUALITY( cross_section, 8.365802734960628732e+04, 1e-12 );


  // Test that the L1 subshell electroionization cross section can be returned
  reaction = MonteCarlo::L1_SUBSHELL_ELECTROIONIZATION_ADJOINT_ELECTROATOMIC_REACTION;
  cross_section = atom->getReactionCrossSection( 1e-5, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 1e-3, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );

  cross_section = atom->getReactionCrossSection( 20.0, reaction );
  FRENSIE_CHECK_EQUAL( cross_section, 0.0 );
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
  electroatom_name = "H-Native";
  atomic_weight = 1.00794;

  {
    // Create the native data file container
    data_container.reset( new Data::AdjointElectronPhotonRelaxationDataContainer(
                             test_native_file_name ) );
  }
}

FRENSIE_CUSTOM_UNIT_TEST_SETUP_END();

//---------------------------------------------------------------------------//
// end tstAdjointElectroatomNativeFactory.cpp
//---------------------------------------------------------------------------//
