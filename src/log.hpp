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
#include "tools.hpp"

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
// 		message_t(string mtype,std::string class_func_name, string variable_condition, bool variable_contents_b);
		const string to_string() const;
	};
public:
	static const bool instant_print_messages=true;
// 	static bool log_debug;
	//default is 0 -> no record; increase for more verbosity
	static int verbosity_level;
	static vector<message_t> messages;
	///adds a fatal message to the messages
	static void fatal(string class_func_name, string variable_condition, string variable_contents="", string solution="dying...");
	///adds a error message to the messages
	static void error(string class_func_name, string variable_condition, string variable_contents="", string solution="");
	///adds a warning message to the messages
	static void warning(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="",string solution="");
	///adds a info message to the messages
	static void info(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="", string solution="");
	///adds a debug message to the messages
	static void debug(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents="", string solution="");
	static void debug(unsigned int verbosity, string class_func_name, string variable_condition, bool variable_contents);
	static void to_screen();
	static string to_string(const string line_delimiter);
	static void to_file(string path_with_filename);
};

#endif // LOG_T_HPP
