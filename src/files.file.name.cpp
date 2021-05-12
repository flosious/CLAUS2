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


#include "files.hpp"


files_::file_t::name_t::name_t(string& filename_with_path_s, 
								  const string delimiter_s, 
								  const std::set< std::__cxx11::string > OR_identifiers_s, 
								  const std::set< std::__cxx11::string > AND_identifiers_s) :
											filename_with_path(filename_with_path_s),
											delimiter(delimiter_s),
											OR_identifiers_s(OR_identifiers_s),
											AND_identifiers_s(AND_identifiers_s)
{	
}

const vector<string> files_::file_t::name_t::methods = {"dsims","tofsims","xps"};

const string files_::file_t::name_t::method() const
{
	for (auto& m : methods)
	{
		if (filename().find(m)!=string::npos)
			return m;
	}
	return "";
}

const string files_::file_t::name_t::simple_name()
{
	string simple_name_p;
	if ((lot()=="") && not_parseable_filename_parts().size()>0) simple_name_p=tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else if ((wafer()<0)) simple_name_p=lot() +lot_split() + tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
	else simple_name_p="";
	if (simple_name_p=="" && lot()=="")
		logger::warning(1,"files_::file_t::name_t::simple_name()","simple_name_p=='' && lot()==''",filename_with_path,"using " + simple_name_p);
	return simple_name_p;
}

const bool files_::file_t::name_t::is_correct_type()
{
	if (filename()=="") return false;
	
	for (auto& fti : AND_identifiers_s)
	{
		if ((filename()+"."+filename_type_ending()).find(fti)==string::npos)
		{
			return false;
		}
	}
	
	if (OR_identifiers_s.size()==0) 
		return true;
	
	for (auto& fti : OR_identifiers_s)
	{
		if ((filename()+"."+filename_type_ending()).find(fti)!=string::npos)
		{
			return true;
		}
	}
	return false;
}

int files_::file_t::name_t::chip_x()
{
	parse_filename_parts();
// 	if (chip_x_p<0)
// 		logger::info("files_::file_t::chip_x() " ,chip_x_p);
	return chip_x_p;
}
int files_::file_t::name_t::chip_y()
{
	parse_filename_parts();
// 	if (chip_y_p<0)
// 		logger::info("files_::file_t::chip_y() " +filename(), chip_y_p);
	return chip_y_p;
}
std::__cxx11::string files_::file_t::name_t::group()
{
	parse_filename_parts();
	if (group_p=="")
		logger::warning(1,"files_::file_t::name_t::group()","group_p==''",filename_with_path,"using " + group_p);
// 		logger::info("files_::file_t::group() unkown" ,filename_with_path);
		
	return group_p;
}
std::__cxx11::string files_::file_t::name_t::lot()
{
	parse_filename_parts();
// 	if (lot_p=="")
// 		logger::warning("files_::file_t::lot() unkown", filename_with_path);
	return lot_p;
}
std::__cxx11::string files_::file_t::name_t::lot_split()
{
	parse_filename_parts();
// 	if (lot_split_p=="")
// 		logger::info("files_::file_t::lot_split() " +filename(), lot_split_p);
	return lot_split_p;
}
std::__cxx11::string files_::file_t::name_t::monitor()
{
	parse_filename_parts();
// 	if (monitor_p=="")
// 		logger::info("files_::file_t::monitor() " +filename(),monitor_p);
	return monitor_p;
}
const int files_::file_t::name_t::olcdb()
{
	parse_filename_parts();
	if (olcdb_p<0)
		logger::warning(1,"files_::file_t::name_t::olcdb()","olcdb_p<0",filename_with_path,"using " + tools::to_string(olcdb_p));
// 		logger::warning("files_::file_t::olcdb() unkown",filename_with_path);
	return olcdb_p;
}
const vector<std::__cxx11::string>& files_::file_t::name_t::not_parseable_filename_parts()
{
	parse_filename_parts();
	return not_parseable_filename_parts_p;
}

int files_::file_t::name_t::wafer()
{
	parse_filename_parts();
// 	if (wafer_p<0)
// 		logger::warning("files_::file_t::wafer() unkown",filename_with_path);
	return wafer_p;
}
std::__cxx11::string files_::file_t::name_t::repetition()
{
	parse_filename_parts();
	return repetition_p;
}

