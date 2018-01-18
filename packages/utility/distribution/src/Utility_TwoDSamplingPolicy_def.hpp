//---------------------------------------------------------------------------//
//!
//! \file   Utility_TwoDSamplingPolicy_def.hpp
//! \author Luke Kersting
//! \brief  Policy struct declarations for sampling on 2D grids
//!
//---------------------------------------------------------------------------//

#ifndef UTILITY_TWO_D_SAMPLING_POLICY_DEF_HPP
#define UTILITY_TWO_D_SAMPLING_POLICY_DEF_HPP

// Std Lib Includes
#include <functional>

// FRENSIE Includes
#include "Utility_SortAlgorithms.hpp"
#include "Utility_SearchAlgorithms.hpp"
#include "Utility_ContractException.hpp"
#include "Utility_RandomNumberGenerator.hpp"

namespace Utility{

template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto Direct::calculateLowerBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  if ( x_indep_value == lower_bin_boundary->first )
  {
    return lower_bin_boundary->second->getLowerBoundOfIndepVar();
  }
  else if ( x_indep_value == upper_bin_boundary->first )
  {
    return upper_bin_boundary->second->getLowerBoundOfIndepVar();
  }
  else
  {
    return std::min( lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                     upper_bin_boundary->second->getLowerBoundOfIndepVar() );
  }
}

// Calculate the Y independent upper bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto Direct::calculateUpperBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  if ( x_indep_value == lower_bin_boundary->first )
  {
    return lower_bin_boundary->second->getUpperBoundOfIndepVar();
  }
  else if ( x_indep_value == upper_bin_boundary->first )
  {
    return upper_bin_boundary->second->getUpperBoundOfIndepVar();
  }
  else
  {
    return std::max( lower_bin_boundary->second->getUpperBoundOfIndepVar(),
                     upper_bin_boundary->second->getUpperBoundOfIndepVar() );
  }
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
//! \details The EvaluationMethod must evalute using a Cosine variable.
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType Direct::evaluatePDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Direct::evaluatePDF<TwoDInterpPolicy, BaseOneDDistributionType,
                             XIndepType, YIndepType,
                             ReturnType, YZIterator, EvaluationMethod,
                             YBoundsFunctor>(
                                    x_indep_value,
                                    y_indep_value,
                                    min_y_indep_functor,
                                    max_y_indep_functor,
                                    evaluate,
                                    lower_bin_boundary,
                                    upper_bin_boundary,
                                    fuzzy_boundary_tol,
                                    rel_error_tol,
                                    error_tol,
                                    max_number_of_iterations );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details Direct interpolation is used to evaluate the probability density
 *  function.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType Direct::evaluatePDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else
  {
    // Get the evaluation at the lower and upper bin boundaries
    ReturnType min_eval_0 = ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
    ReturnType min_eval_1 = ((*upper_bin_boundary->second).*evaluate)(y_indep_value);

    if ( min_eval_0 == min_eval_1 )
      return min_eval_0;
    else
    {
      // Return the interpolated evaluation
      return TwoDInterpPolicy::ZXInterpPolicy::interpolate(
              lower_bin_boundary->first,
              upper_bin_boundary->first,
              x_indep_value,
              min_eval_0,
              min_eval_1 );
    }
  }
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
//! \details The EvaluationMethod must evalute using a Cosine variable.
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double Direct::evaluateCDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Direct::evaluateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                  XIndepType, YIndepType, YZIterator,
                                  EvaluationMethod, YBoundsFunctor>(
                                              x_indep_value,
                                              y_indep_value,
                                              min_y_indep_functor,
                                              max_y_indep_functor,
                                              evaluate,
                                              lower_bin_boundary,
                                              upper_bin_boundary,
                                              fuzzy_boundary_tol,
                                              rel_error_tol,
                                              error_tol,
                                              max_number_of_iterations );
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details The CDF is evaluated from a direct interpolation of the PDF.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double Direct::evaluateCDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Direct::evaluatePDF<TwoDInterpPolicy, BaseOneDDistributionType,
                             XIndepType, YIndepType,
                             double, YZIterator, EvaluationMethod,
                             YBoundsFunctor>(
                                    x_indep_value,
                                    y_indep_value,
                                    min_y_indep_functor,
                                    max_y_indep_functor,
                                    evaluate,
                                    lower_bin_boundary,
                                    upper_bin_boundary,
                                    fuzzy_boundary_tol,
                                    rel_error_tol,
                                    error_tol,
                                    max_number_of_iterations );
}

