//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_ElectroatomicReactionNativeFactoryHelpers_def.hpp
//! \author Luke Kersting
//! \brief  The electroatomic reaction native factory helpers definitions
//!
//---------------------------------------------------------------------------//

namespace MonteCarlo{

//----------------------------------------------------------------------------//
//      ****ELASTIC REACTIONS****
//----------------------------------------------------------------------------//

//! Create an coupled elastic scattering electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createCoupledElasticReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const std::string sampling_method,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Convert the sampling method
  CoupledElasticSamplingMethod method =
    convertStringToCoupledElasticSamplingMethod( sampling_method );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createCoupledElasticReaction<TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      method,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//! Create a decoupled elastic scattering electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createDecoupledElasticReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createDecoupledElasticReaction<TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//! Create a hybrid elastic scattering electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createHybridElasticReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const double cutoff_angle_cosine,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createHybridElasticReaction<TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      cutoff_angle_cosine,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//! Create an cutoff elastic scattering electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createCutoffElasticReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const double cutoff_angle_cosine,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createCutoffElasticReaction<TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      cutoff_angle_cosine,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//! Create the moment preserving elastic scattering electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createMomentPreservingElasticReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const double cutoff_angle_cosine,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createMomentPreservingElasticReaction<TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      cutoff_angle_cosine,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//----------------------------------------------------------------------------//
//      ****ELECTROIONIZATION SUBSHELL REACTIONS****
//----------------------------------------------------------------------------//

//! Create the subshell electroionization electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createSubshellElectroionizationReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const unsigned subshell,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createSubshellElectroionizationReaction<MonteCarlo::ElectroatomicReaction,TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      subshell,
      reaction,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

//! Create the subshell electroionization electroatomic reactions
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::vector<std::shared_ptr<ElectroatomicReaction> >
createSubshellElectroionizationReactions(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid( new std::vector<double> );
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );

  // Create the reaction
  std::vector<std::shared_ptr<ElectroatomicReaction> > reactions;
  ElectroatomicReactionNativeFactory::createSubshellElectroionizationReactions<MonteCarlo::ElectroatomicReaction,TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reactions,
      evaluation_tol );

  return reactions;
}

//----------------------------------------------------------------------------//
//      ****BREMSSTRAHLUNG REACTIONS****
//----------------------------------------------------------------------------//

//! Create the bremsstrahlung electroatomic reaction
template<typename TwoDInterpPolicy,typename TwoDSamplePolicy>
std::shared_ptr<ElectroatomicReaction>
createBremsstrahlungReaction(
    const Data::ElectronPhotonRelaxationDataContainer& raw_electroatom_data,
    const bool dipole_distribution_mode_on,
    const double evaluation_tol )
{
  // Extract the common energy grid
  std::shared_ptr<std::vector<double> > energy_grid;
  energy_grid->assign( raw_electroatom_data.getElectronEnergyGrid().begin(),
                       raw_electroatom_data.getElectronEnergyGrid().end() );

  // Construct the hash-based grid searcher
  std::shared_ptr<const Utility::HashBasedGridSearcher> grid_searcher(
     new Utility::StandardHashBasedGridSearcher<std::vector<double>, false>(
                              energy_grid,
                              energy_grid.size()/10 ) );


  BremsstrahlungAngularDistributionType photon_distribution_function;

  if ( dipole_distribution_mode_on )
  {
    photon_distribution_function = DIPOLE_DISTRIBUTION;
  }
  else
  {
    photon_distribution_function = TWOBS_DISTRIBUTION;
  }

  // Create the reaction
  std::shared_ptr<ElectroatomicReaction> reaction;
  ElectroatomicReactionNativeFactory::createBremsstrahlungReaction<MonteCarlo::ElectroatomicReaction,TwoDInterpPolicy,TwoDSamplePolicy>(
      raw_electroatom_data,
      energy_grid,
      grid_searcher,
      reaction,
      photon_distribution_function,
      evaluation_tol );

  // Make sure the reaction was created correctly
  testPostcondition( reaction.use_count() > 0 );

  return reaction;
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_ElectroatomicReactionNativeFactoryHelpers_def.hpp
//---------------------------------------------------------------------------//
