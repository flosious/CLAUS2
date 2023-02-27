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


files_::file_t::name_t::name_t(std::string& filename_with_path_s,
								  const std::string delimiter_s, 
								  const std::set< std::string > OR_identifiers_s, 
								  const std::set< std::string > AND_identifiers_s) :
											filename_with_path(filename_with_path_s),
											delimiter(delimiter_s),
											OR_identifiers_s(OR_identifiers_s),
                                            AND_identifiers_s(AND_identifiers_s),
                                            logger(__FILE__,"files_::file_t::name_t")
{	
    not_parseable_filename_parts_p = tools::str::get_strings_between_delimiter(filename(),delimiter);
    parse_filename_parts();
}

// const std::vector<std::string> files_::file_t::name_t::methods = {"dsims","tofsims","xps"};

// const std::string files_::file_t::name_t::method() const
// {
// 	for (auto& m : methods)
// 	{
// 		if (filename().find(m)!=std::string::npos)
// 			return m;
// 	}
// 	return "";
// }

const std::string files_::file_t::name_t::simple_name()
{
	std::string simple_name_p;
// 	if ((lot()=="") && not_parseable_filename_parts().size()>0) 
// 		simple_name_p=tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
// 	else if ((wafer()<0)) 
// 		simple_name_p=lot() +lot_split() + tools::vec::combine_vec_to_string(not_parseable_filename_parts(), delimiter);
// 	else 
// 		simple_name_p="";
	
	if ((lot()=="" || wafer() <0 ) && not_parseable_filename_parts().size()>0)
		simple_name_p = not_parseable_filename_parts().at(0);
	
	if (simple_name_p=="")
    {
//		logger::warning(1,"files_::file_t::name_t::simple_name()","simple_name_p=='' && lot()==''",filename_with_path,"using " + simple_name_p);
    }
	return simple_name_p;
}

