#ifndef FILENAMES_COLLECTOR_T_HPP
#define FILENAMES_COLLECTOR_T_HPP

#include <iostream>
#include "tools.hpp"
#include "log.hpp"

class filenames_collector_t
{
private:
    class_logger_t logger;
    std::set<std::string> filenames_with_path_p;
    static std::vector<std::string> files_and_folders_in_path(std::string path) ;
    static std::vector<std::string> files_in_path(std::string path) ;
    static std::vector<std::string> folders_in_path(std::string path) ;
    static bool is_folder(std::string file_or_path);
    static bool is_file(std::string file_or_path);
    void add_file(std::string filename_with_path);
    void add_files(std::vector<std::string> filenames_with_path);
public:
    filenames_collector_t();
    ///if include_sub_folders, scans recursivly
    void add_file_or_folder(std::string file_or_folder, bool include_sub_folders=true);
    void add_files_or_folders(std::vector< std::string> files_or_folders, bool include_sub_folders=true);
    void remove_filename_with_path(std::string filename_with_path);
    std::vector<std::string> filenames() const;

};

#endif // FILENAMES_COLLECTOR_T_HPP
