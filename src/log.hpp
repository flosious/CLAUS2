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


#ifndef LOG_T_HPP
#define LOG_T_HPP

#include <string>
#include <vector>
#include <sstream>
#include "DEFINES.hpp"
#include <iostream>

using namespace std;

static vector<string> debug_messages;
static vector<string> error_messages;
static vector<string> fatal_messages;
static vector<string> warning_messages;
static vector<string> info_messages;

// template <class T>
class logger
{
	friend class config_t;
private:
	static bool activate_debug;
// 	static void log_message(string type, string message, T variable={});
public:
	///  Any error that is forcing a shutdown of the service or application to prevent data loss (or further data loss).
	static void fatal(string message,string variable={});
	static void fatal(string message,int variable);
	static void fatal(string message,double variable);
	static void fatal(string message,bool variable);
	/// Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.). These errors will force user (administrator, or direct user) intervention.
	static void error(string message,string variable={});
	static void error(string message,int variable);
	static void error(string message,double variable);
	static void error(string message,bool variable);
	/// Anything that can potentially cause application oddities, but for which I am automatically recovering.
	static void warning(string message,string variable={});
	static void warning(string message,int variable);
	static void warning(string message,double variable);
	static void warning(string message,bool variable);
	/// Generally useful information to log (service start/stop, configuration assumptions, etc).
	static void info(string message,string variable={});
	static void info(string message,int variable);
	static void info(string message,double variable);
	static void info(string message,bool variable);
	
	static void debug(string message,string variable={});
	static void debug(string message,int variable);
	static void debug(string message,double variable);
	static void debug(string message,bool variable);
};

#endif // LOG_T_HPP
