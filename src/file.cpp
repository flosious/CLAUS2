/*
	Copyright (C) 2020 Florian Bärwolf
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

#include "file.hpp"

const string files::file_t::delimiter="_";

int files::file_t::chip_x() const
{
	return chip_x_p;
}
int files::file_t::chip_y() const
{
	return chip_y_p;
}
std::__cxx11::string files::file_t::group() const
{
	return group_p;
}
std::__cxx11::string files::file_t::lot() const
{
	return lot_p;
}
std::__cxx11::string files::file_t::lot_split() const
{
	return lot_split_p;
}
std::__cxx11::string files::file_t::monitor() const
{
	return monitor_p;
}
int files::file_t::olcdb() const
{
	return olcdb_p;
}
vector<std::__cxx11::string> files::file_t::not_parseable_filename_parts() const
{
	return not_parseable_filename_parts_p;
}
int files::file_t::wafer() const
{
	return wafer_p;
}
std::__cxx11::string files::file_t::repetition() const
{
	return repetition_p;
}



files::file_t::file_t(string filename_with_path_s)
{
	filename_with_path_p = filename_with_path_s;
	parse_filename_parts();
}

std::__cxx11::string files::file_t::type_ending() const
{
	return tools::file::extract_filetype_ending(filename_with_path_p,delimiter);
}

std::__cxx11::string files::file_t::filename_with_path() const
{
	return filename_with_path_p;
}

string files::file_t::filename() const
{
	return tools::file::extract_filename(filename_with_path_p);
}

string files::file_t::directory() const
{
	return tools::file::extract_directory_from_filename(filename_with_path_p);
}

bool files::file_t::parse_all_parts_at_once()
{
	smatch match;
	regex reg ("^([0-9]{4,})_([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)_[wW]?([0-9]{1,2})(_.*)_([0-9]+?)([a-z]*?)$"); 
	string f = filename_with_path();
	if (regex_search(f,match,reg)) 
	{
		olcdb_p = 	tools::str::str_to_int(match[1]);
		lot_p = 		match[2];
		lot_split_p=	match[3];
		wafer_p = 	tools::str::str_to_int(match[4]);
		group_p = 	tools::str::str_to_int(match[6]);
		repetition_p=	match[7];
#ifdef DEBUG
		std::cout << "!!!parsed all parts!!!\n";
		to_screen();
#endif
	}
	else
		return false;
	
	return true;
}


bool files::file_t::parse_filename_parts()
{
	if (parsed) return parsed;
	vector<string> filename_parts = tools::str::get_strings_between_delimiter(filename(),delimiter);
	for (auto filename_part:filename_parts)
	{
		// parse everything just once (except crater depths)
		// THE ORDER IS VERY IMPORTANT
		if (olcdb_p==-1) if(parse_olcdb(filename_part)) continue;
		if (wafer_p==-1) if(parse_wafer(filename_part)) continue;
		if (lot_p=="") if(parse_lot(filename_part)) continue;
		if (chip_x_p==-1)	if(parse_chip(filename_part)) continue;
		if (monitor_p=="") if(parse_monitor(filename_part)) continue;
		if (group_p=="") if(parse_group(filename_part)) continue;
// 		if (parse_crater_depth(filename_part)) continue;
		// no parser worked
		not_parseable_filename_parts_p.push_back(filename_part);
	}
	if (lot_p=="" || olcdb_p==-1 || wafer_p==-1 || group_p=="") parse_all_parts_at_once();
// 	if (lot=="" && not_parseable_filename_parts.size()>0) lot=not_parseable_filename_parts[0];
	parsed = true;
	return true;
}

bool files::file_t::parse_monitor(string filename_part) 
{
// 	string x,y;
	smatch match;
	regex reg1 ("^m-*([a-zA-Z0-9]+)$"); 
	regex reg2 ("^monitor-([a-zA-Z0-9]+)$"); 
	regex reg3 ("^[Q|q]-*([a-zA-Z0-9]+)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		monitor_p = match[1];
		return true;
	}
	return false;
}

bool files::file_t::parse_chip(string filename_part)
{
	smatch match;
	regex reg1 ("^x([0-9]{1,2})y([0-9]{1,2})$", std::regex_constants::icase); 
	regex reg2 ("^chip-([0-9]{1,2})-([0-9]{1,2})$"); 
	regex reg3 ("^c([0-9]{1,2})-([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		chip_x_p = tools::str::str_to_int(match[1]);
		chip_y_p = tools::str::str_to_int(match[2]);
		return true;
	}
	return false;
}

bool files::file_t::parse_olcdb(string filename_part)
{
	smatch match;
	regex reg ("^([0-9]{4,})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		olcdb_p = tools::str::str_to_int(filename_part);
		return true;
	}
	return false;
}

// bool sims_files::file_t::parse_sputter_energy_element(string filename_part)
// {
// 	smatch match;
// 	
// 	regex reg; 
// 	
// 	sputter_energy_p.name = "sputter_energy";
// 	sputter_energy_p.unit = unit_t("eV");
// 	sputter_energy_p.dimension="energy";
// 	
// 	reg = ("^([0-9]{1,2})(kV|keV)(O|Cs)(\\+|-)$"); 
// 	if (regex_search(filename_part,match,reg)) 
// 	{
// 		sputter_energy.data.push_back(tools::str::str_to_double(match[1])*1000);
// 		sputter_element = match[3];
// 		polarity = match[4];
// 		return true;
// 	}
// 	
// 	reg = ("^([0-9]{2,5})(V|eV)(O|Cs)(\\+|-)$"); 
// 	if (regex_search(filename_part,match,reg)) 
// 	{
// 		sputter_energy.data.push_back(tools::str::str_to_double(match[1]));
// 		sputter_element = match[3];
// 		polarity = match[4];
// 		return true;
// 	}
// 	
// 	return false;
// }

bool files::file_t::parse_lot(string filename_part)
{
	smatch match;
	regex reg ("^([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		lot_p = match[1];
		lot_split_p = match[2];
		return true;
	}
	return false;
}

bool files::file_t::parse_wafer(string filename_part)
{
	smatch match;
	regex reg ("^[wW]([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		wafer_p = tools::str::str_to_int(match[1]);
		return true;
	}
	return false;
}

bool files::file_t::parse_group(string filename_part)
{
	smatch match;

	regex reg1 ("^g-?*([0-9]+?)([a-z]*?)$"); 
	regex reg2 ("^group-*([0-9]+?)([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		group_p = tools::str::str_to_int(match[1]);
		repetition_p = match[2];
		return true;
	}
	return false;
}

bool files::file_t::parse_repetitor(string filename_part)
{
	smatch match;

	regex reg1 ("^r-?*([0-9]+?)$"); 
	regex reg2 ("^[rep|repetitor|repetition|repeat]-*([0-9]+?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		repetition_p = match[1];
		return true;
	}
	return false;
}



void files::file_t::to_screen(string prefix) const
{
	
	cout<< prefix << "filename=\t"<< filename()<<endl;
	cout<< prefix << "filename_with_path=\t"<< filename_with_path()<<endl;
	cout << prefix<< "directory=\t"<< directory()<<endl;
	
	cout<< prefix << "wafer=\t"<< wafer()<<endl;
	cout << prefix<< "lot=\t"<< lot()<<endl;
	cout << prefix<< "lot_split=\t"<< lot_split()<<endl;
	cout<< prefix << "chip_x=\t"<< chip_x()<<endl;
	cout << prefix<< "chip_y=\t"<< chip_y()<<endl;
	cout << prefix<< "monitor=\t"<< monitor()<<endl;

	cout << prefix<< "olcdb=\t"<< olcdb()<<endl;
	cout << prefix<< "group=\t"<< group()<<endl;
	cout << prefix<< "repetition=\t"<< repetition()<<endl;
	
// 	cout << prefix<< "polarity=\t"<< polarity<<endl;
// 	cout << prefix<< "sputter_element=\t"<< sputter_element<<endl;
	
// 	if (sputter_energy.is_set()) sputter_energy.to_screen(prefix);
// 	if (total_sputter_depths.is_set()) total_sputter_depths.to_screen(prefix);
	
	cout << prefix<< "not_parseable_filename_parts:" << endl;
// 	print(not_parseable_filename_parts);
}

bool files::file_t::operator<(const files::file_t& fname) const
{
	if (filename_with_path_p >= fname.filename_with_path_p) 
		return false;
	return true;
}

bool files::file_t::operator==(const files::file_t& fname) const
{
	if (filename_with_path()==fname.filename_with_path()) return true;
	return false;
}


// set< files::file_t > files::file_t::filenames(std::vector< std::__cxx11::string > filenames_with_path)
// {
// 	set<files::file_t> filenames;
// 	for (auto& fname:filenames_with_path)
// 		filenames.insert(files::file_t(fname));
// 	return filenames;
// }




/*******************************/



