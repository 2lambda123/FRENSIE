//---------------------------------------------------------------------------//
//!
//! \file   Utility_HDF5File.cpp
//! \author Alex Robinson
//! \brief  The hdf5 file class definition
//!
//---------------------------------------------------------------------------//

// FRENSIE Includes
#include "Utility_HDF5File.hpp"
#include "Utility_LoggingMacros.hpp"

namespace Utility{

namespace Details{

// Enable exceptions in hdf5
void enableExceptionsInHDF5()
{
  static bool hdf5_exceptions_enabled = false;
  
  try{
    H5::Exception::dontPrint();
    hdf5_exceptions_enabled = true;
  }
  HDF5_EXCEPTION_CATCH( "Could not enable exceptions in HDF5!" );
}
  
} // end Details namespace

/*! \brief Specialization of Utility::HDF5TypeTraits for void
 * \ingroup hdf5_type_traits
 */
template<>
struct HDF5TypeTraits<void> : public Details::BasicHDF5TypeTraits<void,1>
{
  //! Returns the HDF5 data type object corresponding to void
  static inline H5::PredType dataType()
  { return H5::NATIVE_OPAQUE; }
};

// Constructor
HDF5File::Exception::Exception( const std::string& message )
  : std::runtime_error( message )
{ /* ... */ }

// Constructor (extra details)
HDF5File::Exception::Exception( const std::string& file,
                                const size_t line,
                                const std::string& hdf5_function_name,
                                const std::string& hdf5_error_message,
                                const std::string& message )
  : std::runtime_error( HDF5File::Exception::createErrorMessage( file, line, hdf5_function_name, hdf5_error_message, message ) )
{ /* ... */ }

// Create an error message
std::string HDF5File::Exception::createErrorMessage(
                                         const std::string& file,
                                         const size_t line,
                                         const std::string& hdf5_function_name,
                                         const std::string& hdf5_error_message,
                                         const std::string& message )
{
  std::ostringstream oss detailed_message;

  detailed_message << FRENSIE_LOG_ERROR_MSG << msg << "\n"
                   << FRENSIE_LOG_EXCEPTION_TYPE_MSG
                   << "H5::Exception" << FRENSIE_LOG_ARROW_SEP
                   << "Utility::HDF5File::Exception" << "\n"
                   << "HDF5 " << FRENSIE_LOG_STACK_MSG_BASIC << " "
                   << hdf5_function_name << "\n"
                   << "HDF5 Error Msg: " << hdf5_error_message << "\n"
                   << FRENSIE_LOG_LOCATION_MSG
                   << file << FRENSIE_LOG_FILE_LINE_SEP << line << "\n";

  return detailed_message.str();
}

// Constructor
HDF5File::HDF5File( const std::string& filename, HDF5File::OpenMode mode )
  : d_filename( filename ),
    d_hdf5_file()
{
  Details::enableExceptionsInHDF5();
  
  switch( mode )
  {
    case READ_ONLY:
    {
      try{
        d_hdf5_file.reset( new H5::H5File( filename, H5F_ACC_RDONLY ) );
      }
      HDF5_EXCEPTION_CATCH( "Could not open hdf5 file " << filename <<
                            " in read-only mode!" );
    }
    case READ_WRITE:
    {
      try{
        d_hdf5_file.reset( new H5::H5File( filename, H5F_ACC_RDWR ) );
      }
      HDF5_EXCEPTION_CATCH( "Could not open hdf5 file " << filename <<
                            " in read-write mode!" );
    }
    case OVERWRITE:
    {
      try{
        d_hdf5_file.reset( new H5::H5File( filename, H5F_ACC_TRUNC ) );
      }
      HDF5_EXCEPTION_CATCH( "Could not open and overwrite hdf5 file "
                            << filename << "!" );
    }
    default:
    {
      THROW_EXCEPTION( HDF5File::Exception, "Unknown HDF5 file open mode!" );
    }
  }
}

// Destructor
HDF5File::~HDF5File()
{ /* ... */ }

// Get the file name
const std::string& HDF5File::getFilename() const
{
  return d_filename;
}

// Check if the group exists
bool HDF5File::doesGroupExist( const std::string& path_to_group ) const
{
  try{
    std::unique_ptr<const H5::Group> group;
    this->openGroup( path_to_group, group );
  }
  catch( ... )
  {
    return false;
  }
  
  return true;
}

// Check if the group attribute exists
bool HDF5File::doesGroupAttributeExist( const std::string& path_to_group,
                                        const std::string& attribute_name ) const
{
  if( this->doesGroupExist( group_location ) )
  {
    std::unique_ptr<const H5::Group> group;
    this->openGroup( path_to_group, group );

    return this->doesGroupAttributeExist( *group, attribute_name );
  }
  else
    return false;
}

// Check if the group attribute exists
bool HDF5File::doesGroupAttributeExist( const H5::Group& group,
                                        const std::string& attribute_name ) const
{
  return group.attrExists( attribute_name );
}

// Check if the data set exists
bool HDF5File::doesDataSetExist( const std::string& path_to_data_set ) const
{
  try{
    std::unique_ptr<const H5::DataSet> data_set;
    this->openDataSet( path_to_data_set, data_set );
  }
  catch( ... )
  {
    return false;
  }

  return true;
}

// Check if the data set attribute exists
bool HDF5File::doesDataSetAttributeExist( const std::string& path_to_data_set,
                                          const std::string& attribute_name ) const
{
  if( this->doesDataSetExist( path_to_data_set ) )
  {
    std::unique_ptr<const H5::DataSet> data_set;
    this->openDataSet( path_to_data_set, data_set );

    return this->doesDataSetAttributeExist( *data_set, attribute_name );
  }
  else
    return false;
}

// Check if the data set attribute exists
bool HDF5File::doesDataSetAttributeExist( const H5::DataSet& data_set,
                                          const std::string& attribute_name ) const
{
  return data_set.attrExists( attribute_name );
}

// Get the size of a data set
hsize_t HDF5File::getDataSetSize( const std::string& path_to_data_set ) const
{
  std::unique_ptr<H5::DataSet> data_set;
  
  this->openDataSet( path_to_data_set, data_set );
  
  return this->getDataSetSize( *data_set );
}

// Get the size of a data set
hsize_t HDF5File::getDataSetSize( const H5::DataSet& data_set ) const
{
  return this->getDataSpaceSize( data_set.getSpace() );
}

// Get the size of a data set attribute
hsize_t HDF5File::getDataSetAttributeSize( const std::string& path_to_data_set,
                                           const std::string& attribute_name ) const
{
  std::unique_ptr<const H5::DataSet> data_set;
  
  this->openDataSet( path_to_data_set, data_set );

  std::unique_ptr<const H5::Attribute> attribute;

  this->openDataSetAttribute( *data_set, attribute_name, attribute );

  return this->getAttributeSize( *attribute );
}

// Get the size of a group attribute
hsize_t HDF5File::getGroupAttributeSize( const std::string& path_to_group,
                                         const std::string& attribute_name ) const
{
  std::unique_ptr<const H5::Group> group;

  this->openGroup( path_to_group, group );

  std::unique_ptr<const H5::Attribute> attribute;

  this->openGroupAttribute( *group, attribute_name, attribute );

  return this->getAttributeSize( *attribute );
}

// Get the size of an attribute
hsize_t HDF5File::getAttributeSize( const H5::Attribute& attribute ) const
{
  return this->getDataSpaceSize( attribute.getSpace() );
}

// Get the size of a data space
hsize_t HDF5File::getDataSpaceSize( const H5::DataSpace& data_space ) const
{
  int rank = data_space.getSimpleExtentNdims();

  hsize_t dimension_sizes[rank];
    
  data_space.getSimpleExtentDims( dimension_sizes, NULL );

  hsize_t size = 0;
  
  for( int i = 0; i < rank; ++i )
    size += dimension_sizes[i];
  
  return size;
}

// Create a hard link
void HDF5File::createHardLink( const std::string& source_path,
                               const std::string& target_path )
{
  try{
    d_hdf5_file->link( H5L_TYPE_HARD, source_path, target_path );
  }
  HDF5_EXCEPTION_CATCH( "Could not create a hard link with name "
                        << target_path << " that points to source "
                        << source_path << "!" );
}

// Create a soft link
void HDF5File::createSoftLink( const std::string& source_path,
                               const std::string& target_path )
{
  try{
    d_hdf5_file->link( H5L_TYPE_SOFT, source_path, target_path );
  }
  HDF5_EXCEPTION_CATCH( "Could not create a soft link with name "
                        << target_path << " that points to source "
                        << source_path << "!" );
}

// Write opaque data to a data set
void writeToDataSet( const std::string& path_to_data_set,
                     const void* const data,
                     const size_t size )
{
  this->writeToDataSet( path_to_data_set, data, size );
}

// Read data from a data set
void HDF5File::readFromDataSet( const std::string& path_to_data_set,
                                const void* data,
                                const size_t size ) const
{
  this->readFromDataSet( path_to_data_set, data, size );
}

// Write data to a data set attribute
void HDF5File::writeToDataSetAttribute( const std::string& path_to_data_set,
                                        const std::string& attribute_name,
                                        const void* const data,
                                        const size_t size )
{
  this->writeToDataSetAttribute( path_to_data_set, attribute_name, data, size );
}

// Read data from a data set attribute
void HDF5File::readFromDataSetAttribute( const std::string& path_to_data_set,
                                         const std::string& attribute_name,
                                         const void* data,
                                         const size_t size ) const
{
  this->readFromDataSetAttribute( path_to_data_set, attribute_name, data, size );
}

// Write data to a group attribute
void HDF5File::writeToGroupAttribute( const std::string& path_to_group,
                                      const std::string& attribute_name,
                                      const void* const data,
                                      const size_t size )
{
  this->writeToGroupAttribute( path_to_group, attribute_name, data, size );
}

// Read data from a group attribute
void HDF5File::readFromGroupAttribute( const std::string& path_to_group,
                                       const std::string& attribute_name,
                                       const void* data,
                                       const size_t size ) const
{
  this->readFromGroupAttribute( path_to_group, attribute_name, data, size );
}

// Extract the parent group path
std::string HDF5File::extractParentGroupPath( const std::string& path ) const
{
  // Start the search for the group name deliminator from the second-to-last
  // character in case the current path is a group path (ends with "/")
  if( path.size() > 1 )
  {
    size_t loc = path.find_last_of( "/", path.size()-2 );

    return path.substr( 0, loc+1 );
  }
  else
    return path;
}

// Check if the parent group exists
bool HDF5File::doesParentGroupExist( const std::string& path ) const
{
  try{
    std::unique_ptr<const H5::Group> group;
    this->openGroup( this->extractParentGroupPath( path ), group );
  }
  catch( ... )
    return false;

  return true;
}

// Create the parent group, if necessary, for the specified path
void HDF5File::createParentGroup( const std::string& path )
{
  // Extract the parent group path
  std::string parent_group_path = this->extractParentGroupPath( path );

  std::unique_ptr<const H5::Group> group;
  this->createGroup( this->extractParentGroupPath( path ), group );
}

// Create a group
void HDF5File::createGroup( const std::string& path_to_group,
                            std::unique_ptr<const H5::Group>& group )
{
  // Recursively create any parent groups that are missing
  if( !this->doesParentGroupExist( path_to_group ) )
    this->createParentGroup( path_to_group );
  
  try{
    group.reset( new H5::Group( d_hdf5_file->createGroup( path_to_group ) ) );
  }
  HDF5_EXCEPTION_CATCH( "Could not create group " << path_to_group << "!" );
}

// Open a group
void HDF5File::openGroup( const std::string& path_to_group,
                          std::unique_ptr<const H5::Group>& group ) const
{
  try{
    group.reset( new H5::Group( d_hdf5_file->openGroup( path_to_group ) ) );
  }
  HDF5_EXCEPTION_CATCH( "Could not open group at location "
                        << path_to_group << "!" );
}

// Open a data set
void HDF5File::openDataSet( const std::string& path_to_data_set,
                            std::unique_ptr<const H5::DataSet>& data_set ) const
{
  try{
    data_set.reset( new H5::DataSet( d_hdf5_file->openDataSet( path_to_data_set ) ) );
  }
  HDF5_EXCEPTION_CATCH( "Could not open data set at location "
                        << path_to_data_set << "!" );
}

// Open a data set attribute
void HDF5File::openDataSetAttribute( const H5::DataSet& data_set,
                                     const std::string& attribute_name,
                                     std::unique_ptr<const H5::Attribute>& attribute ) const
{
  try{
    attribute.reset( new H5::Attribute( data_set.openAttribute( attribute_name ) ) );
  }
  HDF5_EXCEPTION_CATCH( "Could not open data set attribute "
                        << path_to_data_set << ":" << attribute_name << "!" );
}
  
} // end Utility namespace

//---------------------------------------------------------------------------//
// end Utility_HDF5File.cpp
//---------------------------------------------------------------------------//
