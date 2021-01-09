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
	set<string> input_strings(args_p.begin(),args_p.end()); // eliminate same entries
	sputter_energy_t sputter_energy;
	cout << sputter_energy.name() << sputter_energy.unit().name() << endl;
	sputter_energy = sputter_energy_t{sputter_energy.change_unit({"","TESTunit"})};
	cout << sputter_energy.name() << sputter_energy.unit().name() << endl;
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
}



// std::set<filename_t> processor::filenames()
// {
// 	if (filenames_p.size()==0)
// 		filenames_p = filename_t::filenames(args);
// 	return filenames_p;
// }
