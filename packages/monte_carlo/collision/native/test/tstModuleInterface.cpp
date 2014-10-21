//---------------------------------------------------------------------------//
//!
//! \file   tstModuleInterface.cpp
//! \author Alex Robinson
//! \brief  Collision module interface unit tests
//!
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <iostream>

// Trilinos Includes
#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_RCP.hpp>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_XMLParameterListCoreHelpers.hpp>
#include <Teuchos_VerboseObject.hpp>

// FRENSIE Includes
#include "MonteCarlo_CollisionHandlerFactory.hpp"
#include "MonteCarlo_CollisionModuleInterface_Native.hpp"
#include "Geometry_DagMCInstanceFactory.hpp"

//---------------------------------------------------------------------------//
// Testing Variables.
//---------------------------------------------------------------------------//
std::string test_cross_sections_xml_directory;
std::string test_material_xml_file_name;
std::string test_geom_xml_file_name;

//---------------------------------------------------------------------------//
// Testing Functions.
//---------------------------------------------------------------------------//
// Initialize the collision handler
void initializeCollisionHandler()
{
  // Assign the name of the cross_sections.xml file with path
  std::string cross_section_xml_file = test_cross_sections_xml_directory;
  cross_section_xml_file += "/cross_sections.xml";

  // Read in the xml file storing the cross section table information 
  Teuchos::ParameterList cross_section_table_info;
  Teuchos::updateParametersFromXmlFile( 
			         cross_section_xml_file,
			         Teuchos::inoutArg(cross_section_table_info) );

  // Read in the xml file storing the material specifications
  Teuchos::ParameterList material_reps;
  Teuchos::updateParametersFromXmlFile( test_material_xml_file_name,
					Teuchos::inoutArg(material_reps) );

  MonteCarlo::CollisionHandlerFactory::initializeHandlerUsingDagMC( 
					   material_reps,
					   cross_section_table_info,
					   test_cross_sections_xml_directory );
}

