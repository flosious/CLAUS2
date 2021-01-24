#include "file.hpp"

/***********************/
/** D SIMS**/

/***STATICS***/
// const string files::dsims_dp_rpc_asc_t::name_t::test = "dsims_dp_rpc_asc_t-TEST";
// const set<string> files::dsims_dp_rpc_asc_t::contents_t::identifiers={"*** DATA FILES ***"};
// const string files::dsims_dp_rpc_asc_t::contents_t::delimiter = "\t";
// 
// const set<string> files::dsims_dp_rpc_asc_t::name_t::identifiers={"dp_rpc_asc"};
// const string files::dsims_dp_rpc_asc_t::name_t::delimiter = "_";


/**************************************/
/*******files::dsims_dp_rpc_asc_t******/
/**************************************/

files::dsims_dp_rpc_asc_t::dsims_dp_rpc_asc_t(const string& filename_with_path_s,string* contents_s) : 	sims_t(filename_with_path_s, contents_s),
																										name(filename_with_path_s), 
																										contents(filename_with_path_s,contents_s)
{
// 	name = name_t{filename_with_path_s};
}

// const sputter_time_t files::dsims_dp_rpc_asc_t::greatest_common_sputter_time()
// {
// 	if (contents.sputter_time_p.is_set()) return sputter_time_p;
// 	sputter_time_t st(get_quantity_from_dimension_and_clustername("Time","Ipr"));
// 	sputter_time_p = st;
// 	return st;
// }
// 
// const sputter_depth_t files::dsims_dprpc__asc_t::greatest_common_sputter_depth()
// {
// 	if (sputter_depth_p.is_set()) return sputter_depth_p;
// 	sputter_depth_t sd(get_quantity_from_dimension_and_clustername("Depth","Ipr"));
// 	sputter_depth_p = sd;
// 	return sd;
// }


const vector<cluster_t> files::dsims_dp_rpc_asc_t::clusters()
{
	vector<cluster_t> collected_clusters;
	for (auto& clustername : contents.cluster_names())
	{
		if (clustername=="Ipr") continue;
		sputter_time_t sputter_time_s;
		sputter_depth_t sputter_depth_s;
		concentration_t concentration_s;
		intensity_t intensity_s;
		for (auto& col:contents.columns())
		{
			if (col.cluster_name != clustername) continue;
			if (col.dimension=="Time") sputter_time_s = sputter_time_t(col.data,col.unit);
			if (col.dimension=="Depth") sputter_depth_s = sputter_depth_t(col.data,col.unit);
			if (col.dimension=="C") concentration_s = concentration_t(col.data,col.unit);
			if (col.dimension=="I") intensity_s = intensity_t(col.data,col.unit);
		}
		cluster_t cluster(clustername,sputter_time_s,intensity_s,sputter_depth_s,concentration_s);
		if (cluster.is_set()) collected_clusters.push_back(cluster);
		else
			logger::error("files::dsims_dp_rpc_asc_t::clusters() cluster.is_set()", false);
	}
	return collected_clusters;
}


/**************************************************/
/*****  files::dsims_dp_rpc_asc_t::Ipr_t       ****/
/**************************************************/

const sputter_depth_t files::dsims_dp_rpc_asc_t::Ipr_t::sputter_depth() const
{
	return sputter_depth_p;
}

const sputter_time_t files::dsims_dp_rpc_asc_t::Ipr_t::sputter_time() const
{
	return sputter_time_p;
}

const sputter_current_t files::dsims_dp_rpc_asc_t::Ipr_t::sputter_current() const
{
	return sputter_current_p;
}

files::dsims_dp_rpc_asc_t::Ipr_t::Ipr_t(sputter_current_t sputter_current_s, sputter_time_t sputter_time_s, sputter_depth_t sputter_depth_s)
{
	sputter_current_p = sputter_current_s;
	sputter_time_p = sputter_time_s;
	sputter_depth_p = sputter_depth_s;
}



/**************************************************/
/*****  files::dsims_dp_rpc_asc_t::column_t    ****/
/**************************************************/

void files::dsims_dp_rpc_asc_t::column_t::to_screen()
{
	cout << "unit="<<unit<<"\tcluster_name="<<cluster_name<<"\tdimension="<<dimension<<"\tdata.size()="<<data.size()<<endl;
}

/**************************************************/
/*****  files::dsims_dp_rpc_asc_t::name_t      ****/
/**************************************************/

files::dsims_dp_rpc_asc_t::name_t::name_t(const string& name_with_path_s) : sims_t::name_t(name_with_path_s,"_",{"dp_rpc_asc"})
{
}

/**************************************************/
/*****  files::dsims_dp_rpc_asc_t::contents_t  ****/
/**************************************************/


files::dsims_dp_rpc_asc_t::contents_t::contents_t(const string& filename_with_path_s, string* contents_s) : sims_t::contents_t(filename_with_path_s,"\t",{"*** DATA FILES ***"},contents_s)
{
	
}

