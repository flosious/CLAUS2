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
#include <memory>
#include <string_view>
#include "table_log.hpp"

class logger_message_t
{
public:
    ///type levels
    enum mtype_t
    {
        fatal,
        error,
        warning,
        info,
        debug
    };
private:
    static const std::map<mtype_t,string> type_to_string;
    ///type of message (fatal, error, warning, info or debug)

public:
    logger_message_t(unsigned int verbosity_level,
              mtype_t mtype_s,
              std::string_view source_file_name,
              std::string_view class_name,
              std::string_view func_name,
              std::string_view object_name,
              std::string_view message_description,
              std::string_view message_details,
              unsigned long sequence_id);

    unsigned int verbosity_level;
    mtype_t mtype;
    std::string type_string() const;
    std::string source_file_name;
    std::string class_name;
    std::string func_name;
    std::string object_name;
    std::string message_description;
    std::string message_details;
    unsigned long sequence_id;

    string to_string(const string delimiter="\t") const;
    bool operator<(const logger_message_t& msg) const;
};

class logger_t
{
private:
    void print(const logger_message_t& message);
    void print_last();
    void log(unsigned int verbosity_level,
             logger_message_t::mtype_t mtype,
             std::string_view source_file_name,
             std::string_view class_name,
             std::string_view func_name,
             std::string_view object_name,
             std::string_view message_description,
             std::string_view message_details);

protected:
    std::vector<logger_message_t> messages_p;
    ///creates a local copy of the messages and works with that
    class filter_t
    {
    private:
        //a local copy
        std::vector<logger_message_t> messages_p;
    public:
        filter_t(const std::vector<logger_message_t>& messages_s);
        const std::vector<logger_message_t>& messages() const;
        filter_t max_verbosity_level(unsigned int max_lvl) const;
        filter_t type(logger_message_t::mtype_t mtype) const;
        filter_t source_file_name(const string source_file_name_s) const;
        filter_t class_name(const string class_name_s) const;
        filter_t func_name(const string func_name_s) const;
        filter_t object_name(const string object_name_s) const;
    };
    ///logging in use-case style
    class use_case_t
    {
    private:
        logger_message_t::mtype_t mtype;
        std::string_view source_file_name;
        std::string_view class_name;
        std::string_view func_name;
        std::string_view object_name;
        logger_t& logger;
        void log(std::string_view message_description, std::string_view message_details, unsigned int verbosity_level);
    public:
        use_case_t(
                logger_message_t::mtype_t mtype,
                std::string_view source_file_name,
                std::string_view class_name,
                std::string_view func_name,
                std::string_view object_name,
                logger_t& logger);
//                std::string_view message_description, // automatically determined
//                std::string_view message_details=""); // automatically determined
//                unsigned int verbosity_level=0); // automatically determined
        ///simple signaling message: e.g. "HERE"
        void signal(std::string_view message_description, unsigned int verbosity_level = 15, std::string_view message_details="");
        ///enter a function
        void enter(unsigned int verbosity_level = 15, std::string_view message_details="");
        ///exit a function
        void exit(unsigned int verbosity_level = 15, std::string_view message_details="");
        ///inform about a quantity
        void quantity(std::string_view quantity_to_string_short, unsigned int verbosity_level = 10, std::string_view message_details="");
        ///inform about a value
        void value(std::string_view value_to_string, unsigned int verbosity_level = 10, std::string_view message_details="");
        /// condition for existence: if (!A().is_set) --> then exit  :: logger_t::use_case_t::is_set(A().to_string(),FALSE,"exit"); //same as is_set()
        void is_statement(std::string_view concerning_object_name, bool value, std::string_view consequence="" ,unsigned int verbosity_level = 10);
        /// condition for more then existings ...
        void if_statement(std::string_view concerning_object_A_name, std::string_view concerning_object_A_value, std::string_view comparator, std::string_view concerning_object_B_name, std::string_view concerning_object_B_value,unsigned int verbosity_level = 10);
    };
#ifdef QT_DEBUG
    bool print_debug_p=true;
#else
    bool print_debug=false;
#endif
    bool print_warning_p=true;
    bool print_info_p=true;
    ///qt window / gui
    table_log *window_p;
public:
    logger_t(table_log *window_s = nullptr);
#ifdef QT_DEBUG
    bool print_to_standard_output=true;
#else
    bool print_to_standard_output=false;
#endif
    ///creates the window header
    void window_header();
    virtual table_log *window();
    void set_window(table_log *window_s);
    void set_print_debug(bool print_debug_s);
    void set_print_warning(bool print_warning_s);
    void set_print_info(bool print_info_s);
    virtual bool print_debug() const;
    virtual bool print_info() const;
    virtual bool print_warning() const;
    ///clears and reprints the contents()
    void update_window();


    ///adds a fatal message to the messages; using default parameters
    virtual use_case_t fatal(std::string_view func_name,
               std::string_view object_name);
	///adds a fatal message to the messages
    use_case_t fatal(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a error message to the messages; using default parameters
    virtual use_case_t error(std::string_view func_name,
               std::string_view object_name);
    ///adds a error message to the messages
    use_case_t error(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a warning message to the messages; using default parameters
    virtual use_case_t warning(std::string_view func_name,
               std::string_view object_name);
    ///adds a warning message to the messages
    use_case_t warning(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a info message to the messages; using default parameters
    virtual use_case_t info(std::string_view func_name,
               std::string_view object_name);
    ///adds a info message to the messages
    use_case_t info(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a debug message to the messages; using default parameters
    virtual use_case_t debug(std::string_view func_name,
               std::string_view object_name);
    ///adds a debug message to the messages
    use_case_t debug(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///get all the messages
    virtual std::vector<logger_message_t>& messages();
    void clear_messages();
    void clear_messages(std::vector<unsigned long> ids);
    ///adds a mtype message to the messages vector
    virtual void add_message(const logger_message_t& message);
    ///filter type
    filter_t filter();
    void to_screen();
    std::string to_string(const string line_delimiter);
    void to_file(string path_with_filename);
};

using Logger = std::shared_ptr<logger_t>;

///using dependency injection (DI) as singleton replacement
class class_logger_t : public logger_t
{
private:
    Logger logger;
    void add_message(const logger_message_t& message) override;
protected:
    ///default parameter
    std::string source_file_name_p="";
    ///default parameter
    std::string class_name_p="";
//    std::shared_ptr<loggerwindow> window() const override;
public:
    class_logger_t(Logger& logger_s, std::string_view source_file_name_p, std::string_view class_name_p);
    virtual table_log *window() override;
    vector<logger_message_t>& messages() override;
    virtual bool print_debug() const override;
    virtual bool print_info() const override;
    virtual bool print_warning() const override;
    use_case_t fatal(std::string_view func_name, std::string_view object_name) override;
    use_case_t error(std::string_view func_name, std::string_view object_name) override;
    use_case_t warning(std::string_view func_name, std::string_view object_name) override;
    use_case_t info(std::string_view func_name, std::string_view object_name) override;
    use_case_t debug(std::string_view func_name, std::string_view object_name) override;
};

#endif // LOG_T_HPP
