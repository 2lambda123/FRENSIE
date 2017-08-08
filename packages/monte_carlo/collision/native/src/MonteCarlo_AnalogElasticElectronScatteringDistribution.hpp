//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_AnalogElasticElectronScatteringDistribution.hpp
//! \author Luke Kersting
//! \brief  The analog elastic electron scattering distribution base class
//!
//---------------------------------------------------------------------------//

#ifndef MONTE_CARLO_ANALOG_ELASTIC_ELECTRON_SCATTERING_DISTRIBUTION_HPP
#define MONTE_CARLO_ANALOG_ELASTIC_ELECTRON_SCATTERING_DISTRIBUTION_HPP

// FRENSIE Includes
#include "MonteCarlo_ElectronState.hpp"
#include "MonteCarlo_ParticleBank.hpp"
#include "MonteCarlo_ElectronScatteringDistribution.hpp"
#include "MonteCarlo_AdjointElectronScatteringDistribution.hpp"
#include "Utility_InterpolationPolicy.hpp"
#include "Utility_AnalogElasticOneDDistribution.hpp"
#include "Utility_InterpolatedFullyTabularTwoDDistribution.hpp"
#include "Utility_AnalogElasticTraits.hpp"

namespace MonteCarlo{

//! The scattering distribution base class
class AnalogElasticElectronScatteringDistribution : public ElectronScatteringDistribution,
    public AdjointElectronScatteringDistribution
{

public:

  //! Typedef for the this type
  typedef AnalogElasticElectronScatteringDistribution ThisType;

  //! Typedef for the Analog elastic traits
  typedef Utility::AnalogElasticTraits ElasticTraits;

  //! Typedef for the one d distributions
  typedef Utility::OneDDistribution OneDDist;

  //! Typedef for the two d distributions
  typedef Utility::FullyTabularTwoDDistribution TwoDDist;

  //! Constructor
  AnalogElasticElectronScatteringDistribution(
    const std::shared_ptr<const TwoDDist>& analog_elastic_distribution,
    const std::shared_ptr<const OneDDist>& cutoff_cross_section_ratios,
    const std::shared_ptr<const ElasticTraits>& elastic_traits,
    const bool correlated_sampling_mode_on );

  //! Destructor
  virtual ~AnalogElasticElectronScatteringDistribution()
  { /* ... */ }

  //! Evaluate the distribution
  double evaluate( const double incoming_energy,
                   const double scattering_angle_cosine ) const;

  //! Evaluate the PDF
  double evaluatePDF( const double incoming_energy,
                      const double scattering_angle_cosine ) const;

  //! Evaluate the CDF
  double evaluateCDF( const double incoming_energy,
                      const double scattering_angle_cosine ) const;

  //! Sample an outgoing energy and direction from the distribution
  void sample( const double incoming_energy,
               double& outgoing_energy,
               double& scattering_angle_cosine ) const;

  //! Sample an outgoing energy and direction and record the number of trials
  void sampleAndRecordTrials( const double incoming_energy,
                              double& outgoing_energy,
                              double& scattering_angle_cosine,
                              unsigned& trials ) const;

  //! Randomly scatter the electron
  void scatterElectron( MonteCarlo::ElectronState& electron,
                        MonteCarlo::ParticleBank& bank,
                        Data::SubshellType& shell_of_interaction ) const;

  //! Randomly scatter the adjoint electron
  void scatterAdjointElectron( MonteCarlo::AdjointElectronState& adjoint_electron,
                               MonteCarlo::ParticleBank& bank,
                               Data::SubshellType& shell_of_interaction ) const;

  //! Evaluate the distribution at the cutoff angle cosine
  double evaluateAtCutoff( const double incoming_energy ) const;

  //! Evaluate the PDF at the cutoff angle cosine
  double evaluatePDFAtCutoff( const double incoming_energy ) const;

  //! Evaluate the CDF at the cutoff angle cosine
  double evaluateCDFAtCutoff( const double incoming_energy ) const;

  //! Evaluate the screened Rutherford PDF
  double evaluateScreenedRutherfordPDF( const double incoming_energy,
                                        const double scattering_angle_cosine,
                                        const double eta ) const;

  //! Evaluate the screened Rutherford CDF
  double evaluateScreenedRutherfordCDF( const double incoming_energy,
                                        const double scattering_angle_cosine,
                                        const double eta ) const;

protected:

  //! Sample the screened Rutherford peak
  double sampleScreenedRutherfordPeak( const double incoming_energy,
                                       const double random_number,
                                       const double cutoff_ratio ) const;

  //! Sample using the 1-D Union method
  double sampleOneDUnion( const double incoming_energy ) const;

  //! Sample using the 2-D Union method
  double sampleTwoDUnion( const double incoming_energy ) const;

  //! Sample using the Simplified Union method
  double sampleSimplifiedUnion( const double incoming_energy ) const;

  //! Sample an outgoing direction from the distribution
  void sampleAndRecordTrialsImpl( const double incoming_energy,
                                  double& scattering_angle_cosine,
                                  unsigned& trials ) const;

  //! Evaluate the PDF
  double evaluateScreenedRutherfordPDF(
            const double scattering_angle_cosine,
            const double eta,
            const double cutoff_pdf,
            const double cutoff_cdf ) const;

  //! Evaluate the CDF
  double evaluateScreenedRutherfordCDF(
            const double scattering_angle_cosine,
            const double eta,
            const double cutoff_pdf,
            const double cutoff_cdf ) const;

private:

  // Cutoff elastic scattering distribution
  std::shared_ptr<const TwoDDist> d_analog_dist;

  // Cutoff elastic scattering distribution
  std::shared_ptr<const OneDDist> d_cutoff_ratios;

  // Elastic electron traits
  std::shared_ptr<const ElasticTraits> d_elastic_traits;

  // The sampling function pointer
  std::function<double ( const double, const double )> d_sample_function;

  // The sampling method pointer
  std::function<double ( const double )> d_sample_method;
};

} // end MonteCarlo namespace

#endif // end MONTE_CARLO_ANALOG_ELASTIC_ELECTRON_SCATTERING_DISTRIBUTION_HPP

//---------------------------------------------------------------------------//
// end MonteCarlo_AnalogElasticElectronScatteringDistribution.hpp
//---------------------------------------------------------------------------//
