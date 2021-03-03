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

#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <chrono>
// #include "hash_functions.hpp"
#include <list>
#include <string>
#include <vector>

#include "tools.hpp"
#include "file.hpp"
// #include "sample.hpp"
// #include "lists.hpp"
#include "mgroup.hpp"
// #include "measurement_group.hpp"
// #include "measurement.hpp"
// #include <unordered_set>
// #include "quantity.hpp"

using namespace std;


class processor 
{
private:
	list<sample_t> samples_list;
	vector<mgroups::dsims_t> dsims_groups;
	
	///filter filenames for dsims measurement groups
	void populate_dsims_groups(vector<string>& filenames);
	
	template <typename FN, typename F,typename G>
	bool try_insert_groups(FN& fn, F& f, vector<G>& groups)
	{
		if (!fn.is_correct_type() || !f.is_correct_type()) return false;
		G MG(fn,f,samples_list);
		G* MG_p = tools::vec::find_in_vec(MG,groups);
		if (MG_p!=nullptr) // in group list
		{
			mgroups::try_insert_sources_into_target_vec(MG.measurements,MG_p->measurements);
		}
		else
			groups.push_back(MG);
		return true;
}
	
public:
	processor(vector<string> args_p);
};


#endif // PROCESSOR_HPP