//---------------------------------------------------------------------------//
// Tests.
//---------------------------------------------------------------------------//
// Check if cells can be tested as void
TEUCHOS_UNIT_TEST( ModuleInterface, isCellVoid )
{
  typedef MonteCarlo::CollisionModuleInterface<MonteCarlo::CollisionHandler> CMI;
  
  TEST_ASSERT( !CMI::isCellVoid( 26 ) );
  TEST_ASSERT( !CMI::isCellVoid( 27 ) );
  TEST_ASSERT( !CMI::isCellVoid( 28 ) );
  TEST_ASSERT( !CMI::isCellVoid( 29 ) );
  TEST_ASSERT( !CMI::isCellVoid( 30 ) );
  TEST_ASSERT( !CMI::isCellVoid( 31 ) );
  TEST_ASSERT( !CMI::isCellVoid( 32 ) );
  TEST_ASSERT( !CMI::isCellVoid( 33 ) );
  TEST_ASSERT( !CMI::isCellVoid( 34 ) );
  TEST_ASSERT( !CMI::isCellVoid( 35 ) );
  TEST_ASSERT( !CMI::isCellVoid( 36 ) );
  TEST_ASSERT( !CMI::isCellVoid( 37 ) );
  TEST_ASSERT( !CMI::isCellVoid( 48 ) );
  TEST_ASSERT( !CMI::isCellVoid( 49 ) );
  TEST_ASSERT( !CMI::isCellVoid( 50 ) );
  TEST_ASSERT( !CMI::isCellVoid( 51 ) );
  TEST_ASSERT( !CMI::isCellVoid( 52 ) );
  TEST_ASSERT( !CMI::isCellVoid( 53 ) );
  TEST_ASSERT( !CMI::isCellVoid( 54 ) );
  TEST_ASSERT( !CMI::isCellVoid( 55 ) );
  TEST_ASSERT( !CMI::isCellVoid( 56 ) );
  TEST_ASSERT( !CMI::isCellVoid( 57 ) );
  TEST_ASSERT( !CMI::isCellVoid( 58 ) );
  TEST_ASSERT( !CMI::isCellVoid( 59 ) );
  TEST_ASSERT( !CMI::isCellVoid( 70 ) );
  TEST_ASSERT( !CMI::isCellVoid( 71 ) );
  TEST_ASSERT( !CMI::isCellVoid( 72 ) );
  TEST_ASSERT( !CMI::isCellVoid( 73 ) );
  TEST_ASSERT( !CMI::isCellVoid( 74 ) );
  TEST_ASSERT( !CMI::isCellVoid( 75 ) );
  TEST_ASSERT( !CMI::isCellVoid( 76 ) );
  TEST_ASSERT( !CMI::isCellVoid( 77 ) );
  TEST_ASSERT( !CMI::isCellVoid( 78 ) );
  TEST_ASSERT( !CMI::isCellVoid( 79 ) );
  TEST_ASSERT( !CMI::isCellVoid( 80 ) );
  TEST_ASSERT( !CMI::isCellVoid( 81 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 9 ) );
  TEST_ASSERT( !CMI::isCellVoid( 88 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 136 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 19 ) );
  TEST_ASSERT( !CMI::isCellVoid( 41 ) );
  TEST_ASSERT( !CMI::isCellVoid( 63 ) );
  TEST_ASSERT( !CMI::isCellVoid( 82 ) );
  TEST_ASSERT( !CMI::isCellVoid( 152 ) );
  TEST_ASSERT( !CMI::isCellVoid( 166 ) );
  TEST_ASSERT( !CMI::isCellVoid( 184 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 3 ) );
  TEST_ASSERT( !CMI::isCellVoid( 7 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 5 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 1 ) );
  
  TEST_ASSERT( !CMI::isCellVoid( 13 ) );
  TEST_ASSERT( !CMI::isCellVoid( 83 ) );
  TEST_ASSERT( !CMI::isCellVoid( 154 ) );
  TEST_ASSERT( !CMI::isCellVoid( 168 ) );
}

//---------------------------------------------------------------------------//
// Check if the macroscopic total cross section in a cell can be retrieved
TEUCHOS_UNIT_TEST( ModuleInterface, getMacroscopicTotalCrossSection )
{
  typedef MonteCarlo::CollisionModuleInterface<MonteCarlo::CollisionHandler> CMI;
  
  MonteCarlo::NeutronState neutron( 0ull );
  neutron.setEnergy( 1.0e-11 );
  neutron.setCell( 26 );
  
  double cross_section = 
    CMI::getMacroscopicTotalCrossSection( neutron );
  
  TEST_FLOATING_EQUALITY( cross_section, 5508.0178459802, 1e-13 );

  neutron.setEnergy( 2.0e1 );

  cross_section = 
    CMI::getMacroscopicTotalCrossSection( neutron );

  TEST_FLOATING_EQUALITY( cross_section, 2.2587650565199, 1e-9 );
}

//---------------------------------------------------------------------------//
// Check if the reaction cross section in a cell can be retrieved
TEUCHOS_UNIT_TEST( ModuleInterface, getMacroscopicReactionCrossSection )
{
  typedef MonteCarlo::CollisionModuleInterface<MonteCarlo::CollisionHandler> CMI;
  
  MonteCarlo::NeutronState neutron( 0ull );
  neutron.setEnergy( 1.0e-11 );
  neutron.setCell( 26 );
  
  double cross_section = 
    CMI::getMacroscopicReactionCrossSection( neutron, 
					     MonteCarlo::N__TOTAL_REACTION );
  
  TEST_FLOATING_EQUALITY( cross_section, 5508.0178459802, 1e-13 );

  neutron.setEnergy( 2.0e1 );

  cross_section = 
    CMI::getMacroscopicReactionCrossSection( neutron, 
					     MonteCarlo::N__TOTAL_REACTION );

  TEST_FLOATING_EQUALITY( cross_section, 2.2587650565199, 1e-9 );

  neutron.setEnergy( 1.0e-11 );

  cross_section = 
    CMI::getMacroscopicReactionCrossSection( neutron, 
					     MonteCarlo::N__N_ELASTIC_REACTION );
  
  TEST_FLOATING_EQUALITY( cross_section, 5429.8779115975, 1e-13 );

  neutron.setEnergy( 2.0e1 );

  cross_section = 
    CMI::getMacroscopicReactionCrossSection( neutron, 
					     MonteCarlo::N__N_ELASTIC_REACTION );

  TEST_FLOATING_EQUALITY( cross_section, 2.2586376828559, 1e-9 );
}

