
#include "tools.hpp"


using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#define path_del "\\"
#define line_del "\r\n"
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
#define path_del "/"
#define line_del "\n"
#endif


bool tools::transform_string_to_xml(std::string *content,std::string *header,std::string *result) {
// 	std::string result;
	
	*result=string("<")+*header+string(">\n");
	*result=*result+*content;
	*result=*result+string("\n</")+*header+string(">\n");
	
	return true;
}

bool tools::transform_xml_to_string(std::string *content,std::string *header,std::string *xml,int iter) {
	std::string xml_copy=*xml;
	std::string header_end;
// 	std::string content_temp;
	
	for (int i=0;i<=iter;i++) {
		*header=tools::str::get_string_between_string_A_and_next_B(&xml_copy,"<",">");
		if (*header=="") return false;
		*content=tools::str::get_string_between_string_A_and_next_B(&xml_copy,std::string("<")+*header+string(">\n"),std::string("\n</")+*header+string(">"));
		header_end=string("</")+*header+string(">");
		xml_copy.erase(0,xml_copy.find(header_end)+header_end.length());
	}
	return true;
}

bool tools::transform_xml_to_vector_string(std::vector<std::string> *content,std::vector<std::string> *header,std::string *xml)
{
	std::string xml_copy=tools::str::remove_chars_from_string(*xml,"\r"); 
	std::string header_end;
	std::string header_temp;
	
	
	header_temp=tools::str::get_string_between_string_A_and_next_B(&xml_copy,"<",">");
	while (header_temp!="") {
		content->push_back(tools::str::get_string_between_string_A_and_next_B(&xml_copy,std::string("<")+header_temp+string(">\n"),std::string("\n</")+header_temp+string(">")));
		header->push_back(header_temp);
		header_end=string("</")+header_temp+string(">");
		xml_copy.erase(0,xml_copy.find(header_end)+header_end.length());
		header_temp=tools::str::get_string_between_string_A_and_next_B(&xml_copy,"<",">");
	}
    return true;
}

void tools::wait_for_key ()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)  // every keypress registered,std:: also arrow keys
std::cout << std::endl << "Press any key to continue..." << std::endl;

FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
_getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
std::cout << std::endl << "Press ENTER to continue..." << std::endl;

std::cin.clear();
std::cin.ignore(std::cin.rdbuf()->in_avail());
std::cin.get();
#endif
return;
}

