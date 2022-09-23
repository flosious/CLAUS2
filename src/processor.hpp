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
        template<class T>
        class items_t
        {
        protected:
            std::vector<T*> items_p;
        public:
            items_t(const std::vector<T>& measurements);
            items_t(const std::vector<T*>& measurements);
            std::vector<T*> files() const;
            items_t by_olcdb(int olcdb);
            items_t by_lot(std::string_view lot);
            items_t by_wafer(int wafer);
            items_t by_sample(const sample_t& sample);
            items_t by_sample_matrix(const sample_t::matrix_t& matrix);
        };
    };
    template<typename file_type>
    class tool_with_files
    {
    private:
        class_logger_t logger;
    protected:
        ///all available filenames
        std::vector<string>* filenames_list = nullptr;
        ///all samples measured by any method
        std::vector<sample_t>* samples_list = nullptr;
        database_t* database = nullptr;
        config_t* config = nullptr;
    public:
        tool_with_files(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database,config_t& config) :
            filenames_list(&filenames_list), samples_list(&samples_list), database(&database), config(&config),logger(global_logger,__FILE__,"processor::tool_with_files")
        {}
//        tool_with_files(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config);
        std::vector<file_type> files;

        filter::items_t<file_type> filter_files();
    };



    template<typename file_type, typename measurement_type>
    class tool_with_files_measurements : public tool_with_files<file_type>
    {
    private:
        class_logger_t logger;
    public:
        tool_with_files_measurements(
                        std::vector<string>& filenames_list,std::vector<sample_t>& samples_list,database_t& database,config_t& config) :
            tool_with_files<file_type>(filenames_list,samples_list,database,config),logger(global_logger,__FILE__,"processor::tool_with_files_measurements")
        {}
//        tool_with_files_measurements(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config);
        std::vector<measurement_type> measurements;
        void add_all_files_to_measurements()
        {
            for (file_type& F : this->files)
            {
                add_file_to_measurement(F);
            }
        }
//        void add_all_files_to_measurements();
        void add_file_to_measurement(file_type& F)
        {
            measurement_type M(F,*this->database); // is this correct?
            measurements.push_back(M);
        }
//        void add_file_to_measurement(file_type& F);
        filter::items_t<measurement_type> filter_measurement();
        ///true if measurement was successfully removed; false otherwise
        bool remove_measurement(const measurement_type* measurement_pointer)
        {
            for (int i=0;i<measurements.size();i++)
            {
                if (&measurements.at(i) == measurement_pointer)
                {
                    tools::vec::erase(measurements,{static_cast<unsigned int>(i)});
                    return true;
                }
            }
            return false;
        }
    };


