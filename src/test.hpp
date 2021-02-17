

#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include "quantity.hpp"
#include "element.hpp"
#include "cluster.hpp"

using namespace std;

class Tfile_t
{
private:
	const string filename_with_path;
protected:
	class Tcontents_t
	{
	private:
		///should be freed after parse_data_and_header_tensors, as it has no longer use
		string contents_string_p;
		///populates raw_data_tensor_p, raw_header_tensor_p, will clear *contents_p
// 		bool parse_data_and_header_tensors(vector<vector<vector<std::__cxx11::string> > >* raw_header_tensor, vector<vector<vector<std::__cxx11::string> > >* raw_data_tensor);
	protected:
// 		vector<vector<vector<string>>> raw_data_tensor_p, raw_header_tensor_p;
// 		vector<vector<vector<string>>>& raw_data_tensor();
// 		vector<vector<vector<string>>>& raw_header_tensor();
	public:
		const string delimiter;
		const set<string> identifiers;
		const std::__cxx11::string& contents_string();
// 		const bool is_correct_type(); 
		const string filename_with_path;
// 		bool operator==(const Tcontents_t obj);
		Tcontents_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
	};
	class Tname_t
	{
	public:
		const string delimiter;
		const set<string> identifiers;
		const string filename_with_path;
// 		void do_sthelse();
		bool operator==(Tname_t& obj);
		Tname_t(const string& filename_with_path_s, const string delimiter_s,const set<string> identifiers_s);
	};
	
	Tfile_t(Tname_t& name_s, Tcontents_t& contents_s);
public:
	Tname_t& name;
	Tcontents_t& contents;
	void test() {cout << "test" << endl;};
	bool operator==(Tfile_t& obj);
};


namespace Tfiles
{
	
	class Tsims_t : public Tfile_t
	{
	protected:
		///content column
// 		class Tcolumn_t
// 		{
// 		public:
// 			string unit;
// 			string cluster_name;
// 			string dimension;
// 			vector<double> data;
// 			void to_screen();
// 		};
		class Tname_t : public Tfile_t::Tname_t
		{
		private:
// 			vector<total_sputter_depth_t> total_sputter_depths_p;
// 			bool parse_sputter_energy_element_polarity();
// 			string secondary_polarity_p="";
// 			energy_t sputter_energy_p;
// 			string sputter_element_p;
		public:
			Tname_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
// 			const energy_t sputter_energy();
// 			element_t sputter_element();
// 			const string secondary_polarity();
// 			const total_sputter_depth_t total_sputter_depths();
// 			const string filename_without_crater_depths();
			
		};
		class Tcontents_t : public Tfile_t::Tcontents_t
		{
		public:
			///give me contents_s if already loaded somewhere
			Tcontents_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s);
		};
		
		Tsims_t(Tname_t& name_s, Tcontents_t& contents_s);
	public:
		Tname_t& name;
		Tcontents_t& contents;
	};
	
	
	
	
	
	class Tdsims_dp_rpc_asc_t : public Tfiles::Tsims_t
	{
	private:
// 		class TIpr_t
// 		{
// 		private:
// 			sputter_current_t sputter_current_p;
// 			sputter_time_t sputter_time_p;
// 			sputter_depth_t sputter_depth_p;
// 		public:
// 			TIpr_t(sputter_current_t sputter_current_s, sputter_time_t sputter_time_s, sputter_depth_t sputter_depth_s={});
// 			const sputter_time_t sputter_time() const;
// 			const sputter_depth_t sputter_depth() const;
// 			const sputter_current_t sputter_current() const;
// 			const string to_string() const;
// 		};
// 	protected:
	public:
		class Tcontents_t : public Tfiles::Tsims_t::Tcontents_t
		{	
		private:
// 			map<string,string> infos_and_settings_p;
			///will create a map: keys->values from raw_data_tensor()
// 			const map<string,string>& infos_and_settings();
// 			vector<string> units_p;
// 			vector<string> dimensions_p;
// 			vector<string> cluster_names_p;
			/// returns the corrrupted lines (last line of each column,except Ipr), which should be removed
// 			set<int> corrupted_lines();
			/// removes the lines, where some columns are empty
// 			void remove_corrupted_lines();
			/// parses the raw_header_tensor into vectors of dimensions, isotopes and units
// 			bool parse_units_dimensions_clusternames();
			/// corrects tab stops for correct alignment and parsing
// 			bool ipr_shift_correction();
			/// return true if raw_data_mat contains Ipr (primary current)
// 			bool check_Ipr();
			/// true, if Ipr was detected; false if no Ipr
// 			bool is_Ipr;
			///e.g. < sputter_time > = get_quantity_from_dimension_and_clustername("Time","Ipr")
// 			const quantity_t get_quantity_from_dimension_and_clustername(const string col_dimension, const string ignore_clustername="");
// 			const vector<string>& units();
// 			const vector<string>& dimensions();
// 			const vector<Tcolumn_t> columns();
// 			const vector<string>& cluster_names();
		public:
			Tcontents_t(const string& filename_with_path_s);
// 			const vector<cluster_t> clusters();
// 			const tm start_date_time();
// 			const tm creation_date_time();
			///primary current (sputter_current vs time/depth)
// 			const TIpr_t Ipr();
			
// 			const energy_t sputter_energy();
// 			const secondary_voltage_t secondary_voltage();
// 			element_t sputter_element();
// 			const rastersize_t sputter_rastersize();
// 			const rastersize_t analysis_rastersize();
// 			const string secondary_polarity();
// 			const quantity_t chamber_pressure();
// 			const quantity_t egate();
// 			const quantity_t mass_resolution();
// 			const quantity_t field_aperture();
// 			const quantity_t contrast_aperture();
// 			const quantity_t entrance_slit();
// 			const quantity_t exit_slit();
// 			const quantity_t energy_window();
// 			const quantity_t em_yield();
// 			const quantity_t em_voltage();
		};	
		class Tname_t : public Tfiles::Tsims_t::Tname_t
		{
		private:
		public:
			Tname_t(const string& filename_with_path_s);
		};
	public:
		Tdsims_dp_rpc_asc_t(Tname_t& name_s, Tcontents_t& contents_s);
		Tname_t& name;
		Tcontents_t& contents;
// 		void to_screen(string prefix="");
	};	
	
	class Tdsims
	{
	private:
		const string filename_with_path;
	public:
		Tfiles::Tdsims_dp_rpc_asc_t::Tcontents_t contents;
		Tfiles::Tdsims_dp_rpc_asc_t::Tname_t name;
		Tfiles::Tdsims_dp_rpc_asc_t dsims_dp_rpc_asc;
		Tdsims(string filename_with_path_s);
	};
	
}

#endif // TEST_HPP
