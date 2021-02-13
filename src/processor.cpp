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
#include "test.hpp"

processor::processor(vector<string> args_p)
{	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	cout << "processor start" << endl;
	files::load(args_p);

// 	set<string> input_strings(args_p.begin(),args_p.end()); // eliminate same entries
// 	vector<files::dsims_dp_rpc_asc_t> dsims_files = load_files<files::dsims_dp_rpc_asc_t>(input_strings);
	
	
// 	vector<sample_t> samples = ::samples<files::dsims_dp_rpc_asc_t>(dsims_files);
	
	
// 	cout << "f.name.delimiter="<<<<"=" << endl;
	
// 	cout << test.B_p.delimiter << endl;
// 	cout << test.C_p.delimiter << endl;
	
// 	cout << f_p->contents.matrix().to_string() << endl;
// 	cout << f.contents.matrix().to_string() << endl;
// 	for (auto file:dsims_files)
// 	{
// 		file.to_screen();
// 	}
	
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
