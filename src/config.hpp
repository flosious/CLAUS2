/*
	Copyright (C) 2020-2022 Florian Bärwolf
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
*/

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "log.hpp"
#include <unordered_set>
#include <string>
#include <iostream>
#include "tools.hpp"
// #include "print.hpp"
// #include "export.hpp"
// #include "pse.hpp"
// #include "measurement_tools_t.hpp"
// #include "processor.hpp"
// #include "database_t.hpp"
// #include "sample.hpp"

class config_t 
{
private:
    ///tofsims specific settings
    class tofsims_t
    {
    public:
        std::string export_path;
    };
public:	
	/*CALCULATION METHOD STUFF*/
	
    /*DEFAULTS*/
    ///default config file location
    const std::string default_file = "/home/florian/projects/claus2/build/config.conf";
    ///config file endings: *.conf & *.cfg
    std::unordered_set<std::string> file_endings {".conf", ".cfg", ".config"};
    
    
    /*VARIABLES*/
    ///config file list
    std::unordered_set<std::string> files;
    std::unordered_set<std::string> files_loaded;
	
    /*FUNCTIONS*/   
    ///parse the lines to config
    int parse(std::vector<std::string> config_lines);
    ///loads the actual file to RAM
    int load_file(std::string filename_with_path);
    /*save_functions*/
    ///sets the outformat columns (time depth intensity concentration)
//     void save_output_format(std::vector<std::string> values);
	void save_export_column_names(std::string value);
    ///sets value to test
    void save_test(std::string value);

    
    ///adds entry to replacements
    void save_replacements(std::vector<std::string> values);
    ///set types to measurement_file_types
    void save_measurement_file_types(std::string value);
    ///set the measurement tool (dsims, tofsims, xps,...)
//     void save_measurement_tool(std::string value);
	/// defines a measurement_group (groupid + olcdbid + settings + tool)
	void save_measurement_group_definition(std::string value);
	void save_sample_definition(std::string value);
	/// defines a measurement 
	void save_measurement_definition(std::string value);
	/// what should be exported? intensity, concentration, sputter_depth ....
	void save_export_types(std::string value);
    /*************/
	
    /*formatting*/
    ///filename
	std::string export_filename="";
	std::string export_types="";
    ///replace "this" with "with" in all strings (filenames, pathnames, headers, ...)
    std::map<std::string, std::string> replacements;
    ///file endings of the measurement files
//     std::vector<std::string> measurement_file_types {".asc",".txt",".asc_rpc"};
    ///data column delimiter
    std::string data_column_delimiter="\t";

    ///what to put in output files
//     std::vector<std::string> output_format {"time","depth","intensity","concentration"};
	
    /*directories*/
    
    ///directory name of exports; output to measurement directory, when empty
    std::string dsims_export_location="";
	std::string tofsims_export_location="";

    ///constructor
    config_t();
    
    ///destructor
    //~config();
    
    ///load just default config file; returns the number of succesfully loaded files
    int load();
    ///loads one external config file; returns the number of succesfully loaded files
    int load(std::string filename_with_path);
    ///loads multiple external config file; returns the number of succesfully loaded files
    int load(std::unordered_set<std::string> filenames_with_path);
    ///returns all config files in the file list
    std::unordered_set<std::string> filter_files(std::unordered_set<std::string>& file_list);
    
};

// singleton
// extern config_t conf;
#endif // CONFIG.HPP
