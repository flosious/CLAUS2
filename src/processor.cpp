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
	set<string> input_strings(args_p.begin(),args_p.end()); // eliminate same entries

	set<files::dsims_dp_rpc_asc_t> dsims_files = load_files<files::dsims_dp_rpc_asc_t>(input_strings);

	for (auto file:dsims_files)
	{
		cout << file.contents.matrix().to_string() << endl;
	}

	print(debug_messages);
	print(info_messages);
	print(warning_messages);
	print(error_messages);
	print(fatal_messages);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}



// std::set<filename_t> processor::filenames()
// {
// 	if (filenames_p.size()==0)
// 		filenames_p = filename_t::filenames(args);
// 	return filenames_p;
// }
