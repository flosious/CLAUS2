
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


#ifndef DEFINITIONS_T
#define DEFINITIONS_T


using namespace std;

#include <string>
// #include "file.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define PATH_DELIMITER string("\\")
    #define LINE_DELIMITER string("\r\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER string("/")
    #define LINE_DELIMITER string("\n")
#endif

#define FILE_TYPE_DELIMITER string(".")

// namespace 
// {
// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
// 	const std::string PATH_DELIMITER = "\\";
// 	const std::string LINE_DELIMITER = "\r\n";
// #elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
// 	const std::string PATH_DELIMITER = "/";
// 	const std::string LINE_DELIMITER = "\n";
// #endif
// 	const std::string FILE_TYPE_DELIMITER = ".";
// };

/*
class definitions_t
{
public:
	bool olcdbid;
	bool lot;
	bool lot_split;
	bool wafer;
	bool chip;
	bool monitor;
	bool group;
	bool repition;
	bool simple_name;
	definitions_t(bool olcdbid_s, bool lot_s, bool lot_split_s, bool wafer_s, bool monitor_s, bool chip_s, bool group_s, bool repetition_s, bool simple_name_s);
	
// 	const bool operator==(const file_t::name_t& obj1, const file_t::name_t& obj2) const;
};*/

#endif // DEFINITIONS_T
