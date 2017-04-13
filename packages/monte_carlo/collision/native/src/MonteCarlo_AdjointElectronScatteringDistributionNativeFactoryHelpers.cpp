//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_AdjointElectronScatteringDistributionNativeFactoryHelpers.cpp
//! \author Luke Kersting
//! \brief  The adjoint electron scattering distribution native factory helpers definitions
//!
//---------------------------------------------------------------------------//

// FRENSIE Includes
#include "MonteCarlo_AdjointElectronScatteringDistributionNativeFactoryHelpers.hpp"

namespace MonteCarlo{

//----------------------------------------------------------------------------//
//      ****ELASTIC DISTRIBUTIONS****
//----------------------------------------------------------------------------//

// Create the analog elastic distribution ( combined Cutoff and Screened Rutherford )
std::shared_ptr<const AnalogElasticElectronScatteringDistribution> createAnalogElasticDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const double evaluation_tol )
{
  std::shared_ptr<const MonteCarlo::AnalogElasticElectronScatteringDistribution>
    distribution;

  if ( linlinlog_interpolation_mode_on )
  {
    ElasticElectronScatteringDistributionNativeFactory::createAnalogElasticDistribution<Utility::LinLinLog>(
        distribution,
        data_container,
        correlated_sampling_mode_on,
        evaluation_tol );
  }
  else
  {
    ElasticElectronScatteringDistributionNativeFactory::createAnalogElasticDistribution<Utility::LinLinLin>(
        distribution,
        data_container,
        correlated_sampling_mode_on,
        evaluation_tol );
  }

  return distribution;
}

