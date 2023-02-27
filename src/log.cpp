#include "log.hpp"

/***************************/
/*** logger_message_t ******/
/***************************/

logger_message_t::logger_message_t(unsigned int verbosity_level,
                               mtype_t mtype_s,
                               std::string_view source_file_name,
                               std::string_view class_name,
                               std::string_view func_name,
                               std::string_view object_name,
                               std::string_view message_description,
                               std::string_view message_details,
                               unsigned long sequence_id)
                                :
     verbosity_level(verbosity_level),
     mtype(mtype_s),
     source_file_name(source_file_name),
     class_name(class_name),
     func_name(func_name),
     object_name(object_name),
     message_description(message_description),
     message_details(message_details),
     sequence_id(sequence_id)
{
}

const std::map<logger_message_t::mtype_t,std::string> logger_message_t::type_to_string
{
    {fatal,"FATAL"},
    {error,"ERROR"},
    {warning,"WARNING"},
    {info,"INFO"},
    {debug,"DEBUG"},
};


std::string logger_message_t::type_string() const
{
    return type_to_string.at(mtype);
}

std::string logger_message_t::to_string(const std::string delimiter) const
{
	std::stringstream out;
    out << type_to_string.at(mtype);
    out << delimiter << source_file_name;
    out << delimiter << class_name;
    out << delimiter << func_name;
    out << delimiter << object_name;
    if (message_description!="")
    {
        out << delimiter << message_description;
    }
    if (message_details!="")
    {
        out << delimiter << message_details;
    }
	return out.str();
}

bool logger_message_t::operator<(const logger_message_t& msg) const
{
    if (sequence_id<msg.sequence_id)
        return true;
    return false;
}

/*********************/
/***   filter_t    ***/
/*********************/

logger_t::filter_t::filter_t(const std::vector<logger_message_t>& messages_s) : messages_p(messages_s)
{
}

const std::vector<logger_message_t>& logger_t::filter_t::messages() const
{
    return messages_p;
}