// Sample between bin boundaries using the desired sampling functor
/*! \details A direct statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the direct evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Direct::sample(
          const SampleFunctor& sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return Direct::sampleDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            min_y_indep_functor,
            max_y_indep_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

//! Sample between bin boundaries using the desired sampling functor
/* \details The SampleFunctor must sample a Cosine variable.
 * A direct statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the direct evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType Direct::sampleCos( const SampleFunctor& sample_functor,
                                   const XIndepType& x_indep_value,
                                   const YZIterator& lower_bin_boundary,
                                   const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return Direct::sampleCosDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

// Sample between bin boundaries using the desired subrange sampling functor
/* \details The SampleFunctor must be of the form that it takes a subrange
 * sampling function from a OneDDistribution and the max indep variable.
 * A direct statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the direct evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Direct::sampleInSubrange(
          const SampleFunctor& subrange_sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary,
          const YIndepType& subrange_max_y_indep_value )
{
  YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );
  YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );

  // Make sure the y subrange limit is valid
  testPrecondition( subrange_max_y_indep_value >= min_y_indep_value )
  // Make sure the y limits is valid
  testPrecondition( max_y_indep_value >= min_y_indep_value );

  // Get the sampled bin boundary
  YZIterator sampled_bin_boundary =
    Direct::sampleBinBoundary<TwoDInterpPolicy,XIndepType,YZIterator>(
                              x_indep_value,
                              lower_bin_boundary,
                              upper_bin_boundary );

  // Make sure subrange max value is below the max value of the sampled bin
  YIndepType max_y_indep_value_bin_bound =
        std::min( subrange_max_y_indep_value,
                  sampled_bin_boundary->second->getUpperBoundOfIndepVar() );

  // Sample in the bin's subrange
  YIndepType raw_sample =
    subrange_sample_functor( *sampled_bin_boundary->second,
                              max_y_indep_value_bin_bound );

  return raw_sample;
}

// Sample between bin boundaries using the desired sampling functor
/*! \details A direct statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the direct evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Direct::sampleDetailed(
            const SampleFunctor& sample_functor,
            const YBoundsFunctor& min_y_indep_functor,
            const YBoundsFunctor& max_y_indep_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  // Get the sampled bin boundary
  sampled_bin_boundary =
    Direct::sampleBinBoundary<TwoDInterpPolicy,XIndepType,YZIterator>(
                            x_indep_value,
                            lower_bin_boundary,
                            upper_bin_boundary );

  // Create the raw sample
  raw_sample = sample_functor( *sampled_bin_boundary->second );

  // Note: This is a stochastic procedure. The intermediate distribution that
  //       has been sampled is not the true distribution. The expected value
  //       of a sample will be a sample from the true distribution though.
  return raw_sample;
}

// Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType Direct::sampleCosDetailed(
            const SampleFunctor& sample_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{

  std::function<YIndepType(const XIndepType)> dummy_functor =
    [](XIndepType x){return QuantityTraits<YIndepType>::zero();};

  return Direct::sampleDetailed<TwoDInterpPolicy, XIndepType, YIndepType, YZIterator, SampleFunctor>(
              sample_functor,
              dummy_functor,
              dummy_functor,
              x_indep_value,
              lower_bin_boundary,
              upper_bin_boundary,
              sampled_bin_boundary,
              raw_sample );
}

// Sample the bin boundary that will be used for statistical sampling
/*! \details This method will throw an exception if the primary independent
 * value is outside of the primary grid limits and the primary grid has not
 * been extended.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YZIterator>
YZIterator Direct::sampleBinBoundary(
    const XIndepType& x_indep_value,
    const YZIterator& lower_bin_boundary,
    const YZIterator& upper_bin_boundary )
{
  // Calculate the interpolation fraction
  double interpolation_fraction;

  {
    const double processed_lower_bin_boundary =
      TwoDInterpPolicy::processFirstIndepVar( lower_bin_boundary->first );

    interpolation_fraction =
      (TwoDInterpPolicy::processFirstIndepVar( x_indep_value ) -
        processed_lower_bin_boundary)/
      (TwoDInterpPolicy::processFirstIndepVar( upper_bin_boundary->first ) -
        processed_lower_bin_boundary );
  }

  // Sample to determine the distribution that will be used
  double random_number =
    Utility::RandomNumberGenerator::getRandomNumber<double>();

  if( random_number < interpolation_fraction )
    return upper_bin_boundary;
  else
    return lower_bin_boundary;
}

// The name of the policy
inline const std::string Direct::name()
{
  return "Direct";
}

// Calculate the Y independent lower bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto UnitBase::calculateLowerBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return TwoDInterpPolicy::calculateIntermediateGridLimit(
                       lower_bin_boundary->first,
                       upper_bin_boundary->first,
                       x_indep_value,
                       lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                       upper_bin_boundary->second->getLowerBoundOfIndepVar() );
}

// Calculate the Y independent upper bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto UnitBase::calculateUpperBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return TwoDInterpPolicy::calculateIntermediateGridLimit(
                       lower_bin_boundary->first,
                       upper_bin_boundary->first,
                       x_indep_value,
                       lower_bin_boundary->second->getUpperBoundOfIndepVar(),
                       upper_bin_boundary->second->getUpperBoundOfIndepVar() );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
//! \details The EvaluationMethod must evalute using a Cosine variable.
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType UnitBase::evaluatePDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Direct::evaluatePDF<TwoDInterpPolicy, BaseOneDDistributionType,
                             XIndepType, YIndepType,
                             ReturnType, YZIterator, EvaluationMethod,
                             YBoundsFunctor>(
                                    x_indep_value,
                                    y_indep_value,
                                    min_y_indep_functor,
                                    max_y_indep_functor,
                                    evaluate,
                                    lower_bin_boundary,
                                    upper_bin_boundary,
                                    fuzzy_boundary_tol,
                                    rel_error_tol,
                                    error_tol,
                                    max_number_of_iterations );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details Unit-base interpolation is used to evaluate the probability density
 *  function.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType UnitBase::evaluatePDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Create the grid evaluation functors
  std::function<ReturnType(const YIndepType)>
    evaluate_grid_0_functor =
    std::bind<ReturnType>( evaluate,
                           std::cref( *lower_bin_boundary->second ),
                           std::placeholders::_1 );

  std::function<ReturnType(const YIndepType)>
    evaluate_grid_1_functor =
    std::bind<ReturnType>( evaluate,
                           std::cref( *upper_bin_boundary->second ),
                           std::placeholders::_1 );

  return TwoDInterpPolicy::interpolateUnitBase(
                        lower_bin_boundary->first,
                        upper_bin_boundary->first,
                        x_indep_value,
                        y_indep_value,
                        lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                        lower_bin_boundary->second->getUpperBoundOfIndepVar(),
                        upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                        upper_bin_boundary->second->getUpperBoundOfIndepVar(),
                        evaluate_grid_0_functor,
                        evaluate_grid_1_functor,
                        QuantityTraits<ReturnType>::zero(),
                        QuantityTraits<ReturnType>::zero(),
                        fuzzy_boundary_tol );
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
//! \details The EvaluationMethod must evalute using a Cosine variable.
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double UnitBase::evaluateCDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Direct::evaluateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                  XIndepType, YIndepType, YZIterator,
                                  EvaluationMethod, YBoundsFunctor>(
                                              x_indep_value,
                                              y_indep_value,
                                              min_y_indep_functor,
                                              max_y_indep_functor,
                                              evaluate,
                                              lower_bin_boundary,
                                              upper_bin_boundary,
                                              fuzzy_boundary_tol,
                                              rel_error_tol,
                                              error_tol,
                                              max_number_of_iterations );
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details The CDF is evaluated from a unit-base interpolation of the PDF.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double UnitBase::evaluateCDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else
  {
    // Calculate the lower bin grid length
    typename QuantityTraits<YIndepType>::RawType
      grid_length_0 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
            lower_bin_boundary->second->getLowerBoundOfIndepVar(),
            lower_bin_boundary->second->getUpperBoundOfIndepVar() );

    // Calculate the upper bin grid length
    typename QuantityTraits<YIndepType>::RawType
      grid_length_1 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
            upper_bin_boundary->second->getLowerBoundOfIndepVar(),
            upper_bin_boundary->second->getUpperBoundOfIndepVar() );

    // Calculate the intermediate grid limits
    YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );
    YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );

    typename QuantityTraits<YIndepType>::RawType
      intermediate_grid_length =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                min_y_indep_value,
                max_y_indep_value );

    // Calculate the tol to the intermediate grid limits
    YIndepType min_y_indep_value_with_tol =
      TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyLowerBound(
                                                      min_y_indep_value,
                                                      fuzzy_boundary_tol );
    YIndepType max_y_indep_value_with_tol =
      TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyUpperBound(
                                                      max_y_indep_value,
                                                      fuzzy_boundary_tol );

    // Check if the secondary indep value is in the intermediate grid
    if( y_indep_value >= min_y_indep_value_with_tol &&
        y_indep_value <= max_y_indep_value_with_tol )
    {
      // Calculate the unit base independent variable
      typename QuantityTraits<YIndepType>::RawType eta;

      if( y_indep_value > min_y_indep_value && y_indep_value < max_y_indep_value )
      {
        eta = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                                          y_indep_value,
                                          min_y_indep_value,
                                          intermediate_grid_length );
      }
      else if( y_indep_value <= min_y_indep_value &&
               y_indep_value >= min_y_indep_value_with_tol )
        eta = 0.0;
      else // y_indep_value >= max_y_indep_value && y_indep_value <= max_y_indep_value_with_tol
        eta = 1.0;

      // Scale eta to the lower bin grid length
      YIndepType y_indep_value_0 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                          eta,
                          lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                          grid_length_0 );

      // Scale eta to the upper bin grid length
      YIndepType y_indep_value_1 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                          eta,
                          upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                          grid_length_1 );

      // Get the evaluation at the lower and upper bin boundaries
      double min_eval_0 = ((*lower_bin_boundary->second).*evaluate)(y_indep_value_0);
      double min_eval_1 = ((*upper_bin_boundary->second).*evaluate)(y_indep_value_1);

      if ( min_eval_0 == min_eval_1 )
        return min_eval_0;
      else
      {
        // Return the interpolated evaluation
        return TwoDInterpPolicy::ZXInterpPolicy::interpolate(
                lower_bin_boundary->first,
                upper_bin_boundary->first,
                x_indep_value,
                min_eval_0,
                min_eval_1 );
      }
    }
    else // y_indep_value < min_y_indep_value || y_indep_value > max_y_indep_value
    {
      if( y_indep_value < min_y_indep_value_with_tol )
        return 0.0;
      else // y_indep_value > max_y_indep_value_with_tol
        return 1.0;
    }
  }
}

// Sample between bin boundaries using the desired sampling functor
/*! \details A unit-base statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries and then scales the
 * sample so that it preserves intermediate grid limits. Certain methods require
 * the unscaled (or raw) sample, which can be acquired with this method.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the unit-base evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBase::sample(
          const SampleFunctor& sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return UnitBase::sampleDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            min_y_indep_functor,
            max_y_indep_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

//! Sample between bin boundaries using the desired sampling functor
/* \details The SampleFunctor must sample a Cosine variable.
 * A unit-base statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries and then scales the
 * sample so that it preserves intermediate grid limits. Certain methods require
 * the unscaled (or raw) sample, which can be acquired with this method.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the unit-base evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType UnitBase::sampleCos( const SampleFunctor& sample_functor,
                                   const XIndepType& x_indep_value,
                                   const YZIterator& lower_bin_boundary,
                                   const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return UnitBase::sampleCosDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

// Sample between bin boundaries using the desired subrange sampling functor
/* \details The SampleFunctor must be of the form that it takes a subrange
 * sampling function from a OneDDistribution and the max indep variable.
 * A unit-base statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries and then scales the
 * sample so that it preserves intermediate grid limits. Certain methods require
 * the unscaled (or raw) sample, which can be acquired with this method.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the unit-base evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBase::sampleInSubrange(
          const SampleFunctor& subrange_sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary,
          const YIndepType& subrange_max_y_indep_value )
{
  YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );
  YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );

  // Make sure the y subrange limit is valid
  testPrecondition( subrange_max_y_indep_value >= min_y_indep_value )
  // Make sure the y limits is valid
  testPrecondition( max_y_indep_value >= min_y_indep_value );

  // Get the sampled bin boundary
  YZIterator sampled_bin_boundary =
    UnitBase::sampleBinBoundary<TwoDInterpPolicy,XIndepType,YZIterator>(
                              x_indep_value,
                              lower_bin_boundary,
                              upper_bin_boundary );

  // Calculate the intermediate grid limits
  typename QuantityTraits<YIndepType>::RawType
      intermediate_grid_length =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                                                  min_y_indep_value,
                                                  max_y_indep_value );

  // Calculate the unit base variable on the sampled bin boundary
  typename QuantityTraits<YIndepType>::RawType eta;

  {
    // Calculate the limit on the sampled bin boundary
    typename QuantityTraits<YIndepType>::RawType grid_length =
      TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      sampled_bin_boundary->second->getLowerBoundOfIndepVar(),
                      sampled_bin_boundary->second->getUpperBoundOfIndepVar() );

    YIndepType raw_sample;
    if( subrange_max_y_indep_value < max_y_indep_value )
    {
      typename QuantityTraits<YIndepType>::RawType subrange_eta_max =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                                                    subrange_max_y_indep_value,
                                                    min_y_indep_value,
                                                    intermediate_grid_length );

      YIndepType max_y_indep_value_bin_bound =
        TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                          subrange_eta_max,
                          sampled_bin_boundary->second->getLowerBoundOfIndepVar(),
                          grid_length );

      // Sample in the bin's subrange
      raw_sample =
        subrange_sample_functor( *sampled_bin_boundary->second,
                                  max_y_indep_value_bin_bound );
    }
    else
    {
      raw_sample =
        subrange_sample_functor( *sampled_bin_boundary->second,
                                 sampled_bin_boundary->second->getUpperBoundOfIndepVar() );
    }

    // Scale the sample
    eta = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                        raw_sample,
                        sampled_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length );
  }

  return TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                                                eta,
                                                min_y_indep_value,
                                                intermediate_grid_length );
}

// Sample between bin boundaries using the desired sampling functor
/*! \details A unit-base statistical sampling procedure is used which samples
 * from one of the distributions on the bin boundaries and then scales the
 * sample so that it preserves intermediate grid limits. Certain methods require
 * the unscaled (or raw) sample, which can be acquired with this method.
 * Statistical sampling does not sample the true intermediate distribution and
 * will not match the unit-base evaluated PDF and CDF distributions. The
 * expected value of a sample, however, will be a sample from the true
 * distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBase::sampleDetailed(
            const SampleFunctor& sample_functor,
            const YBoundsFunctor& min_y_indep_functor,
            const YBoundsFunctor& max_y_indep_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  YIndepType min_y_indep_value = min_y_indep_functor(x_indep_value);
  YIndepType max_y_indep_value = max_y_indep_functor(x_indep_value);

  // Make sure the y limits is valid
  testPrecondition( max_y_indep_value >= min_y_indep_value );

  // Get the sampled bin boundary
  sampled_bin_boundary =
    UnitBase::sampleBinBoundary<TwoDInterpPolicy,XIndepType,YZIterator>(
                            x_indep_value,
                            lower_bin_boundary,
                            upper_bin_boundary );

  // Calculate the intermediate grid limits
  typename QuantityTraits<YIndepType>::RawType
    intermediate_grid_length =
    TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                min_y_indep_value, max_y_indep_value );

  // Calculate the unit base variable on the bin boundary corresponding to the
  // raw sample
  typename QuantityTraits<YIndepType>::RawType eta;

  {
    // Calculate the limit on the sampled bin boundary
    typename QuantityTraits<YIndepType>::RawType grid_length =
      TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      sampled_bin_boundary->second->getLowerBoundOfIndepVar(),
                      sampled_bin_boundary->second->getUpperBoundOfIndepVar());

    // Create the raw sample
    raw_sample = sample_functor( *sampled_bin_boundary->second );

    eta = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                       raw_sample,
                       sampled_bin_boundary->second->getLowerBoundOfIndepVar(),
                       grid_length );
  }
  // Scale the sample so that it preserves the intermediate limits.
  // Note: This is a stochastic procedure. The intermediate distribution that
  //       has been sampled is not the true distribution. The expected value
  //       of a sample will be a sample from the true distribution though.
  return TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                eta, min_y_indep_value, intermediate_grid_length );
}

// Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType UnitBase::sampleCosDetailed(
            const SampleFunctor& sample_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  // Get the sampled bin boundary
  sampled_bin_boundary =
    UnitBase::sampleBinBoundary<TwoDInterpPolicy,XIndepType,YZIterator>(
                            x_indep_value,
                            lower_bin_boundary,
                            upper_bin_boundary );

  // Create the raw sample
  raw_sample = sample_functor( *sampled_bin_boundary->second );

  return raw_sample;
}

// Sample the bin boundary that will be used for statistical sampling
/*! \details This method will throw an exception if the primary independent
 * value is outside of the primary grid limits and the primary grid has not
 * been extended.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YZIterator>
YZIterator UnitBase::sampleBinBoundary(
    const XIndepType& x_indep_value,
    const YZIterator& lower_bin_boundary,
    const YZIterator& upper_bin_boundary )
{
  // Calculate the interpolation fraction
  double interpolation_fraction;

  {
    const double processed_lower_bin_boundary =
      TwoDInterpPolicy::processFirstIndepVar( lower_bin_boundary->first );

    interpolation_fraction =
      (TwoDInterpPolicy::processFirstIndepVar( x_indep_value ) -
        processed_lower_bin_boundary)/
      (TwoDInterpPolicy::processFirstIndepVar( upper_bin_boundary->first ) -
        processed_lower_bin_boundary );
  }

  // Sample to determine the distribution that will be used
  double random_number =
    Utility::RandomNumberGenerator::getRandomNumber<double>();

  if( random_number < interpolation_fraction )
    return upper_bin_boundary;
  else
    return lower_bin_boundary;
}

// The name of the policy
inline const std::string UnitBase::name()
{
  return "Unit Base";
}

// Calculate the Y independent lower bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto Correlated::calculateLowerBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return UnitBase::calculateLowerBound<TwoDInterpPolicy, YIndepType, XIndepType, YZIterator>(
                        x_indep_value,
                        lower_bin_boundary,
                        upper_bin_boundary );
}

// Calculate the Y independent upper bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto Correlated::calculateUpperBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return UnitBase::calculateUpperBound<TwoDInterpPolicy, YIndepType, XIndepType, YZIterator>(
                        x_indep_value,
                        lower_bin_boundary,
                        upper_bin_boundary );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details The EvaluationMethod must evalute using a Cosine variable. The edge
 *  case of no scattering (i.e. y = 1 ) is must be specially handled with
 *  LogCosLog interpolation in order to avoid a ln(1). This method uses an
 *  iterative method to estimate the CDF for correlated sampling to a relative
 *  error tolerance in order to get the proper interpolation parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType Correlated::evaluatePDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the grid limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else // x indep value is in between the grid limits
  {
    // Calculate the bin length of the x variable
    const typename QuantityTraits<XIndepType>::RawType x_bin_length =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                    lower_bin_boundary->first,
                                                    upper_bin_boundary->first );

      // Calculate the x variable bin ratio (beta)
    const typename QuantityTraits<XIndepType>::RawType beta =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                    x_indep_value,
                                                    lower_bin_boundary->first,
                                                    x_bin_length );

    // Get the min and max y value at the given x value
    YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );
    YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );

    // Calculate the correlated y value at the lower and upper bin boundary
    YIndepType lower_y_value, upper_y_value;

    // Check for a secondary indep value outside of the secondary indep grid limits
    if ( y_indep_value < min_y_indep_value || y_indep_value > max_y_indep_value )
      return QuantityTraits<ReturnType>::zero();
    else if ( y_indep_value == min_y_indep_value ) // At min y value
    {
      lower_y_value = min_y_indep_value;
      upper_y_value = min_y_indep_value;
    }
    else if ( y_indep_value == max_y_indep_value ) // At max y value
    {
      lower_y_value = max_y_indep_value;
      upper_y_value = max_y_indep_value;
    }
    else // Between min and max y values
    {
      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      {
        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );


        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      double est_cdf =
        Correlated::estimateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                XIndepType, YIndepType, YZIterator,
                                EvaluationMethod, YBoundsFunctor>(
                                        lower_cdf_bound,
                                        upper_cdf_bound,
                                        lower_y_value,
                                        upper_y_value,
                                        beta,
                                        y_indep_value,
                                        lower_bin_boundary,
                                        upper_bin_boundary,
                                        rel_error_tol,
                                        error_tol,
                                        max_number_of_iterations );
    }

    // Interpolate using the correlated upper and lower y values
    ReturnType lower_eval = ((*lower_bin_boundary->second).*evaluate)(lower_y_value);
    ReturnType upper_eval = ((*upper_bin_boundary->second).*evaluate)(upper_y_value);

    if( lower_eval == upper_eval )
      return lower_eval;
    else
    {
      /* NOTE: The special edge case of cosine of 1 (i.e. y = 1 ) will give a
       * ln(1) for LogCosLog interpolation unless treated specifically. At the
       * limits -1 and 1, the y values are the same (i.e. y_0 = y_1 = y ) and
       * the LogCosLog equation reduces to the LinLin equation.
       */
      if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LinLin" ||
           TwoDInterpPolicy::YXInterpPolicy::name() == "LinLog" ||
         ( lower_y_value == upper_y_value && lower_y_value == y_indep_value ) )
      {
        /* The PDF for lin-lin interpolation is defined as:
          * f(x,y) = ( f_0( y_0 ) * f_1( y_1 ) )/
          *          ( f_1(y_1) + ( f_0(y_0) - f_1(y_1) )* beta )
          */
        return (lower_eval*upper_eval)/LinLin::interpolate( beta, upper_eval, lower_eval );
      }
      else if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLog" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLin" )
      {
        /* The PDF for log-log interpolation is defined as:
          * f(x,y) = (1/y)*( y_0*f_0( y_0 ) * y_1*f_1( y_1 ) )/
          *          ( y_1*f_1(y_1)+( (y_0*f_0(y_0)) - (y_1*f_1(y_1)) )*beta )
          */
        auto lower_product = lower_eval*TwoDInterpPolicy::YXInterpPolicy::convertCosineVar(lower_y_value);
        auto upper_product = upper_eval*TwoDInterpPolicy::YXInterpPolicy::convertCosineVar(upper_y_value);

        return (lower_product*upper_product)/
               (LinLin::interpolate( beta, upper_product, lower_product )*
               TwoDInterpPolicy::YXInterpPolicy::convertCosineVar(y_indep_value) );
      }
      else
      {
        THROW_EXCEPTION( std::runtime_error,
                          "The interpolation mode " << TwoDInterpPolicy::ZXInterpPolicy::name() <<
                          " is currently not supported.\n" );
      }
    }
  }
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details This function uses an iterative method to estimate the CDF for
 *  correlated sampling to a relative error tolerance in order to get the proper
 *  interpolation parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType Correlated::evaluatePDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the bin limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else
  {
    // Calculate the bin length of the x variable
    const typename QuantityTraits<XIndepType>::RawType x_bin_length =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                    lower_bin_boundary->first,
                                                    upper_bin_boundary->first );

      // Calculate the x variable bin ratio (beta)
    const typename QuantityTraits<XIndepType>::RawType beta =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                    x_indep_value,
                                                    lower_bin_boundary->first,
                                                    x_bin_length );

    // Get the lower secondary indep grid limits at the primary value
    YIndepType min_y_indep_value =
      TwoDInterpPolicy::YXInterpPolicy::interpolate(
                      beta,
                      lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                      upper_bin_boundary->second->getLowerBoundOfIndepVar() );

    // Get the upper secondary indep grid limits at the primary value
    YIndepType max_y_indep_value =
      TwoDInterpPolicy::YXInterpPolicy::interpolate(
                      beta,
                      lower_bin_boundary->second->getUpperBoundOfIndepVar(),
                      upper_bin_boundary->second->getUpperBoundOfIndepVar() );

    // Calculate the correlated y value at the lower and upper bin boundary
    YIndepType lower_y_value, upper_y_value;

    // Check for a secondary indep value outside of the secondary indep grid limits
    if ( y_indep_value < min_y_indep_value || y_indep_value > max_y_indep_value )
      return QuantityTraits<ReturnType>::zero();
    else if ( y_indep_value == min_y_indep_value) // At min y value
    {
      lower_y_value = lower_bin_boundary->second->getLowerBoundOfIndepVar();
      upper_y_value = upper_bin_boundary->second->getLowerBoundOfIndepVar();
    }
    else if ( y_indep_value == max_y_indep_value ) // At max y value
    {
      lower_y_value = lower_bin_boundary->second->getUpperBoundOfIndepVar();
      upper_y_value = upper_bin_boundary->second->getUpperBoundOfIndepVar();
    }
    else // Between min and max y values
    {
      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      {
        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );

        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      double est_cdf =
        Correlated::estimateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                XIndepType, YIndepType, YZIterator,
                                EvaluationMethod, YBoundsFunctor>(
                                        lower_cdf_bound,
                                        upper_cdf_bound,
                                        lower_y_value,
                                        upper_y_value,
                                        beta,
                                        y_indep_value,
                                        lower_bin_boundary,
                                        upper_bin_boundary,
                                        rel_error_tol,
                                        error_tol,
                                        max_number_of_iterations );
    }

    ReturnType lower_eval = ((*lower_bin_boundary->second).*evaluate)(lower_y_value);
    ReturnType upper_eval = ((*upper_bin_boundary->second).*evaluate)(upper_y_value);

    if( lower_eval == upper_eval )
      return lower_eval;
    else
    {
      if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LinLin" ||
           TwoDInterpPolicy::YXInterpPolicy::name() == "LinLog" )
      {
        /* The PDF for lin-lin interpolation is defined as:
        * f(x,y) = ( f_0( y_0 ) * f_1( y_1 ) )/
        *          ( f_1(y_1) + ( f_0(y_0) - f_1(y_1) )* beta )
        */
        return (lower_eval*upper_eval)/LinLin::interpolate( beta, upper_eval, lower_eval );
      }
      else if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LogLog" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLog" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogLin" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLin" )
      {
        /* The PDF for log-log interpolation is defined as:
        * f(x,y) = (1/y)*( y_0*f_0( y_0 ) * y_1*f_1( y_1 ) )/
        *          ( y_1*f_1(y_1) + ( (y_0*f_0(y_0))-(y_1*f_1(y_1)) )*beta )
        */
        auto lower_product = lower_eval*lower_y_value;
        auto upper_product = upper_eval*upper_y_value;

        return (lower_product*upper_product)/(LinLin::interpolate( beta, upper_product, lower_product )*y_indep_value);
      }
      else
      {
        THROW_EXCEPTION( std::runtime_error,
                          "The interpolation mode " << TwoDInterpPolicy::ZXInterpPolicy::name() <<
                          " is currently not supported.\n" );
      }
    }
  }
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details The EvaluationMethod must evalute using a Cosine variable. This
 *  function uses an iterative method to estimate the CDF for correlated sampling
 *  to a relative error tolerance in order to get the proper interpolation
 *  parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double Correlated::evaluateCDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the grid limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else // x indep value is in between the grid limits
  {
    // Get the min and max y value at the given x value
    YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );
    YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );

    // Calculate the CDF value
    if ( y_indep_value <= min_y_indep_value )
      return 0.0;
    else if ( y_indep_value >= max_y_indep_value )
      return 1.0;
    else
    {
      // Calculate the bin length of the x variable
      const typename QuantityTraits<XIndepType>::RawType x_bin_length =
        TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                      lower_bin_boundary->first,
                                                      upper_bin_boundary->first );

        // Calculate the x variable bin ratio (beta)
      const typename QuantityTraits<XIndepType>::RawType beta =
        TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                      x_indep_value,
                                                      lower_bin_boundary->first,
                                                      x_bin_length );

      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      {
        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );

        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      YIndepType lower_y_value, upper_y_value;
      double est_cdf =
        Correlated::estimateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                XIndepType, YIndepType, YZIterator,
                                EvaluationMethod, YBoundsFunctor>(
                                        lower_cdf_bound,
                                        upper_cdf_bound,
                                        lower_y_value,
                                        upper_y_value,
                                        beta,
                                        y_indep_value,
                                        lower_bin_boundary,
                                        upper_bin_boundary,
                                        rel_error_tol,
                                        error_tol,
                                        max_number_of_iterations );

      return est_cdf;
    }
  }
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details This function uses an iterative method to estimate the CDF for
 *  correlated sampling to a relative error tolerance in order to get the proper
 *  interpolation parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double Correlated::evaluateCDF( const XIndepType& x_indep_value,
                                const YIndepType& y_indep_value,
                                const YBoundsFunctor& min_y_indep_functor,
                                const YBoundsFunctor& max_y_indep_functor,
                                const EvaluationMethod& evaluate,
                                const YZIterator& lower_bin_boundary,
                                const YZIterator& upper_bin_boundary,
                                const double fuzzy_boundary_tol,
                                const double rel_error_tol,
                                const double error_tol,
                                unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the bin limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else // x indep value is in between the grid limits
  {
    // Calculate the bin length of the x variable
    const typename QuantityTraits<XIndepType>::RawType x_bin_length =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                    lower_bin_boundary->first,
                                                    upper_bin_boundary->first );

      // Calculate the x variable bin ratio (beta)
    const typename QuantityTraits<XIndepType>::RawType beta =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                    x_indep_value,
                                                    lower_bin_boundary->first,
                                                    x_bin_length );

    // Get the lower secondary indep grid limits at the primary value
    YIndepType min_y_indep_value =
      TwoDInterpPolicy::YXInterpPolicy::interpolate(
                      beta,
                      lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                      upper_bin_boundary->second->getLowerBoundOfIndepVar() );

    // Get the upper secondary indep grid limits at the primary value
    YIndepType max_y_indep_value =
      TwoDInterpPolicy::YXInterpPolicy::interpolate(
                      beta,
                      lower_bin_boundary->second->getUpperBoundOfIndepVar(),
                      upper_bin_boundary->second->getUpperBoundOfIndepVar() );

    // Calculate the CDF value
    if ( y_indep_value <= min_y_indep_value )
      return 0.0;
    else if ( y_indep_value >= max_y_indep_value )
      return 1.0;
    else
    {
      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      {
        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value );

        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      YIndepType lower_y_value, upper_y_value;
      double est_cdf =
        Correlated::estimateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                XIndepType, YIndepType, YZIterator,
                                EvaluationMethod, YBoundsFunctor>(
                                        lower_cdf_bound,
                                        upper_cdf_bound,
                                        lower_y_value,
                                        upper_y_value,
                                        beta,
                                        y_indep_value,
                                        lower_bin_boundary,
                                        upper_bin_boundary,
                                        rel_error_tol,
                                        error_tol,
                                        max_number_of_iterations );

      return est_cdf;
    }
  }
}

