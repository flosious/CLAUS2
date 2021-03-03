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

#ifndef MEASUREMENT_T_HPP
#define MEASUREMENT_T_HPP

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "sample.hpp"
// #include "crater.hpp"
#include "file.hpp"
// #include "mgroup.hpp"
// #include "msettings.hpp"



/************************/


using namespace std;



class measurements_
{
public:	
	class measurement_t
	{
	protected:
		///use these figures to diistinguish one measurement from another
		static bool use_repetition;
		static bool use_sample;
		void add_sample(filenames::filename_t& filename, files::file_t& file, list<sample_t>& samples_list);
		measurement_t(filenames::filename_t& filename, files::file_t& file, list<sample_t>& samples_list);
	public:
		string to_string(const string del = ", ") const;
		bool is_set() const;
// 		int olcdb=-1;
		const string repetition;
		sample_t* sample=nullptr;
		bool operator==(measurement_t& obj);
		bool operator!=(measurement_t& obj);
	};
	
	class sims_t : public measurement_t
	{
	protected:
		void add_clusters(vector<cluster_t>& clusters_s);
	public:
		string to_string(const string del = ", ") const;
		sims_t(filenames::sims_t& filename, files::sims_t& file, list<sample_t>& samples_list);
		vector<cluster_t> clusters;
		///returns the cluster corresponding isotope
		isotope_t* isotope(cluster_t& cluster);
		///isotopes collected from clusters
		set<isotope_t*> isotopes();
		///returns the isotope corresponding clusterS (there can be more than one)
		///e.g. isotope(31P) --> cluster(74Ge 31P) & cluster(31P) & ...
		set<cluster_t*> clusters_from_iso(isotope_t& isotope);
// 		crater_t& crater;
	};
	
	class dsims_t : public sims_t
	{
	public:
		dsims_t(filenames::dsims_t& filename, files::dsims_t& file, list<sample_t>& samples_list);
	};
};

#endif // MEASUREMENT_T_HPP

