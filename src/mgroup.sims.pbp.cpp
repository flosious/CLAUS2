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

// mgroups_::sims_t::pbp_t::pbp_t(sims_t& MG)
// {
// 	/*do not apply any calculation here.
// 	 * apply when neccessary, because this can be really heavy
// 	 */
// }
// 
// // mgroups_::pbp_t::pbp_t(vector<measurements_::sims_t>& Ms)
// // {
// // 	measurements_s = Ms; // save a copy
// // 	/*do not apply any calculation here.
// // 	 * apply when neccessary, because this can be really heavy
// // 	 */
// // }
// 
// vector<measurements_::sims_t*> mgroups_::sims_t::pbp_t::measurements()
// {
// 	if (measurements_s.size()==0)
// 		return {};
// 	vector<measurements_::sims_t*> Ms(measurements_s.size());
// 	for (int i=0;i<Ms.size();i++)
// 		Ms.at(i)=&measurements_s.at(i);
// 	return Ms;
// }
// 
// const std::map< matrix_t, SR_t > mgroups_::sims_t::pbp_t::SRs()
// {
// 	return {};
// }
// 
// const SR_t mgroups_::sims_t::pbp_t::SR(matrix_t& matrix)
// {
// 	if (SRs().find(matrix)!=SRs().end())
// 		return SRs().at(matrix);
// 	return {};
// }
// 
