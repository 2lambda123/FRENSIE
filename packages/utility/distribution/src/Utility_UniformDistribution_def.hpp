//---------------------------------------------------------------------------//
//!
//! \file   Utility_UniformDistribution_def.hpp
//! \author Alex Robinson
//! \brief  Uniform distribution class definition
//!
//---------------------------------------------------------------------------//

#ifndef UTILITY_UNIFORM_DISTRIBUTION_DEF_HPP
#define UTILITY_UNIFORM_DISTRIBUTION_DEF_HPP

// FRENSIE Includes
#include "Utility_RandomNumberGenerator.hpp"
#include "Utility_ExceptionTestMacros.hpp"
#include "Utility_ExceptionCatchMacros.hpp"

BOOST_SERIALIZATION_DISTRIBUTION2_EXPORT_IMPLEMENT( UnitAwareUniformDistribution );

namespace Utility{

// Constructor
/*! \details A quantity with a different unit can be used as an input. This
 * will be explicitly cast to the desired unit during object construction.
 */
template<typename IndependentUnit, typename DependentUnit>
template<typename InputIndepQuantity, typename InputDepQuantity>
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::UnitAwareUniformDistribution(
	                       const InputIndepQuantity& min_independent_value,
			       const InputIndepQuantity& max_independent_value,
			       const InputDepQuantity& dependent_value )
  : d_min_independent_value( min_independent_value ),
    d_max_independent_value( max_independent_value ),
    d_dependent_value( dependent_value ),
    d_pdf_value()
{
  // Verify that shape parameters are valid
  this->verifyValidShapeParameters( d_min_independent_value,
                                    d_max_independent_value,
                                    d_dependent_value );
  
  // Calculate the pdf value
  this->calculatePDFValue();

  BOOST_SERIALIZATION_CLASS_EXPORT_IMPLEMENT_FINALIZE( ThisType );
}

// Copy constructor
/*! \details Just like boost::units::quantity objects, the unit-aware
 * distribution can be explicitly cast to a distribution with compatible
 * units. If the units are not compatible, this function will not compile. Note
 * that this allows distributions to be scaled safely (unit conversions
 * are completely taken care of by boost::units)!
 * Note: The Dummy template parameter is used to keep the Python interfaces
 *       generated by SWIG clean.
 */
template<typename IndependentUnit, typename DependentUnit>
template<typename InputIndepUnit, typename InputDepUnit, typename Dummy>
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::UnitAwareUniformDistribution( const UnitAwareUniformDistribution<InputIndepUnit,InputDepUnit>& dist_instance )
  : d_min_independent_value( dist_instance.d_min_independent_value ),
    d_max_independent_value( dist_instance.d_max_independent_value ),
    d_dependent_value( dist_instance.d_dependent_value ),
    d_pdf_value()
{
  // Calculate the pdf value
  this->calculatePDFValue();

  BOOST_SERIALIZATION_CLASS_EXPORT_IMPLEMENT_FINALIZE( ThisType );
}

// Copy constructor (copying from unitless distribution only)
template<typename IndependentUnit, typename DependentUnit>
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::UnitAwareUniformDistribution( const UnitAwareUniformDistribution<void,void>& unitless_dist_instance, int )
  : d_min_independent_value( IQT::initializeQuantity( unitless_dist_instance.d_min_independent_value ) ),
    d_max_independent_value( IQT::initializeQuantity( unitless_dist_instance.d_max_independent_value ) ),
    d_dependent_value( DQT::initializeQuantity( unitless_dist_instance.d_dependent_value ) ),
    d_pdf_value()
{
  // Calculate the pdf value
  this->calculatePDFValue();

  BOOST_SERIALIZATION_CLASS_EXPORT_IMPLEMENT_FINALIZE( ThisType );
}

// Construct distribution from a unitless dist. (potentially dangerous)
/*! \details Constructing a unit-aware distribution from a unitless
 * distribution is potentially dangerous. By forcing users to construct objects
 * using this method instead of a standard constructor we are trying to make
 * sure users are aware of the danger. This is designed to mimic the interface
 * of the boost::units::quantity, which also has to deal with this issue.
 */
template<typename IndependentUnit, typename DependentUnit>
UnitAwareUniformDistribution<IndependentUnit,DependentUnit> UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::fromUnitlessDistribution( const UnitAwareUniformDistribution<void,void>& unitless_distribution )
{
  return ThisType( unitless_distribution, 0 );
}

// Assignment operator
template<typename IndependentUnit, typename DependentUnit>
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>& UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::operator=(
       const UnitAwareUniformDistribution<IndependentUnit,DependentUnit>& dist_instance )
{
  if( this != &dist_instance )
  {
    d_min_independent_value = dist_instance.d_min_independent_value;
    d_max_independent_value = dist_instance.d_max_independent_value;
    d_dependent_value = dist_instance.d_dependent_value;
    d_pdf_value = dist_instance.d_pdf_value;
  }

  return *this;
}

// Evaluate the distribution
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::DepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::evaluate(
const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity indep_var_value ) const
{
  if( indep_var_value >= d_min_independent_value &&
      indep_var_value <= d_max_independent_value )
    return d_dependent_value;
  else
    return QuantityTraits<DepQuantity>::zero();
}

// Evaluate the PDF
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::InverseIndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::evaluatePDF(
const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity indep_var_value ) const
{
  if( indep_var_value >= d_min_independent_value &&
      indep_var_value <= d_max_independent_value )
    return d_pdf_value;
  else
    return QuantityTraits<InverseIndepQuantity>::zero();
}

// Evaluate the CDF
template<typename IndependentUnit, typename DependentUnit>
double UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::evaluateCDF(
const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity indep_var_value ) const
{
  if( indep_var_value >= d_min_independent_value &&
      indep_var_value <= d_max_independent_value )
    return d_pdf_value*(indep_var_value - d_min_independent_value);
  else if( indep_var_value < d_min_independent_value )
    return 0.0;
  else
    return 1.0;
}

// Return a random sample from the distribution
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sample() const
{
  return ThisType::sample( d_min_independent_value, d_max_independent_value );
}

// Return a random sample from the distribution
template<typename IndependentUnit, typename DependentUnit>
inline typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sample(
 const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity min_independent_value,
 const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity max_independent_value )
{
  // Make sure that the max value is greater than the min value
  testPrecondition( max_independent_value > min_independent_value );

  return ThisType::sampleWithRandomNumber(
			    min_independent_value,
			    max_independent_value,
			    RandomNumberGenerator::getRandomNumber<double>() );
}

// Return a random sample from the corresponding CDF and record the number of trials
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleAndRecordTrials( DistributionTraits::Counter& trials ) const
{
  return ThisType::sampleAndRecordTrials( d_min_independent_value,
					  d_max_independent_value,
					  trials );
}

// Return a random sample from the distribution and record the number of trials
template<typename IndependentUnit, typename DependentUnit>
inline typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleAndRecordTrials(
 const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity min_independent_value,
 const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity max_independent_value,
 DistributionTraits::Counter& trials )
{
  // Make sure that the max value is greater than the min value
  testPrecondition( max_independent_value > min_independent_value );

  ++trials;

  return ThisType::sample( min_independent_value, max_independent_value );
}

// Return a random sample from the distribution at the given CDF value
template<typename IndependentUnit, typename DependentUnit>
inline typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleWithRandomNumber(
					     const double random_number ) const
{
  // Make sure the random number is valid
  testPrecondition( random_number >= 0.0 );
  testPrecondition( random_number <= 1.0 );

  return ThisType::sampleWithRandomNumber( d_min_independent_value,
					   d_max_independent_value,
					   random_number );
}

// Return a random sample from the distribution at the given CDF value
template<typename IndependentUnit, typename DependentUnit>
inline typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleWithRandomNumber(
  const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity min_independent_value,
  const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity max_independent_value,
  const double random_number )
{
  // Make sure that the max value is greater than the min value
  testPrecondition( max_independent_value > min_independent_value );
  // Make sure the random number is valid
  testPrecondition( random_number >= 0.0 );
  testPrecondition( random_number <= 1.0 );

  return random_number*(max_independent_value - min_independent_value) +
    min_independent_value;
}

// Return a random sample from the distribution at the given CDF value in a subrange
template<typename IndependentUnit, typename DependentUnit>
inline typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleWithRandomNumberInSubrange(
	    const double random_number,
	    const typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity max_indep_var ) const
{
  // Make sure the random number is valid
  testPrecondition( random_number >= 0.0 );
  testPrecondition( random_number <= 1.0 );
  // Make sure the upper bound of the subrange is valid
  testPrecondition( max_indep_var <= d_max_independent_value );
  testPrecondition( max_indep_var >= d_min_independent_value );

  return ThisType::sampleWithRandomNumber( d_min_independent_value,
					   max_indep_var,
					   random_number );
}

// Return a random sample and sampled index from the corresponding CDF
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleAndRecordBinIndex(
					    unsigned& sampled_bin_index ) const
{
  sampled_bin_index = 0u;

  return ThisType::sample( d_min_independent_value, d_max_independent_value );
}

// Return a random sample from the corresponding CDF in a subrange
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::sampleInSubrange(const UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity max_indep_var ) const
{
  // Make sure the upper bound of the subrange is valid
  testPrecondition( max_indep_var <= d_max_independent_value );
  testPrecondition( max_indep_var >= d_min_independent_value );

  return ThisType::sampleWithRandomNumber(
			    d_min_independent_value,
			    max_indep_var,
			    RandomNumberGenerator::getRandomNumber<double>() );
}

// Return the upper bound of the distribution independent variable
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::getUpperBoundOfIndepVar() const
{
  return d_max_independent_value;
}

// Return the lower bound of the distribution independent variable
template<typename IndependentUnit, typename DependentUnit>
typename UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::IndepQuantity
UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::getLowerBoundOfIndepVar() const
{
  return d_min_independent_value;
}

// Return the distribution type
template<typename IndependentUnit, typename DependentUnit>
UnivariateDistributionType UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::getDistributionType() const
{
  return UnitAwareUniformDistribution::distribution_type;
}

// Test if the distribution is continuous
template<typename IndependentUnit, typename DependentUnit>
bool UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::isContinuous() const
{
  return true;
}

// Method for placing the object in an output stream
template<typename IndependentUnit, typename DependentUnit>
void UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::toStream( std::ostream& os ) const
{
  this->toStreamWithLimitsDistImpl( os,
                                    std::make_pair( "dependent value", d_dependent_value ) );
}

// Save the distribution to an archive
template<typename IndependentUnit, typename DependentUnit>
template<typename Archive>
void UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::save( Archive& ar, const unsigned version ) const
{
  // Save the base class first
  ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( BaseType );

  // Save the local member data
  ar & BOOST_SERIALIZATION_NVP( d_min_independent_value );
  ar & BOOST_SERIALIZATION_NVP( d_max_independent_value );
  ar & BOOST_SERIALIZATION_NVP( d_dependent_value );
  ar & BOOST_SERIALIZATION_NVP( d_pdf_value );
}

// Load the distribution from an archive
template<typename IndependentUnit, typename DependentUnit>
template<typename Archive>
void UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::load( Archive& ar, const unsigned version )
{
  // Load the base class first
  ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( BaseType );

  // Load the local member data
  ar & BOOST_SERIALIZATION_NVP( d_min_independent_value );
  ar & BOOST_SERIALIZATION_NVP( d_max_independent_value );
  ar & BOOST_SERIALIZATION_NVP( d_dependent_value );
  ar & BOOST_SERIALIZATION_NVP( d_pdf_value );
}

// Verify that the distribution values are valid
template<typename IndependentUnit, typename DependentUnit>
void UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::verifyValidShapeParameters(
                                          const IndepQuantity& min_indep_value,
                                          const IndepQuantity& max_indep_value,
                                          const DepQuantity& multiplier )
{
  TEST_FOR_EXCEPTION( IQT::isnaninf( min_indep_value ),
		      Utility::BadUnivariateDistributionParameter,
		      "The uniform distribution cannot be "
		      "constructed because of an invalid min "
		      "independent value!" );
  
  TEST_FOR_EXCEPTION( IQT::isnaninf( max_indep_value ),
		      Utility::BadUnivariateDistributionParameter,
		      "The uniform distribution cannot be "
		      "constructed because of an invalid max "
		      "independent value!" );
  
  TEST_FOR_EXCEPTION( max_indep_value <= min_indep_value,
		      Utility::BadUnivariateDistributionParameter,
		      "The uniform distribution cannot be constructed because "
                      "of invalid independent values!" );

  TEST_FOR_EXCEPTION( DQT::isnaninf( multiplier ),
		      Utility::BadUnivariateDistributionParameter,
		      "The uniform distribution cannot be "
		      "constructed because of an invalid dependent "
		      "value!" );

  TEST_FOR_EXCEPTION( multiplier <= DQT::zero(),
		      Utility::BadUnivariateDistributionParameter,
		      "The uniform distribution cannot be "
		      "constructed because of an invalid dependent "
		      "value!" );
}

// Calculate the PDF value
template<typename IndependentUnit, typename DependentUnit>
void UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::calculatePDFValue()
{
  d_pdf_value = 1.0/(d_max_independent_value - d_min_independent_value);
}

// Test if the dependent variable can be zero within the indep bounds
template<typename IndependentUnit, typename DependentUnit>
bool UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::canDepVarBeZeroInIndepBounds() const
{
  return false;
}

// Equality comparison operator
template<typename IndependentUnit, typename DependentUnit>
bool UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::operator==( const UnitAwareUniformDistribution& other ) const
{
  return d_min_independent_value == other.d_min_independent_value &&
    d_max_independent_value == other.d_max_independent_value &&
    d_dependent_value == other.d_dependent_value;
}

// Inequality comparison operator
template<typename IndependentUnit, typename DependentUnit>
bool UnitAwareUniformDistribution<IndependentUnit,DependentUnit>::operator!=( const UnitAwareUniformDistribution& other ) const
{
  return d_min_independent_value != other.d_min_independent_value ||
    d_max_independent_value != other.d_max_independent_value ||
    d_dependent_value != other.d_dependent_value;
}

} // end Utility namespace

EXTERN_EXPLICIT_DISTRIBUTION_INST( UnitAwareUniformDistribution<void,void> );

#endif // end UTILITY_UNIFORM_DISTRIBUTION_DEF_HPP

//---------------------------------------------------------------------------//
// end Utility_UniformDistribution_def.hpp
//---------------------------------------------------------------------------//
