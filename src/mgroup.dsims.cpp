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


#include "mgroup.hpp"

mgroups::dsims_t::dsims_t(filenames::dsims_t& fn, files::dsims_t& f, list<sample_t>& samples_list) : sims_t(fn, f, samples_list), settings(msettings::dsims_t(fn,f))
{
	measurements.push_back({fn,f,samples_list});
}

// bool mgroups::mgroup_t::is_set() const
// {
// 	if (use_olcdb && olcdb==-1) return false;
// 	if (use_group && group=="") return false;
// 	return true;
// }

bool mgroups::dsims_t::operator==(const mgroups::dsims_t& obj) const
{
	if (sims_t::operator!=(obj)) return false;
	if (settings != obj.settings) return false;
	return true;
}

bool mgroups::dsims_t::operator!=(const mgroups::dsims_t& obj) const
{
	return !operator==(obj);
}

std::__cxx11::string mgroups::dsims_t::to_string(const string del)
{
	stringstream ss;
	ss << mgroup_t::to_string(del) << del;
	if (use_settings) ss << settings.to_string(del) << del;
	ss << "measurements: <" << measurements.size() << ">" << endl;
	ss << "{" << endl;
	for (auto& M : measurements)
		ss << "\t" << M.to_string(del) << endl;
	ss << "}" << endl;
	string out = ss.str();
// 	out.erase(out.end()-del.length(),out.end()); // remove the las del
	return out;
}



// void mgroups::dsims_t::update(mgroups::dsims_t& MG)
// {
// 	if (*this == MG)
// 	{
// 		for (auto& M:MG.measurements)
// 		{
// 			measurements::dsims_t* M_p = tools::vec::find_in_vec(M,measurements);
// 			if (M_p == nullptr) // M  not  found in measurements
// 			{
// 				measurements.push_back(M);
// 			}
// 			else
// 			{
// 				M_p->update(M);
// 			}
// 		}
// 	}
// }

// void mgroups::dsims_t::try_insert_measurement(measurements::dsims_t& M)
// {
// 	measurements::dsims_t* M_p = tools::vec::find_in_vec(M,measurements);
// 	if (M_p == nullptr) // M  not  found in measurements
// 	{
// 		measurements.push_back(M);
// 	}
// }