// Estimate the interpolated CDF and the corresponding lower and upper y indep values
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor,
         typename T>
double Correlated::estimateCDF(
                          double& lower_cdf_est,
                          double& upper_cdf_est,
                          YIndepType& y_indep_value_0,
                          YIndepType& y_indep_value_1,
                          const T& beta,
                          const YIndepType& y_indep_value,
                          const YZIterator& lower_bin_boundary,
                          const YZIterator& upper_bin_boundary,
                          const double rel_error_tol,
                          const double error_tol,
                          unsigned max_number_of_iterations )
{
  unsigned number_of_iterations = 0;
  double rel_error = 1.0;
  YIndepType error_norm_constant = y_indep_value;
  double tolerance = rel_error_tol;

  /*! \detials If the secondary indep var value is zero the relative error
    *  will always zero or inf. When this is the case the error tolerance will
   *  be used instead of the relative error tolerance.
   */
  if ( y_indep_value == QuantityTraits<YIndepType>::zero() )
  {
    error_norm_constant = QuantityTraits<YIndepType>::one();
    tolerance = error_tol;
  }

  // Refine the estimated cdf value until it meet the tolerance
  double estimated_cdf = 0.0;
  while ( rel_error > tolerance )
  {
    // Estimate the cdf as the midpoint of the lower and upper boundaries
    estimated_cdf = 0.5*( lower_cdf_est + upper_cdf_est );

    // Get the sampled values at the upper and lower bin for the estimated_cdf
    y_indep_value_0 =
      ((*lower_bin_boundary->second).*&BaseOneDDistributionType::sampleWithRandomNumber)( estimated_cdf );
    y_indep_value_1 =
      ((*upper_bin_boundary->second).*&BaseOneDDistributionType::sampleWithRandomNumber)( estimated_cdf );

    // Interpolate using the templated TwoDInterpPolicy::YXInterpPolicy
    YIndepType est_y_indep_value =
      TwoDInterpPolicy::YXInterpPolicy::interpolate( beta,
                                                     y_indep_value_0,
                                                     y_indep_value_1 );

    if ( y_indep_value == est_y_indep_value )
      break;

    // Calculate the relative error between the y_indep_value and the estimate
    rel_error = (y_indep_value - est_y_indep_value )/ error_norm_constant;

    // Make sure the relative error is positive
    rel_error = rel_error < 0 ? -rel_error : rel_error;

    // Update the number of iterations
    ++number_of_iterations;

    // If tolerance is met exit loop
    if ( rel_error <= tolerance )
      break;

    // Update the estimated_cdf estimate
    if ( est_y_indep_value < y_indep_value )
    {
      // Old estimated_cdf estimate is new lower cdf boundary
      lower_cdf_est = estimated_cdf;
    }
    else
    {
      // Old estimated_cdf estimate is new upper cdf boundary
      upper_cdf_est = estimated_cdf;
    }

    // Check for the max number of iterations
    if ( number_of_iterations > max_number_of_iterations )
    {
      // Get error in estimate
      double error =
        (y_indep_value - est_y_indep_value )/QuantityTraits<YIndepType>::one();
      error = error < 0 ? -error : error;

      // If error meets error tolerance accept estimate
      if ( error < error_tol )
          break;
      else
      {
            THROW_EXCEPTION( std::logic_error,
                            "Error: The evaluation could not be completed. "
                            "The max number of iterations ("
                            << max_number_of_iterations
                            << ") was reached before the relative error ("
                            << rel_error
                            << ") reached the evaluation tolerance ("
                            << tolerance
                            << ")"
                            << " or the error ("
                            << error
                            << ") reached the error tolerance ("
                            << error_tol
                            << ")." );
      }
    }
  }
  return estimated_cdf;
}

