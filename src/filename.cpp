/*
	Copyright (C) 2021 Florian Bärwolf
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


#include "filename.hpp"

filenames::filename_t::filename_t(const string& filename_with_path_s, 
								  const string delimiter_s, 
								  const set<string> identifiers_s) :
								  filename_with_path_p(filename_with_path_s),
								  delimiter(delimiter_s),
								  identifiers(identifiers_s)
{	
}

const string filenames::filename_t::simple_name()
{
	string simple_name_p;
	if ((lot()=="") && not_parseable_filename_parts().size()>0) simple_name_p=tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else if ((wafer()<0)) simple_name_p=lot() +lot_split() + tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else simple_name_p="";
	return simple_name_p;
}

const bool filenames::filename_t::is_correct_type()
{
	if (filename()=="") return false;
	for (auto& fti : identifiers)
	{
// 		cout << "name_t fti=" << fti << endl;
// 		if (fti==filename_type_ending()) 
		if ((filename()+filename_type_ending()).find(fti)!=string::npos)
		{
// 			logger::info("filenames::filename_t::is_correct_file_type()","TRUE");
			return true;
		}
	}
// 	logger::info("filenames::filename_t::is_correct_file_type()","FALSE");
	return false;
}

int filenames::filename_t::chip_x()
{
	parse_filename_parts();
// 	if (chip_x_p<0)
// 		logger::info("files::file_t::chip_x() " ,chip_x_p);
	return chip_x_p;
}
int filenames::filename_t::chip_y()
{
	parse_filename_parts();
// 	if (chip_y_p<0)
// 		logger::info("files::file_t::chip_y() " +filename(), chip_y_p);
	return chip_y_p;
}
std::__cxx11::string filenames::filename_t::group()
{
	parse_filename_parts();
	if (group_p=="")
		logger::info("files::file_t::group() " +filename(),group_p);
	return group_p;
}
std::__cxx11::string filenames::filename_t::lot()
{
	parse_filename_parts();
	if (lot_p=="")
		logger::warning("files::file_t::lot() " +filename(), lot_p);
	return lot_p;
}
std::__cxx11::string filenames::filename_t::lot_split()
{
	parse_filename_parts();
// 	if (lot_split_p=="")
// 		logger::info("files::file_t::lot_split() " +filename(), lot_split_p);
	return lot_split_p;
}
std::__cxx11::string filenames::filename_t::monitor()
{
	parse_filename_parts();
// 	if (monitor_p=="")
// 		logger::info("files::file_t::monitor() " +filename(),monitor_p);
	return monitor_p;
}
const int filenames::filename_t::olcdb()
{
	parse_filename_parts();
	if (olcdb_p<0)
		logger::warning("files::file_t::olcdb() ",olcdb_p);
	return olcdb_p;
}
const vector<std::__cxx11::string>& filenames::filename_t::not_parseable_filename_parts()
{
	parse_filename_parts();
	return not_parseable_filename_parts_p;
}
int filenames::filename_t::wafer()
{
	parse_filename_parts();
	if (wafer_p<0)
		logger::warning("files::file_t::wafer() ",wafer_p);
	return wafer_p;
}
std::__cxx11::string filenames::filename_t::repetition()
{
	parse_filename_parts();
	return repetition_p;
}

void filenames::filename_t::to_screen(string prefix)
{
	cout << prefix <<"delimiter\t'" << delimiter << "'" << endl;
	if (identifiers.size()==1) cout << prefix <<"identifier\t" << *identifiers.begin()  << endl;
	else cout << prefix <<"identifiers\t<" << identifiers.size() << ">" << endl;
	cout<< prefix << "filename=\t"<< filename()<<endl;
	cout<< prefix << "filename_with_path=\t"<< filename_with_path()<<endl;
	cout<< prefix << "filename_type_ending=\t"<< filename_type_ending()<<endl;
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
	
	cout << prefix<< "not_parseable_filename_parts:\t<" << not_parseable_filename_parts().size()  << ">" << endl;
}

void filenames::filename_t::parse_all_parts_at_once()
{
	smatch match;
	regex reg ("^([0-9]{4,})_([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)_[wW]?([0-9]{1,2})(_.*?)([0-9]*?)([a-z]*?)$"); 
	string f = filename();
	if (regex_search(f,match,reg)) 
	{
		olcdb_p = 	tools::str::str_to_int(match[1]);
		lot_p = 		match[2];
		lot_split_p=	match[3];
		wafer_p = 	tools::str::str_to_int(match[4]);
		group_p = 	match[6];
		repetition_p=	match[7];
		not_parseable_filename_parts_p = tools::str::get_strings_between_delimiter(filename(),delimiter);
		for (auto& m:match)
			not_parseable_filename_parts_p.erase(remove(not_parseable_filename_parts_p.begin(),not_parseable_filename_parts_p.end(),m), not_parseable_filename_parts_p.end());
// 		logger::info("filenames::filename_t::parse_all_parts_at_once()");
	}
}

void filenames::filename_t::parse_filename_parts()
{
	if (parsed_filename_parts) return;
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
		if (repetition_p=="") if(parse_repetitor(filename_part)) continue;
// 		if (parse_crater_depth(filename_part)) continue;
		// no parser worked
		not_parseable_filename_parts_p.push_back(filename_part);
	}
	if (lot_p=="" || olcdb_p==-1 || wafer_p==-1 || group_p=="") parse_all_parts_at_once();
	parsed_filename_parts = true;
// 	if (lot=="" && not_parseable_filename_parts.size()>0) lot=not_parseable_filename_parts[0];
}

bool filenames::filename_t::parse_monitor(string filename_part) 
{
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

bool filenames::filename_t::parse_chip(string filename_part)
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

bool filenames::filename_t::parse_olcdb(string filename_part)
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

bool filenames::filename_t::parse_lot(string filename_part)
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

bool filenames::filename_t::parse_wafer(string filename_part)
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

bool filenames::filename_t::parse_group(string filename_part)
{
	smatch match;
	regex reg1 ("^g-?*([0-9]+?)([a-z]*?)$"); 
	regex reg2 ("^group-*([0-9]+?)([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
// 		cout << "filename_part=" << filename_part << endl;
		group_p = match[1];
		repetition_p = match[2];
// 		cout << "match[1]=" << match[1] << endl;
// 		cout << "match[2]=" << match[2] << endl;
// 		cout << "group_p=" << group_p << endl;
// 		cout << "repetition_p=" << repetition_p << endl;
		return true;
	}
	return false;
}

bool filenames::filename_t::parse_repetitor(string filename_part)
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

std::__cxx11::string filenames::filename_t::filename_type_ending() const
{
	string filename_type_ending_p =tools::file::extract_filetype_ending(filename_with_path_p,".");
// 	if (filename_type_ending_p=="")
// 		logger::error("files::file_t::filename_type_ending()",filename_type_ending_p);
	return filename_type_ending_p;
}

std::__cxx11::string filenames::filename_t::filename_with_path() const
{
	return filename_with_path_p;
}

const string filenames::filename_t::filename() const
{
	return tools::file::extract_filename(filename_with_path_p);
}

const string filenames::filename_t::directory() const
{
	return tools::file::extract_directory_from_filename(filename_with_path_p);
}



std::__cxx11::string filenames::filename_t::to_string()
{
	stringstream ss;
	const string del = ",";
	ss <<"delimiter '" << delimiter << "'" << del;
	if (identifiers.size()==1) ss <<"identifier " << *identifiers.begin()  << del;
	else ss <<"identifiers <" << identifiers.size() << ">" << del;
	ss << "filename= "<< filename()<<del;
	ss << "filename_with_path= "<< filename_with_path()<<del;
	ss << "filename_type_ending= "<< filename_type_ending()<<del;
	ss<< "directory= "<< directory()<<del;
	
	ss << "wafer= "<< wafer()<<del;
	ss<< "lot= "<< lot()<<del;
	ss<< "lot_split= "<< lot_split()<<del;
	ss << "chip_x= "<< chip_x()<<del;
	ss<< "chip_y= "<< chip_y()<<del;
	ss<< "monitor= "<< monitor()<<del;

	ss<< "olcdb= "<< olcdb()<<del;
	ss<< "group= "<< group()<<del;
	ss<< "repetition= "<< repetition()<<del;
	
	ss<< "not_parseable_filename_parts: <" << not_parseable_filename_parts().size()  << ">" ;
	return ss.str();
}


























