/*
	Copyright (C) 2020 Florian Bärwolf
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

#include "processor.hpp"

processor::processor(vector<string> args_p)
{	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	cout << "processor start" << endl;

	
	list<files::dsims_dp_rpc_asc_t> dsims_Fs;
	lists::feed_files_list(args_p,dsims_Fs,true);
	
	list<sample_t> samples;
	list<measurements::dsims_t> dsims_Ms;
	
	for (auto& df:dsims_Fs)
	{
		measurements::measurement_t mf(df.name,df.contents,samples);
		(*mf.sample()->filenames.begin())->to_screen(); // this works
	}
	
	
	
	logger::to_screen();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}



// std::set<filename_t> processor::filenames()
// {
// 	if (filenames_p.size()==0)
// 		filenames_p = filename_t::filenames(args);
// 	return filenames_p;
// }
