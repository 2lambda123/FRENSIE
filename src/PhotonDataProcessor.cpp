//---------------------------------------------------------------------------//
// \file PhotonDataProcessor.cpp
// \author Alex Robinson
// \brief Photon Data Processor definitions
//---------------------------------------------------------------------------//

// Std Lib Includes
#include <cmath>
#include <fstream>

// FACEMC Includes
#include "PhotonDataProcessor.hpp"
#include "FACEMC_Assertion.hpp"
#include "HDF5DataFileNames.hpp"
#include "Tuple.hpp" 
#include "ElectronShells.hpp"

namespace FACEMC{

//! Constructor
PhotonDataProcessor::PhotonDataProcessor( const std::string epdl_file_name,
					  const std::string eadl_file_name,
					  const std::string compton_file_prefix,
					  const std::string output_directory,
					  const double energy_min,
					  const double energy_max )
  : EPDL97DataProcessor(), 
    d_epdl_file_name(epdl_file_name), 
    d_eadl_file_name(eadl_file_name),
    d_compton_file_prefix(compton_file_prefix), 
    d_output_directory( output_directory ),
    d_energy_min(energy_min),
    d_energy_max(energy_max)
{ 
  FACEMC_ASSERT_ALWAYS( d_energy_min > 0.0 );
  FACEMC_ASSERT_ALWAYS( d_energy_min < d_energy_max );
}
  
/*! Process Photon Data Files
 * \brief Unfortunately, the data file processing must be done in this order.
 * This is because when an HDF5 file is opened, one must specify if the file
 * will be overwritten, appended and/or read. The processEPDLFile() member 
 * function will overwrite an existing HDF5 file while the two other member
 * functions will only append to an existing HDF5 file.
 */
void PhotonDataProcessor::processDataFiles()
{
  processEPDLFile();
  processEADLFile();
  processComptonFiles();
}

//! Process EPDL file
void PhotonDataProcessor::processEPDLFile()
{ 
  // EPDL file
  std::ifstream epdl( d_epdl_file_name.c_str() );
  FACEMC_ASSERT_ALWAYS( epdl );

  // HDF5 File Handler
  HDF5FileHandler hdf5_file_handler;

  // Atomic number of element currently being processed
  unsigned int atomic_number = 0;

  // Information in first header of the EPDL file
  unsigned int atomic_number_in_table = 0;
  unsigned int outgoing_particle_designator = 0;
  double atomic_weight = 0.0;
  unsigned int interpolation_flag = 0;

  // Information in the second header of the EPDL file
  unsigned int reaction_type = 0;
  unsigned int electron_shell = 0;

  // Electron shells with Photoelectric data
  Teuchos::Array<unsigned int> photoelectric_shells;
  
  // Process every element (Z=1-100) in the EPDL file
  while( epdl )
  {
    
    // Read first table header and determine which element is being processed
    readFirstTableHeader( epdl,
			  atomic_number_in_table,
			  outgoing_particle_designator,
			  atomic_weight,
			  interpolation_flag );
    
    // If a new element is found, close the current HDF5 file and open a new one
    if( atomic_number != atomic_number_in_table )
    {
      if( atomic_number != 0 )
      {
	// Create an attribute for the shells with Photoelectric data
	if( photoelectric_shells.size() == 0)
	  photoelectric_shells.push_back( 0 );
	  
	hdf5_file_handler.writeArrayToGroupAttribute( photoelectric_shells,
							PHOTOELECTRIC_SUBSHELL_CROSS_SECTION_ROOT,
							PHOTOELECTRIC_SHELL_ATTRIBUTE
							);
	photoelectric_shells.clear();
	  
	hdf5_file_handler.closeHDF5File();
      }

      // Check that the EPDL file is still valid (eof has not been reached)
      if( epdl.eof() )
	continue;
      
      atomic_number = atomic_number_in_table;

      // Open a new HDF5 file
      std::ostringstream file_number;
      file_number << atomic_number;
      std::string hdf5_file_name = d_output_directory + 
	PHOTON_DATA_FILE_PREFIX + file_number.str() + DATA_FILE_SUFFIX;
      hdf5_file_handler.openHDF5FileAndOverwrite( hdf5_file_name );
      
      // Create a top level attribute to store the atomic weight
      hdf5_file_handler.writeValueToGroupAttribute( atomic_weight,
						      ROOT,
						      ATOMIC_WEIGHT_ATTRIBUTE );

      // Create a top level attribute to store the energy limits
      Teuchos::Array<double> energy_limits( 2 );
      energy_limits[0] = d_energy_min;
      energy_limits[1] = d_energy_max;
      hdf5_file_handler.writeArrayToGroupAttribute( energy_limits,
						      ROOT,
						      ENERGY_LIMITS_ATTRIBUTE );
    }

    // Read second table header and determine the reaction type
    readSecondTableHeader( epdl,
			   reaction_type,
			   electron_shell );

    // Read and process the data in the current table, then store in the HDF5
    // file
    switch( reaction_type )
    {
  
    
    case 71000:
      // Read in the integrated coherent cross section data
      
      // The interpolation flag should be log-log (5)
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 COHERENT_CROSS_SECTION_LOC );
      }
      
      break;

    case 71010:
      // Average energy of scattered photon from coherent scattering ignored
      
      skipTwoColumnTable( epdl );
      
      break;

    case 72000:
      // Read in the integrated incoherent cross section data
      
      // The interpolation flag should be log-log (5)
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 INCOHERENT_CROSS_SECTION_LOC );
      }
      
      break;

    case 72010:
      // Average energy of scattered photon from incoherent scattering 
      // currently ignored ( Yo == 7 )
      // Average energy of scattered electron from incoherent scattering
      // currently ignored ( Yo == 9 )
      
      skipTwoColumnTable( epdl );

      break;

    case 73000:
      // Read the total integrated photoelectric cross section
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      
      if( electron_shell == 0 )
      {	
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 PHOTOELECTRIC_CROSS_SECTION_LOC );
      }
      // Read the total integrated photoelectric cross section for a subshell
      else
      {	
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 PHOTOELECTRIC_SUBSHELL_CROSS_SECTION_ROOT + uintToShellStr( electron_shell ) );

	photoelectric_shells.push_back( electron_shell );
      }
      break;

    case 73011:
      // Average energy to residual atom from photoelectric effect currently
      // ignored.
      
      skipTwoColumnTable( epdl );

      break;

    case 73010:
      // Average energy of secondary photon from photoelectric effect ignored 
      // ( Yo == 7 )
      // Average energy of secondary electron from photoelectric effect ignored
      // ( Yo == 9 )

      skipTwoColumnTable( epdl );

      break;

    case 74000:
      // Read the integrated pair production cross section
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );
      
	// Approximate the cross section as constant in the first bin 
	// (not linear)
	if( d_energy_min < 1.022 )
	  data[0].second = data[1].second;

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 PAIR_PRODUCTION_CROSS_SECTION_LOC );
      }
      
      break;

    case 74010:
      // Average energy of secondary positron from pair production ignored
      // ( Yo == 8 )
      // Average energy of secondary electron from pair production ignored
      // ( Yo == 9 )
      
      skipTwoColumnTable( epdl );

      break;

    case 75000:
      // Read the integrated triplet production cross section
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTableInRange<LogLogDataProcessingPolicy>( epdl,
							       data,
							       d_energy_min,
							       d_energy_max );
	
	// Approximate the cross section as constant in the first bin 
	// (not linear)
	if( d_energy_min < 2.044 )
	  data[0].second = data[1].second;

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 TRIPLET_PRODUCTION_CROSS_SECTION_LOC );
      }
      
      break;

    case 75010:
      // Average energy of secondary positron from triplet production ignored
      // ( Yo == 8 )
      // Average energy of secondary electron from triplet production ignored
      // ( Yo == 9 )

      skipTwoColumnTable( epdl );

      break;

    case 93941:
      // Read the atomic form factor
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Quad<double,double,double,double> > data;
	
	readTwoColumnTable<LinearLinearDataProcessingPolicy>( epdl,
							      data );
	// For efficient sampling, the atomic form factor must be squared and
	// integrated over its squared argument
	for( unsigned int i; i < data.size(); i++ )
	{
	  data[i].first *= data[i].first;
	  data[i].second *= data[i].second;
	}

	createContinuousCDFAtFourthTupleLoc( data );
	
	calculateSlopesAtThirdTupleLoc( data );

	hdf5_file_handler.writeArrayToDataSet( data,
						 ATOMIC_FORM_FACTOR_LOC );
      }
      
      break;

    case 93942:
      // Read the scattering function
      FACEMC_ASSERT_ALWAYS( interpolation_flag == 5 );
      {
	Teuchos::Array<Trip<double,double,double> > data;
	
	readTwoColumnTable<LogLogDataProcessingPolicy>( epdl,
							data );
	
	// The first data point needs to be erased since it is always
	// (0.0, 0.0)
	data.erase( data.begin() );

	calculateSlopesAtThirdTupleLoc( data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 SCATTERING_FUNCTION_LOC );
      }
      
      break;

    case 93943:
      // Imaginary anomalous scattering factor ignored
       
      skipTwoColumnTable( epdl );

      break;

    case 93944:
      // Real anomalous scattering factor ignored
      
      skipTwoColumnTable( epdl );

      break;

    default:
      // Unknown reaction type found
      {
	bool known_reaction_type = false;
	FACEMC_ASSERT_ALWAYS( known_reaction_type );
      }
      break;
    }
  }
  
  // Close the last HDF5 file
  hdf5_file_handler.closeHDF5File();

  // Close the EPDL data file
  epdl.close();
}

