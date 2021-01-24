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
	set<files::dsims_dp_rpc_asc_t> files = load_files<files::dsims_dp_rpc_asc_t>(input_strings);
	
	for (auto f:files)
	{
		f.name.to_screen();
	}

// 	files::dsims_dp_rpc_asc_t file(*input_strings.begin());
// 	file.name.chip_x()


// 	for (int i=0;i<10;i++)
// 		file.sputter_time();




	
// 	files::file_t file_ = file.file();
// 	sample_t sample{file_};
// 	sample.to_screen();
	
// 	sputter_energy_t sputter_energy2 = sputter_energy.change_unit(unit_t{"","success"});
// 	cout << sputter_energy.change_unit(unit_t{"","success"}).unit().base() << endl;
// 	files = file_t::files(input_strings);
	
	
	
// 	for (auto& input_string:input_strings)
// 		cout << input_string << endl;
	// works till here
// 	files_t files;
// 	if (!files.filter(input_strings)) cout << "processor::processor !files.filter(input_strings)" << endl;
// 	filenames = files.filenames();

	
// 	for (auto& sample:samples)
// 		sample.to_screen();
	
// 	dsims_measurements = dsims_measurement_t::measurements(&filenames);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}



// std::set<filename_t> processor::filenames()
// {
// 	if (filenames_p.size()==0)
// 		filenames_p = filename_t::filenames(args);
// 	return filenames_p;
// }
