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

#ifndef LISTS_HPP
#define LISTS_HPP

#include <list>
#include "files/file.hpp"
#include "sample.hpp"
#include "measurements/measurement.hpp"
#include "mgroups/mgroup.hpp"
#include "log.hpp"

using namespace std;

/*
 * storing the actual data in linked lists, so their pointers are preserved
 * this is neccessary, because each measurement, group, sample 
 * and some of its members have to be able to access non-copies
 * of other measurements, samples or groups
 * in that way, CPU time and RAM is saved ... I think ...
 */
class lists
{
public:
	static list<sample_t> samples;
	

// 	static list<sample_t> samples;
	static list<filenames::dsims_t> dsims_filenames;
	static list<files::dsims_t> dsims_files;
	static list<measurements::dsims_t> dsims_measurements;
	static list<mgroups::dsims_t> dsims_groups;
	
// 	static list<filenames::tofsims_t> tofsims_filenames;
// 	static list<files::tofsims_t> tofsims_files;
// 	static list<measurements::tofsims_t> tofsims_measurements;
// 	static list<mgroups::tofsims_t> tofsims_groups;
	
	///populates all samples with their corresponding measurements (measurments list has to be populated 1st)
	static void update_measurement_pointers_in_samples();
// 	static void feed_measurements_list_dsims(files::dsims_t& file, list<::measurements::dsims_t>& measurements);
// 	static void feed_measurements_list_dsims(list<files::dsims_t>& file, list<::measurements::dsims_t>& measurements);
	
	
	template <typename M, typename MG>
	static void feed_mgroup_list(M& measurement, list<MG>& mgroups)
	{
		MG G(&measurement);
		MG* G_p = find_in_list(G,mgroups);
		if (G_p==nullptr)
		{
			mgroups.push_back(G);
			G_p = &mgroups.back();
		}
		else
			G_p->insert_measurement(&measurement);
	}
	template <typename M, typename MG>
	static void feed_mgroup_list(list<M>& measurements_p, list<MG>& mgroups)
	{
		for (auto& M_p : measurements_p)
			feed_mgroup_list(M_p,mgroups);
		update_measurement_pointers_in_samples;
	}
	
	///populates measurements-list, pointers for measurement_p in measurements-list and sample_p in samples-list
	template <typename F, typename M>
	static void feed_measurements_list(F& file, list<M>& measurements, M** measurement_p, sample_t** sample_p)
	{
		sample_t sample(&file);
		*sample_p=find_in_list(sample,samples);
		if (*sample_p==nullptr)
		{
			samples.push_back(sample);
			*sample_p = &samples.back();
		}
		M m(&file,*sample_p);
		*measurement_p = find_in_list(m,measurements);
		if (*measurement_p==nullptr) 
		{
			measurements.push_back(m);
			*measurement_p = &measurements.back();
		}
		else
			(*measurement_p)->insert_file(&file);
	}
	template <typename F, typename M>
	static void feed_measurements_list(list<F>& files_s, list<M>& measurements)
	{
		M* M_p;
		sample_t* S_p;
		for (auto& f: files_s)
			feed_measurements_list(f,measurements,&M_p,&S_p);
// 		update_measurement_pointers_in_samples();
	}
	
	/*files -> samples*/
	template <typename F>
	static void feed_samples_list(F& file)
	{
		sample_t sample(file.name,file.contents);
		for (auto& s_in_list : samples)
		{
			if (s_in_list==sample)
			{
				s_in_list.files.insert(&file);
				return;
			}
		}
		samples.push_back(sample);
	}
	template <typename F>
	static void feed_samples_list(list<F>& files)
	{
		for (auto& f: files)
			feed_samples_list(f,samples);
	}

	/*filenames -> files*/
	template <typename FN, typename F>
	static void feed_files_list(FN& filename_s, list<F>& files_s)
	{
		F file(filename_s);
		if (file.is_correct_type())
			files_s.push_back(file);
	}
	template <typename FN, typename F>
	static void feed_files_list(list<FN>& filenames_s, list<F>& files_s)
	{
		for (auto& fn : filenames_s)
			feed_files_list(fn, files_s);
	}
	
	/*strings -> filenames*/
	template <typename FN>
	static void feed_filenames_list(string& filename_s, list<FN>& filenames)
	{
		FN file(filename_s);
		if (file.is_correct_type())
			filenames.push_back(file);
	}
	template <typename FN>
	static void feed_filenames_list(vector<string>& filenames_s, list<FN>& filenames)
	{
		for (auto& fn : filenames_s)
			feed_filenames_list(fn, filenames);
	}
	
	///returns the pointer to key in keys_list
	template <typename F>
	static F* find_in_list(F& f,list<F>& keys)
	{
		for (auto& key : keys)
		{
			if (key==f)
				return &key;
		}
		return nullptr;
	}
};

#endif // LISTS_HPP
