/*
    Copyright (C) 2021-2022 Florian Bärwolf
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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <set>

// #include <omp.h>
#include "tools.hpp"
// #include "sample.hpp"
// #include "lists.hpp"
#include "mgroup.hpp"
// #include "measurement_group.hpp"
// #include "measurement.hpp"
// #include <unordered_set>
#include "quantity.hpp"
#include "files.hpp"
//#include "plot.hpp"
#include "database_t.hpp"
#include "calc.hpp"
#include "config.hpp"
#include "filenames_collector_t.hpp"
#include "log.hpp"
// #include "plc++demos.h"


class processor 
{

private:

    class filter
    {
    public:
        template<class T>
        class files_t
        {
        protected:
            std::vector<T*> files_p;
        public:
            files_t(const std::vector<T>& files);
            files_t(const std::vector<T*>& files);
            std::vector<T*> files() const;
            files_t by_olcdb(int olcdb);
            files_t by_lot(std::string_view lot);
            files_t by_wafer(int wafer);
            files_t by_sample(const sample_t& sample);
            files_t by_sample_matrix(const sample_t::matrix_t& matrix);
        };
    };

    class camera_t
    {

    };
	
    class dektak6m_t
    {

    };
	
    class dsims_t
    {
    private:
        class_logger_t logger;
        ///all available filenames
        std::vector<string>* filenames_list = nullptr;
        ///all samples measured by any method
        std::vector<sample_t>* samples_list = nullptr;
        database_t* database = nullptr;
        config_t* config = nullptr;
    public:
        filter::files_t<files_::dsims_t> filter_files(const std::vector<files_::dsims_t*>& files_s);
        filter::files_t<files_::dsims_t> filter_files();
        std::vector<files_::dsims_t> files;
        std::vector<measurements_::dsims_t> measurements;
        std::vector<mgroups_::dsims_t> mgroups;

        dsims_t(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config);
        void add_all_files_to_measurements();
        void add_to_measurement(files_::dsims_t& F);
    };
public:
    class tofsims_t
    {
    private:
        class_logger_t logger;
        std::vector<mgroups_::tofsims_t> mgroups_p;
        ///all available filenames
        std::vector<string>* filenames_list = nullptr;
        ///all samples measured by any method
        std::vector<sample_t>* samples_list = nullptr;
        database_t* database = nullptr;
        config_t* config = nullptr;
        ///ungrouped measurements
        std::vector<measurements_::tofsims_t> measurements_p;
        ///filename+path --> files_::tofsims_t
        std::vector<files_::tofsims_t> files_p;
    public:
        tofsims_t(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config);
        std::vector<files_::tofsims_t>& files();
        ///ungrouped measurements
        std::vector<measurements_::tofsims_t>& measurements();
        std::vector<mgroups_::tofsims_t>& mgroups();
        ///just samples measured by tofsims
        std::vector<sample_t> samples() const;
        ///add the new group MG to the std::vector mgroups_p
        void add( mgroups_::tofsims_t& MG);
        ///add the new measurement M to the std::vector meaasurements_p
        void add(measurements_::tofsims_t& M);
        ///adds multiple measurements Ms to the std::vector measurements_p (more efficient)
        void add(std::vector<measurements_::tofsims_t>& Ms);
        ///add the new file F to the std::vector files_p
        void add( files_::tofsims_t& F);
        ///adds multiple files Fs to the std::vector filesp (more efficient)
        void add(std::vector<files_::tofsims_t>& Fs);
        ///removes the file F from the std::vector files_p
        void remove(files_::tofsims_t& F);
        ///removes the measurement M from the std::vector meaasurements_p
        void remove(measurements_::tofsims_t& M);
        ///will move the Ms to a new group or adds to the existing one
        void group(std::vector<measurements_::tofsims_t>& Ms);
        ///will create a measurement from the File F and save it in the std::vector measurements_p
        void add_all_files_to_measurements();
        void add_to_measurement(files_::tofsims_t& F);
        ///will move all measurements to the already existing mgroup MG
        void move_to_existing_group(std::vector<measurements_::tofsims_t>& Ms, mgroups_::tofsims_t& MG);
        ///will move all Ms within MG to the meausrements_p std::vector and erase MG from mgroups_p std::vector
        void ungroup(mgroups_::tofsims_t& MG);
        ///will remove M within MG from the group MG to the std::vector measurements_p
        void remove_from_group(measurements_::tofsims_t& M, mgroups_::tofsims_t& MG);
        ///will remove MG and its containing Ms
        void remove(mgroups_::tofsims_t& MG);
    };
    class aishu_t
    {
    private:
    public:
        std::vector<files_::aishu_t> files;
    };

private:
    sqlite3* sql = nullptr;
    std::vector<sample_t> samples_list_p;
    class_logger_t logger;
    config_t config_p;
public:
    processor(std::vector<string> args_p);
    ~processor();
    database_t database;
    const config_t& config() const;
    ///all samples measured by any method
    const std::vector<sample_t>& samples_list() const;
    aishu_t aishu;
    tofsims_t tofsims;
    dsims_t dsims;
    dektak6m_t dektak6m;
    camera_t camera_images;
    void parse_unknown_filenames();
    void parse_filename(std::string filename);
    void parse_filenames(const std::vector<std::string>& filenames);
    std::vector<std::string> unknown_filenames;
};

// extern database_t db;
extern Logger global_logger;

#endif // PROCESSOR_HPP
