//---------------------------------------------------------------------------//
//!
//! \file   Utility_HDF5CommonArchive.hpp
//! \author Alex Robinson
//! \brief  The hdf5 archive base class declaration
//!
//---------------------------------------------------------------------------//

#ifndef UTILITY_HDF5_COMMON_ARCHIVE_HPP
#define UTILITY_HDF5_COMMON_ARCHIVE_HPP

// FRENSIE Includes
#include "Utility_HDF5File.hpp"
#incluce "Utility_HDF5ArchiveException.hpp"
#include "Utility_ExceptionTestMacros.hpp"
#include "Utility_ExceptionCatchMacros.hpp"

namespace Utility{

/*! The HDF5 common archive base class
 * \ingroup hdf5
 */
class HDF5CommonArchive : public HDF5File
{

public:

  //! Return the archive properties directory
  static const std::string& getPropertiesDir();

  //! Return the hdf5 data directory
  static const std::string& getDataDir();

  //! Return the hdf5 tracked objects directory
  static const std::string& getTrackedObjectsDir();

  //! Return the hdf5 tree directory
  static const std::string& getTreeDir();

  //! Return the signature attribute name
  static const std::string& getSignatureAttributeName();

  //! Return the version attribute name
  static const std::string& getVersionAttributeName();

  //! Return the hdf5 object data path
  static std::string getObjectDataPath( const size_t object_count );

  //! Return the hdf5 tracked objects path
  static std::string getTrackedObjectsPath( const unsigned object_count );

private:

  // The properties directory
  static const std::string s_properties_dir;

  // The data directory
  static const std::string s_data_dir;

  // The tracked objects directory
  static const std::string s_tracked_objects_dir;

  // The tree directory
  static const std::string s_tree_dir;

  // The signature attribute name
  static const std::string s_signature_attribute_name;

  // The version attribute name
  static const std::string s_version_attribute_name;
};
  
} // end Utility namespace

/*! \brief Test for a boost archive exception
 * \ingroup hdf5
 */
#define TEST_FOR_ARCHIVE_EXCEPTION( throw_exception_test, error_type ) \
{                                                                     \
  const bool throw_exception = (throw_exception_test);                \
  if( throw_exception )                                               \
  {                                                                 \
    boost::serialization::throw_exception(                          \
                                    boost::archive_exception( error_type ) ); \
  }                                                                     \
}

/*! \brief Throw a boost::archive::archive_exception
 * \ingroup hdf5
 */
#define THROW_ARCHIVE_EXCEPTION( error_type ) \
  boost::serialization::throw_exception(                                \
       boost::archive::archive_exception( error_type ) ) 

/*! \brief Throw a Utility::HDF5ArchiveException
 * \ingroup hdf5
 */
#define THROW_HDF5_ARCHIVE_EXCEPTION( raw_msg ) \
  THROW_EXCEPTION( Utility::HDF5ArchiveException, raw_msg )

/*! \brief Catch statement macro for catching a Utility::HDF5File::Exception 
 * and rethrowing them as a Utility::HDF5ArchiveException.
 * 
 * \ingroup hdf5
 */
#define HDF5_FILE_EXCEPTION_CATCH_RETHROW( raw_msg ) \
EXCEPTION_CATCH_RETHROW_AS( Utility::HDF5File::Exception, Utility::HDF5ArchiveException, raw_msg ) \
catch( ... )                                                           \
{                                                                    \
  THROW_EXCEPTION( HDF5ArchiveException, raw_msg );                  \
}                                                                   

#endif // end UTILITY_HDF5_COMMON_ARCHIVE_HPP

//---------------------------------------------------------------------------//
// end Utility_HDF5CommonArchive.hpp
//---------------------------------------------------------------------------//
