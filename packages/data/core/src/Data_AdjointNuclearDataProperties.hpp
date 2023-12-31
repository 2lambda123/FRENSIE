//---------------------------------------------------------------------------//
//!
//! \file   Data_AdjointNuclearDataProperties.hpp
//! \author Alex Robinson
//! \brief  The adjoint nuclear data properties class declaration
//!
//---------------------------------------------------------------------------//

#ifndef DATA_ADJOINT_NUCLEAR_DATA_PROPERTIES_HPP
#define DATA_ADJOINT_NUCLEAR_DATA_PROPERTIES_HPP

// Boost Includes
#include <boost/filesystem/path.hpp>
#include <boost/serialization/split_member.hpp>

// FRENSIE Includes
#include "Data_NuclearDataProperties.hpp"
#include "Utility_ToStringTraits.hpp"
#include "Utility_SerializationHelpers.hpp"
#include "Utility_ExceptionTestMacros.hpp"

namespace Data{

//! The adjoint nuclear data properties base class
class AdjointNuclearDataProperties
{

public:

  /*! The file types
   *
   * When adding a new file type the toString and serialize method must be
   * updated.
   */
  enum FileType{
    Native_FILE = 0
  };

  //! The energy unit
  typedef NuclearDataProperties::EnergyUnit EnergyUnit;

  //! The energy quantity
  typedef NuclearDataProperties::Energy Energy;

  //! The temperature unit
  typedef NuclearDataProperties::TemperatureUnit TemperatureUnit;

  //! The temperature quantity
  typedef NuclearDataProperties::Temperature Temperature;

  //! Default constructor
  AdjointNuclearDataProperties();

  //! Destructor
  virtual ~AdjointNuclearDataProperties()
  { /* ... */ }

  //! Get the ZAID that the file specifies data for
  virtual Data::ZAID zaid() const = 0;

  //! Get the atomic weight ratio of the nuclide that the file specifies data for
  virtual double atomicWeightRatio() const = 0;

  //! Get the adjoint nuclear data evaluation temperature (MeV)
  virtual Energy evaluationTemperatureInMeV() const = 0;

  //! Get the adjoint nuclear data evaluation temperature (Kelvin)
  virtual Temperature evaluationTemperature() const;

  //! Get the adjoint nuclear data file type
  virtual FileType fileType() const = 0;

  //! Get the adjoint nuclear data file path (relative to the data directory)
  virtual boost::filesystem::path filePath() const = 0;

  //! Get the adjoint nuclear data file start line
  virtual size_t fileStartLine() const = 0;

  //! Get the nuclear data file major version
  virtual unsigned fileMajorVersion() const;

  //! Get the nuclear data file version
  virtual unsigned fileVersion() const = 0;

  //! Get the adjoint nuclear data file table name
  virtual std::string tableName() const = 0;

  //! Clone the properties
  virtual AdjointNuclearDataProperties* clone() const = 0;

private:

  // Save the properties to an archive
  template<typename Archive>
  void save( Archive& ar, const unsigned version ) const
  { /* ... */ }

  // Load the properties from an archive
  template<typename Archive>
  void load( Archive& ar, const unsigned version )
  { /* ... */ }

  BOOST_SERIALIZATION_SPLIT_MEMBER();

  // Declare the boost serialization access object as a friend
  friend class boost::serialization::access;
};

} // end Data namespace

namespace Utility{

/*! \brief Specialization of Utility::ToStringTraits for
 * Data::AdjointNuclearDataProperties::FileType
 * \ingroup to_string_traits
 */
template<>
struct ToStringTraits<Data::AdjointNuclearDataProperties::FileType>
{
  //! Convert a Data::AdjointNuclearDataProperties::FileType to a string
  static std::string toString( const Data::AdjointNuclearDataProperties::FileType type );

  //! Place the Data::AdjointNuclearDataProperties::FileType in a stream
  static void toStream( std::ostream& os, const Data::AdjointNuclearDataProperties::FileType type );
};

} // end Utility namespace

namespace std{

//! Stream operator for printing Data::AdjointNuclearDataProperties::FileType enums
inline std::ostream& operator<<( std::ostream& os,
                                 const Data::AdjointNuclearDataProperties::FileType type )
{
  Utility::toStream( os, type );

  return os;
}

} // end std namespace

namespace boost{

namespace serialization{

//! Serialize the file type enum
template<typename Archive>
void serialize( Archive& archive,
                Data::AdjointNuclearDataProperties::FileType& file_type,
                const unsigned version )
{
  if( Archive::is_saving::value )
    archive & (int)file_type;
  else
  {
    int raw_file_type;

    archive & raw_file_type;

    switch( raw_file_type )
    {
      BOOST_SERIALIZATION_ENUM_CASE( Data::AdjointNuclearDataProperties::Native_FILE, int, file_type );
      default:
      {
        THROW_EXCEPTION( std::logic_error,
                         "Unable to deserialize file type "
                         << raw_file_type << "!" );
      }
    }
  }
}

} // end serialization namespace

} // end boost namespace

BOOST_SERIALIZATION_ASSUME_ABSTRACT_CLASS( AdjointNuclearDataProperties, Data );
BOOST_SERIALIZATION_CLASS_VERSION( AdjointNuclearDataProperties, Data, 0 );


#endif // end DATA_ADJOINT_NUCLEAR_DATA_PROPERTIES_HPP

//---------------------------------------------------------------------------//
// end Data_AdjointNuclearDataProperties.hpp
//---------------------------------------------------------------------------//
