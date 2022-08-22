/*
	Copyright (C) 2022 Florian Bärwolf
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

#include "files.hpp"

/***********************/
/*****  TOF-SIMS   *****/
/***********************/


files_::tofsims_t::contents_t::contents_t(string& filename_with_path)
    : files_::sims_t::contents_t(filename_with_path,"\t",{"# Profile Smoothing","# Profile Compression"}),
      logger(global_logger,__FILE__,"files_::tofsims_t::contents_t")
{
}


vector<isotope_t> files_::tofsims_t::contents_t::parse_isotopes(string isotopes) const
{
	vector<isotope_t> isotope_list;
	isotopes = tools::str::remove_chars_from_string(isotopes,"-+");
	smatch match;
	regex reg ("^\\^?([0-9]{0,3})([A-Z]{1}[a-z]{0,1})([0-9]{0,3})(.*?)$"); 
	while (regex_search(isotopes,match,reg)) 
	{
		// order is important, because it will be cleared on access
		string nucleons = match[1];
		string symbol = match[2];
		string atoms = match[3];
		isotopes = match[4];

		isotope_t isotope;
		isotope.symbol=symbol;
		if (nucleons!="") isotope.nucleons = tools::str::str_to_int(nucleons);
		else isotope.nucleons = PSE.element(symbol)->isotope_with_highest_abundance()->nucleons;
		if (atoms!="") isotope.substance_amount = quantity::substance_amount_t({tools::str::str_to_double(atoms)});
// 		else isotope.substance_amount=1;
// 		cout << "nucleons="<<nucleons<<"\tsymbol="<<symbol<<"\tatoms="<<atoms<<"\tisotopes="<<isotopes<<endl;
// 		isotope.to_screen("");
		isotope_list.push_back(isotope);
	}
	return isotope_list;
}

const vector<files_::sims_t::contents_t::column_t> files_::tofsims_t::contents_t::columns()
{
	if (cols_p.size()>0)
	{
		return cols_p;
	}
	const int c=0;
	auto header = raw_header_tensor().at(c);
	auto data = tools::mat::transpose_matrix(raw_data_tensor().at(0));
	tools::str::replace_chars(&header,"#\t","");
	tools::str::replace_chars(&header,"# ","");
	tools::str::replace_chars(&header,"#","");
// 	print(data);
// 	print(header);
// 	cout << header.size() << endl;
	const int cluster_names_line=(header).size()-3;
	const int masses_line=(header).size()-2;
	const int dimensions_units_line=(header).size()-1;
	const unsigned int max_cols = header.at(cluster_names_line).size();
	cols_p.resize(max_cols-1); // last col is completly empty
	for (int col_n=0; col_n < max_cols-1; col_n++)
	{
		files_::sims_t::contents_t::column_t col;
		col.cluster_name = header.at(cluster_names_line).at(col_n);
		col.dimension = header.at(dimensions_units_line).at(col_n);
		col.data = tools::mat::str_vec_to_double_vec(data.at(col_n));
// 		col.unit = tools::str::get_string_between_string_A_and_next_B(&col.dimension,"(",")");
		cols_p.at(col_n)=(col);
	}
	return cols_p;
}

quantity::sputter_time_t files_::tofsims_t::contents_t::sputter_time()
{
	auto col = columns().at(0);
	if (col.dimension.find("Sputter Time")!=string::npos)
	{
		return quantity::sputter_time_t(col.data,tools::str::get_string_between_string_A_and_next_B(&col.dimension,"(",")"));
	}
	return {};
}

quantity::sputter_depth_t files_::tofsims_t::contents_t::sputter_depth()
{
	auto col = columns().at(0);
	if (col.dimension.find("Depth")!=string::npos)
	{
		return quantity::sputter_depth_t(col.data,tools::str::get_string_between_string_A_and_next_B(&col.dimension,"(",")"));
	}
	return {};
}

vector<cluster_t> files_::tofsims_t::contents_t::clusters()
{
	const auto cols = columns();
	vector<cluster_t> clusters_p;
	clusters_p.reserve((cols.size()));
	for (int i=1; i < cols.size() ; i++)
	{
		auto& col = cols.at(i);
		if (col.cluster_name!="")
		{
			clusters_p.push_back(parse_isotopes(col.cluster_name));
		}
		if (col.dimension.find("Intensity")!=string::npos)
		{
			clusters_p.back().intensity = quantity::intensity_t(col.data);
		}
		if (col.dimension.find("Concentration")!=string::npos)
		{
			if (col.dimension.find("%")!=string::npos)
				clusters_p.back().concentration = quantity::concentration_t(col.data, units::derived::atom_percent, quantity::dimensions::SI::relative);
			clusters_p.back().concentration = quantity::concentration_t(col.data);
		}
// 		clusters_p.push_back(cluster);
	}
	return clusters_p;
}
