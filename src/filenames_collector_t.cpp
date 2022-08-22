#include "filenames_collector_t.hpp"

filenames_collector_t::filenames_collector_t() : logger(global_logger,__FILE__,"filenames_collector_t")
{

}

std::vector<std::string> filenames_collector_t::files_and_folders_in_path(std::string path)
{
    if (!is_folder(path))
        return {};
    return tools::file::get_file_list_from_dir_name(path,true);
}

std::vector<std::string> filenames_collector_t::files_in_path(std::string path)
{
    std::vector<std::string> files_in_path_s;
    const auto files_and_folders_in_path_s = files_and_folders_in_path(path);
    files_in_path_s.reserve(files_and_folders_in_path_s.size());
    for (auto& F : files_and_folders_in_path_s)
    {
        if (is_file(F))
            files_in_path_s.push_back(F);
    }
    return files_in_path_s;
}

std::vector<std::string> filenames_collector_t::folders_in_path(std::string path)
{
    std::vector<std::string> folders_in_path_s;
    const auto files_and_folders_in_path_s = files_and_folders_in_path(path);
    folders_in_path_s.reserve(files_and_folders_in_path_s.size());
    for (auto& F : files_and_folders_in_path_s)
    {
        if (is_folder(F))
            folders_in_path_s.push_back(F);
    }
    return folders_in_path_s;
}

bool filenames_collector_t::is_folder(std::string file_or_path)
{
    if (tools::file::is_file_or_folder(file_or_path)==2)
        return true;
    return false;
}

bool filenames_collector_t::is_file(std::string file_or_path)
{

    if (tools::file::is_file_or_folder(file_or_path)==1)
        return true;
    return false;
}

void filenames_collector_t::add_file(std::string filename_with_path)
{
    filenames_with_path_p.insert(filename_with_path);
}


void filenames_collector_t::add_files(std::vector<std::string> filenames_with_path)
{
    for (auto F:filenames_with_path)
        add_file(F);
}

void filenames_collector_t::add_file_or_folder(std::string file_or_folder, bool include_sub_folders)
{
    if (is_file(file_or_folder))
    {
//        cout << "FILE: " << file_or_folder << endl;
        add_file(file_or_folder);
    }
    else if (is_folder(file_or_folder))
    {
//        cout << "FOLDER: " << file_or_folder << endl;
        if (include_sub_folders)
            add_files_or_folders(files_and_folders_in_path(file_or_folder),include_sub_folders);
        else
            add_files(files_in_path(file_or_folder));
    }
    else
    {
        //logger::error("filenames_collector_t::add_file_or_folder","neither file nor folder", file_or_folder);
    }
}

void filenames_collector_t::add_files_or_folders(std::vector<std::string> files_or_folders, bool include_sub_folders)
{
    for (auto F: files_or_folders)
        add_file_or_folder(F, include_sub_folders);
}

void filenames_collector_t::remove_filename_with_path(std::string filename_with_path)
{
    filenames_with_path_p.erase(filename_with_path);
}

std::vector<std::string> filenames_collector_t::filenames() const
{
    return {filenames_with_path_p.begin(),filenames_with_path_p.end()};
}