//---------------------------------------------------------------------------//
// Check that a neutron can collide with the material in a cell
TEUCHOS_UNIT_TEST( ModuleInterface, collideWithCellMaterial )
{
  typedef MonteCarlo::CollisionModuleInterface<MonteCarlo::CollisionHandler> CMI;
  
  MonteCarlo::NeutronState neutron( 0ull );
  neutron.setCell( 26 );
  neutron.setDirection( 0.0, 0.0, 1.0 );
  neutron.setEnergy( 1.0 );
  neutron.setWeight( 1.0 );

  MonteCarlo::ParticleBank bank;

  CMI::collideWithCellMaterial( neutron, bank, true );

  TEST_EQUALITY_CONST( neutron.getWeight(), 1.0 );

  neutron.setEnergy( 1.0 );
  
  CMI::collideWithCellMaterial( neutron, bank, true );

  TEST_EQUALITY_CONST( neutron.getWeight(), 1.0 );  

  neutron.setEnergy( 1.03125e-11 );

  CMI::collideWithCellMaterial( neutron, bank, false );

  TEST_FLOATING_EQUALITY( neutron.getWeight(), 0.98581348192787, 1e-14 );
}

//---------------------------------------------------------------------------//
// Custom main function
//---------------------------------------------------------------------------//
int main( int argc, char** argv )
{
  Teuchos::CommandLineProcessor& clp = Teuchos::UnitTestRepository::getCLP();

  clp.setOption( "test_cross_sections_xml_directory",
		 &test_cross_sections_xml_directory,
		 "Test cross_sections.xml file directory name" );

  clp.setOption( "test_mat_xml_file",
		 &test_material_xml_file_name,
		 "Test xml material file name" );

  clp.setOption( "test_geom_xml_file",
		 &test_geom_xml_file_name,
		 "Test xml geometry file name" );

  const Teuchos::RCP<Teuchos::FancyOStream> out = 
    Teuchos::VerboseObjectBase::getDefaultOStream();
  
  Teuchos::CommandLineProcessor::EParseCommandLineReturn parse_return = 
    clp.parse(argc,argv);

  if ( parse_return != Teuchos::CommandLineProcessor::PARSE_SUCCESSFUL ) {
    *out << "\nEnd Result: TEST FAILED" << std::endl;
    return parse_return;
  }

  // Initialize DagMC
  Teuchos::RCP<Teuchos::ParameterList> geom_rep = 
    Teuchos::getParametersFromXmlFile( test_geom_xml_file_name );

  Geometry::DagMCInstanceFactory::initializeDagMC( *geom_rep );

  // Initialize the random number generator
  Utility::RandomNumberGenerator::createStreams();

  // Initialize the collison handler
  initializeCollisionHandler();

  // Run the unit tests
  Teuchos::GlobalMPISession mpiSession( &argc, &argv );
  
  const bool success = Teuchos::UnitTestRepository::runUnitTests(*out);

  if (success)
    *out << "\nEnd Result: TEST PASSED" << std::endl;
  else
    *out << "\nEnd Result: TEST FAILED" << std::endl;

  clp.printFinalTimerSummary(out.ptr());

  return (success ? 0 : 1);
}

//---------------------------------------------------------------------------//
// end tstModuleInterface.cpp
//---------------------------------------------------------------------------//