public:
    template<typename file_type, typename measurement_type, typename mgroup_type>
    class tool_with_files_measurements_groups : public tool_with_files_measurements<file_type,measurement_type>
    {
    private:
        class_logger_t logger;
    public:
        tool_with_files_measurements_groups(std::vector<string>& filenames_list,std::vector<sample_t>& samples_list,database_t& database,config_t& config) :
            tool_with_files_measurements<file_type, measurement_type>(filenames_list,samples_list,database,config),logger(global_logger,__FILE__,"processor::tool_with_files_measurements_groups")
        {}
//        tool_with_files_measurements_groups(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config);
        std::vector<mgroup_type> mgroups;

        filter::items_t<mgroup_type> filter_mgroups();
        bool ungroup(int index)
        {
            if (index >= 0 && index < mgroups.size())
            {
                this->measurements.insert(this->measurements.end(),mgroups[index].measurements_p.begin(),mgroups[index].measurements_p.end());
                mgroups.erase(mgroups.begin()+index);
                return true;
            }
            return false;
        }
        ///removes the group; add its measurements back to the vector
        bool ungroup(const mgroup_type* mgroup_pointer)
        {
            int index = tools::vec::get_index_position_by_comparing_pointers(mgroups,mgroup_pointer);
            return ungroup(index);
        }

        void group_measurements(std::vector<unsigned int> measurement_indexes)
        {
            std::vector<measurement_type> Ms = tools::vec::filter_as_copies_from_indices(this->measurements,measurement_indexes);
            mgroup_type G(Ms);
            mgroups.push_back(G);
            tools::vec::erase(this->measurements,measurement_indexes);
        }
        void add_measurements_to_group(unsigned int mgroup_idx, std::vector<unsigned int> measurement_indexes)
        {
            if (mgroup_idx>=mgroups.size()) return;
            auto& G = mgroups.at(mgroup_idx);
            std::vector<measurement_type> Ms = tools::vec::filter_as_copies_from_indices(this->measurements,measurement_indexes);
            G.measurements_p.insert(G.measurements_p.end(),Ms.begin(),Ms.end());
            tools::vec::erase(this->measurements,measurement_indexes);
        }
        void auto_group_measurements(std::vector<unsigned int> measurement_indexes,bool ignore_olcdb=false)
        {
            for (auto& idx : measurement_indexes)
            {
                if (idx >= this->measurements.size()) continue;
                measurement_type& M = this->measurements.at(idx);
                bool found_group=false;
                for (auto& MG : this->mgroups)
                {
                    if (!ignore_olcdb && (M.olcdb != MG.olcdb))
                    {
                        logger.debug(__func__,MG.to_string_short()).if_statement("M.olcdb",M.settings.to_string_short(),"!=","MG.olcdb",MG.settings_p.to_string_short());
                        continue;
                    }
                    if (M.group_id != MG.group_id)
                    {
                        logger.debug(__func__,MG.to_string_short()).if_statement("M.group_id",M.settings.to_string_short(),"!=","MG.group_id",MG.settings_p.to_string_short());
                        continue;
                    }
                    if (M.settings != MG.settings_p)
                    {
                        logger.debug(__func__,MG.to_string_short()).if_statement("M.settings",M.settings.to_string_short(),"!=","MG.settings_p",MG.settings_p.to_string_short());
                        continue;
                    }
                    found_group = true;
                    MG.measurements_p.push_back(M);
//                    logger.debug(M.to_string_short()).value("measurement added to existing group",19,"adding into group: " + MG.to_string_short());
                }
                if (!found_group)
                {
                    mgroup_type new_MG(M);
                    this->mgroups.push_back(new_MG);
                }
            }
            tools::vec::erase(this->measurements,measurement_indexes);
        }

        void auto_group_ungrouped_measurements(bool ignore_olcdb=false)
        {
            std::vector<unsigned int> measurement_indexes(this->measurements.size());
            std::iota(measurement_indexes.begin(),measurement_indexes.end(),0);
            return auto_group_measurements(measurement_indexes,ignore_olcdb);
        }

        ///true if measurement was successfully removed; false otherwise
        bool remove_mgroup(const mgroup_type* mgroup_pointer)
        {
            int idx = tools::vec::get_index_position_by_comparing_pointers(mgroups,mgroup_pointer);
            if (idx>=0)
            {
                tools::vec::erase(mgroups,{static_cast<unsigned int>(idx)});
                return true;
            }
            return false;
        }
        ///returns the total number of erased elements
        int ungroup_measurements_from_group(std::vector<measurement_type*> measurements_in_group, const mgroup_type* mgroup_pointer=nullptr)
        {
            int c=0;
            int idx;
            std::vector<unsigned int> skip_indices;
            for (auto& mgroup : this->mgroups)
            {
                std::vector<unsigned int> erase_idx;
                for (int i=0;i<measurements_in_group.size();i++)
                {
                    for (auto& skip : skip_indices)
                        if (skip==i)
                            continue;
                    auto* measurement = measurements_in_group.at(i);
                    idx = tools::vec::get_index_position_by_comparing_pointers(mgroup.measurements_p,measurement);
                    if (idx>=0)
                    {
                        c++;
                        this->measurements.push_back(mgroup.measurements_p.at(idx));
                        erase_idx.push_back(idx);
                        skip_indices.push_back(i);
                    }
                }
                if (erase_idx.size()>0)
                {
                    tools::vec::erase(mgroup.measurements_p,erase_idx);
                }
            }
            return c;
        }
        bool is_grouped(const measurement_type* measurement)
        {
            if (mgroup_from_measurement(measurement) == nullptr)
                return false;
            return true;
        }
        mgroup_type* mgroup_from_measurement(const measurement_type* measurement)
        {
            if (measurement==nullptr)
                return nullptr;
            mgroup_type* g = nullptr;
            for (auto& mgroup : mgroups)
            {
                for (auto& m : mgroup.measurements_p)
                {
                    if (&m == measurement)
                        return &mgroup;
                }
            }
            return nullptr;
        }
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
    tool_with_files<files_::aishu_t> aishu;
    tool_with_files_measurements_groups<files_::tofsims_t,measurements_::tofsims_t,mgroups_::tofsims_t> tofsims;
    tool_with_files_measurements_groups<files_::dsims_t,measurements_::dsims_t,mgroups_::dsims_t> dsims;
    tool_with_files_measurements<files_::profilers_t::dektak6m_t,measurements_::profilers_t::dektak6m_t> dektak6m;
    tool_with_files<files_::jpg_t> jpg;
    void parse_unknown_filenames();
    void parse_filename(std::string filename);
    void parse_filenames(const std::vector<std::string>& filenames);
    std::vector<std::string> unknown_filenames;
};

// extern database_t db;
extern Logger global_logger;

#endif // PROCESSOR_HPP
