//---------------------------------------------------------------------------//
//!
//! \file   Data_ACETableName.cpp
//! \author Alex Robinson
//! \brief  The ACE table name class definition
//!
//---------------------------------------------------------------------------//

// Boost Includes
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/algorithm/string.hpp>

// FRENSIE Includes
#include "Data_ACETableName.hpp"
#include "Utility_HDF5IArchive.hpp"
#include "Utility_HDF5OArchive.hpp"
#include "Utility_ExceptionTestMacros.hpp"
#include "Utility_ContractException.hpp"

namespace Data{

// Default constructor
ACETableName::ACETableName()
{ /* ... */ }

// C-string constructor
ACETableName::ACETableName( const char* raw_ace_table_name )
  : ACETableName( std::string( raw_ace_table_name ) )
{ /* ... */ }

// String constructor
ACETableName::ACETableName( const std::string& raw_ace_table_name )
  : d_raw_table_name( raw_ace_table_name ),
    d_table_name_zaid(),
    d_table_name_version(),
    d_table_name_type_key()
{
  // Make sure that the raw ace table name is valid
  testPrecondition( !raw_ace_table_name.empty() );

  // Extract the raw zaid and version from the table name
  std::vector<std::string> table_name_components;

  boost::split( table_name_components,
                raw_ace_table_name,
                boost::is_any_of( "." ) );

  TEST_FOR_EXCEPTION( table_name_components.size() != 2,
                      std::runtime_error,
                      "The table name must have a format of \"zaid.##@\"!" );

  d_table_name_zaid = Data::ZAID( Utility::fromString<unsigned>( table_name_components.front() ) );

  TEST_FOR_EXCEPTION( table_name_components.back().size() != 3,
                      std::runtime_error,
                      "The table name version/key component is invalid!" );

  d_table_name_version =
    Utility::fromString<size_t>( table_name_components.back().substr( 0, 2 ) );
  d_table_name_type_key = table_name_components.back()[2];
}

// Component constructor
ACETableName::ACETableName( const Data::ZAID& zaid,
                            const size_t table_version,
                            const char table_type_key )
  : d_raw_table_name( Utility::toString( zaid ) + "." +
                      Utility::toString( table_version ) + table_type_key ),
    d_table_name_zaid( zaid ),
    d_table_name_version( table_version ),
    d_table_name_type_key( table_type_key )
{
  TEST_FOR_EXCEPTION( table_version > 99,
                      std::runtime_error,
                      "The table version must currently be <= 99!" );
}

// Copy constructor
ACETableName::ACETableName( const ACETableName& that )
  : d_raw_table_name( that.d_raw_table_name ),
    d_table_name_zaid( that.d_table_name_zaid ),
    d_table_name_version( that.d_table_name_version ),
    d_table_name_type_key( that.d_table_name_type_key )
{ /* ... */ }

// Assignment operator
ACETableName& ACETableName::operator=( const ACETableName& that )
{
  if( this != &that )
  {
    d_raw_table_name = that.d_raw_table_name;
    d_table_name_zaid = that.d_table_name_zaid;
    d_table_name_version = that.d_table_name_version;
    d_table_name_type_key = that.d_table_name_type_key;
  }

  return *this;
}

// Equality operator
bool ACETableName::operator==( const ACETableName& that ) const
{
  return d_raw_table_name == that.d_raw_table_name;
}

// Inequality operator
bool ACETableName::operator!=( const ACETableName& that ) const
{
  return d_raw_table_name != that.d_raw_table_name;
}

// Return the zaid
const Data::ZAID& ACETableName::zaid() const
{
  return d_table_name_zaid;
}

// Return the table version
size_t ACETableName::version() const
{
  return d_table_name_version;
}

// Return the type key
char ACETableName::typeKey() const
{
  return d_table_name_type_key;
}

// Return the raw table name
const std::string& ACETableName::toRaw() const
{
  return d_raw_table_name;
}

// Implicit conversion to std::string
ACETableName::operator std::string() const
{
  return d_raw_table_name;
}

// Save the model to an archive
template<typename Archive>
void ACETableName::save( Archive& ar, const unsigned version ) const
{
  ar & boost::serialization::make_nvp( "raw_table_name", d_raw_table_name );
}

// Load the model from an archive
template<typename Archive>
void ACETableName::load( Archive& ar, const unsigned version )
{
  std::string raw_table_name;

  ar & boost::serialization::make_nvp( "raw_table_name", raw_table_name );

  *this = ACETableName( raw_table_name );
}

EXPLICIT_DATA_CLASS_SAVE_LOAD_INST( ACETableName );
  
} // end Data namespace

namespace Utility{

// Convert a Data::ACETableName to a string
std::string ToStringTraits<Data::ACETableName>::toString( const Data::ACETableName obj )
{
  return Utility::toString<std::string>( obj );
}

// Place the Data::ACETableName in a stream
void ToStringTraits<Data::ACETableName>::toStream( std::ostream& os,
                                                   const Data::ACETableName obj )
{
  return Utility::toStream<std::string>( os, obj );
}

} // end Utility namespace

BOOST_SERIALIZATION_CLASS_EXPORT_IMPLEMENT( ACETableName, Data );

//---------------------------------------------------------------------------//
// end Data_ACETableName.cpp
//---------------------------------------------------------------------------//
