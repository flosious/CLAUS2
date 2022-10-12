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

#ifndef FILES_HPP
#define FILES_HPP


#include <string>

#include "tools.hpp"
#include "quantity.hpp"
#include "cluster.hpp"
#include "element.hpp"
#include "ion.hpp"
#include "crater.hpp"
#include <iostream>
// #include "matrix.hpp"

class files_
{
public:
	class file_t
	{
	public:
		class name_t
		{
		private:
            class_logger_t logger;
			std::string group_p="";
			std::string lot_p="";
			std::string lot_split_p="";
			std::string monitor_p="";
			std::string repetition_p="";
			int wafer_p=-1;
			int chip_x_p=-1;
			int chip_y_p=-1;
			int olcdb_p=-1;
			
			void parse_all_parts_at_once();
			///order of parsing the parts
			void parse_filename_parts();
			
			bool parse_olcdb(std::string filename_part);
			bool parse_lot(std::string filename_part);
			bool parse_wafer(std::string filename_part);
			bool parse_chip(std::string filename_part);
			bool parse_monitor(std::string filename_part);
			bool parse_group(std::string filename_part);
			bool parse_repetitor(std::string filename_part);
			bool parsed_filename_parts = false;
		protected:
			std::vector<std::string> not_parseable_filename_parts_p;
            name_t(std::string& filename_with_path_s, const std::string delimiter_s,const std::set<std::string> OR_identifiers_s,const std::set<std::string> AND_identifiers_s);
		public:
			std::string to_string(const std::string del = ",");
			std::string delimiter;
			std::set<std::string> OR_identifiers_s;
			std::set<std::string> AND_identifiers_s;
			virtual const std::vector<std::string>& not_parseable_filename_parts();
			std::string filename_with_path;
			std::string filename_type_ending() const;
			const std::string filename() const;
			const std::string directory() const;
			std::string group();
			std::string lot();
			std::string lot_split();
			std::string monitor();
			std::string repetition();
			///empty if lot and wafer is set
			virtual const std::string simple_name();
			int chip_x();
			int chip_y();
            int olcdb();
			int wafer();
			void to_screen(std::string prefix="");
            bool is_correct_type();
			bool operator==(name_t& obj);
			bool operator!=(name_t& obj);
			bool operator<(name_t& obj);
		};
		///extends name_t for crater detection
		class crater_in_name_t : public name_t
		{
        private:
            class_logger_t logger;
		protected:
			std::string filename_without_crater_depths_s="";
			quantity::total_sputter_depth_t total_sputter_depths_p;
		public:
            crater_in_name_t(std::string& filename_with_path_s,const std::string delimiter_s,const std::set<std::string> OR_identifiers_s,const std::set<std::string> AND_identifiers_s);
			const quantity::total_sputter_depth_t& total_sputter_depths();
            const std::string& filename_without_crater_depths();
		};
		
		class contents_t
		{
			friend class config_t;
        private:
            class_logger_t logger;
            std::vector<unsigned int> delete_cols_before_parsing;
		protected:
			///should be freed after parse_data_and_header_tensors, as it has no longer use
			std::string contents_p;
			///populates raw_data_tensor_p, raw_header_tensor_p, will clear *contents_p
			bool parse_data_and_header_tensors(std::vector<std::vector<std::vector<std::string> > >* raw_header_tensor, std::vector<std::vector<std::vector<std::string> > >* raw_data_tensor);
			std::vector<std::vector<std::vector<std::string>>> raw_data_tensor_p, raw_header_tensor_p;
			std::vector<std::vector<std::vector<std::string>>>& raw_data_tensor();
			std::vector<std::vector<std::vector<std::string>>>& raw_header_tensor();
			std::string value_by_key(std::string key);
			/// "matrix = 30Si5 70Ge85 Sn10"
			
			void to_screen(std::string prefix="");
			std::string delimiter;
			std::set<std::string> identifiers;
//            const std::string& contents_string();
			std::string filename_with_path;
			/*ctors*/
            contents_t(std::string& filename_with_path,const std::string& delimiter,const std::set<std::string>& identifiers,std::vector<unsigned int> delete_cols_before_parsing={}, const std::string& contents_string="");
		public:
            const std::string& contents_string();
			std::string matrix();
            bool is_correct_type();
			std::string to_string(const std::string del = ",");
			///this does not work as intended for some reason
			bool operator< (const contents_t& obj) const;
			bool operator== (const contents_t& obj) const;
			const std::string creation_date_time() const;
		};
	};
public:
	