const vector<files::dsims_dp_rpc_asc_t::column_t> files::dsims_dp_rpc_asc_t::contents_t::columns()
{
	if (check_Ipr()) ipr_shift_correction();
	remove_corrupted_lines();
	if (cluster_names().size()<1 && !parse_units_dimensions_clusternames())
	{
		logger::debug("files::dsims_dp_rpc_asc_t::columns(): !parse_units_dimensions_clusternames");
		return {};
	}
	
	vector<dsims_dp_rpc_asc_t::column_t> cols;
	vector<vector<string>> data_cols_lines = tools::mat::transpose_matrix(raw_data_tensor()[0]);
	const int cols_per_element = dimensions().size()/cluster_names().size();
	for (int i=0;i<dimensions().size() && i<data_cols_lines.size() ;i++)
	{
		dsims_dp_rpc_asc_t::column_t col;
		col.cluster_name=cluster_names()[i/cols_per_element];
		col.dimension=dimensions()[i];
		col.unit=units()[i];
		vector<string> col_data_string = data_cols_lines[i];
		col.data=tools::mat::str_vec_to_double_vec(col_data_string);
		if (col.data.size()==0) continue;
		cols.push_back(col);
	}
	return cols;
}

const files::dsims_dp_rpc_asc_t::Ipr_t files::dsims_dp_rpc_asc_t::contents_t::Ipr()
{
	sputter_current_t sputter_current_s;
	sputter_time_t sputter_time_s;
	sputter_depth_t sputter_depth_s;
	for (auto& col:columns())
	{
		if (col.cluster_name != "Ipr") continue;
		if (col.dimension=="Time") sputter_time_s = sputter_time_t(col.data,col.unit);
		if (col.dimension=="Depth") sputter_depth_s = sputter_depth_t(col.data,col.unit);
		if (col.dimension=="I") sputter_current_s = sputter_current_t(col.data,col.unit);
	}
	return {sputter_current_s,sputter_time_s,sputter_depth_s};
}

const vector<std::__cxx11::string>& files::dsims_dp_rpc_asc_t::contents_t::dimensions()
{
	if (dimensions_p.size()>0) return dimensions_p;
	parse_units_dimensions_clusternames();
	return dimensions_p;
}
const vector<std::__cxx11::string>& files::dsims_dp_rpc_asc_t::contents_t::units()
{
	if (units_p.size()>0) return units_p;
	parse_units_dimensions_clusternames();
	return units_p;
}
const vector<std::__cxx11::string>& files::dsims_dp_rpc_asc_t::contents_t::cluster_names()
{
	if (cluster_names_p.size()>0) return cluster_names_p;
	parse_units_dimensions_clusternames();
	return cluster_names_p;
}

bool files::dsims_dp_rpc_asc_t::contents_t::parse_units_dimensions_clusternames()
{
	cluster_names_p.clear();
	units_p.clear();
	dimensions_p.clear();
	/*1st header */
	static const int c=0;
	tools::str::replace_chars(&raw_header_tensor().at(c)," [","[");
	const int sample_names_line_number=(raw_header_tensor().at(c)).size()-3;
	const int elements_line_number=(raw_header_tensor().at(c)).size()-2;
	const int dimensions_units_line_number=(raw_header_tensor().at(c)).size()-1;
	
	
	vector<string> samples;
	for (int i=0;i<((raw_header_tensor().at(c))[sample_names_line_number]).size();i++) {
		if (( (raw_header_tensor().at(c))[sample_names_line_number][i]).size()>0 ) {
			string result = ((raw_header_tensor().at(c))[sample_names_line_number][i]);
			samples.push_back(result);
		}
	}
	if (samples.size()!=1) 
	{
// 		error=true;
		logger::debug("files::dsims_dp_rpc_asc_t::parse_raw_header_tensor(): samples.size()!=1");
		return false;
	}
	
	for (int i=0;i<((raw_header_tensor().at(c))[elements_line_number]).size();i++) {
        string result = ((raw_header_tensor().at(c))[elements_line_number][i]);
		if (result.size()>0 ) {
			cluster_names_p.push_back(result);
		} else {
        }
	}
	
	for (int i=0;i<((raw_header_tensor().at(c))[dimensions_units_line_number]).size();i++) {
		if ((((raw_header_tensor().at(c))[dimensions_units_line_number])[i]).size()>0) {
			units_p.push_back(tools::str::get_string_between_string_A_and_next_B(&((raw_header_tensor().at(c))[dimensions_units_line_number][i]),"[","]"));
			dimensions_p.push_back((tools::str::get_strings_between_delimiter(((raw_header_tensor().at(c))[dimensions_units_line_number][i]),"[")).at(0));
		}
	}
        
	if (units_p.size()!=dimensions_p.size()) {
		logger::debug("files::dsims_dp_rpc_asc_t::parse_raw_header_tensor(): units.size()!=dimensions.size()");
		return false;
	}
	
	
	int cols_per_element = dimensions_p.size()/cluster_names_p.size();
	if (cols_per_element!=3)
	{
		/* 
		 * dp_asc has a bug in the exported data: dimension and unit are one X- and one Y-axis to long
		 * to fix it, we remove it from the end
		 */
		if (check_Ipr())
		{
			dimensions_p.erase(dimensions_p.end()-4,dimensions_p.end()-2);
			units_p.erase(units_p.end()-4,units_p.end()-2);
		} 
	}

    return true;
}

