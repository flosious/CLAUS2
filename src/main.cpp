/* 
 * 	CLever AUtomated Scientist
 * 	Version 2020-11-11_1
 * 	Florian Bärwolf 2020
 *	
 * 
 *  Copyright (C) 2020 Florian Bärwolf
	floribaer@gmx.de
	
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
                        *********************************************************************************************
                        *                                                                                           *
                        *                                       DEDICATION                                          *
                        *                                                                                           *
                        *   This program is dedicated to my uncle, Klaus Bärwolf, who never hesitated to help me.   *
                        *                                                                                           *
                        *********************************************************************************************
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 * Purpose:
 * 		This software mimics the solution of the daily tasks and routines of material scientists within a semiconductor fab: Using very few, but similar methods over and over again.
 * 
 * Motivation:
 * 		This software was created in the hope to be useful in automating calculation of raw data measurements (e.g. intensities) to characteristic material parameters (e.g. concentrations).
 * 		While doing that, it decreases operator errors, saves time and opens new opportunities in decreasing measurement uncertainties.
 * 
 * Functionality:
 * 		fast, modular and reliable
 * 
 * Input:
 * 		exported, raw measurement files of (different) measurement tools
 * 
 * Output:
 * 		evaluated measurement results as files
 *		graphs (needs gnuplot)
 * 
 * Necessities:
 * 		config file(s) (config.conf)
 * 		periodic table of elements (pse.csv)
 *		sqlite3 database filled with reference measurements
 
 
 Program paradigms:
	- object-oriented: 	for description of actual physical properties (samples, measurements, samples properties like concentrations, depths, etc.)
	- kind of declarative:		for calculation/searching of missing properties (e.g. concentration of a cluster can be calculated be searching a similar measurement/database or by calculation from dose, or maximum implant concentration, etc.). 
	- 							the algorithm is designed to iterate available calculation methos from highest priority (asuming lowest calc error) to lowest (asuming highest calc error), e.g. prefering dose over one concentration(depth) data point
 
 Main Classes Dependencies:
	main(args) -> processor(args)	-> vector<string> filenames = args
	
	-> <list>configs(args)
	
	//WARNING idk if this is neccessary
	-> <inter mediate class> measurements(filenames)
		-> <list>DSIMS() // returns all DSIMS measurements within the filenames
		-> <list>TOFSIMS() // returns all TOFSIMS measurements with the filenames
		-> ...
		
	-> D:SIMS:measurement	-> *sample
							-> *measurement_group
							-> *secondaries (measurements of this measurement)
	-> TOF:SIMS:measurement	-> *sample
							-> *measurement_group
	-> XPS:measurement		-> *sample
							-> *measurement_group
	-> TEM/EDX:measurement	-> *sample
							-> *measurement_group
	-> Ref:measurement		-> *sample
							-> *measurement_group
	-> Profiler:measurement -> *sample
							-> *secondary: this is special, because profiler measurements are (in general) secondary. that means they are a measurement of a measurement (crater depth)
							
	
	-> Secondary:measurement	-> How to filter secondary measurements? What is the unique identifier -> filename (reduced by crater depth)? samplename+group? There could be multiple measurements of the same file within the same group
								olcdb + lot + lot_split + wafer + chip + monitor + m_group + repetitor + settings (NOT METHOD!)
	
	-> <list>samples(filenames)
	
	-> sample	// sample properties, independend of measurement_settings, measurement_parameters, tools_parameters, ...
				-> measurements()
				-> measurement_groups()
				
	-> meausrement_group	// group of measurements with same measurement_settings, measurement_parameters, tool_parameters, ...
				-> measurements()
				-> samples()
															
	-> <list>measurement_groups(filenames)
								
	-> cluster	-> intensity
				-> concentration
				-> sputter_depth
				-> sputter_time
				
	-> concentration	-> simulation 
						-> reference
						-> calculations
						
	-> depth	-> simulation
				-> reference
				-> calculations

	-> quantity	-> unit
				-> data
				-> name
				-> is_skalar()
				-> is_vec()
				-> is_mat()

	-> fit_functions	-> polynom(degree)
	
	-> file		-> name/filename
 
	using std::list to maintain order and "pointer security"
	// general ideas
	-> samples
	-> sample 								-> measurement_tool_groups (e.g. dsims_measurement_groups, xps_measurement_groups) + name (lot+split+wafer+chip+monitor)
	-> measurement_group					-> measurements
	-> measurement 							-> clusters + filename + crater
	-> cluster								-> quantities
	-> quantity								-> unit + name + data



	basic processor algorithm:
	-	load config ()
	-	populate: <list>
			
			-	filenames
			-	measurements(&filenames):
                - dsims
                - tofsims
                - ...
			-	measurement_groups(&filenames)
                - dsims
                - tofsims
                - ...
			-	samples(&filenames)
		
	-	populate cross reference pointers
			-	*measurement 		-> secondary_measurement, measurement_group, sample
			-	*measurement_group	-> measurement
			-	*sample				-> measurement
	-	load PSE (when used)
	
 */


#include "processor.hpp"

int main( int    argc, char **argv )
{ 
    processor Klaus(tools::str::args_to_vec(argc,argv));
    return 0;
}