	class sims_t
	{
	public:
		class name_t : public file_t::crater_in_name_t
		{
		private:
            class_logger_t logger;
			std::string filename_without_crater_depths_s="";
			std::vector<quantity::total_sputter_depth_t> total_sputter_depths_p;
			bool parse_sputter_energy_element_polarity();
			std::string secondary_polarity_p="";
			quantity::energy_t sputter_energy_p;
			std::string sputter_element_p;
		protected:
            name_t(std::string& filename_with_path_s,const std::string delimiter_s,const std::set<std::string> OR_identifiers_s,const std::set<std::string> AND_identifiers_s);
		public:
			const std::vector<std::string>& not_parseable_filename_parts() override;
			const std::string simple_name() override;
			std::string to_string();
			const quantity::energy_t sputter_energy();
			element_t sputter_element();
			const std::string secondary_polarity();
			///parses crater depths and populates filename_wo_crater
// 			quantity::total_sputter_depth_t total_sputter_depths();
// 			const std::string filename_without_crater_depths();
			bool operator==(name_t& obj);
			bool operator!=(name_t& obj);
		};
		class contents_t : public file_t::contents_t
		{
        private:
            class_logger_t logger;
		public:
			virtual std::vector<cluster_t> clusters();
			std::string to_string(const std::string del = ",");
			///content column
			class column_t
			{
			public:
				std::string unit;
				std::string cluster_name;
				std::string dimension;
				std::vector<double> data;
				void to_screen();
				std::string to_string() const;
			};
		protected:
            contents_t(std::string& filename_with_path,const std::string& delimiter,const std::set<std::string>& identifiers, const std::string& contents_string="");
			std::vector<column_t> columns_s;
		};
	}; // sims_t
	
	class dsims_t
	{
    private:
        class_logger_t logger;
	public:
		class name_t : public sims_t::name_t
		{
		public:
            name_t(std::string& filename_with_path_s);
		};
		class contents_t : public sims_t::contents_t
		{
		private:
            class_logger_t logger;
			std::map<std::string,std::string> infos_and_settings_p;
			///will create a map: keys->values from raw_data_tensor()
			const std::map<std::string,std::string>& infos_and_settings();
			const std::vector<double> infos_and_settings_data(const std::string find_this); 
			std::vector<std::string> units_p;
			std::vector<std::string> dimensions_p;
			std::vector<std::string> cluster_names_p;
			/// returns the corrrupted lines (last line of each column,except Ipr), which should be removed
			std::set<int> corrupted_lines();
			/// removes the lines, where some columns are empty
			void remove_corrupted_lines();
			/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
			bool parse_units_dimensions_clusternames();
			/// corrects tab stops for correct alignment and parsing
			bool ipr_shift_correction();
			/// return true if raw_data_mat contains Ipr (primary current)
			bool check_Ipr();
			/// true, if Ipr was detected; false if no Ipr
			bool is_Ipr;
			///e.g. < sputter_time > = get_quantity_from_dimension_and_clustername("Time","Ipr")
			const quantity::quantity_t get_quantity_from_dimension_and_clustername(const std::string col_dimension, const std::string ignore_clustername="");
			
			const std::vector<std::string>& units();
			const std::vector<std::string>& dimensions();
			const std::vector<column_t> columns();
			const std::vector<std::string>& cluster_names();
		public:
			std::vector<cluster_t> clusters() override;
			const tm start_date_time();
			const tm creation_date_time();
			///primary current (sputter_current vs time/depth)
			const crater_t::sputter_beam_t Ipr();
			const quantity::energy_t sputter_energy(const std::string find_this="Impact energy (eV)");
			const quantity::secondary_voltage_t secondary_voltage(const std::string find_this="Secondary ion energy (V)");
			const element_t sputter_element(const std::string find_this="Primary ions");
			const quantity::rastersize_t sputter_rastersize(const std::string find_this="Raster size (um)");
			const quantity::rastersize_t analyzed_area(const std::string find_this="Analyzed area size (um)");
			const std::string secondary_polarity(const std::string find_this="Secondary ion polarity");
			const quantity::quantity_t chamber_pressure(const std::string find_this="Analysis chamber press (mbar)");
			const quantity::quantity_t egate(const std::string find_this="EGate rate (%)");
			const quantity::quantity_t mass_resolution(const std::string find_this="mass_resolution");
			const quantity::quantity_t field_aperture(const std::string find_this="Field aperture (um)");
			const quantity::quantity_t contrast_aperture(const std::string find_this="Contrast aperture (um)");
			const quantity::quantity_t entrance_slit(const std::string find_this="Entrance slit (um)");
			const quantity::quantity_t exit_slit(const std::string find_this="Exit slit (um)");
			const quantity::quantity_t energy_window(const std::string find_this="Energy window (eV)");
			const quantity::quantity_t em_yield(const std::string find_this="EM yield (%)");
			const quantity::quantity_t em_voltage(const std::string find_this="EM HV (V)");
			const quantity::quantity_t total_sputtering_time(const std::string find_this="Total sputtering time (s)");
			const quantity::quantity_t total_acquisition_time(const std::string find_this="Total acquisition time (s)");
			const crater_t crater();
			void to_screen(std::string prefix="");
            contents_t(std::string& filename_with_path, const std::string& contents_string="");
		};	
        dsims_t(std::string& filename, const std::string& contents_string="");
		dsims_t(files_::dsims_t::name_t& name_s, files_::dsims_t::contents_t& contents_s);
        bool operator<(const dsims_t& obj) const;
        name_t name;
		contents_t contents;
	}; //dsims_t
	