//! Process EADL file 
void PhotonDataProcessor::processEADLFile()
{
  // EPDL file
  std::ifstream eadl( d_eadl_file_name.c_str() );
  FACEMC_ASSERT_ALWAYS( eadl );

  // HDF5 file handler
  HDF5FileHandler hdf5_file_handler;

  // Atomic number of element currently being processed
  unsigned int atomic_number = 0;

  // Information in first header of the EPDL file
  unsigned int atomic_number_in_table = 0;
  unsigned int outgoing_particle_designator = 0;
  double atomic_weight = 0.0;
  unsigned int interpolation_flag = 0;

  // Information in the second header of the EPDL file
  unsigned int reaction_type = 0;
  unsigned int electron_shell = 0;

  // Electron Shells with relaxation data
  Teuchos::Array<unsigned int> relaxation_shells;
  
  // Process every element (Z=1-100) in the EPDL file
  while( eadl )
  {
    // Read first table header and determine which element is being processed
    readFirstTableHeader( eadl,
			  atomic_number_in_table,
			  outgoing_particle_designator,
			  atomic_weight,
			  interpolation_flag );

    // If a new element is found, close the current HDF5 file and open a new one
    if( atomic_number != atomic_number_in_table )
    {
      if( atomic_number != 0 )
      {
	// Create an attribute for the shells with atomic relaxation data
	// Note: only Z=6 and above have data
	if( relaxation_shells.size() > 0 )
	{
	  hdf5_file_handler.writeArrayToGroupAttribute( relaxation_shells,
							  RADIATIVE_TRANSITION_PROBABILITY_ROOT,
							  ATOMIC_RELAXATION_SHELL_ATTRIBUTE
							  );
	  relaxation_shells.clear();
	}
	
	hdf5_file_handler.closeHDF5File();
      }

      // Check that the EPDL file is still valid (eof has not been reached)
      if( eadl.eof() )
	continue;
      
      atomic_number = atomic_number_in_table;

      // Open a new HDF5 file
      std::ostringstream file_number; 
      file_number << atomic_number;
      std::string hdf5_file_name = d_output_directory + 
	PHOTON_DATA_FILE_PREFIX + file_number.str() + DATA_FILE_SUFFIX;
      hdf5_file_handler.openHDF5FileAndAppend( hdf5_file_name );
    }

    // Read second table header and determine the reaction type
    readSecondTableHeader( eadl,
			   reaction_type,
			   electron_shell );

    // Read and process the data in the current table, then store in the HDF5
    // file
    switch( reaction_type )
    {
    case 91912:
      // Read number of electrons per subshell
      {
	Teuchos::Array<Pair<unsigned int,double> > data;
	
	readTwoColumnTable<LinearLinearDataProcessingPolicy>( eadl,
							      data );
	
	// Z = 1 and Z = 2 only have one electron shell
	if( data.size() > 1 )
	  createDiscreteCDFAtSecondTupleLoc( data );
	else
	  data[0].second = 1.0;
	
	// Create the Electron Shell Index Map
	Teuchos::Array<Pair<unsigned int,unsigned int > >
	  electron_shell_index_map;      
	createElectronShellIndexMap( atomic_number,
				     electron_shell_index_map );
	
	FACEMC_ASSERT( (electron_shell_index_map.size() == 
			data.size()) );
	FACEMC_ASSERT( (electron_shell_index_map[0].first ==
			data[0].first) );
	FACEMC_ASSERT( (electron_shell_index_map.back().first ==
			data.back().first) );

	// Create the complete data array
	Teuchos::Array<Trip<double,unsigned int,unsigned int> > 
	  complete_data( data.size() );
	
	for( int i = 0; i < data.size(); ++i )
	{
	  complete_data[i].first = data[i].second;
	  complete_data[i].second = electron_shell_index_map[i].first;
	  complete_data[i].third = electron_shell_index_map[i].second;
	}
	
	hdf5_file_handler.writeArrayToDataSet( complete_data,
						 ELECTRON_SHELL_CDF_LOC );
      }
      
      break;

    case 91913:
      // Read binding energy per subshell
      {
	Teuchos::Array<Pair<unsigned int,double> > data;
	
	readTwoColumnTable<LinearLinearDataProcessingPolicy>( eadl,
							      data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 ELECTRON_SHELL_BINDING_ENERGY_LOC );
      }
      
      break;

    case 91914:
      // Read kinetic energy per subshell
      {
	Teuchos::Array<Pair<unsigned int,double> > data;
	
	readTwoColumnTable<LinearLinearDataProcessingPolicy>( eadl,
							      data );
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 ELECTRON_SHELL_KINETIC_ENERGY_LOC );
      }
      
      break;

    case 92931:
      // Read radiative transition probability per subshell
      {
	Teuchos::Array<Trip<unsigned int,double,double> > data;
	
	readThreeColumnTable( eadl,
			      data );
	
	// Calculate the total radiative transition probability for
	// this subshell and store it in an attribute
	double total_radiative_trans_prob = 0.0;
	for( int i = 0; i < data.size(); ++i )
	  total_radiative_trans_prob += data[i].second;

	// Only create the cdf if more than one data point is present
	if( data.size() > 1 )
	  createDiscreteCDFAtSecondTupleLoc( data );
	else
	  data[0].second = 1.0;
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 RADIATIVE_TRANSITION_PROBABILITY_ROOT + uintToShellStr( electron_shell ) );

	hdf5_file_handler.writeValueToDataSetAttribute( total_radiative_trans_prob,
							  RADIATIVE_TRANSITION_PROBABILITY_ROOT + uintToShellStr( electron_shell ),
							  TOTAL_RAD_TRANS_PROB_ATTRIBUTE );

	relaxation_shells.push_back( electron_shell );
      }
      
      break;

    case 92932:
      // Read nonradiative transition probability per subshell
      {
	Teuchos::Array<Quad<unsigned int,unsigned int,double,double> > data;
	
	readFourColumnTable( eadl,
			     data );

	// Only create the cdf if more than one data point is present
	if( data.size() > 1 )
	  createDiscreteCDFAtThirdTupleLoc( data );
	else
	  data[0].third = 1.0;
	
	hdf5_file_handler.writeArrayToDataSet( data,
						 NONRADIATIVE_TRANSITION_PROBABILITY_ROOT + uintToShellStr( electron_shell ) );
      }
      
      break;

    case 91915:
      // Average radius of subshell ignored
      
      skipTwoColumnTable( eadl );

      break;

    case 91921:
      // Radiative level width ignored
      
      skipTwoColumnTable( eadl );
      
      break;

    case 91922:
      // Nonradiative level width ignored

      skipTwoColumnTable( eadl );

      break;

    case 92933:
      // Average number of photons emitted per initial vacancy ignored 
      // ( Yo == 7 )
      // Average number of electrons emitted per initial vacancy ignored
      // ( Yo == 9 )
      
      skipTwoColumnTable( eadl );
      
      break;
      
    case 92934:
      // Average energy of photons emitted per initial vacancy ignored
      // ( Yo == 7 )
      // Average energy of electrons emitted per initial vacancy ignored
      // ( Yo == 9 )
      
      skipTwoColumnTable( eadl );

      break;

    case 92935:
      // Average energy to residual atom per initial vacancy ignored

      skipTwoColumnTable( eadl );

      break;

    case 93941:
      // Atomic form factor ignored
      
      skipTwoColumnTable( eadl );
      
      break;

    case 93942:
      // Scattering function ignored
      
      skipTwoColumnTable( eadl );

      break;
      
    case 93943:
      // Imaginary anomalous scattering factor ignored
      
      skipTwoColumnTable( eadl );

      break;

    case 93944:
      // Real anomalous scattering factor ignored
      
      skipTwoColumnTable( eadl );

      break;

    default:
      // Unknown reaction type found
      {
	bool known_reaction_type = false;
        FACEMC_ASSERT_ALWAYS( known_reaction_type );
      }
      
      break;
    }
  }

  // Close the last HDF5 file
  hdf5_file_handler.closeHDF5File();

  // Close the EPDL data file
  eadl.close();
}

