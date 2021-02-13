#include "log.hpp"

bool logger::activate_debug = false;
vector<string> logger::log_messages;

void logger::debug(std::__cxx11::string message, string variable)
{
	stringstream crit;
	crit << "*** DEBUG ***"  << "\t\t" << message << "\t\t" << variable ;
	log_messages.push_back(crit.str());
// 	cout << crit.str();
}
void logger::debug(std::__cxx11::string message, int variable)
{
	debug(message,to_string(variable));
}
void logger::debug(std::__cxx11::string message, double variable)
{
	debug(message,to_string(variable));
}
// void logger::debug(std::__cxx11::string message, bool variable)
// {
// 	if (variable==true)
// 		debug(message,"TRUE");
// 	else
// 		debug(message,"FALSE");
// }


void logger::fatal(std::__cxx11::string message, string variable)
{
	stringstream crit;
	crit << "*** FATAL ***"  << "\t\t" << message << "\t\t" << variable ;
	log_messages.push_back(crit.str());
// 	cout << crit.str();
}
void logger::fatal(std::__cxx11::string message, int variable)
{
	fatal(message,to_string(variable));
}
void logger::fatal(std::__cxx11::string message, double variable)
{
	fatal(message,to_string(variable));
}
// void logger::fatal(std::__cxx11::string message, bool variable)
// {
// 	if (variable==true)
// 		fatal(message,"TRUE");
// 	else
// 		fatal(message,"FALSE");
// }

void logger::error(std::__cxx11::string message, string variable)
{
	stringstream crit;
	crit << "*** ERROR ***"  << "\t\t" << message << "\t\t" << variable ;
	log_messages.push_back(crit.str());
// 	cout << crit.str();
// 	log::log_message("*** ERROR ***",message, variable);
}
void logger::error(std::__cxx11::string message, int variable)
{
	error(message,to_string(variable));
}
void logger::error(std::__cxx11::string message, double variable)
{
	error(message,to_string(variable));
}
// void logger::error(std::__cxx11::string message, bool variable)
// {
// 	if (variable==true)
// 		error(message,"TRUE");
// 	else
// 		error(message,"FALSE");
// }

void logger::info(std::__cxx11::string message, string variable)
{
	stringstream crit;
	crit << "*** INFO ***"  << "\t\t" << message << "\t\t" << variable ;
	log_messages.push_back(crit.str());
// 	cout << crit.str();
// 	log::log_message("*** INFO ***",message, variable);
}
void logger::info(std::__cxx11::string message, int variable)
{
	info(message,to_string(variable));
}
void logger::info(std::__cxx11::string message, double variable)
{
	info(message,to_string(variable));
}
// void logger::info(std::__cxx11::string message, bool variable)
// {
// 	if (variable==true)
// 		info(message,"TRUE");
// 	else
// 		info(message,"FALSE");
// }

void logger::warning(std::__cxx11::string message, string variable)
{
	stringstream crit;
	crit << "*** WARNING ***"  << "\t\t" << message << "\t\t" << variable ;
	log_messages.push_back(crit.str());
// 	cout << crit.str();
}
void logger::warning(std::__cxx11::string message, int variable)
{
	warning(message,to_string(variable));
}
void logger::warning(std::__cxx11::string message, double variable)
{
	warning(message,to_string(variable));
}
// void logger::warning(std::__cxx11::string message, bool variable)
// {
// 	if (variable==true)
// 		warning(message,"TRUE");
// 	else
// 		warning(message,"FALSE");
// }

void logger::to_screen()
{
	print(log_messages);
}