void files_::file_t::name_t::to_screen(string prefix)
{
	cout << prefix <<"delimiter\t'" << delimiter << "'" << endl;
	cout << prefix << "OR-identifiers\t";
	for (auto& id : OR_identifiers_s)
		cout << id << " ";
	cout << endl;
	cout << prefix << "AND-identifiers\t";
	for (auto& id : AND_identifiers_s)
		cout << id << " ";
	cout << endl;
	cout<< prefix << "filename=\t"<< filename()<<endl;
	cout<< prefix << "filename_with_path=\t"<< filename_with_path<<endl;
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

void files_::file_t::name_t::parse_all_parts_at_once()
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
// 		logger::info("files_::file_t::name_t::parse_all_parts_at_once()");
	}
}

void files_::file_t::name_t::parse_filename_parts()
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

bool files_::file_t::name_t::parse_monitor(string filename_part) 
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

bool files_::file_t::name_t::parse_chip(string filename_part)
{
	smatch match;
	regex reg1 ("^x([0-9]{1,2})y([0-9]{1,2})$", std::regex_constants::icase); 
	regex reg2 ("^cX([0-9]{1,2})Y([0-9]{1,2})$"); 
	regex reg3 ("^c([0-9]{1,2})-([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		chip_x_p = tools::str::str_to_int(match[1]);
		chip_y_p = tools::str::str_to_int(match[2]);
		return true;
	}
	return false;
}

bool files_::file_t::name_t::parse_olcdb(string filename_part)
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

bool files_::file_t::name_t::parse_lot(string filename_part)
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

bool files_::file_t::name_t::parse_wafer(string filename_part)
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

bool files_::file_t::name_t::parse_group(string filename_part)
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

bool files_::file_t::name_t::parse_repetitor(string filename_part)
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

std::__cxx11::string files_::file_t::name_t::filename_type_ending() const
{
	string filename_type_ending_p =tools::file::extract_filetype_ending(filename_with_path,".");
// 	if (filename_type_ending_p=="")
// 		logger::error("files_::file_t::filename_type_ending()",filename_type_ending_p);
	return filename_type_ending_p;
}

// std::__cxx11::string& files_::file_t::name_t::filename_with_path() const
// {
// 	return filename_with_path;
// }

const string files_::file_t::name_t::filename() const
{
	return tools::file::extract_filename(filename_with_path);
}

const string files_::file_t::name_t::directory() const
{
	return tools::file::extract_directory_from_filename(filename_with_path);
}



std::__cxx11::string files_::file_t::name_t::to_string(const string del)
{
	stringstream ss;
	ss <<"delimiter '" << delimiter << "'" << del;
	ss << "OR-identifiers ";
	for (auto& id : OR_identifiers_s)
		ss << id << " ";
	ss << del;
	ss  << "AND-identifiers ";
	for (auto& id : AND_identifiers_s)
		cout << id << " ";
	ss << del;
	ss << "filename= "<< filename()<<del;
	ss << "filename_with_path= "<< filename_with_path <<del;
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



bool files_::file_t::name_t::operator==(files_::file_t::name_t& obj)
{
	if (filename_with_path == obj.filename_with_path) 
// 		logger::error("comparing identical filenames, you should never read this","tell florian");
		logger::error("files_::file_t::name_t::operator==","filename_with_path == obj.filename_with_path",tools::to_string(filename_with_path)+"=="+tools::to_string(obj.filename_with_path),"you should never read this, tell florian");
	
	if (olcdb() != obj.olcdb()) return false;
	if (lot() != obj.lot()) return false;
	if (lot_split() != obj.lot_split()) return false;
	if (wafer() != obj.wafer()) return false;
	if (chip_x() != obj.chip_x()) return false;
	if (chip_y() != obj.chip_y()) return false;
	if (monitor() != obj.monitor()) return false;
	if (repetition() != obj.repetition()) return false;
	if (simple_name() != obj.simple_name()) return false;
	
	return true;
}

bool files_::file_t::name_t::operator!=(files_::file_t::name_t& obj)
{
	return !operator==(obj);
}

// bool files_::file_t::name_t::operator<(files_::file_t::name_t& obj)
// {
// 	if (olcdb() < obj.olcdb()) return true;
// 	if (olcdb() > obj.olcdb()) return false;
// 	return false;
// }
// 














