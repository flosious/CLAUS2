#include "log.hpp"

/********************/
/*** message_t ******/
/********************/

// constexpr logger::message_t::message_t(logger::message_t::type_t mtype, 
// 							 std::__cxx11::string class_name, 
// 							 std::__cxx11::string function_name, 
// 							 std::__cxx11::string variable_name, 
// 							 std::__cxx11::string variable_contents, 
// 							 std::__cxx11::string condition_string, 
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
int logger::verbosity_level = 22;
vector<logger::message_t> logger::messages;
// bool logger::log_debug=true;

void logger::to_screen()
{
	for (auto& M : messages)
		cout << M.to_string() << endl;
}

void logger::fatal(std::__cxx11::string class_func_name, std::__cxx11::string variable_condition, std::__cxx11::string variable_contents, std::__cxx11::string solution)
{
	messages.push_back({"FATAL",class_func_name,variable_condition,variable_contents,solution});
	cout << messages.back().to_string() << endl;
}

void logger::error(std::__cxx11::string class_func_name, std::__cxx11::string variable_condition, std::__cxx11::string variable_contents, std::__cxx11::string solution)
{
	messages.push_back({"ERROR",class_func_name,variable_condition,variable_contents, solution});
	if (instant_print_messages) cout << messages.back().to_string() << endl;
}

void logger::warning(unsigned int verbosity, std::__cxx11::string class_func_name, std::__cxx11::string variable_condition, std::__cxx11::string variable_contents, std::__cxx11::string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
		messages.push_back({"warning",class_func_name,variable_condition,variable_contents,solution});
	
}

void logger::info(unsigned int verbosity, std::__cxx11::string class_func_name, std::__cxx11::string variable_condition, std::__cxx11::string variable_contents, std::__cxx11::string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
		messages.push_back({"info",class_func_name,variable_condition,variable_contents,solution});
}

void logger::debug(unsigned int verbosity, std::__cxx11::string class_func_name, std::__cxx11::string variable_condition, std::__cxx11::string variable_contents, std::__cxx11::string solution)
{
	//just record if verbosity is high enough
	if (verbosity_level>=verbosity)
	{
		messages.push_back({"debug",class_func_name,variable_condition,variable_contents,solution});
		if (instant_print_messages) cout << messages.back().to_string() << endl;
	}
}





// bool logger::activate_debug = false;
// vector<string> logger::log_messages;
// 
// void logger::debug(std::__cxx11::string message, string variable,int verbosity)
// {
// 	stringstream crit;
// 	crit << "*** DEBUG ***"  << "\t\t" << message << "\t\t" << variable ;
// 	log_messages.push_back(crit.str());
// // 	cout << crit.str();
// }
// void logger::debug(std::__cxx11::string message, int variable,int verbosity)
// {
// 	debug(message,to_string(variable),verbosity);
// }
// void logger::debug(std::__cxx11::string message, double variable,int verbosity)
// {
// 	debug(message,to_string(variable),verbosity);
// }
// 
// void logger::fatal(std::__cxx11::string message, string variable)
// {
// 	stringstream crit;
// 	crit << "*** FATAL ***"  << "\t\t" << message << "\t\t" << variable ;
// 	log_messages.push_back(crit.str());
// 	cout << crit.str();
// }
// void logger::fatal(std::__cxx11::string message, int variable)
// {
// 	fatal(message,to_string(variable));
// }
// void logger::fatal(std::__cxx11::string message, double variable)
// {
// 	fatal(message,to_string(variable));
// }
// 
// void logger::error(std::__cxx11::string message, string variable)
// {
// 	stringstream crit;
// 	crit << "*** ERROR ***"  << "\t\t" << message << "\t\t" << variable ;
// 	log_messages.push_back(crit.str());
// // 	cout << crit.str();
// // 	log::log_message("*** ERROR ***",message, variable);
// }
// void logger::error(std::__cxx11::string message, int variable)
// {
// 	error(message,to_string(variable));
// }
// void logger::error(std::__cxx11::string message, double variable)
// {
// 	error(message,to_string(variable));
// }
// 
// void logger::info(std::__cxx11::string message, string variable)
// {
// 	stringstream crit;
// 	crit << "*** INFO ***"  << "\t\t" << message << "\t\t" << variable ;
// 	log_messages.push_back(crit.str());
// // 	cout << crit.str();
// // 	log::log_message("*** INFO ***",message, variable);
// }
// void logger::info(std::__cxx11::string message, int variable)
// {
// 	info(message,to_string(variable));
// }
// void logger::info(std::__cxx11::string message, double variable)
// {
// 	info(message,to_string(variable));
// }
// 
// 
// void logger::warning(std::__cxx11::string message, string variable)
// {
// 	stringstream crit;
// 	crit << "*** WARNING ***"  << "\t\t" << message << "\t\t" << variable ;
// 	log_messages.push_back(crit.str());
// // 	cout << crit.str();
// }
// void logger::warning(std::__cxx11::string message, int variable)
// {
// 	warning(message,to_string(variable));
// }
// void logger::warning(std::__cxx11::string message, double variable)
// {
// 	warning(message,to_string(variable));
// }