	class tofsims_t
	{
    private:
        class_logger_t logger;
	public:
		class name_t : public sims_t::name_t
		{
		private:
            class_logger_t logger;
			bool parse_analysis_energy_element();
			ion_t analysis_ion_p;
			quantity::energy_t analysis_energy_p;
		public:
            name_t(std::string& filename_with_path_s);
			const ion_t& analysis_ion() const;
			const quantity::energy_t& analysis_energy() const;
		};
		class contents_t : public sims_t::contents_t
		{
		private:
            class_logger_t logger;
			std::vector<column_t> cols_p;
		protected:
			std::vector<isotope_t> parse_isotopes(std::string isotopes);
			const std::vector<column_t> columns();
		public:
			quantity::sputter_time_t sputter_time();
			quantity::sputter_depth_t sputter_depth();
			std::vector<cluster_t> clusters() override;
            contents_t(std::string& filename_with_path, const std::string& contents_string="");
		};
        tofsims_t(std::string& filename, const std::string& contents_string="");
		tofsims_t(files_::tofsims_t::name_t& name_s, files_::tofsims_t::contents_t& contents_s);
		bool operator==(const tofsims_t& obj) const;
		bool operator!=(const tofsims_t& obj) const;
// 		bool operator<(tofsims_t& obj);
		bool operator<(const tofsims_t& obj) const;
		name_t name;
		contents_t contents;
	}; // tofsims_t
	
	///contains all profiler files
	class profilers_t
	{
    private:

	protected:
		///a general class
		class profiler_t
		{
		public:
			class contents_t : public file_t::contents_t
			{
			public:
                contents_t(std::string& filename_with_path,const std::string& delimiter,const std::set<std::string>& identifiers, const std::string& contents_string="");
				///should be overwritten
				virtual crater_t::linescan_t linescan();
			};
		};
	public:
		class dektak6m_t 
		{
        private:
            class_logger_t logger;
		public:
			class contents_t : public profiler_t::contents_t
			{
            private:
                class_logger_t logger;
			public:
                contents_t(std::string& filename_with_path, const std::string& contents_string="");
				crater_t::linescan_t linescan() override;
			};
            dektak6m_t(std::string& filename, const std::string& contents_string="");
			file_t::crater_in_name_t name;
			contents_t contents;
		}; // dektak6m_t
		class P17_t : profiler_t
		{
        private:
            class_logger_t logger;
		public:
			class contents_t : public profiler_t::contents_t
			{
            private:
                class_logger_t logger;
			public:
                contents_t(std::string& filename_with_path, const std::string& contents_string="");
				crater_t::linescan_t linescan() override;
			};
            P17_t(std::string& filename, const std::string& contents_string="");
			tofsims_t::name_t name;
			contents_t contents;
		}; //P17_t
	}; //profiler_t
	
	class jpg_t
	{
    private:
        class_logger_t logger;
	public:
        jpg_t(std::string& filename);
		bool operator<(jpg_t& obj);
		file_t::crater_in_name_t name;
	};
    ///replace "#" in csv-file with ",,#"
    class aishu_t
    {
    private:
        class_logger_t logger;
        class name_t : public file_t::name_t
        {
        public:
            name_t(std::string filename);
        };

        class contents_t : public file_t::contents_t
        {
        private:
            class_logger_t logger;
            class block_t
            {
            private:
                ///pure data structure
                class chip_t
                {
                public:
                    int pos_x;
                    int pos_y;
                    std::vector<quantity::quantity_t> quantities;
                };
                std::vector<std::vector<std::string>> header;
                std::vector<std::vector<std::string>> data;
                const std::string header_to_string() const;
                ///chip1 - col_start=2; chip2 - col_start=9
                chip_t chip(int chip_number);
                class_logger_t logger;
            public:
                block_t(const std::vector<std::vector<std::string>>& header,
                        const std::vector<std::vector<std::string>>& data);
                quantity::quantity_t temperature();
                chip_t chip1();
                chip_t chip2();
            };
        public:
            contents_t(std::string& filename_with_path, const std::string& contents_string="");
            std::vector<block_t> blocks();
        };
    public:
        aishu_t(std::string& filename, const std::string& contents_string="");
        name_t name;
        contents_t contents;
        void export_to_aishu_format(std::string export_filename_w_path="");
        bool operator<(const aishu_t& obj) const;
    };
};
extern Logger global_logger;
#endif // FILES_HPP