bool files::dsims_dp_rpc_asc_t::contents_t::check_Ipr() 
{
	is_Ipr=false;
	is_Ipr = tools::mat::find_str_in_mat(cluster_names(),"Ipr");
	return is_Ipr;
}

set<int> files::dsims_dp_rpc_asc_t::contents_t::corrupted_lines()
{
	set<int> corrupted_lines_p;
	vector<vector<string>> cols_lines = tools::mat::transpose_matrix(raw_data_tensor().at(0));
	vector<double> col;
	
	int colssize=cols_lines.size();
	int max_size=-1;
	if (check_Ipr()) colssize = colssize-3;
	for (int c=0;c<colssize;c++)
	{
		col = tools::mat::str_vec_to_double_vec(cols_lines[c]);
		if (col.size()<2) continue;
		if (max_size==-1) 
		{
			max_size = col.size();
			continue;
		}
		
		if (col.size()<max_size) 
		{
			corrupted_lines_p.insert(max_size-1);
			max_size = col.size();
		}
		if (col.size()>max_size) 
		{
			corrupted_lines_p.insert(col.size()-1);
		}
	}
	
	return corrupted_lines_p;
}

void files::dsims_dp_rpc_asc_t::contents_t::remove_corrupted_lines()
{
	set<int> corrupted_lines_p=corrupted_lines();
	if (raw_data_tensor().size()==0) return;
	
	for (set<int>::reverse_iterator it=corrupted_lines_p.rbegin();it!=corrupted_lines_p.rend();++it)
	{
		logger::info("files::dsims_dp_rpc_asc_t::remove_corrupted_lines()", *it);
		raw_data_tensor_p.at(0).erase(raw_data_tensor_p.at(0).begin()+*it);
	}
}

/*
 * exported data is bugged, there are to many "\t" in the lines, where there is no cluster data, but Ipr data
 */
bool files::dsims_dp_rpc_asc_t::contents_t::ipr_shift_correction() 
{
	raw_data_tensor();
	vector<vector<string>>* mat = &raw_data_tensor_p.at(0);
    if (mat->size()==0) 
	{
		logger::error("files::dsims_dp_rpc_asc_t::ipr_shift_correction(): raw_data_tensor_p.at(0).size()==0", (int)mat->size());
		return false;
	}
	int cols_per_element = dimensions().size()/cluster_names().size();
	if (cols_per_element!=3) cols_per_element=2;
	int Ipr_col;
	if (cols_per_element==3) Ipr_col = ((tools::mat::find_in_vec(cluster_names(),"Ipr"))+1)*3;
	else Ipr_col = dimensions().size()-1-1;
	int Ipr_offset_line;
	int Ipr_col_at_EOL;
    int total_lines=mat->size();
	for (Ipr_col_at_EOL=0;Ipr_col_at_EOL<mat->back().size();Ipr_col_at_EOL++) {
		if (!tools::str::is_empty(mat->back().at(Ipr_col_at_EOL))) break;
	}
	Ipr_col_at_EOL++;
	for (Ipr_offset_line=0;Ipr_offset_line<total_lines;Ipr_offset_line++) 
	{
		if (mat->at(Ipr_offset_line)[cols_per_element-1]=="") break; // die 3. spalte enthaelt immer daten (intensität des 1. isotops)
	}
	vector<vector<string>> Ipr_mat;
	// cut off Ipr from data mat
	Ipr_mat.insert(Ipr_mat.begin(),mat->begin()+Ipr_offset_line,mat->end());	
	// delete first columns within the matrix
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	Ipr_mat.erase(Ipr_mat.begin(),Ipr_mat.begin()+(Ipr_mat.size()-cluster_names().size()*cols_per_element));
	Ipr_mat=tools::mat::transpose_matrix(Ipr_mat);
	mat->erase(mat->begin()+Ipr_offset_line,mat->end()); 
	// add them together
	mat->insert(mat->end(),Ipr_mat.begin(),Ipr_mat.end());
    return true;
}

const quantity_t files::dsims_dp_rpc_asc_t::contents_t::get_quantity_from_dimension_and_clustername(const string col_dimension, const string ignore_clustername)
{
	string unit="";
	vector<vector<double>> data_vecs;
	for (auto& col:columns())
	{
		if (col.dimension!=col_dimension) continue;
		if (col.cluster_name==ignore_clustername) continue;
		data_vecs.push_back(col.data);
		unit = col.unit;
	}
	if (unit=="") 
	{
		logger::error("files::dsims_dp_rpc_asc_t::sputter_time() unit",unit);
		return {};
	}
	return quantity_t{statistics::common_vector(data_vecs),unit};
}

map<string,string>& files::dsims_dp_rpc_asc_t::contents_t::infos_and_settings()
{
// 	if (infos_and_settings_p.size()>0) return infos_and_settings_p;
// 	print(raw_header_tensor()[0]);
}