// Sample between bin boundaries using the desired sampling functor
//! \details A direct correlated routine is used to sample the distribution.
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Correlated::sample(
          const SampleFunctor& sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return Correlated::sampleDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            min_y_indep_functor,
            max_y_indep_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

// Sample between bin boundaries using the desired sampling functor
/*! \details The SampleFunctor must return a Cosine variable.
 * A direct correlated routine is used to sample the distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType Correlated::sampleCos( const SampleFunctor& sample_functor,
                             const XIndepType& x_indep_value,
                             const YZIterator& lower_bin_boundary,
                             const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return Correlated::sampleCosDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

// Sample between bin boundaries using the desired subrange sampling functor
/* \details The SampleFunctor must be of the form that it takes a subrange
 * sampling function from a OneDDistribution and the max indep variable. A
 * direct correlated routine is used to sample the distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Correlated::sampleInSubrange(
          const SampleFunctor& subrange_sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary,
          const YIndepType& subrange_max_y_indep_value )
{
  // Make sure the max y independent variable is above the lower bound of the
  // conditional independent variable
  testPrecondition( subrange_max_y_indep_value >
                    min_y_indep_functor( x_indep_value ) );

  // Get the max y indep value at the lower bin boundary
  YIndepType max_y_indep_value_0 =
    lower_bin_boundary->second->getUpperBoundOfIndepVar();

  // Only set the subrange if it is below the max y indep value
  if( subrange_max_y_indep_value < max_y_indep_value_0 )
    max_y_indep_value_0 = subrange_max_y_indep_value;

  // Get the max y indep value at the upper bin boundary
  YIndepType max_y_indep_value_1 =
    upper_bin_boundary->second->getUpperBoundOfIndepVar();

  // Only set the subrange if it is below the max y indep value
  if( subrange_max_y_indep_value < max_y_indep_value_1 )
    max_y_indep_value_1 = subrange_max_y_indep_value;

  // Check for a primary value at the primary grid upper limit
  if( x_indep_value == upper_bin_boundary->first )
  {
    return subrange_sample_functor( *upper_bin_boundary->second,
                                    max_y_indep_value_0 );
  }
  // Check for a primary value at the primary grid lower limit
  else if( x_indep_value == lower_bin_boundary->first )
  {
    return subrange_sample_functor( *lower_bin_boundary->second,
                                    max_y_indep_value_1 );
  }
  else
  {
    YIndepType lower_sample =
      subrange_sample_functor( *lower_bin_boundary->second,
                               max_y_indep_value_0 );
    YIndepType upper_sample =
      subrange_sample_functor( *upper_bin_boundary->second,
                               max_y_indep_value_1 );

    // Check if the samples are equal
    /*! \details Special cases can arise where the min/max of a secondary
     * distribution is zero. If the min/max is sampled both the upper and lower
     * samples will be zero.
     * In order to avoid log interpolation errors no interpolation is performed.
     */
    if( lower_sample == upper_sample )
      return lower_sample;
    else
    {
      return TwoDInterpPolicy::YXInterpPolicy::interpolate(
                            lower_bin_boundary->first,
                            upper_bin_boundary->first,
                            x_indep_value,
                            lower_sample,
                            upper_sample );
    }
  }
}