//! Create the hybrid elastic distribution ( combined Cutoff and Moment Preserving )
std::shared_ptr<const HybridElasticElectronScatteringDistribution> createHybridElasticDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const double cutoff_angle_cosine,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const double evaluation_tol )
{
  // Extract the common energy grid used for this atom
  Teuchos::ArrayRCP<double> energy_grid;
  energy_grid.assign( data_container.getAdjointElectronEnergyGrid().begin(),
                      data_container.getAdjointElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher for this atom
  Teuchos::RCP<Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<Teuchos::ArrayRCP<const double>, false>(
                             energy_grid,
                             100 ) );

  // Cutoff elastic cross section
  Teuchos::ArrayRCP<double> cutoff_cross_section;
  cutoff_cross_section.assign(
    data_container.getAdjointCutoffElasticCrossSection().begin(),
    data_container.getAdjointCutoffElasticCrossSection().end() );

  // Cutoff elastic cross section threshold energy bin index
  unsigned cutoff_threshold_energy_index =
    data_container.getAdjointCutoffElasticCrossSectionThresholdEnergyIndex();

  // Moment preserving elastic cross section
  Teuchos::ArrayRCP<double> mp_cross_section;
  mp_cross_section.assign(
    data_container.getAdjointMomentPreservingCrossSection().begin(),
    data_container.getAdjointMomentPreservingCrossSection().end() );

  // Moment preserving elastic cross section threshold energy bin index
  unsigned mp_threshold_energy_index =
    data_container.getAdjointMomentPreservingCrossSectionThresholdEnergyIndex();

  // Create the hybrid elastic scattering distribution
  std::shared_ptr<const HybridElasticElectronScatteringDistribution>
    distribution;

  if ( linlinlog_interpolation_mode_on )
  {
    ElasticElectronScatteringDistributionNativeFactory::createHybridElasticDistribution<Utility::LinLinLog>(
        distribution,
        grid_searcher,
        energy_grid,
        cutoff_cross_section,
        mp_cross_section,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }
  else
  {
    ElasticElectronScatteringDistributionNativeFactory::createHybridElasticDistribution<Utility::LinLinLin>(
        distribution,
        grid_searcher,
        energy_grid,
        cutoff_cross_section,
        mp_cross_section,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }

  return distribution;
}

//! Create a cutoff elastic distribution
std::shared_ptr<const CutoffElasticElectronScatteringDistribution> createCutoffElasticDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const double cutoff_angle_cosine,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const double evaluation_tol )
{
  std::shared_ptr<const CutoffElasticElectronScatteringDistribution>
    distribution;

  if ( linlinlog_interpolation_mode_on )
  {
    ElasticElectronScatteringDistributionNativeFactory::createCutoffElasticDistribution<Utility::LinLinLog>(
        distribution,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }
  else
  {
    ElasticElectronScatteringDistributionNativeFactory::createCutoffElasticDistribution<Utility::LinLinLin>(
        distribution,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }

  return distribution;
}

//! Create a moment preserving elastic distribution
std::shared_ptr<const MomentPreservingElasticElectronScatteringDistribution> createMomentPreservingElasticDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const double cutoff_angle_cosine,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const double evaluation_tol )
{
  std::shared_ptr<const MomentPreservingElasticElectronScatteringDistribution>
    distribution;

  if ( linlinlog_interpolation_mode_on )
  {
    ElasticElectronScatteringDistributionNativeFactory::createMomentPreservingElasticDistribution<Utility::LinLinLog>(
        distribution,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }
  else
  {
    ElasticElectronScatteringDistributionNativeFactory::createMomentPreservingElasticDistribution<Utility::LinLinLin>(
        distribution,
        data_container,
        cutoff_angle_cosine,
        correlated_sampling_mode_on,
        evaluation_tol );
  }

  return distribution;
}

//----------------------------------------------------------------------------//
//      ****BREMSSTRAHLUNG DISTRIBUTIONS****
//----------------------------------------------------------------------------//

// Create an adjoint bremsstrahlung distribution
std::shared_ptr<const BremsstrahlungAdjointElectronScatteringDistribution> createBremsstrahlungDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const bool unit_based_interpolation_mode_on,
    const double evaluation_tol )
{
  std::shared_ptr<const BremsstrahlungAdjointElectronScatteringDistribution>
    distribution;

  if (linlinlog_interpolation_mode_on)
  {
  BremsstrahlungAdjointElectronScatteringDistributionNativeFactory::createBremsstrahlungDistribution<Utility::LinLinLog>(
    data_container,
    data_container.getAdjointElectronEnergyGrid(),
    distribution,
    correlated_sampling_mode_on,
    unit_based_interpolation_mode_on,
    evaluation_tol );
  }
  else
  {
  BremsstrahlungAdjointElectronScatteringDistributionNativeFactory::createBremsstrahlungDistribution<Utility::LinLinLin>(
    data_container,
    data_container.getAdjointElectronEnergyGrid(),
    distribution,
    correlated_sampling_mode_on,
    unit_based_interpolation_mode_on,
    evaluation_tol );
  }

  return distribution;
}

//----------------------------------------------------------------------------//
//      ****ELECTROIONIZATION SUBSHELL DISTRIBUTIONS****
//----------------------------------------------------------------------------//

//! Create a electroionization subshell distribution
std::shared_ptr<const ElectroionizationSubshellAdjointElectronScatteringDistribution> createElectroionizationSubshellDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container,
    const unsigned subshell,
    const double binding_energy,
    const bool linlinlog_interpolation_mode_on,
    const bool correlated_sampling_mode_on,
    const bool unit_based_interpolation_mode_on,
    const double evaluation_tol )
{
  std::shared_ptr<const ElectroionizationSubshellAdjointElectronScatteringDistribution>
    distribution;

  if (linlinlog_interpolation_mode_on)
  {
    ElectroionizationSubshellAdjointElectronScatteringDistributionNativeFactory::createElectroionizationSubshellDistribution<Utility::LinLinLog>(
    data_container,
    subshell,
    binding_energy,
    distribution,
    correlated_sampling_mode_on,
    unit_based_interpolation_mode_on,
    evaluation_tol );
  }
  else
  {
    ElectroionizationSubshellAdjointElectronScatteringDistributionNativeFactory::createElectroionizationSubshellDistribution<Utility::LinLinLin>(
    data_container,
    subshell,
    binding_energy,
    distribution,
    correlated_sampling_mode_on,
    unit_based_interpolation_mode_on,
    evaluation_tol );
  }

  return distribution;
}

//----------------------------------------------------------------------------//
//      ****ATOMIC EXCITATION DISTRIBUTION****
//----------------------------------------------------------------------------//

//! Create a atomic excitation distribution
std::shared_ptr<const AtomicExcitationAdjointElectronScatteringDistribution> createAtomicExcitationDistribution(
    const Data::AdjointElectronPhotonRelaxationDataContainer& data_container )
{
  std::shared_ptr<const AtomicExcitationAdjointElectronScatteringDistribution>
    distribution;

  AtomicExcitationAdjointElectronScatteringDistributionNativeFactory::createAtomicExcitationDistribution(
    data_container, distribution );

  return distribution;
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_AdjointElectronScatteringDistributionNativeFactoryHelpers.cpp
//---------------------------------------------------------------------------//