bool files_::file_t::name_t::is_correct_type()
{
	if (filename()=="") return false;
	
	for (auto& fti : AND_identifiers_s)
	{
        if ((filename()+"."+filename_type_ending()).find(fti)==std::string::npos)
        {
            //logger::debug(33,"files_::file_t::name_t::is_correct_type()","wrong AND file type: " + filename_with_path );
			return false;
		}
	}
	
	if (OR_identifiers_s.size()==0) 
		return true;
	
	for (auto& fti : OR_identifiers_s)
	{
		
        std::string T = filename()+"."+filename_type_ending();
        if (T.find(fti)!=std::string::npos)
		{
			return true;
		}
	}
    //logger::debug(33,"files_::file_t::name_t::is_correct_type()","wrong OR file type: " +  filename_with_path );
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
std::string files_::file_t::name_t::group()
{
	parse_filename_parts();
	if (group_p=="")
    {
//		logger::warning(1,"files_::file_t::name_t::group()","group_p==''",filename_with_path,"using " + group_p);
    }
// 		logger::info("files_::file_t::group() unkown" ,filename_with_path);
		
	return group_p;
}
std::string files_::file_t::name_t::lot()
{
	parse_filename_parts();
	if (lot_p=="")
    {
//		logger::warning(1,"files_::file_t::lot() unkown", filename_with_path);
    }

// 	if (lot_p=="" && not_parseable_filename_parts().size()>0)
// 		lot_p =  not_parseable_filename_parts().at(0);
	
	return lot_p;
}
std::string files_::file_t::name_t::lot_split()
{
	parse_filename_parts();
// 	if (lot_split_p=="")
// 		logger::info("files_::file_t::lot_split() " +filename(), lot_split_p);
	return lot_split_p;
}
std::string files_::file_t::name_t::monitor()
{
	parse_filename_parts();
// 	if (monitor_p=="")
// 		logger::info("files_::file_t::monitor() " +filename(),monitor_p);
	return monitor_p;
}
int files_::file_t::name_t::olcdb()
{
	parse_filename_parts();
	if (olcdb_p<0)
    {
//		logger::warning(1,"files_::file_t::name_t::olcdb()","olcdb_p<0",filename_with_path,"using " + tools::to_string(olcdb_p));
    }
// 		logger::warning("files_::file_t::olcdb() unkown",filename_with_path);
	return olcdb_p;
}
const std::vector<std::string>& files_::file_t::name_t::not_parseable_filename_parts()
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
std::string files_::file_t::name_t::repetition()
{
	parse_filename_parts();
	return repetition_p;
}

void files_::file_t::name_t::to_screen(std::string prefix)
{
	std::cout << prefix <<"delimiter\t'" << delimiter << "'" << std::endl;
	std::cout << prefix << "OR-identifiers\t";
	for (auto& id : OR_identifiers_s)
		std::cout << id << " ";
	std::cout << std::endl;
	std::cout << prefix << "AND-identifiers\t";
	for (auto& id : AND_identifiers_s)
		std::cout << id << " ";
	std::cout << std::endl;
    std::cout<< prefix << "filename=\t"<< filename() << std::endl;
    std::cout<< prefix << "filename_with_path=\t"<< filename_with_path<<std::endl;
    std::cout<< prefix << "filename_type_ending=\t"<< filename_type_ending()<<std::endl;
    std::cout << prefix<< "directory=\t"<< directory()<<std::endl;
    std::cout << prefix<< "lot=\t"<< lot()<<std::endl;
    std::cout << prefix<< "lot_split=\t"<< lot_split()<<std::endl;
    std::cout<< prefix << "wafer=\t"<< wafer()<<std::endl;
    std::cout<< prefix << "chip_x=\t"<< chip_x()<<std::endl;
    std::cout << prefix<< "chip_y=\t"<< chip_y()<<std::endl;
    std::cout << prefix<< "monitor=\t"<< monitor()<<std::endl;

    std::cout << prefix<< "olcdb=\t"<< olcdb()<<std::endl;
    std::cout << prefix<< "group=\t"<< group()<<std::endl;
    std::cout << prefix<< "repetition=\t"<< repetition()<<std::endl;
	
	std::cout << prefix<< "not_parseable_filename_parts:\t<" << not_parseable_filename_parts().size()  << ">" << std::endl;
}

void files_::file_t::name_t::parse_all_parts_at_once()
{
	std::smatch match;
	std::regex reg ("^([0-9]{4,})_([A-Z]{1,4}[0-9]{1,5})([#[0-9A-Za-z]*?]*?)_[wW]?([0-9]{1,2})(_.*?)([0-9]*?)([a-z]*?)$"); 
	std::string f = filename();
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
//	std::vector<std::string> filename_parts = tools::str::get_strings_between_delimiter(filename(),delimiter);
    std::vector<std::string> still_not_parsed_filename_parts_p;
    still_not_parsed_filename_parts_p.reserve(not_parseable_filename_parts_p.size());
    for (auto filename_part:not_parseable_filename_parts_p)
	{
//        std::cout << std::endl << filename_part << std::endl;
		// parse everything just once (except crater depths)
		// THE ORDER IS VERY IMPORTANT
		if (olcdb_p==-1) if(parse_olcdb(filename_part)) continue;
        if (repetition_p=="") if(parse_repetitor(filename_part)) continue;
        if (group_p=="") if(parse_group(filename_part)) continue;
        if (wafer_p==-1) if(parse_wafer(filename_part)) continue;
		if (chip_x_p==-1)	if(parse_chip(filename_part)) continue;
        if (monitor_p=="") if(parse_monitor(filename_part)) continue;
        if (lot_p=="") if(parse_lot(filename_part)) continue;
        still_not_parsed_filename_parts_p.push_back(filename_part);
	}
    not_parseable_filename_parts_p = still_not_parsed_filename_parts_p;
    if (lot_p=="" && not_parseable_filename_parts_p.size()>0)
    {
        lot_p = not_parseable_filename_parts_p.at(0);
    }
	parsed_filename_parts = true;
}

bool files_::file_t::name_t::parse_monitor(std::string filename_part) 
{
	std::smatch match;
	std::regex reg1 ("^m-*([a-zA-Z0-9]+)$"); 
	std::regex reg2 ("^monitor-([a-zA-Z0-9]+)$"); 
	std::regex reg3 ("^[Q|q]-*([a-zA-Z0-9]+)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		monitor_p = match[1];
//		logger::debug(17,"files_::file_t::name_t::parse_monitor",monitor_p);
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no monitor");
	return false;
}

bool files_::file_t::name_t::parse_chip(std::string filename_part)
{
	std::smatch match;
	std::regex reg1 ("^x([0-9]{1,2})y([0-9]{1,2})$", std::regex_constants::icase); 
	std::regex reg2 ("^cX([0-9]{1,2})Y([0-9]{1,2})$"); 
	std::regex reg3 ("^c([0-9]{1,2})-([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) || regex_search(filename_part,match,reg3)) 
	{
		chip_x_p = tools::str::str_to_int(match[1]);
		chip_y_p = tools::str::str_to_int(match[2]);
//		logger::debug(7,"files_::file_t::name_t::parse_chip",::to_string( chip_x_p) + " " + ::to_string( chip_y_p));
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no chip");
	return false;
}

bool files_::file_t::name_t::parse_olcdb(std::string filename_part)
{
	std::smatch match;
	std::regex reg ("^([0-9]{4,})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		olcdb_p = tools::str::str_to_int(filename_part);
        logger.debug(__func__,filename_with_path).signal(olcdb_p,15,"filename_part="+filename_part);
//        logger::debug(17,"files_::file_t::name_t::parse_olcdb",::to_string(olcdb_p));
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no olcdb");
	return false;
}

bool files_::file_t::name_t::parse_lot(std::string filename_part)
{
	std::smatch match;
//    std::regex reg ("^([a-zA-Z]{1,4}[0-9]{1,4})([#[0-9A-Za-z]*?]*?)$");
    std::regex reg ("^([a-zA-Z]{1,4}[0-9]{1,4})([#[0-9A-Za-z]*?]*?)$");
    if (regex_search(filename_part,match,reg))
	{
        std::stringstream lot_ss;
        lot_ss << match[1] ;
        lot_p = lot_ss.str();
        lot_split_p = match[2];
        logger.debug(__func__,filename_with_path).value(lot_p,10,lot_split_p);
//		logger::debug(17,"files_::file_t::name_t::parse_lot",lot_p + " " + lot_split_p);
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no lot");
	return false;
}

bool files_::file_t::name_t::parse_wafer(std::string filename_part)
{
	std::smatch match;
	std::regex reg ("^[wW]([0-9]{1,2})$"); 
	if (regex_search(filename_part,match,reg)) 
	{
		wafer_p = tools::str::str_to_int(match[1]);
        //logger::debug(17,"files_::file_t::name_t::parse_wafer",::to_string(wafer_p));
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no wafer");

	return false;
}

bool files_::file_t::name_t::parse_group(std::string filename_part)
{
	std::smatch match;
	std::regex reg1 ("^g-?*([0-9]+?)([a-z]*?)$"); 
	std::regex reg2 ("^group-*([0-9]+?)([a-z]*?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
// 		std::cout << "filename_part=" << filename_part << std::endl;
		group_p = match[1];
		repetition_p = match[2];
        //logger::debug(17,"files_::file_t::name_t::parse_group",group_p + " " + repetition_p);
// 		std::cout << "match[1]=" << match[1] << std::endl;
// 		std::cout << "match[2]=" << match[2] << std::endl;
// 		std::cout << "group_p=" << group_p << std::endl;
// 		std::cout << "repetition_p=" << repetition_p << std::endl;
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no group");
	return false;
}

bool files_::file_t::name_t::parse_repetitor(std::string filename_part)
{
	std::smatch match;
	std::regex reg1 ("^r-?*([0-9]+?)$"); 
	std::regex reg2 ("^[rep|repetitor|repetition|repeat]-*([0-9]+?)$"); 
	if (regex_search(filename_part,match,reg1) || regex_search(filename_part,match,reg2) /*|| regex_search(filename_part,match,reg3)*/) 
	{
		repetition_p = match[1];
        //logger::debug(17,"files_::file_t::name_t::parse_repetitor",repetition_p);
		return true;
	}
    else
        logger.debug(__func__,filename_with_path).signal(filename_part + " is no repetitor");
    return false;
}

std::string files_::file_t::name_t::filename_type_ending() const
{
	std::string filename_type_ending_p =tools::file::extract_filetype_ending(filename_with_path,".");
// 	if (filename_type_ending_p=="")
// 		logger::error("files_::file_t::filename_type_ending()",filename_type_ending_p);
	return filename_type_ending_p;
}

// string& files_::file_t::name_t::filename_with_path() const
// {
// 	return filename_with_path;
// }

const std::string files_::file_t::name_t::filename() const
{
    return tools::file::extract_filename(filename_with_path,".");
}

const std::string files_::file_t::name_t::directory() const
{
	return tools::file::extract_directory_from_filename(filename_with_path);
}



std::string files_::file_t::name_t::to_string(const std::string del)
{
	std::stringstream ss;
	ss <<"delimiter '" << delimiter << "'" << del;
	ss << "OR-identifiers ";
	for (auto& id : OR_identifiers_s)
		ss << id << " ";
	ss << del;
	ss  << "AND-identifiers ";
	for (auto& id : AND_identifiers_s)
		std::cout << id << " ";
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
	
// 	ss<< "not_parseable_filename_parts: <" << not_parseable_filename_parts().size()  << ">" ;
	ss << "not_parseable_filename_parts=";
	for (auto& f : not_parseable_filename_parts())
		ss  << f << ",";
	ss << del;
	return ss.str();
}



bool files_::file_t::name_t::operator==(files_::file_t::name_t& obj)
{
	if (filename_with_path == obj.filename_with_path) 
// 		logger::error("comparing identical filenames, you should never read this","tell florian");
        //logger::error("files_::file_t::name_t::operator==","filename_with_path == obj.filename_with_path",tools::to_string(filename_with_path)+"=="+tools::to_string(obj.filename_with_path),"you should never read this, tell florian");
	
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

/************************************************/
/******   		crater_in_name_t   		   ******/
/************************************************/


files_::file_t::crater_in_name_t::crater_in_name_t(std::string& filename_with_path_s,const std::string delimiter_s,const std::set<std::string> OR_identifiers_s,const std::set<std::string> AND_identifiers_s) :
    files_::file_t::name_t(filename_with_path_s,delimiter_s,OR_identifiers_s,AND_identifiers_s), logger(__FILE__,"files_::file_t::crater_in_name_t")
{
}

const quantity::total_sputter_depth_t& files_::file_t::crater_in_name_t::total_sputter_depths()
{
	if (total_sputter_depths_p.data().size()==0)
	{
		filename_without_crater_depths_s = filename();
		for (std::vector<std::string>::iterator FNp=not_parseable_filename_parts_p.begin();FNp!=not_parseable_filename_parts_p.end();FNp++)
		{
			std::smatch match;
			std::regex reg ("^([0-9]{2,})(nm|A)$"); 
			if (regex_search(*FNp,match,reg)) 
			{
				std::string value = match[1];
				std::string unit = match[2];
				double depth = {tools::str::str_to_double(value)};
				if (unit=="A")
					depth = depth/10;
				total_sputter_depths_p << quantity::total_sputter_depth_t({depth},units::SI::meter*units::prefixes::nano);
				not_parseable_filename_parts_p.erase(FNp);
				FNp--;
				tools::str::remove_substring_from_mainstring(&filename_without_crater_depths_s,delimiter+value+unit);
				tools::str::remove_substring_from_mainstring(&filename_without_crater_depths_s,value+unit+delimiter);
			}
		}
	}
	return total_sputter_depths_p;
}

const std::string& files_::file_t::crater_in_name_t::filename_without_crater_depths()
{
	if (filename_without_crater_depths_s=="" && total_sputter_depths_p.data().size()==0)
		total_sputter_depths();
	return filename_without_crater_depths_s;
}









