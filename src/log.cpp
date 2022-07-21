#include "log.hpp"

/********************/
/*** message_t ******/
/********************/

// constexpr logger::message_t::message_t(logger::message_t::type_t mtype, 
// 							 string class_name, 
// 							 string function_name, 
// 							 string variable_name, 
// 							 string variable_contents, 
// 							 string condition_string, 
// 							 unsigned int verbosity) :
// 							 mtype(mtype),
// 							 class_name(class_name),
// 							 function_name(function_name),
// 							 variable_name(variable_name),
// 							 variable_contents(variable_contents),
// 							 condition_string(condition_string),
// 							 verbosity(verbosity)
// {
// }

// const map<logger::message_t::type_t,string> logger::message_t::type_to_string
// {
// 	{fatal,"FATAL"},
// 	{error,"ERROR"},
// 	{warning,"WARNING"},
// 	{info,"INFO"},
// 	{debug,"DEBUG"},
// };

// logger::message_t::message_t(string mtype,std::string class_func_name, string variable_condition, bool variable_contents_b) : 
// 			 mtype(mtype), class_func_name(class_func_name),  variable_condition(variable_condition)
// {
// 	if (variable_contents_b)
// 		variable_contents="true";
// 	else
// 		variable_contents="false";
// }

const string logger::message_t::to_string() const
{
	stringstream out;
	out <<  mtype ;
	out << "\t" << class_func_name;
	if (variable_condition!="") out << "\t" << variable_condition;
	if (variable_contents!="") out <<"\t'"<< variable_contents << "'";
// 	if (condition_string!="") out << "'\t"<<condition_string;
	if (solution!="") out <<"\t"<<solution;
	return out.str();
}

/*********************/
/*********************/
/*********************/

//default 0 -> no record
int logger::verbosity_level = 11;
vector<logger::message_t> logger::messages;
// bool logger::log_debug=true;

void logger::to_screen()
{
	for (auto& M : messages)
		cout << M.to_string() << endl;
}

string logger::to_string(const string line_delimiter)
{
	stringstream out;
	for (auto& M : messages)
		out << M.to_string() << line_delimiter;
	return out.str();
}

void logger::to_file(string path_with_filename)
{
	string contents = to_string("\n");
	tools::file::write_to_file_and_create_folders(path_with_filename,&contents,false);
}

void logger::fatal(string class_func_name, string variable_condition, string variable_contents, string solution)
{
	messages.push_back({"FATAL",class_func_name,variable_condition,variable_contents,solution});
	cout << messages.back().to_string() << endl;
}

void logger::error(string class_func_name, string variable_condition, string variable_contents, string solution)
{
	messages.push_back({"ERROR",class_func_name,variable_condition,variable_contents, solution});
	if (instant_print_messages) cout << messages.back().to_string() << endl;
}

void logger::warning(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents, string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
	{
		messages.push_back({"warning",class_func_name,variable_condition,variable_contents,solution});
		if (instant_print_messages) cout << messages.back().to_string() << endl;
	}
}

void logger::info(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents, string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
	{
		messages.push_back({"info",class_func_name,variable_condition,variable_contents,solution});
		if (instant_print_messages) cout << messages.back().to_string() << endl;
	}
}

void logger::debug(unsigned int verbosity, string class_func_name, string variable_condition, string variable_contents, string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
	{
		messages.push_back({"debug",class_func_name,variable_condition,variable_contents,solution});
		if (instant_print_messages) cout << messages.back().to_string() << endl;
	}
}

void logger::debug(unsigned int verbosity, string class_func_name, string variable_condition, bool variable_contents)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
	{
		if (variable_contents)
			messages.push_back({"debug",class_func_name,variable_condition,"true",""});
		else 
			messages.push_back({"debug",class_func_name,variable_condition,"false",""});
		if (instant_print_messages) cout << messages.back().to_string() << endl;
	}
}



