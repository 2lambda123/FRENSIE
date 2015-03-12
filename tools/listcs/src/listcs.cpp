//---------------------------------------------------------------------------//
//!
//! \file   listcs.cpp
//! \author Alex Robinson
//! \brief  listcs tool
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>
#include <sstream>
#include <fstream>

// Trilinos Includes
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_XMLParameterListCoreHelpers.hpp>
#include <Teuchos_CommandLineProcessor.hpp>
#include <Teuchos_FancyOStream.hpp>
#include <Teuchos_VerboseObject.hpp>

// FRENSIE Includes
#include "MonteCarlo_CrossSectionsXMLProperties.hpp"
#include "Utility_ExceptionCatchMacros.hpp"

int main( int argc, char** argv )
{
  Teuchos::RCP<Teuchos::FancyOStream> out = 
    Teuchos::VerboseObjectBase::getDefaultOStream();

  // Set up the command line options
  Teuchos::CommandLineProcessor listcs_clp;
  
  std::string cs_xml_name( "cross_sections.xml" );
  
  listcs_clp.setDocString( "List the cross sections in the cross_sections.xml "
			   "file\n" );
  listcs_clp.setOption( "cross_sections.xml",
			&cs_xml_name,
			"The name (and location) of the cross_sections.xml "
			"file" );

  // Parse the command line
  Teuchos::CommandLineProcessor::EParseCommandLineReturn
    parse_return = listcs_clp.parse( argc, argv );

  if( parse_return != Teuchos::CommandLineProcessor::PARSE_SUCCESSFUL )
  {
    listcs_clp.printHelpMessage( argv[0], *out );
    
    return parse_return;
  }
  
  // Open the cross_sections.xml file
  Teuchos::RCP<Teuchos::ParameterList> cross_sections_xml = 
    Teuchos::getParametersFromXmlFile( cs_xml_name );
							    
  // Loop over all entries in the parameter list
  Teuchos::ParameterList::ConstIterator entry_it =
    cross_sections_xml->begin();

  while( entry_it != cross_sections_xml->end() )
  {
    const Teuchos::ParameterEntry& entry = 
      cross_sections_xml->entry( entry_it );

    if( entry.isList() )
    {
      std::cout.width( 20 );
      std::cout << std::left << cross_sections_xml->name( entry_it ) << " ";
      
      const Teuchos::any& any_value = entry.getAny();

      const Teuchos::ParameterList& sublist =
	Teuchos::any_cast<Teuchos::ParameterList>( any_value );

      if( sublist.isParameter( MonteCarlo::CrossSectionsXMLProperties::nuclear_file_path_prop ) )
      {
	std::cout << "N-" << sublist.get<std::string>( 
	       MonteCarlo::CrossSectionsXMLProperties::nuclear_file_type_prop )
		  << " ";
      }

      if( sublist.isParameter( MonteCarlo::CrossSectionsXMLProperties::s_alpha_beta_file_path_prop ) )
      {
	std::cout << "SAB-" << sublist.get<std::string>(
	  MonteCarlo::CrossSectionsXMLProperties::s_alpha_beta_file_type_prop )
		  << " ";
      }

      if( sublist.isParameter( MonteCarlo::CrossSectionsXMLProperties::photonuclear_file_path_prop ) )
      {
	std::cout << "GN-" << sublist.get<std::string>(
	  MonteCarlo::CrossSectionsXMLProperties::photonuclear_file_type_prop )
		  << " ";
      }
      
      if( sublist.isParameter( MonteCarlo::CrossSectionsXMLProperties::photoatomic_file_path_prop ) )
      {
	std::cout << "G-" << sublist.get<std::string>(
	  MonteCarlo::CrossSectionsXMLProperties::photoatomic_file_type_prop )
		  << " ";
      }
      
      if( sublist.isParameter( MonteCarlo::CrossSectionsXMLProperties::electroatomic_file_path_prop ) )
      {
	std::cout << "E-" << sublist.get<std::string>(
	MonteCarlo::CrossSectionsXMLProperties::electroatomic_file_type_prop );
      }
      
      std::cout << std::endl;
    }

    ++entry_it;
  }

  return 0;
}

//---------------------------------------------------------------------------//
// end listcs.cpp
//---------------------------------------------------------------------------//
 