// Sample between bin boundaries using the desired sampling functor
//! \details A direct correlated routine is used to sample the distribution.
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType Correlated::sampleDetailed(
            const SampleFunctor& sample_functor,
            const YBoundsFunctor& min_y_indep_functor,
            const YBoundsFunctor& max_y_indep_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  // Check for a primary value at the primary grid upper limit
  if( x_indep_value == upper_bin_boundary->first )
  {
    raw_sample = sample_functor( *upper_bin_boundary->second );
    sampled_bin_boundary = upper_bin_boundary;
  }
  else if( x_indep_value == lower_bin_boundary->first )
  {
    raw_sample = sample_functor( *lower_bin_boundary->second );
    sampled_bin_boundary = lower_bin_boundary;
  }
  else
  {
    YIndepType lower_sample = sample_functor( *lower_bin_boundary->second );
    YIndepType upper_sample = sample_functor( *upper_bin_boundary->second );

    // Check if the samples are equal
    /*! \details Special cases can arise where the min/max of a secondary
     * distribution is zero. If the min/max is sampled both the upper and lower
     * samples will be zero.
     * In order to avoid log interpolation errors no interpolation is performed.
     */
    if( lower_sample == upper_sample )
    {
      raw_sample = lower_sample;
      sampled_bin_boundary = lower_bin_boundary;
    }
    else
    {
      raw_sample = TwoDInterpPolicy::YXInterpPolicy::interpolate(
                            lower_bin_boundary->first,
                            upper_bin_boundary->first,
                            x_indep_value,
                            lower_sample,
                            upper_sample );

      sampled_bin_boundary = lower_bin_boundary;
    }
  }
  return raw_sample;
}

