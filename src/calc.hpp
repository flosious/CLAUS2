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

#ifndef CALC_T_HPP
#define CALC_T_HPP

#include "measurement.hpp"
#include "mgroup.hpp"

using namespace std;


class calc_t
{
	class sims_t
	{
	private:
		class SF_c
		{
			
		};
		class RSF_c
		{
			vector<measurements_::sims_t*> measurements_s;
		public:
			RSF_c(vector<measurements_::sims_t*> measurements_s, SF_c& SF);
		};
		const bool overwrite;
		vector<measurements_::sims_t*> measurements_s;
	public:
		SF_c& SF();
		RSF_c& RSF();
		sims_t(measurements_::sims_t measurement , bool overwrite=true);
		sims_t(vector<measurements_::sims_t> measurements , bool overwrite=true);
		sims_t(vector<measurements_::sims_t>& measurements , bool overwrite=true);
		sims_t(vector<measurements_::sims_t*> measurements , bool overwrite=true);
		sims_t(mgroups_::sims_t& mgroup , bool overwrite=true);
		
		mgroups_::sims_t mgroup();
		vector<measurements_::sims_t*> measurements();
	};
};


#endif // CALC_T_HPP