//! Process Compton files
void PhotonDataProcessor::processComptonFiles( unsigned int atomic_number_start,
					       unsigned int atomic_number_end )
{
  // Compton file information
  std::ifstream compton_file_stream;
  std::string compton_file_name;

  // HDF5 file handler
  HDF5FileHandler hdf5_file_handler;
  
  // Compton Profile Q values
  Teuchos::Array<double> q_values( 31 );
  q_values[0] = 0.00;
  q_values[1] = 0.05;
  q_values[2] = 0.10;
  q_values[3] = 0.15;
  q_values[4] = 0.20;
  q_values[5] = 0.30;
  q_values[6] = 0.40;
  q_values[7] = 0.50;
  q_values[8] = 0.60;
  q_values[9] = 0.70;
  q_values[10] = 0.80;
  q_values[11] = 1.00;
  q_values[12] = 1.20;
  q_values[13] = 1.40;
  q_values[14] = 1.60;
  q_values[15] = 1.80;
  q_values[16] = 2.00;
  q_values[17] = 2.40;
  q_values[18] = 3.00;
  q_values[19] = 4.00;
  q_values[20] = 5.00;
  q_values[21] = 6.00;
  q_values[22] = 7.00;
  q_values[23] = 8.00;
  q_values[24] = 10.00;
  q_values[25] = 15.00;
  q_values[26] = 20.00;
  q_values[27] = 30.00;
  q_values[28] = 40.00;
  q_values[29] = 60.00;
  q_values[30] = 100.00;

  for( unsigned int atomic_number = atomic_number_start; 
       atomic_number <= atomic_number_end; ++atomic_number )
  {
    std::ostringstream file_number;
    file_number << atomic_number;
    std::string hdf5_file_name = d_output_directory + 
	PHOTON_DATA_FILE_PREFIX + file_number.str() + DATA_FILE_SUFFIX;
    hdf5_file_handler.openHDF5FileAndAppend( hdf5_file_name );
    
    compton_file_name = d_compton_file_prefix + file_number.str() + ".txt";
    compton_file_stream.open( compton_file_name.c_str() );
    FACEMC_ASSERT_ALWAYS( compton_file_stream );

    Teuchos::Array<Quad<double,double,double,double> > compton_profile_cdfs;
    Teuchos::Array<double> compton_profile_data;
    double compton_profile_data_point;
    while( !compton_file_stream.eof() )
    {
      compton_file_stream >> compton_profile_data_point;
      compton_profile_data.push_back( compton_profile_data_point );
    }

    compton_profile_cdfs.resize( compton_profile_data.size() );

    // Each shell has a Compton Profile with 31 data points
    for( unsigned int i = 0; i < compton_profile_data.size()/31; ++i )
    {
      for( unsigned int j = 0; j < 31; ++j )
      {
	compton_profile_cdfs[i*31+j].first = q_values[j];
	compton_profile_cdfs[i*31+j].second = compton_profile_data[i*31+j];
      }
      
      Teuchos::ArrayView<Quad<double,double,double,double> > 
	compton_profile_shell_cdf = compton_profile_cdfs( i*31, 31 );

      createContinuousCDFAtFourthTupleLoc( compton_profile_shell_cdf );
      
      calculateSlopesAtThirdTupleLoc( compton_profile_shell_cdf );
    }

    // To save memory, the q_values will be extracted from the array
    Teuchos::Array<Trip<double,double,double> > 
      compton_profile_cdfs_reduced( compton_profile_cdfs.size() );
    for( unsigned int i = 0; i < compton_profile_cdfs.size(); ++i )
    {
      compton_profile_cdfs_reduced[i].first = compton_profile_cdfs[i].second;
      compton_profile_cdfs_reduced[i].second = compton_profile_cdfs[i].third;
      compton_profile_cdfs_reduced[i].third = compton_profile_cdfs[i].fourth;
    }

    hdf5_file_handler.writeArrayToDataSet( compton_profile_cdfs_reduced,
					     COMPTON_PROFILE_CDF_LOC );

    hdf5_file_handler.writeArrayToDataSetAttribute( q_values,
						      COMPTON_PROFILE_CDF_LOC,
						      COMPTON_PROFILE_MOMENTUM_PROJECTIONS_ATTRIBUTE );
    
    // Close the Compton Profile stream
    compton_file_stream.close();

    // Close the HDF5 file
    hdf5_file_handler.closeHDF5File();
  }
}