// Sample between bin boundaries using the desired sampling functor
/*! \details The SampleFunctor must return a Cosine variable.
 * A direct correlated routine is used to sample the distribution.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType Correlated::sampleCosDetailed(
            const SampleFunctor& sample_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  // Dummy variables
  double dummy_functor;

  return Correlated::sampleDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            dummy_functor,
            dummy_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            sampled_bin_boundary,
            raw_sample );
}

// The name of the policy
inline const std::string Correlated::name()
{
  return "Correlated";
}

// Calculate the Y independent lower bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto UnitBaseCorrelated::calculateLowerBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return UnitBase::calculateLowerBound<TwoDInterpPolicy, YIndepType, XIndepType, YZIterator>(
                        x_indep_value,
                        lower_bin_boundary,
                        upper_bin_boundary );
}

// Calculate the Y independent upper bound between bin boundaries
template<typename TwoDInterpPolicy,
         typename YIndepType,
         typename XIndepType,
         typename YZIterator>
auto UnitBaseCorrelated::calculateUpperBound(
                      const XIndepType& x_indep_value,
                      const YZIterator& lower_bin_boundary,
                      const YZIterator& upper_bin_boundary ) -> YIndepType
{
  return UnitBase::calculateUpperBound<TwoDInterpPolicy, YIndepType, XIndepType, YZIterator>(
                        x_indep_value,
                        lower_bin_boundary,
                        upper_bin_boundary );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details The EvaluationMethod must evalute using a Cosine variable. This
 *  method uses an iterative method to estimate the CDF for unit-base correlated
 *  sampling to a relative error tolerance in order to get the proper
 *  interpolation parameters. The lower and upper bounds of the secondary
 *  independent variable (cosine) are fixed (-1 <= cosine <= 1). Therefore a
 *  unit base method is not necessary and the direct correlated method is used
 *  to evaluate instead.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType UnitBaseCorrelated::evaluatePDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Correlated::evaluatePDFCos<TwoDInterpPolicy,
                                       BaseOneDDistributionType,
                                       XIndepType,
                                       YIndepType,
                                       ReturnType,
                                       YZIterator,
                                       EvaluationMethod,
                                       YBoundsFunctor>(
                                                    x_indep_value,
                                                    y_indep_value,
                                                    min_y_indep_functor,
                                                    max_y_indep_functor,
                                                    evaluate,
                                                    lower_bin_boundary,
                                                    upper_bin_boundary,
                                                    fuzzy_boundary_tol,
                                                    rel_error_tol,
                                                    error_tol,
                                                    max_number_of_iterations );
}

// Evaluate the PDF between bin boundaries using the desired evaluation method
/*! \details This method uses an iterative method to estimate the CDF for
 *  unit-base correlated sampling to a relative error tolerance in order to get
 *  the proper interpolation parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename ReturnType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
ReturnType UnitBaseCorrelated::evaluatePDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the bin limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else
  {
    // Calculate the bin length of the x variable
    const typename QuantityTraits<XIndepType>::RawType x_bin_length =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                    lower_bin_boundary->first,
                                                    upper_bin_boundary->first );

    // Calculate the x variable bin ratio (beta)
    const typename QuantityTraits<XIndepType>::RawType beta =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                    x_indep_value,
                                                    lower_bin_boundary->first,
                                                    x_bin_length );

    // Get the min and max y indep values
    YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );
    YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );

    // Get the intermediate grid lengths for the indep value and the upper and lower bin boundary
    typename QuantityTraits<YIndepType>::RawType grid_length_0 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                      lower_bin_boundary->second->getUpperBoundOfIndepVar() );

    typename QuantityTraits<YIndepType>::RawType grid_length_1 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                      upper_bin_boundary->second->getUpperBoundOfIndepVar() );

    typename QuantityTraits<YIndepType>::RawType
    intermediate_grid_length =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      min_y_indep_value,
                      max_y_indep_value );

    // Calculate the correlated y value at the lower and upper bin boundary
    YIndepType lower_y_value, upper_y_value;
    typename QuantityTraits<YIndepType>::RawType eta;

    // Check for a y indep value outside of the y indep grid limits
    if ( y_indep_value < min_y_indep_value || y_indep_value > max_y_indep_value )
      return QuantityTraits<ReturnType>::zero();
    else if ( y_indep_value == min_y_indep_value ) // At min y value
    {
      lower_y_value = lower_bin_boundary->second->getLowerBoundOfIndepVar();
      upper_y_value = upper_bin_boundary->second->getLowerBoundOfIndepVar();
      eta = 0.0;
    }
    else if ( y_indep_value == max_y_indep_value ) // At max y value
    {
      lower_y_value = lower_bin_boundary->second->getUpperBoundOfIndepVar();
      upper_y_value = upper_bin_boundary->second->getUpperBoundOfIndepVar();
      eta = 1.0;
    }
    else // Between min and max y values
    {
      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      {

        YIndepType min_y_indep_value_with_tol =
            TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyLowerBound(
                        min_y_indep_value );

        YIndepType max_y_indep_value_with_tol =
            TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyUpperBound(
                        max_y_indep_value );

        // Calculate the unit base variable on the intermediate grid
        eta = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                y_indep_value,
                min_y_indep_value,
                intermediate_grid_length );

        // Get the y indep var value for the upper and lower bin boundaries
        YIndepType y_indep_value_0 =
            TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                        eta,
                        lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_0 );

        YIndepType y_indep_value_1 =
            TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                        eta,
                        upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_1 );

        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value_0 );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value_1 );

        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      double cdf =
        UnitBaseCorrelated::estimateCDF<TwoDInterpPolicy, BaseOneDDistributionType,
                                        XIndepType, YIndepType, YZIterator,
                                        EvaluationMethod, YBoundsFunctor>(
                                          lower_cdf_bound,
                                          upper_cdf_bound,
                                          lower_y_value,
                                          upper_y_value,
                                          beta,
                                          grid_length_0,
                                          grid_length_1,
                                          eta,
                                          lower_bin_boundary,
                                          upper_bin_boundary,
                                          rel_error_tol,
                                          error_tol,
                                          max_number_of_iterations );
    }

    ReturnType lower_eval = ((*lower_bin_boundary->second).*evaluate)(lower_y_value);
    ReturnType upper_eval = ((*upper_bin_boundary->second).*evaluate)(upper_y_value);

    if( lower_eval == upper_eval )
      return lower_eval;
    else
    {
      if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LinLin" ||
           TwoDInterpPolicy::YXInterpPolicy::name() == "LinLog" ||
           eta == 0.0 )
      {
        /* The PDF for lin-lin interpolation is defined as:
        * f(x,y) = 1/L * ( L_0f_0( y_0 ) * L_1f_1( y_1 ) )/
        *          ( L_1f_1(y_1) + ( L_0f_0(y_0) - L_1f_1(y_1) )* beta )
        */
        auto lower_product = lower_eval*grid_length_0;
        auto upper_product = upper_eval*grid_length_1;

        return (lower_product*upper_product)/LinLin::interpolate( beta, upper_product, lower_product )/intermediate_grid_length;
      }
      else if ( TwoDInterpPolicy::YXInterpPolicy::name() == "LogLog" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLog" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogLin" ||
                TwoDInterpPolicy::YXInterpPolicy::name() == "LogCosLin" )
      {
        /* The PDF for log-log interpolation is defined as:
        * f(x,y) = 1/(eta*L)*( eta_0*L_0*f_0( y_0 ) * eta_1*L_1*f_1( y_1 ) )/
        * ( eta_1*L_1*f_1(y_1) + ( (eta_0*L_0*f_0(y_0)) - (eta_1*L_1*f_1(y_1)) )*beta )
        */
        auto lower_product =
          lower_eval*log( lower_y_value/lower_bin_boundary->second->getLowerBoundOfIndepVar() );
        auto upper_product =
          upper_eval*log( upper_y_value/upper_bin_boundary->second->getLowerBoundOfIndepVar() );

        return (lower_product*upper_product)/LinLin::interpolate( beta, upper_product, lower_product )/log(y_indep_value/min_y_indep_value);
      }
      else
      {
        THROW_EXCEPTION( std::runtime_error,
                          "The interpolation mode " << TwoDInterpPolicy::YXInterpPolicy::name() <<
                          " is currently not supported.\n" );
      }
    }
  }
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details The EvaluationMethod must evalute using a Cosine variable. This
 *  method uses an iterative method to estimate the CDF for unit-base correlated
 *  sampling to a relative error tolerance in order to get the proper
 *  interpolation parameters. The lower and upper bounds of the secondary
 *  independent variable (cosine) are fixed (-1 <= cosine <= 1). Therefore a
 *  unit base method is not necessary and the direct correlated method is used
 *  to evaluate instead.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double UnitBaseCorrelated::evaluateCDFCos(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  return Correlated::evaluateCDFCos<TwoDInterpPolicy,
                                       BaseOneDDistributionType,
                                       XIndepType,
                                       YIndepType,
                                       YZIterator,
                                       EvaluationMethod,
                                       YBoundsFunctor>(
                                                    x_indep_value,
                                                    y_indep_value,
                                                    min_y_indep_functor,
                                                    max_y_indep_functor,
                                                    evaluate,
                                                    lower_bin_boundary,
                                                    upper_bin_boundary,
                                                    fuzzy_boundary_tol,
                                                    rel_error_tol,
                                                    error_tol,
                                                    max_number_of_iterations );
}