total_sputter_depth_t files::sims_t::total_sputter_depths()
{
	total_sputter_depth_t tspd{unit_t{"n","m"}};
	for (auto& t : total_sputter_depths_p)
	{
// 		tspd.data.push_back(*t.change_unit(unit_t{"n","m"}).data.begin());
		tspd += t;
	}
	return tspd;
}

bool files::sims_t::parse_crater_depth(string filename_part)
{
	smatch match;
	regex reg ("^([0-9]{2,})(nm|A)$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		string value = match[1];
		string unit = match[2];
// 		sputter_depth_t total_sputter_depth{unit_t{"","unit"}};
// 		if (unit=="nm")  total_sputter_depth = sputter_depth_t{unit_t{"n","m"}};
// 		total_sputter_depth.data.push_back(tools::str::str_to_double(value));
		
		
// 		if (unit=="nm") total_sputter_depths_p.data.push_back(tools::str::str_to_double(value));
// 		else if (unit=="A") total_sputter_depths_p.data.push_back(tools::str::str_to_double(value)/10); // A in nm
		return true;
	}
	return false;
}

string files::sims_t::filename_without_crater_depths()
{
	string filename_wo_crater_depths  = tools::file::extract_filename(filename_with_path_p);
	string unit = total_sputter_depths().unit().name();
	stringstream remove;
	for (double& total_sputter_depth:total_sputter_depths().data)
	{
		remove.str("");
		remove << total_sputter_depth << unit;
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,remove.str() + delimiter);
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,delimiter+ remove.str());
	}
	return filename_wo_crater_depths;
}
