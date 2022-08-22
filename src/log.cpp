#include "log.hpp"

/********************/
/*** logger_message_t ******/
/********************/

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

const map<logger_message_t::mtype_t,string> logger_message_t::type_to_string
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

string logger_message_t::to_string(const string delimiter) const
{
	stringstream out;
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

logger_t::filter_t logger_t::filter_t::source_file_name(const string source_file_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.source_file_name == source_file_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::class_name(const string class_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.class_name == class_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::func_name(const string func_name_s) const
{
    std::vector<logger_message_t> new_messages;
    for (const auto& m : messages())
    {
        if (m.func_name == func_name_s)
            new_messages.push_back(m);
    }
    return filter_t(new_messages);
}

logger_t::filter_t logger_t::filter_t::object_name(const string object_name_s) const
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



logger_t::use_case_t::use_case_t(
        logger_message_t::mtype_t mtype,
        std::string_view source_file_name,
        std::string_view class_name,
        std::string_view func_name,
        std::string_view object_name,
        logger_t& logger)
            : mtype(mtype),
              source_file_name(source_file_name),
              class_name(class_name),
              func_name(func_name),
              object_name(object_name),
              logger(logger)
{

}

void logger_t::use_case_t::log(std::string_view message_description, std::string_view message_details, unsigned int verbosity_level)
{
    logger.log(verbosity_level,mtype,source_file_name,class_name,func_name,object_name,message_description, message_details);
}

void logger_t::use_case_t::signal(std::string_view message_description,unsigned int verbosity_level, std::string_view message_details)
{
    log(message_description,message_details,verbosity_level);
}
void logger_t::use_case_t::enter(unsigned int verbosity_level, std::string_view message_details)
{
    log("enter",message_details,verbosity_level);
}
void logger_t::use_case_t::exit(unsigned int verbosity_level, std::string_view message_details)
{
    log("exit",message_details,verbosity_level);
}

void logger_t::use_case_t::quantity(std::string_view quantity_to_string_short, unsigned int verbosity_level, std::string_view message_details)
{
    log(quantity_to_string_short,message_details,verbosity_level);
}

void logger_t::use_case_t::value(std::string_view value_to_string, unsigned int verbosity_level, std::string_view message_details)
{
    log(value_to_string,message_details,verbosity_level);
}

void logger_t::use_case_t::is_statement(std::string_view concerning_object_name, bool value, std::string_view consequence ,unsigned int verbosity_level)
{
    stringstream message_description, message_details;
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

void logger_t::use_case_t::if_statement(std::string_view concerning_object_A_name, std::string_view concerning_object_A_value, std::string_view comparator, std::string_view concerning_object_B_name, std::string_view concerning_object_B_value, unsigned int verbosity_level)
{
    stringstream message_description, message_details;
    message_description << concerning_object_A_name << comparator << concerning_object_B_name;
    message_details << concerning_object_A_value << comparator << concerning_object_B_value;
    log(message_description.str(),message_details.str(),verbosity_level);
}

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

void logger_t::print(const logger_message_t& message)
{
//    std::cout << "logger_t::print::enter" << std::endl;
//    debug(__FILE__,"logger_t",__func__,"this").enter();
    auto& M = message;
//    debug(__FILE__,"logger_t",__func__,"message").value(message.to_string());
    if (print_to_standard_output || window() == nullptr)
    {
        std::cout << M.to_string() << std::endl;
    }
    if (window()==nullptr)
    {
        std::cout << "logger_t::print::window() == nullptr" << std::endl;
        return;
    }
    switch (M.mtype)
    {
    case logger_message_t::fatal:
        window()->add_row({M.type_string(),M.source_file_name,M.class_name,M.func_name,M.object_name,M.message_description,M.message_details},table_log::black);
        break;
    case logger_message_t::error:
        window()->add_row({M.type_string(),M.source_file_name,M.class_name,M.func_name,M.object_name,M.message_description,M.message_details},table_log::magenta);
        break;
    case logger_message_t::warning:
        if (print_warning())
            window()->add_row({M.type_string(),M.source_file_name,M.class_name,M.func_name,M.object_name,M.message_description,M.message_details},table_log::yellow);
        break;
    case logger_message_t::info:
        if (print_info())
            window()->add_row({M.type_string(),M.source_file_name,M.class_name,M.func_name,M.object_name,M.message_description,M.message_details},table_log::green);
        break;
    case logger_message_t::debug:
        if (print_debug())
            window()->add_row({M.type_string(),M.source_file_name,M.class_name,M.func_name,M.object_name,M.message_description,M.message_details},table_log::cyan);
        break;
    }
//    std::cout << "logger_t::print::exit" << std::endl;
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
    logger_message_t T {              verbosity_level,
                               mtype,
                               source_file_name,
                               class_name,
                               func_name,
                               object_name,
                               message_description,
                               message_details,
                               messages_p.size()
                       };
    add_message(T);
    print_last();
}

void logger_t::add_message(const logger_message_t& message)
{
    messages_p.push_back(message);
}

logger_t::logger_t(table_log *window_s) : window_p(window_s)
{
}

void logger_t::window_header()
{
    if (window() == nullptr)
        return;
    window()->set_header({"TYPE","FILE","CLASS","FUNCTION","OBJECT","DESCRIPTION","DETAILS"});
}

table_log* logger_t::window()
{
    return window_p;
}

void logger_t::set_window(table_log *window_s)
{
    window_p = window_s;
}

void logger_t::set_print_debug(bool print_debug_s)
{
    if (print_debug()==print_debug_s)
        return;
    print_debug_p = print_debug_s;
    update_window();
}
void logger_t::set_print_warning(bool print_warning_s)
{
    if (print_warning()==print_warning_s)
        return;
    print_warning_p = print_warning_s;
    update_window();
}
void logger_t::set_print_info(bool print_info_s)
{
    if (print_info()==print_info_s)
        return;
    print_info_p = print_info_s;
    update_window();
}

bool logger_t::print_debug() const
{
    return print_debug_p;
}
bool logger_t::print_info() const
{
    return print_info_p;
}
bool logger_t::print_warning() const
{
    return print_warning_p;
}

void logger_t::update_window()
{
    debug(__FILE__,"logger_t",__func__,"this").enter();
    if (window()==nullptr)
    {
        debug(__FILE__,"logger_t",__func__,"window()").value("nullptr");
        return;
    }
    //filter messages
    std::vector<logger_message_t> fatals, errors, warnings, infos, debugs;
    fatals = filter().type(logger_message_t::fatal).messages();
    errors = filter().type(logger_message_t::error).messages();
    if (print_warning())
        warnings = filter().type(logger_message_t::warning).messages();
    if (print_info())
        infos = filter().type(logger_message_t::info).messages();
    if (print_debug())
        debugs = filter().type(logger_message_t::debug).messages();
    debug(__FILE__,"logger_t",__func__,"messages().size()").value(std::to_string(messages().size()));
    //collect filtered messages
    std::vector<logger_message_t> new_messages;
    new_messages.reserve(fatals.size()+errors.size()+warnings.size()+infos.size()+debugs.size());
    new_messages.insert(new_messages.end(),fatals.begin(),fatals.end());
    new_messages.insert(new_messages.end(),errors.begin(),errors.end());
    new_messages.insert(new_messages.end(),warnings.begin(),warnings.end());
    new_messages.insert(new_messages.end(),infos.begin(),infos.end());
    new_messages.insert(new_messages.end(),debugs.begin(),debugs.end());
    std::sort(new_messages.begin(),new_messages.end());
    debug(__FILE__,"logger_t",__func__,"new_messages.size()").value(std::to_string(new_messages.size()));
    //print the new messages
    window()->clearContents();
    window()->setRowCount(0);
    for (auto& M : new_messages)
        print(M);
//    window()->setColumnWidth()
//    window()->horizontalHeader()->setStretchLastSection(true);
    debug(__FILE__,"logger_t",__func__,"this").exit();
}

std::vector<logger_message_t>& logger_t::messages()
{
    return messages_p;
}

void logger_t::clear_messages()
{
    messages().clear();
}

void logger_t::clear_messages(std::vector<unsigned long> ids)
{
    std::remove_if(messages().begin(),messages().end(), [ids](logger_message_t& M) {if (std::find(ids.begin(),ids.end(),M.sequence_id)==ids.end()) return false; return true; } );
}

logger_t::filter_t logger_t::filter()
{
    return filter_t(messages());
}

void logger_t::to_screen()
{
    std::cout << "[TYPE]\t" << "[source_file_name]\t" << "[class_name]\t" << "[func_name]\t" << "[object_name]\t" << "[message_description]\t" << "[message_details]" << std::endl;
    for (auto& M : messages())
        std::cout << M.to_string() << std::endl;
}

string logger_t::to_string(const string line_delimiter)
{
	stringstream out;
    for (auto& M : messages_p)
		out << M.to_string() << line_delimiter;
	return out.str();
}

void logger_t::to_file(string path_with_filename)
{
	string contents = to_string("\n");
	tools::file::write_to_file_and_create_folders(path_with_filename,&contents,false);
}


/******************/

logger_t::use_case_t logger_t::fatal(std::string_view func_name,
           std::string_view object_name)
{
    return fatal("","",func_name,object_name);
}

logger_t::use_case_t logger_t::fatal(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
//    log(verbosity_level,logger_message_t::fatal,source_file_name,class_name,func_name,object_name,message_description,message_details);
    return use_case_t(logger_message_t::fatal, source_file_name, class_name,func_name,object_name,*this);
}

/******************/
logger_t::use_case_t logger_t::error(std::string_view func_name,
           std::string_view object_name)
{
    return error("","",func_name,object_name);
}
logger_t::use_case_t logger_t::error(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::error, source_file_name, class_name,func_name,object_name,*this);
}
/******************/
logger_t::use_case_t logger_t::warning(std::string_view func_name,
           std::string_view object_name)
{
    return warning("","",func_name,object_name);
}
logger_t::use_case_t logger_t::warning(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::warning, source_file_name, class_name,func_name,object_name,*this);
}
/******************/
logger_t::use_case_t logger_t::info(std::string_view func_name,
           std::string_view object_name)
{
    return info("","",func_name,object_name);
}
logger_t::use_case_t logger_t::info(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::info, source_file_name, class_name,func_name,object_name,*this);
}
/******************/
logger_t::use_case_t logger_t::debug(std::string_view func_name,
           std::string_view object_name)
{
    return debug("","",func_name,object_name);
}
logger_t::use_case_t logger_t::debug(std::string_view source_file_name,
                     std::string_view class_name,
                     std::string_view func_name,
                     std::string_view object_name)

{
    return use_case_t(logger_message_t::debug, source_file_name, class_name,func_name,object_name,*this);
}

/**************************************/
/******     class_logger_t       ******/
/**************************************/

//std::shared_ptr<loggerwindow> class_logger_t::window() const
//{
//    return logger->window_p;
//}

void class_logger_t::add_message(const logger_message_t& message)
{
    logger->add_message(message);
}

class_logger_t::class_logger_t(Logger& logger_s,
                               std::string_view source_file_name_p,
                               std::string_view class_name_p)
    : logger(logger_s), source_file_name_p(source_file_name_p), class_name_p(class_name_p)
{

}

table_log* class_logger_t::window()
{
//    cout << "class_logger_t::window()" << endl;
    return logger->window();
}

vector<logger_message_t>& class_logger_t::messages()
{
    return logger->messages();
}

bool class_logger_t::print_debug() const
{
    logger->print_debug();
}
bool class_logger_t::print_info() const
{
    logger->print_info();
}
bool class_logger_t::print_warning() const
{
    logger->print_warning();
}

logger_t::use_case_t class_logger_t::fatal(std::string_view func_name, std::string_view object_name)
{
    return logger_t::fatal(source_file_name_p, class_name_p, func_name, object_name);
}

logger_t::use_case_t class_logger_t::error(std::string_view func_name, std::string_view object_name)
{
    return logger_t::error(source_file_name_p, class_name_p, func_name, object_name);
}

logger_t::use_case_t class_logger_t::warning(std::string_view func_name, std::string_view object_name)
{
    return logger_t::warning(source_file_name_p, class_name_p, func_name, object_name);
}

logger_t::use_case_t class_logger_t::info(std::string_view func_name, std::string_view object_name)
{
    return logger_t::info(source_file_name_p, class_name_p, func_name, object_name);
}

logger_t::use_case_t class_logger_t::debug(std::string_view func_name, std::string_view object_name)
{
    return logger_t::debug(source_file_name_p, class_name_p, func_name, object_name);
}

/**************************************/
/**************************************/
/**************************************/



