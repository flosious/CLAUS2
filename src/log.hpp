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
//#include "table_log.hpp"

class logger_t;

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
    static const std::map<mtype_t,std::string> type_to_string;
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

    std::string to_string(const std::string delimiter="\t") const;
    bool operator<(const logger_message_t& msg) const;
};

class use_case_t
{
private:
    logger_message_t::mtype_t mtype;
    std::string_view source_file_name;
    std::string_view class_name;
    std::string_view func_name;
    std::string_view object_name;
    void log(std::string_view message_description, std::string_view message_details, unsigned int verbosity_level);
public:
    use_case_t(
            logger_message_t::mtype_t mtype,
            std::string_view source_file_name,
            std::string_view class_name,
            std::string_view func_name,
            std::string_view object_name);
    ///simple signaling message: e.g. "HERE"
    template<typename A, typename B>
    void signal(A message_description, unsigned int verbosity_level = 15, B message_details="")
    {
        std::stringstream ss, ssB;
        ss << message_description;
        ssB << message_details;
        log(ss.str(),ssB.str(),verbosity_level);
    }
    template<typename A>
    void signal(A message_description, unsigned int verbosity_level = 15, std::string_view message_details="")
    {
        std::stringstream ss, ssB;
        ss << message_description;
        log(ss.str(),message_details,verbosity_level);
    }
    ///enter a function
    template<typename A>
    void enter(unsigned int verbosity_level = 15, A message_details="")
    {
        std::stringstream ss;
        ss << message_details;
        log("enter",ss.str(),verbosity_level);
    }
    void enter(unsigned int verbosity_level = 15, std::string_view message_details="")
    {
        log("enter",message_details,verbosity_level);
    }
    ///exit a function
    void exit(unsigned int verbosity_level = 15, std::string_view message_details="");
    ///inform about a quantity
    void quantity(std::string_view quantity_to_string_short, unsigned int verbosity_level = 10, std::string_view message_details="");
    ///inform about a value
    template<typename V>
    void value(V value, unsigned int verbosity_level = 10, std::string_view message_details="")
    {
        std::stringstream ss;
        ss << value;
        log(ss.str(),message_details,verbosity_level);
    }

    /// condition for existence: if (!A().is_set) --> then exit  :: logger_t::use_case_t::is_set(A().to_string(),FALSE,"exit"); //same as is_set()
    void is_statement(std::string_view concerning_object_name, bool value, std::string_view consequence="" ,unsigned int verbosity_level = 10);
    /// condition for more then existings ...
//        void if_statement(std::string_view concerning_object_A_name, std::string_view concerning_object_A_value, std::string_view comparator, std::string_view concerning_object_B_name, std::string_view concerning_object_B_value,unsigned int verbosity_level = 10);
    template<typename A, typename B>
    void if_statement(std::string_view concerning_object_A_name, A concerning_object_A_value, std::string_view comparator, std::string_view concerning_object_B_name, B concerning_object_B_value,unsigned int verbosity_level = 10)
    {
        std::stringstream message_description, message_details;
        message_description << concerning_object_A_name << comparator << concerning_object_B_name;
        message_details << concerning_object_A_value << comparator << concerning_object_B_value;
        log(message_description.str(),message_details.str(),verbosity_level);
    }
};

class logger_t
{
private:
    static void print(const logger_message_t& message);
    static void print_last();


protected:
    static std::vector<logger_message_t> messages_p;
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
        filter_t source_file_name(const std::string source_file_name_s) const;
        filter_t class_name(const std::string class_name_s) const;
        filter_t func_name(const std::string func_name_s) const;
        filter_t object_name(const std::string object_name_s) const;
    };
    ///logging in use-case style

public:
    logger_t();
    static bool print_to_standard_output;
    static const std::vector<logger_message_t>& messages();
    static void log(unsigned int verbosity_level,
             logger_message_t::mtype_t mtype,
             std::string_view source_file_name,
             std::string_view class_name,
             std::string_view func_name,
             std::string_view object_name,
             std::string_view message_description,
             std::string_view message_details);
    ///adds a fatal message to the messages; using default parameters
//    virtual use_case_t fatal(std::string_view func_name,
//               std::string_view object_name);
	///adds a fatal message to the messages
    static use_case_t fatal(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a error message to the messages; using default parameters
//    virtual use_case_t error(std::string_view func_name,
//               std::string_view object_name);
    ///adds a error message to the messages
    static use_case_t error(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a warning message to the messages; using default parameters
//    virtual use_case_t warning(std::string_view func_name,
//               std::string_view object_name);
    ///adds a warning message to the messages
    static use_case_t warning(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a info message to the messages; using default parameters
//    virtual use_case_t info(std::string_view func_name,
//               std::string_view object_name);
    ///adds a info message to the messages
    static use_case_t info(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///adds a debug message to the messages; using default parameters
//    virtual use_case_t debug(std::string_view func_name,
//               std::string_view object_name);
    ///adds a debug message to the messages
    static use_case_t debug(std::string_view source_file_name,
               std::string_view class_name,
               std::string_view func_name,
               std::string_view object_name);
    ///get all the messages
//    virtual std::vector<logger_message_t>& messages();
    static void clear_messages();
    static void clear_messages(std::vector<unsigned long> ids);
    ///adds a mtype message to the messages vector
    static void add_message(const logger_message_t& message);
    ///filter type
    static filter_t filter();
    static void to_screen();
    static std::string to_string(const std::string line_delimiter);
    static void to_file(std::string path_with_filename);
}; // logger_t

class class_logger_t
{
private:
    logger_t logger;
//    static void add_message(const logger_message_t& message);
protected:
    ///default parameter
    std::string source_file_name_p="";
    ///default parameter
    std::string class_name_p="";
//    std::shared_ptr<loggerwindow> window() const override;
public:
    class_logger_t(const std::string source_file_name_p, const std::string class_name_p);
    class_logger_t() = delete;
//    std::vector<logger_message_t>& messages() override;
    use_case_t fatal(std::string_view func_name, std::string_view object_name);
    use_case_t error(std::string_view func_name, std::string_view object_name);
    use_case_t warning(std::string_view func_name, std::string_view object_name);
    use_case_t info(std::string_view func_name, std::string_view object_name);
    use_case_t debug(std::string_view func_name, std::string_view object_name);
}; // class_logger_t

class function_logger_t
{
protected:
    class_logger_t class_logger;
    std::string func_name_p;
public:
    function_logger_t(const class_logger_t& class_logger, std::string_view func_name);
    use_case_t fatal(std::string_view object_name);
    use_case_t error(std::string_view object_name);
    use_case_t warning(std::string_view object_name);
    use_case_t info(std::string_view object_name);
    use_case_t debug(std::string_view object_name);
};

#endif // LOG_T_HPP
