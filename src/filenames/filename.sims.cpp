
#include "filename.hpp"


/*********************/
/****   sims_t   *****/
/*********************/

filenames::sims_t::sims_t(const string& filename_with_path_s,const string delimiter_s,const set<string> identifiers_s) :
							filename_t(filename_with_path_s, delimiter_s, identifiers_s)
{
}
const total_sputter_depth_t filenames::sims_t::total_sputter_depths()
{
	if (total_sputter_depths_p.size()==0)
	{
		for (auto& filename_part : not_parseable_filename_parts())
		{
			smatch match;
			regex reg ("^([0-9]{2,})(nm|A)$"); 
			if (regex_search(filename_part,match,reg)) 
			{
				string value = match[1];
				string unit = match[2];
				total_sputter_depths_p.push_back({{tools::str::str_to_double(value)},unit});
			}
		}
	}
	total_sputter_depth_t tspd;
	for (total_sputter_depth_t& t : total_sputter_depths_p)
		tspd << t;
	return tspd;
}

const string filenames::sims_t::filename_without_crater_depths()
{
	string filename_wo_crater_depths  = tools::file::extract_filename(filename_with_path());
	stringstream remove;
	for (double total_sputter_depth:total_sputter_depths().data())
	{
		remove.str("");
		remove << total_sputter_depth << total_sputter_depths().unit().name();
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,remove.str() + delimiter);
		tools::str::remove_substring_from_mainstring(&filename_wo_crater_depths ,delimiter+ remove.str());
	}
	return filename_wo_crater_depths;
}

bool filenames::sims_t::parse_sputter_energy_element_polarity()
{
	smatch match;
	regex reg;
	for (auto& filename_part : not_parseable_filename_parts())
	{
		reg = ("^([0-9]{1,2})(kV|keV)(O|Cs)(\\+|-)$"); 
		if (regex_search(filename_part,match,reg)) 
		{	
			secondary_voltage_p = secondary_voltage_t{{tools::str::str_to_double(match[1])*1000},{"V"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			return true;
		}
	
		reg = ("^([0-9]{2,5})(V|eV)(O|Cs)(\\+|-)$"); 
		if (regex_search(filename_part,match,reg)) 
		{
			secondary_voltage_p = secondary_voltage_t{{tools::str::str_to_double(match[1])},{"V"}};
			sputter_element_p = (match[3]);
			secondary_polarity_p = match[4];
			return true;
		}
	}
	return false;
}

const secondary_voltage_t filenames::sims_t::secondary_voltage()
{
	if (!secondary_voltage_p.is_set()) 
		parse_sputter_energy_element_polarity();
	return secondary_voltage_p;
}

element_t filenames::sims_t::sputter_element()
{
	if (sputter_element_p=="") 
	{
		parse_sputter_energy_element_polarity();
	}
	return sputter_element_p;
}

const string filenames::sims_t::secondary_polarity()
{
	if (secondary_polarity_p=="") 
		parse_sputter_energy_element_polarity();
	return secondary_polarity_p;
}

/*********************************/
/****   dsims_t   *****/
/*********************************/
filenames::dsims_t::dsims_t(const string& filename_with_path_s) :
													sims_t(filename_with_path_s,"_",{"dp_rpc_asc","dp_asc"})
{
}

/*********************************/
/****     tofsims_t     *****/
/*********************************/
filenames::tofsims_t::tofsims_t(const string& filename_with_path_s) :
										sims_t(filename_with_path_s,"_",{"TXT"})
{
}
