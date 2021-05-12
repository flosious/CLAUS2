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

#include "print.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "definitions.hpp"

using namespace std;

// static vector<string> fatal_messages; // 4
// static vector<string> error_messages; // 3
// static vector<string> warning_messages; // 2
// static vector<string> info_messages; // 1
// static vector<string> debug_messages; // 0



// template <class T>
class logger
{
	friend class config_t;
private:
	static bool activate_debug;
// 	static void log_message(string type, string message, T variable={});
	class message_t
	{
	private:
// 		enum type_t
// 		{
// 			fatal,
// 			error,
// 			warning,
// 			info,
// 			debug
// 		};
// 		static const map<type_t,string> type_to_string;
		///type of message (fatal, error, warning, info or debug)
		const string mtype;
		const string class_func_name;
// 		const string function_name;
// 		const string condition_string;
		const string variable_condition;
		const string variable_contents;
		///suggestion(in case of fatal or error) or automatically(in case of warning, info) used solution
		const string solution;
		
	public:
		message_t(string mtype,std::string class_func_name, string variable_condition, string variable_contents, string solution) : 
			 mtype(mtype), class_func_name(class_func_name),  variable_condition(variable_condition), variable_contents(variable_contents), solution(solution)
							  {};
		const string to_string() const;
	};
public:
	///strong type: mtype should contain a strong map fatal->"fatal", error->"error", ...
// 	struct mtype {string type;};
// 	static const mtype fatal{"fatal"};
	
	
// 	static bool log_debug;
	//default is 0 -> no record; increase for more verbosity
	static int verbosity_level;
	static vector<message_t> messages;
	///adds a fatal message to the messages
	static void fatal(string class_func_name, string variable_condition, string variable_contents="", string solution="dying...");
// 	static void fatal(string class_name, string function_name, string variable_name, int variable_contents, string condition_string="", string solution="dying...");
// 	static void fatal(string class_name, string function_name, string variable_name, bool variable_contents, string condition_string="", string solution="dying...");
	///adds a error message to the messages
	static void error(string class_func_name, string variable_condition, string variable_contents="", string solution="");
// 	static void error(string class_name, string function_name, string variable_name, int variable_contents, string condition_string="", string solution="");
// 	static void error(string class_name, string function_name, string variable_name, bool variable_contents, string condition_string="", string solution="");
	///adds a warning message to the messages
	static void warning(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="",string solution="");
// 	static void warning(unsigned int verbosity, string class_name, string function_name, string variable_name, int variable_contents, string condition_string="", string solution="");
// 	static void warning(unsigned int verbosity, string class_name, string function_name, string variable_name, bool variable_contents, string condition_string="", string solution="");
	///adds a info message to the messages
	static void info(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="", string solution="");
// 	static void info(unsigned int verbosity, string class_name, string function_name, string variable_name, int variable_contents, string condition_string="", string solution="");
// 	static void info(unsigned int verbosity, string class_name, string function_name, string variable_name, bool variable_contents, string condition_string="", string solution="");
	///adds a debug message to the messages
	static void debug(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="", string solution="");
// 	static void debug(unsigned int verbosity, string class_name, string function_name, string variable_name, int variable_contents, string condition_string, string solution);
// 	static void debug(unsigned int verbosity, string class_name, string function_name, string variable_name, bool variable_contents, string condition_string, string solution);
	
// 	static vector<string> log_messages;
// 	///chronological order of all recorded messages
// 	///  Any error that is forcing a shutdown of the service or application to prevent data loss (or further data loss).
// 	static void fatal(string message,string variable={});
// 	static void fatal(string message,int variable);
// 	static void fatal(string message,double variable);
// // 	static void fatal(string message,bool variable);
// 	/// Any error which is fatal to the operation, but not the service or application (can't open a required file, missing data, etc.). These errors will force user (administrator, or direct user) intervention.
// 	static void error(string message,string variable={});
// 	static void error(string message,int variable);
// 	static void error(string message,double variable);
// // 	static void error(string message,bool variable);
// 	/// Anything that can potentially cause application oddities, but for which I am automatically recovering.
// 	static void warning(string message,string variable={});
// 	static void warning(string message,int variable);
// 	static void warning(string message,double variable);
// // 	static void warning(string message,bool variable);
// 	/// Generally useful information to log (service start/stop, configuration assumptions, etc).
// 	static void info(string message,string variable={});
// 	static void info(string message,int variable);
// 	static void info(string message,double variable);
// // 	static void info(string message,bool variable);
// 	
// 	static void debug(string message,string variable={},int verbosity=0);
// 	static void debug(string message,int variable,int verbosity=0);
// 	static void debug(string message,double variable,int verbosity=0);
// // 	static void debug(string message,bool variable);
	static void to_screen();
};

#endif // LOG_T_HPP
