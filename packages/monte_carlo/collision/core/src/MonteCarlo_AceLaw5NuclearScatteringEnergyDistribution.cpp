//---------------------------------------------------------------------------//
//!
//! \file   MonteCarlo_AceLaw5NuclearScatteringEnergyDistribution.cpp
//! \author Eli Moll
//! \brief  The nuclear law 5 scattering energy distribution class
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <limits>

// FRENSIE Includes
#include "MonteCarlo_AceLaw5NuclearScatteringEnergyDistribution.hpp"
#include "Utility_DesignByContract.hpp"
#include "Utility_ExceptionTestMacros.hpp"
#include "Utility_SearchAlgorithms.hpp"
#include "Utility_RandomNumberGenerator.hpp"

namespace MonteCarlo{

// Constructor
AceLaw5NuclearScatteringEnergyDistribution::AceLaw5NuclearScatteringEnergyDistribution(
						 EnergyDistribution& energy_distribution,
						 EnergyDistribution& probabilistic_function )
  : NuclearScatteringEnergyDistribution( 1u ),
    d_energy_distribution( energy_distribution ),
    d_probabilistic_function( probabilistic_function )
{
  // Check to make sure the energy grid is filled
  testPrecondition( d_energy_distribution.size() > 0 );
}

// Sample a scattering energy
double AceLaw5NuclearScatteringEnergyDistribution::sampleEnergy(
						    const double energy ) const
{
  // Make sure the energy is valid
  testPrecondition( energy > 0 );
  testPrecondition( energy < std::numeric_limits<double>::infinity() );

  // For Ace Law 5 (General Evaporation Spectrum), the outgoing energy is
  //   equal to the multiplication of a tabulated energy function T(E) and
  //   a randomly sampled variable X(xi)

  // Sample the probabilistic function
  double random_number =
                    Utility::RandomNumberGenerator::getRandomNumber<double>();

  double probabilistic_sample;

  // Check that the random number falls within the grid
  if( random_number >= Utility::get<0>( d_probabilistic_function.front() ) &&
      random_number <= Utility::get<0>( d_probabilistic_function.back() ) )
  {
    EnergyDistribution::const_iterator lower_bin_boundary, upper_bin_boundary;

    lower_bin_boundary = d_probabilistic_function.begin();
    upper_bin_boundary = d_probabilistic_function.end();

    lower_bin_boundary = Utility::Search::binaryLowerBound<Utility::FIRST>(
							    lower_bin_boundary,
							    upper_bin_boundary,
							    random_number );

    upper_bin_boundary = lower_bin_boundary;
    ++upper_bin_boundary;

    // Calculate the interpolation fraction
    double interpolation_fraction =
      (random_number - Utility::get<0>(*lower_bin_boundary))/
      (Utility::get<0>(*upper_bin_boundary) - Utility::get<0>(*lower_bin_boundary));

    probabilistic_sample = interpolation_fraction*
      (Utility::get<1>(*upper_bin_boundary) - Utility::get<1>(*lower_bin_boundary)) +
      Utility::get<1>(*lower_bin_boundary);
  }

  double outgoing_energy;

  // Check if energy is outside the grid
  if( energy >= Utility::get<0>(d_energy_distribution.front()) &&
      energy <= Utility::get<0>(d_energy_distribution.back()) )
  {
    EnergyDistribution::const_iterator lower_bin_boundary, upper_bin_boundary;

    lower_bin_boundary = d_energy_distribution.begin();
    upper_bin_boundary = d_energy_distribution.end();

    lower_bin_boundary = Utility::Search::binaryLowerBound<Utility::FIRST>(
							    lower_bin_boundary,
							    upper_bin_boundary,
							    energy );

    upper_bin_boundary = lower_bin_boundary;
    ++upper_bin_boundary;

    // Calculate the interpolation fraction
    double interpolation_fraction =
      (energy - Utility::get<0>(*lower_bin_boundary))/
      (Utility::get<0>(*upper_bin_boundary) - Utility::get<0>(*lower_bin_boundary));

    double T = interpolation_fraction*
      (Utility::get<1>(*upper_bin_boundary) - Utility::get<1>(*lower_bin_boundary)) +
      Utility::get<1>(*lower_bin_boundary);

    outgoing_energy = T*probabilistic_sample;
  }
  else if( energy < Utility::get<0>( d_energy_distribution.front() ) )
  {
    // If below the energy grid, use the lowest possible energy
    double T = Utility::get<1>( d_energy_distribution.front() );

    outgoing_energy = T*probabilistic_sample;
  }
  else
  {
    // If above the energy grid, use the highest possible energy
    double T = Utility::get<1>( d_energy_distribution.back() );

    outgoing_energy = T*probabilistic_sample;
  }

  return outgoing_energy;
}

} // end MonteCarlo namespace

//---------------------------------------------------------------------------//
// end MonteCarlo_AceLaw5NuclearScatteringEnergyDistribution.cpp
//---------------------------------------------------------------------------//