logger_t::filter_t logger_t::filter_t::max_verbosity_level(unsigned int max_lvl) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.verbosity_level<=max_lvl)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::type(logger_message_t::mtype_t mtype) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.mtype == mtype)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::source_file_name(const std::string source_file_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.source_file_name == source_file_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::class_name(const std::string class_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.class_name == class_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::func_name(const std::string func_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.func_name == func_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::object_name(const std::string object_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.object_name == object_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}




/*********************/
/***  use_case_t   ***/
/*********************/



use_case_t::use_case_t(
        logger_message_t::mtype_t mtype,
        std::string_view source_file_name,
        std::string_view class_name,
        std::string_view func_name,
        std::string_view object_name)
            : mtype(mtype),
              source_file_name(source_file_name),
              class_name(class_name),
              func_name(func_name),
              object_name(object_name)
{

}

void use_case_t::log(std::string_view message_description, std::string_view message_details, unsigned int verbosity_level)
{
    logger_t::log(verbosity_level,mtype,source_file_name,class_name,func_name,object_name,message_description, message_details);
}

//void use_case_t::signal(std::string_view message_description,unsigned int verbosity_level, std::string_view message_details)
//{
//    log(message_description,message_details,verbosity_level);
//}
//void use_case_t::enter(unsigned int verbosity_level, std::string_view message_details)
//{
//    log("enter",message_details,verbosity_level);
//}
void use_case_t::exit(unsigned int verbosity_level, std::string_view message_details)
{
    log("exit",message_details,verbosity_level);
}

void use_case_t::quantity(std::string_view quantity_to_string_short, unsigned int verbosity_level, std::string_view message_details)
{
    log(quantity_to_string_short,message_details,verbosity_level);
}

//void use_case_t::value(std::string_view value_to_string, unsigned int verbosity_level, std::string_view message_details)
//{
//    log(value_to_string,message_details,verbosity_level);
//}

void use_case_t::is_statement(std::string_view concerning_object_name, bool value, std::string_view consequence ,unsigned int verbosity_level)
{
    std::stringstream message_description, message_details;
    if (value)
    {
        message_description << concerning_object_name << " is TRUE";
    }
    else
    {
        message_description << concerning_object_name << " is false";
    }
    message_details << consequence;
    log(message_description.str(),message_details.str(),verbosity_level);
}

//void use_case_t::if_statement(std::string_view concerning_object_A_name, std::string_view concerning_object_A_value, std::string_view comparator, std::string_view concerning_object_B_name, std::string_view concerning_object_B_value, unsigned int verbosity_level)
//{
//    std::stringstream message_description, message_details;
//    message_description << concerning_object_A_name << comparator << concerning_object_B_name;
//    message_details << concerning_object_A_value << comparator << concerning_object_B_value;
//    log(message_description.str(),message_details.str(),verbosity_level);
//}

/*********************/
/*********************/
/*********************/

//default 0 -> no record
//int logger_t::verbosity_level() = 11;
//vector<logger_message_t> logger_t::messages_p;
// bool logger_t::log_debug=true;

void logger_t::print_last()
{
    if (messages().size()>0)
        print(messages().back());
}

bool logger_t::print_to_standard_output = true;

void logger_t::print(const logger_message_t& message)
{
    auto& M = message;
    if (print_to_standard_output)
    {
        std::cout << M.to_string() << std::endl;
    }
    return ; // rest make the program very slow
}

void logger_t::log(unsigned int verbosity_level,
         logger_message_t::mtype_t mtype,
         std::string_view source_file_name,
         std::string_view class_name,
         std::string_view func_name,
         std::string_view object_name,
         std::string_view message_description,
         std::string_view message_details)
{
    logger_message_t T {       verbosity_level,
                               mtype,
                               source_file_name,
                               class_name,
                               func_name,
                               object_name,
                               message_description,
                               message_details,
                               messages().size()
                       };
    add_message(T);
    print_last();
}

std::vector<logger_message_t> logger_t::messages_p;

void logger_t::add_message(const logger_message_t& message)
{
    messages_p.push_back(message);
}

logger_t::logger_t()
{

}

const std::vector<logger_message_t>& logger_t::messages()
{
    return messages_p;
}

//std::vector<logger_message_t>& logger_t::messages()
//{
//    return messages_p;
//}

void logger_t::clear_messages()
{
    messages_p.clear();
}

void logger_t::clear_messages(std::vector<unsigned long> ids)
{
    std::remove_if(messages_p.begin(),messages_p.end(), [ids](logger_message_t& M) {if (std::find(ids.begin(),ids.end(),M.sequence_id)==ids.end()) return false; return true; } );
}

logger_t::filter_t logger_t::filter()
{
    return filter_t(messages_p);
}

void logger_t::to_screen()
{
    std::cout << "[TYPE]\t" << "[source_file_name]\t" << "[class_name]\t" << "[func_name]\t" << "[object_name]\t" << "[message_description]\t" << "[message_details]" << std::endl;
    for (auto& M : messages())
        std::cout << M.to_string() << std::endl;
}

std::string logger_t::to_string(const std::string line_delimiter)
{
	std::stringstream out;
    for (auto& M : messages_p)
		out << M.to_string() << line_delimiter;
	return out.str();
}

void logger_t::to_file(std::string path_with_filename)
{
	std::string contents = to_string("\n");
	tools::file::write_to_file_and_create_folders(path_with_filename,&contents,false);
}


/******************/

//logger_t::use_case_t logger_t::fatal(std::string_view func_name,
//           std::string_view object_name)
//{
//    return fatal("","",func_name,object_name);
//}

use_case_t logger_t::fatal(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
//    log(verbosity_level,logger_message_t::fatal,source_file_name,class_name,func_name,object_name,message_description,message_details);
    return use_case_t(logger_message_t::fatal, source_file_name, class_name,func_name,object_name);
}

/******************/
//logger_t::use_case_t logger_t::error(std::string_view func_name,
//           std::string_view object_name)
//{
//    return error("","",func_name,object_name);
//}
use_case_t logger_t::error(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::error, source_file_name, class_name,func_name,object_name);
}
/******************/
//logger_t::use_case_t logger_t::warning(std::string_view func_name,
//           std::string_view object_name)
//{
//    return warning("","",func_name,object_name);
//}
use_case_t logger_t::warning(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::warning, source_file_name, class_name,func_name,object_name);
}
/******************/
//logger_t::use_case_t logger_t::info(std::string_view func_name,
//           std::string_view object_name)
//{
//    return info("","",func_name,object_name);
//}
use_case_t logger_t::info(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::info, source_file_name, class_name,func_name,object_name);
}
/******************/
//logger_t::use_case_t logger_t::debug(std::string_view func_name,
//           std::string_view object_name)
//{
//    return debug("","",func_name,object_name);
//}
use_case_t logger_t::debug(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::debug, source_file_name, class_name,func_name,object_name);
}