// Evaluate the CDF between bin boundaries using the desired evaluation method
/*! \details This method uses an iterative method to estimate the CDF for
 *  unit-base correlated sampling to a relative error tolerance in order to get
 *  the proper interpolation parameters.
 */
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor>
double UnitBaseCorrelated::evaluateCDF(
                                  const XIndepType& x_indep_value,
                                  const YIndepType& y_indep_value,
                                  const YBoundsFunctor& min_y_indep_functor,
                                  const YBoundsFunctor& max_y_indep_functor,
                                  const EvaluationMethod& evaluate,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary,
                                  const double fuzzy_boundary_tol,
                                  const double rel_error_tol,
                                  const double error_tol,
                                  unsigned max_number_of_iterations )
{
  // Check to see if the x indep value is on the bin limits
  if( lower_bin_boundary->first == x_indep_value )
  {
    return ((*lower_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else if( upper_bin_boundary->first == x_indep_value )
  {
    return ((*upper_bin_boundary->second).*evaluate)(y_indep_value);
  }
  else
  {
    // Calculate the bin length of the x variable
    const typename QuantityTraits<XIndepType>::RawType x_bin_length =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseGridLength(
                                                    lower_bin_boundary->first,
                                                    upper_bin_boundary->first );

    // Calculate the x variable bin ratio (beta)
    const typename QuantityTraits<XIndepType>::RawType beta =
      TwoDInterpPolicy::ZXInterpPolicy::calculateUnitBaseIndepVar(
                                                    x_indep_value,
                                                    lower_bin_boundary->first,
                                                    x_bin_length );

    // Get the min and max y indep values
    YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );
    YIndepType max_y_indep_value = max_y_indep_functor( x_indep_value );

    // Get the intermediate grid lengths for the indep value and the upper and lower bin boundary
    typename QuantityTraits<YIndepType>::RawType grid_length_0 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                      lower_bin_boundary->second->getUpperBoundOfIndepVar() );

    typename QuantityTraits<YIndepType>::RawType grid_length_1 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                      upper_bin_boundary->second->getUpperBoundOfIndepVar() );

    typename QuantityTraits<YIndepType>::RawType
    intermediate_grid_length =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      min_y_indep_value,
                      max_y_indep_value );

    // Check for a y indep value outside of the y indep grid limits
    if ( y_indep_value <= min_y_indep_value )
      return 0.0;
    else if ( y_indep_value >= max_y_indep_value )
      return 1.0;
    else
    {
      // Get the lower and upper boundaries of the evaluated cdf
      double lower_cdf_bound, upper_cdf_bound;
      typename QuantityTraits<YIndepType>::RawType eta;
      {

        YIndepType min_y_indep_value_with_tol =
            TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyLowerBound(
                        min_y_indep_value );

        YIndepType max_y_indep_value_with_tol =
            TwoDInterpPolicy::ZYInterpPolicy::calculateFuzzyUpperBound(
                        max_y_indep_value );

        // Calculate the unit base variable on the intermediate grid
        eta = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                y_indep_value,
                min_y_indep_value,
                intermediate_grid_length );

        // Get the y indep var value for the upper and lower bin boundaries
        YIndepType y_indep_value_0 =
            TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                        eta,
                        lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_0 );

        YIndepType y_indep_value_1 =
            TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                        eta,
                        upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_1 );

        // Evaluate the cdf at the upper and lower bin boundaries
        double bin_eval_0 =
          ((*lower_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value_0 );
        double bin_eval_1 =
          ((*upper_bin_boundary->second).*&BaseOneDDistributionType::evaluateCDF)( y_indep_value_1 );

        if ( bin_eval_0 <= bin_eval_1 )
        {
          lower_cdf_bound = bin_eval_0;
          upper_cdf_bound = bin_eval_1;
        }
        else
        {
          lower_cdf_bound = bin_eval_1;
          upper_cdf_bound = bin_eval_0;
        }
      }

      YIndepType lower_y_value, upper_y_value;
      return UnitBaseCorrelated::estimateCDF<TwoDInterpPolicy,
                                             BaseOneDDistributionType, XIndepType,
                                             YIndepType, YZIterator,
                                             EvaluationMethod, YBoundsFunctor>(
                                          lower_cdf_bound,
                                          upper_cdf_bound,
                                          lower_y_value,
                                          upper_y_value,
                                          beta,
                                          grid_length_0,
                                          grid_length_1,
                                          eta,
                                          lower_bin_boundary,
                                          upper_bin_boundary,
                                          rel_error_tol,
                                          error_tol,
                                          max_number_of_iterations );
    }
  }
}

// Estimate the interpolated CDF and the corresponding lower and upper y indep values
template<typename TwoDInterpPolicy,
         typename BaseOneDDistributionType,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename EvaluationMethod,
         typename YBoundsFunctor,
         typename T>
double UnitBaseCorrelated::estimateCDF(
        double& lower_cdf_est,
        double& upper_cdf_est,
        YIndepType& y_indep_value_0,
        YIndepType& y_indep_value_1,
        const T& beta,
        const typename QuantityTraits<YIndepType>::RawType& grid_length_0,
        const typename QuantityTraits<YIndepType>::RawType& grid_length_1,
        const typename QuantityTraits<YIndepType>::RawType& eta,
        const YZIterator& lower_bin_boundary,
        const YZIterator& upper_bin_boundary,
        const double rel_error_tol,
        const double error_tol,
        unsigned max_number_of_iterations )
{
  unsigned number_of_iterations = 0;
  double rel_error = 1.0;
  typename QuantityTraits<YIndepType>::RawType error_norm_constant = eta;
  double tolerance = rel_error_tol;

  /*! \detials If the y indep var value is zero the relative error
    *  will always zero or inf. When this is the case the error tolerance will
    *  be used instead of the relative error tolerance.
    */
    if ( eta == 0.0 )
    {
      error_norm_constant = 1.0;
      tolerance = error_tol;
    }

  // Refine the estimated cdf value until it meet the tolerance
  double estimated_cdf = 0.0;
  while ( rel_error > tolerance )
  {
    // Estimate the cdf as the midpoint of the lower and upper boundaries
    estimated_cdf = 0.5*( lower_cdf_est + upper_cdf_est );

    // Get the sampled values at the upper and lower bin for the estimated_cdf
    y_indep_value_0 =
      ((*lower_bin_boundary->second).*&BaseOneDDistributionType::sampleWithRandomNumber)( estimated_cdf );
    y_indep_value_1 =
      ((*upper_bin_boundary->second).*&BaseOneDDistributionType::sampleWithRandomNumber)( estimated_cdf );

    // Calculate the unit base variable on the intermediate grid corresponding to the
    // raw samples on the lower and upper boundaries
    typename QuantityTraits<YIndepType>::RawType
      eta_estimate, eta_0, eta_1;

    eta_0 =
      TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
          y_indep_value_0,
          lower_bin_boundary->second->getLowerBoundOfIndepVar(),
          grid_length_0 );

    eta_1 =
      TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
          y_indep_value_1,
          upper_bin_boundary->second->getLowerBoundOfIndepVar(),
          grid_length_1 );

    // Interpolate using the templated TwoDInterpPolicy::YXInterpPolicy
    eta_estimate =
      TwoDInterpPolicy::YXInterpPolicy::interpolate( beta, eta_0, eta_1 );

    // Update the number of iterations
    ++number_of_iterations;

    if ( eta == eta_estimate )
      break;

    // Calculate the relative error between eta and the estimate
    rel_error = ( eta - eta_estimate )/error_norm_constant;

    // Make sure the relative error is positive
    rel_error = rel_error < 0 ? -rel_error : rel_error;

    // If tolerance is met exit loop
    if ( rel_error <= tolerance )
      break;

    // Update the estimated_cdf estimate
    if ( eta_estimate < eta )
    {
      // Old estimated_cdf estimate is new lower cdf boundary
      lower_cdf_est = estimated_cdf;
    }
    else
    {
      // Old estimated_cdf estimate is new upper cdf boundary
      upper_cdf_est = estimated_cdf;
    }

    // Check for the max number of iterations
    if ( number_of_iterations > max_number_of_iterations )
    {
      // Get error in estimate
      double error = ( eta - eta_estimate );
      error = error < 0 ? -error : error;

      // If error meets error tolerance accept estimate
      if ( error < error_tol )
          break;
      else
      {
      THROW_EXCEPTION( std::logic_error,
                      "Error: The evaluation could not be completed. "
                      "The max number of iterations ("
                      << max_number_of_iterations
                      << ") was reached before the relative error ("
                      << rel_error
                      << ") reached the evaluation tolerance ("
                      << rel_error_tol
                      << ")." );
      }
    }
  }
  return estimated_cdf;
}

// Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBaseCorrelated::sample(
          const SampleFunctor& sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return UnitBaseCorrelated::sampleDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            min_y_indep_functor,
            max_y_indep_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

//! Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType UnitBaseCorrelated::sampleCos( const SampleFunctor& sample_functor,
                                  const XIndepType& x_indep_value,
                                  const YZIterator& lower_bin_boundary,
                                  const YZIterator& upper_bin_boundary )
{
  // Dummy variables
  YIndepType dummy_raw_sample;
  YZIterator dummy_sampled_bin_boundary;

  return UnitBaseCorrelated::sampleCosDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            dummy_sampled_bin_boundary,
            dummy_raw_sample );
}

// Sample between bin boundaries using the desired subrange sampling functor
/* \details The SampleFunctor must be of the form that it takes a subrange
 * sampling function from a OneDDistribution and the max indep variable.
 */
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBaseCorrelated::sampleInSubrange(
          const SampleFunctor& subrange_sample_functor,
          const YBoundsFunctor& min_y_indep_functor,
          const YBoundsFunctor& max_y_indep_functor,
          const XIndepType& x_indep_value,
          const YZIterator& lower_bin_boundary,
          const YZIterator& upper_bin_boundary,
          const YIndepType& subrange_max_y_indep_value )
{
  // Get the min y indep var values for the x_indep_value
  YIndepType min_y_indep_value = min_y_indep_functor( x_indep_value );

  // Get the max y indep var values for the x_indep_value
  YIndepType max_y_indep_value;
  if( subrange_max_y_indep_value > max_y_indep_functor( x_indep_value ) )
    max_y_indep_value = max_y_indep_functor( x_indep_value );
  else
    max_y_indep_value = subrange_max_y_indep_value;

  // Check for a x value at a grid limit
  if( x_indep_value == upper_bin_boundary->first )
    return subrange_sample_functor( *upper_bin_boundary->second, max_y_indep_value );
  else if( x_indep_value == lower_bin_boundary->first )
    return subrange_sample_functor( *lower_bin_boundary->second, max_y_indep_value );
  else
  {
    typename QuantityTraits<YIndepType>::RawType
    intermediate_grid_length =
      TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                  min_y_indep_value, max_y_indep_value );

    // Calculate the unit base variable on the intermediate grid corresponding to the
    // raw samples on the lower and upper boundaries
    typename QuantityTraits<YIndepType>::RawType eta, eta_0, eta_1;

    {
      // Get the max y indep value at the lower bin boundary
      YIndepType max_y_indep_value_0 =
        lower_bin_boundary->second->getUpperBoundOfIndepVar();

      // Only set the subrange if it is below the max y indep value
      if( subrange_max_y_indep_value < max_y_indep_value_0 )
        max_y_indep_value_0 = subrange_max_y_indep_value;

      // Calculate the unit base variable on the lower grid corresponding to the
      // lower raw sample
      typename QuantityTraits<YIndepType>::RawType grid_length_0 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                    lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                    max_y_indep_value_0 );

      eta_0 = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
            subrange_sample_functor( *lower_bin_boundary->second, max_y_indep_value_0 ),
            lower_bin_boundary->second->getLowerBoundOfIndepVar(),
            grid_length_0 );


      // Get the max y indep value at the upper bin boundary
      YIndepType max_y_indep_value_1 =
        upper_bin_boundary->second->getUpperBoundOfIndepVar();

      // Only set the subrange if it is below the max y indep value
      if( subrange_max_y_indep_value < max_y_indep_value_1 )
        max_y_indep_value_1 = subrange_max_y_indep_value;

      // Calculate the unit base variable on the upper grid corresponding to the
      // upper raw sample
      typename QuantityTraits<YIndepType>::RawType grid_length_1 =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                    upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                    max_y_indep_value_1 );

      eta_1 = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
            subrange_sample_functor( *upper_bin_boundary->second, max_y_indep_value_1 ),
            upper_bin_boundary->second->getLowerBoundOfIndepVar(),
            grid_length_1 );

      // Interpolate between the lower and upper unit base variables
      eta = TwoDInterpPolicy::YXInterpPolicy::interpolate(
                                                  lower_bin_boundary->first,
                                                  upper_bin_boundary->first,
                                                  x_indep_value,
                                                  eta_0,
                                                  eta_1 );
    }

    // Scale the sample so that it preserves the intermediate limits.
    return TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
              eta, min_y_indep_value, intermediate_grid_length );
  }
}

// Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor,
         typename YBoundsFunctor>
YIndepType UnitBaseCorrelated::sampleDetailed(
            const SampleFunctor& sample_functor,
            const YBoundsFunctor& min_y_indep_functor,
            const YBoundsFunctor& max_y_indep_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  // Check for a primary value at the primary grid upper limit
  if( x_indep_value == upper_bin_boundary->first )
  {
    raw_sample = sample_functor( *upper_bin_boundary->second );
    sampled_bin_boundary = upper_bin_boundary;
  }
  else if( x_indep_value == lower_bin_boundary->first )
  {
    raw_sample = sample_functor( *lower_bin_boundary->second );
    sampled_bin_boundary = lower_bin_boundary;
  }
  else
  {
    YIndepType min_y_indep_value = min_y_indep_functor(x_indep_value);
    YIndepType max_y_indep_value = max_y_indep_functor(x_indep_value);

    // Make sure the y limits is valid
    testPrecondition( max_y_indep_value >= min_y_indep_value );

    if( min_y_indep_value == max_y_indep_value )
    {
      // The min equals the max
      raw_sample = min_y_indep_value;
      sampled_bin_boundary = lower_bin_boundary;
    }
    else
    {
      typename QuantityTraits<YIndepType>::RawType
      intermediate_grid_length =
        TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                            min_y_indep_value, max_y_indep_value );

      // Calculate the unit base variable on the intermediate grid corresponding to the
      // raw samples on the lower and upper boundaries
      typename QuantityTraits<YIndepType>::RawType eta, eta_0, eta_1;

      {
        // Calculate the unit base variable on the lower grid corresponding to the
        // lower raw sample
        typename QuantityTraits<YIndepType>::RawType grid_length_0 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                      lower_bin_boundary->second->getUpperBoundOfIndepVar());

        eta_0 = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                        sample_functor( *lower_bin_boundary->second ),
                        lower_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_0 );

        // Calculate the unit base variable on the upper grid corresponding to the
        // upper raw sample
        typename QuantityTraits<YIndepType>::RawType grid_length_1 =
          TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseGridLength(
                      upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                      upper_bin_boundary->second->getUpperBoundOfIndepVar());

        eta_1 = TwoDInterpPolicy::ZYInterpPolicy::calculateUnitBaseIndepVar(
                        sample_functor( *upper_bin_boundary->second ),
                        upper_bin_boundary->second->getLowerBoundOfIndepVar(),
                        grid_length_1 );

        // Take into account special case where both eta values equal zero
        // (e.g. random number of zero)
        if( eta_0 == eta_1)
          eta = eta_0;
        else
        {
          // Interpolate between the lower and upper unit base variables
          eta = TwoDInterpPolicy::YXInterpPolicy::interpolate(
                                                  lower_bin_boundary->first,
                                                  upper_bin_boundary->first,
                                                  x_indep_value,
                                                  eta_0,
                                                  eta_1 );
        }
      }
    // Scale the sample so that it preserves the intermediate limits.
    raw_sample = TwoDInterpPolicy::ZYInterpPolicy::calculateIndepVar(
                  eta, min_y_indep_value, intermediate_grid_length );
    sampled_bin_boundary = lower_bin_boundary;
    }
  }
  return raw_sample;
}

// Sample between bin boundaries using the desired sampling functor
template<typename TwoDInterpPolicy,
         typename XIndepType,
         typename YIndepType,
         typename YZIterator,
         typename SampleFunctor>
YIndepType UnitBaseCorrelated::sampleCosDetailed(
            const SampleFunctor& sample_functor,
            const XIndepType& x_indep_value,
            const YZIterator& lower_bin_boundary,
            const YZIterator& upper_bin_boundary,
            YZIterator& sampled_bin_boundary,
            YIndepType& raw_sample )
{
  return Correlated::sampleCosDetailed<TwoDInterpPolicy,XIndepType,YIndepType,YZIterator,SampleFunctor>(
            sample_functor,
            x_indep_value,
            lower_bin_boundary,
            upper_bin_boundary,
            sampled_bin_boundary,
            raw_sample );
}

// The name of the policy
inline const std::string UnitBaseCorrelated::name()
{
  return "Unit-base Correlated";
}

} // end Utility namespace

#endif // end UTILITY_TWO_D_SAMPLING_POLICY_DEF_HPP

//---------------------------------------------------------------------------//
// end Utility_TwoDSamplingPolicy_def.hpp
//---------------------------------------------------------------------------//
