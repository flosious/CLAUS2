
/*
    Copyright (C) 2021-2022 Florian Bärwolf
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




#include <string>
// #include "filecontents.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #define PATH_DELIMITER std::string("\\")
    #define LINE_DELIMITER std::string("\r\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    #define PATH_DELIMITER std::string("/")
    #define LINE_DELIMITER std::string("\n")
#endif

#define FILE_TYPE_DELIMITER std::string(".")

#define __CLASS__ typeid(*this).name()
//definition; used in class constructor
#define log_c class_logger(__FILE__,__CLASS__)

//used in a class function to start logging
#define log_f function_logger_t logger(class_logger, __func__);

#endif // DEFINITIONS_T