/**************************************/
/******     class_logger_t       ******/
/**************************************/

//std::shared_ptr<loggerwindow> class_logger_t::window() const
//{
//    return logger->window_p;
//}

//void class_logger_t::add_message(const logger_message_t& message)
//{
//    logger_t::add_message(message);
//}

class_logger_t::class_logger_t(const std::string source_file_name_p,
                               const std::string class_name_p)
    : source_file_name_p(source_file_name_p), class_name_p(class_name_p)
{

}

//table_log* class_logger_t::window()
//{
////    std::cout << "class_logger_t::window()" << std::endl;
//    return logger->window();
//}

//std::vector<logger_message_t>& class_logger_t::messages()
//{
//    return logger->messages();
//}

//bool class_logger_t::print_debug() const
//{
//    logger->print_debug();
//}
//bool class_logger_t::print_info() const
//{
//    logger->print_info();
//}
//bool class_logger_t::print_warning() const
//{
//    logger->print_warning();
//}

use_case_t class_logger_t::fatal(std::string_view func_name, std::string_view object_name)
{
    return logger_t::fatal(source_file_name_p, class_name_p, func_name, object_name);
}

use_case_t class_logger_t::error(std::string_view func_name, std::string_view object_name)
{
    return logger_t::error(source_file_name_p, class_name_p, func_name, object_name);
}

use_case_t class_logger_t::warning(std::string_view func_name, std::string_view object_name)
{
    return logger_t::warning(source_file_name_p, class_name_p, func_name, object_name);
}

use_case_t class_logger_t::info(std::string_view func_name, std::string_view object_name)
{
    return logger_t::info(source_file_name_p, class_name_p, func_name, object_name);
}

use_case_t class_logger_t::debug(std::string_view func_name, std::string_view object_name)
{
    return logger_t::debug(source_file_name_p, class_name_p, func_name, object_name);
}

/**************************************/
/******       function_logger_t      ********/
/**************************************/

function_logger_t::function_logger_t(const class_logger_t& class_logger, std::string_view func_name) :
    func_name_p(func_name),class_logger(class_logger)
{
}

use_case_t function_logger_t::fatal(std::string_view object_name)
{
    return class_logger.fatal(func_name_p,object_name);
}
use_case_t function_logger_t::error(std::string_view object_name)
{
    return class_logger.error(func_name_p,object_name);
}
use_case_t function_logger_t::warning(std::string_view object_name)
{
    return class_logger.warning(func_name_p,object_name);
}
use_case_t function_logger_t::info(std::string_view object_name)
{
    return class_logger.info(func_name_p,object_name);
}
use_case_t function_logger_t::debug(std::string_view object_name)
{
    return class_logger.debug(func_name_p,object_name);
}