//! Create the Electron Shell Index Map
void PhotonDataProcessor::createElectronShellIndexMap( 
			       unsigned int atomic_number,
			       Teuchos::Array<Pair<unsigned int,unsigned int> >
			         &map )
{
  Pair<unsigned int, unsigned int> data_point;
  // Non-Relativistic profiles are given for atomic numbers < 36
  if( atomic_number < 36 )
  {
    // Z = 1 or 2
    data_point.first = 1;
    data_point.second = 0;
    map.push_back( data_point );
    
    // Z = 3 or 4
    if( atomic_number > 2 )
    {
      data_point.first = 3;    
      data_point.second = 1;
      map.push_back( data_point );
    }  
    // Z = 5, 6, 7, 8, 9 or 10
    if( atomic_number > 4 )
    {
      data_point.first = 5;
      data_point.second = 2;
      map.push_back( data_point );
      
      data_point.first = 6;
      data_point.second = 2;
      map.push_back( data_point );
    }
    // Z = 11 or 12
    if( atomic_number > 10 )
    {
      data_point.first = 8;
      data_point.second = 3;
      map.push_back( data_point );
    }
    // Z = 13, 14, 15, 16, 17, 18
    if( atomic_number > 12 )
    {
      data_point.first = 10;
      data_point.second = 4;
      map.push_back( data_point );
      
      data_point.first = 11;
      data_point.second = 4;
      map.push_back( data_point );
    }
    
    // Filling order changes for Z=19 and 20 (4s1/2 before 3d)
    // Need to keep shell indices monotonically increasing  
    if( atomic_number == 19 ||
	atomic_number == 20 )
    {
      data_point.first = 16;
      data_point.second = 5;
      map.push_back( data_point );
    }
    // Z = 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 or 30
    if( atomic_number > 20 )
    {
      data_point.first = 13;
      data_point.second = 5;
      map.push_back( data_point );
      
      data_point.first = 14;
      data_point.second = 5;
      map.push_back( data_point );
      
      data_point.first = 16;
      data_point.second = 6;
      map.push_back( data_point );
    }
    // Z = 31, 32, 33, 34, 35
    if( atomic_number > 30 )
    {
      data_point.first = 18;
      data_point.second = 7;
      map.push_back( data_point );
      
      data_point.first = 19;
      data_point.second = 7;
      map.push_back( data_point );
    }
  }
  // Relativistic profiles are given for atomic numbers >= 36
  else
  {
    data_point.first = 1;
    data_point.second = 0;
    map.push_back( data_point );
    
    data_point.first = 3;
    data_point.second = 1;
    map.push_back( data_point );
    
    data_point.first = 5;
    data_point.second = 2;
    map.push_back( data_point );
    
    data_point.first = 6;
    data_point.second = 3;
    map.push_back( data_point );
    
    data_point.first = 8;
    data_point.second = 4;
    map.push_back( data_point );
    
    data_point.first = 10;
    data_point.second = 5;
    map.push_back( data_point );
    
    data_point.first = 11;
    data_point.second = 6;
    map.push_back( data_point );
    
    data_point.first = 13;
    data_point.second = 7;
    map.push_back( data_point );
    
    data_point.first = 14;
    data_point.second = 8;
    map.push_back( data_point );
    
    data_point.first = 16;
    data_point.second = 9;
    map.push_back( data_point );
    
    data_point.first = 18;
    data_point.second = 10;
    map.push_back( data_point );
    
    data_point.first = 19;
    data_point.second = 11;
    map.push_back( data_point );
    
    // Filling order changes for Z=37 and 38 (5s1/2 before 4d)
    // Need to keep shell indices monotonically increasing 
    if( atomic_number == 37 ||
	atomic_number == 38 )
    {
      data_point.first = 27; 
      data_point.second = 12;
      map.push_back( data_point );
    }
    else if( atomic_number == 39 ||
	     atomic_number == 40 ||
	     atomic_number == 41 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 13;
      map.push_back( data_point );
    }
    else if( atomic_number == 42 ||
	     atomic_number == 43 ||
	     atomic_number == 44 ||
	     atomic_number == 45 ||
	     atomic_number == 47 ||
	     atomic_number == 48 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 14;
      map.push_back( data_point );
    }
    else if( atomic_number == 46 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
    }
    else if( atomic_number == 49 ||
	     atomic_number == 50 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 15;
      map.push_back( data_point );
    }
    else if( atomic_number == 51 ||
	     atomic_number == 52 ||
	     atomic_number == 53 ||
	     atomic_number == 54 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 16;
      map.push_back( data_point );
    }
    else if( atomic_number == 55 ||
	     atomic_number == 56 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 17;
      map.push_back( data_point );
    }
    else if( atomic_number == 57 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 18;
      map.push_back( data_point );
    }
    else if( atomic_number == 58 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 19;
      map.push_back( data_point );
    }
    else if( atomic_number == 59 ||
	     atomic_number == 60 ||
	     atomic_number == 61 ||
	     atomic_number == 62 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 18;
      map.push_back( data_point );
    }
    else if( atomic_number == 63 ||
	     atomic_number == 65 ||
	     atomic_number == 66 ||
	     atomic_number == 67 ||
	     atomic_number == 68 ||
	     atomic_number == 69 ||
	     atomic_number == 70 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 19;
      map.push_back( data_point );
    }
    else if( atomic_number == 64 ||
	     atomic_number == 71 ||
	     atomic_number == 72 ||
	     atomic_number == 73 ||
	     atomic_number == 74 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 20;
      map.push_back( data_point );
    }
    else if( atomic_number == 75 ||
	     atomic_number == 76 ||
	     atomic_number == 78 ||
	     atomic_number == 79 ||
	     atomic_number == 80 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 21;
      map.push_back( data_point );
    }
    else if( atomic_number == 77 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
    }
    else if( atomic_number == 81 ||
	     atomic_number == 82 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 22;
      map.push_back( data_point );
    }
    else if( atomic_number == 83 ||
	     atomic_number == 84 ||
	     atomic_number == 85 ||
	     atomic_number == 86 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
     
      data_point.first = 41;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 23;
      map.push_back( data_point );
    }
    else if( atomic_number == 87 ||
	     atomic_number == 88 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 24;
      map.push_back( data_point );
    }
    else if( atomic_number == 89 ||
	     atomic_number == 90 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 46;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 47;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 25;
      map.push_back( data_point );
    }
    else if( atomic_number == 91 ||
	     atomic_number == 92 ||
	     atomic_number == 93 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 35;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 36;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 46;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 47;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 26;
      map.push_back( data_point );
    }
    else if( atomic_number == 94 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 35;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 36;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 25;
      map.push_back( data_point );
    }
    else if( atomic_number == 95 ||
	     atomic_number == 98 ||
	     atomic_number == 99 ||
	     atomic_number == 100 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 35;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 36;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 26;
      map.push_back( data_point );
    }
    else if( atomic_number == 96 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 35;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 36;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 46;
      data_point.second = 26;
      map.push_back( data_point );
      
      data_point.first = 47;
      data_point.second = 26;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 27;
      map.push_back( data_point );
    }
    // WARNING: No Hartree-Fock Compton Profile data for 6d3/2 or 6d5/2
    // map these shells to the 6p+ shell for now...
    else if( atomic_number == 97 )
    {
      data_point.first = 21;
      data_point.second = 12;
      map.push_back( data_point );
      
      data_point.first = 22;
      data_point.second = 13;
      map.push_back( data_point );
      
      data_point.first = 24;
      data_point.second = 14;
      map.push_back( data_point );
      
      data_point.first = 25;
      data_point.second = 15;
      map.push_back( data_point );
      
      data_point.first = 27;
      data_point.second = 16;
      map.push_back( data_point );
      
      data_point.first = 29;
      data_point.second = 17;
      map.push_back( data_point );
      
      data_point.first = 30;
      data_point.second = 18;
      map.push_back( data_point );
      
      data_point.first = 32;
      data_point.second = 19;
      map.push_back( data_point );
      
      data_point.first = 33;
      data_point.second = 20;
      map.push_back( data_point );
      
      data_point.first = 35;
      data_point.second = 21;
      map.push_back( data_point );
      
      data_point.first = 36;
      data_point.second = 22;
      map.push_back( data_point );
      
      data_point.first = 41;
      data_point.second = 23;
      map.push_back( data_point );
      
      data_point.first = 43;
      data_point.second = 24;
      map.push_back( data_point );
      
      data_point.first = 44;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 46;
      data_point.second = 25;
      map.push_back( data_point );

      data_point.first = 47;
      data_point.second = 25;
      map.push_back( data_point );
      
      data_point.first = 58;
      data_point.second = 26;
      map.push_back( data_point );
    }
  }
}

} // end FACEMC namespace

//---------------------------------------------------------------------------//
// end PhotonDataProcessor.cpp
//---------------------------------------------------------------------------//
