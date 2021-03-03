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
	
	populate_dsims_groups(args_p);
	
	cout << "samples_list.size()=" << samples_list.size() << endl;
	cout << "dsims_groups.size()=" << dsims_groups.size() << endl;
	cout << "dsims_groups[0].measurements.size()=" << dsims_groups[0].measurements.size() << endl;
	
	for (auto& MG : dsims_groups)
		cout << MG.to_string() << endl;
	/*get filename objects*/
// 	lists::feed_filenames_list(args_p,lists::dsims_filenames);
// 	lists::feed_filenames_list(args_p,lists::tofsims_filenames);
// 	lists::feed_filenames_list(args_p,lists::filenames::tofsims);

	/*get file objects*/
// 	lists::feed_files_list(lists::dsims_filenames,lists::dsims_files);
// 	lists::feed_files_list(lists::tofsims_filenames,lists::tofsims_files);
// 	lists::feed_files_list(lists::filenames::tofsims,lists::files::tofsims);
	
	/*get measurement objects*/
// 	lists::feed_measurements_list(lists::dsims_files,lists::dsims_measurements);
// 	lists::feed_measurements_list(lists::tofsims_files,lists::tofsims_measurements);
	
	
// 	lists::update_measurement_pointers_in_samples();
	
// 	lists::feed_mgroup_list(lists::dsims_measurements,lists::dsims_groups);
	

/*
	cout << "lists::samples.size()=" << lists::samples.size() << endl;
	for (auto& S : lists::samples)
	{
		cout << "\t" << S.to_string() << endl;
	}
	cout << "lists::dsims_groups.size()=" << lists::dsims_groups.size() << endl;
	for (auto& MG : lists::dsims_groups)
	{
		cout  << MG.to_string() << endl;
		cout << "{" << endl;
		for (auto& M:MG.measurements)
		{
			cout << "\t"<< M->to_string() << endl;
// 			cout << "\t" << M->settings().to_string() << endl;
			cout << "\t{" << endl;
			if (M->Ipr()==nullptr)
				cout << "Ipr is null" << endl;
			else
				cout << "\t" << M->Ipr()->to_string() << endl;
			for (auto* cluster : M->clusters())
				cout << "\t\t" << cluster->to_string() << endl;
// 			for (files::file_t* F:M->files)
// 				cout << "\t\t" << F->name.to_string() << endl;
			cout << "\t}" << endl;
		}
		cout << "}" << endl;
	}
*/

	logger::to_screen();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}

void processor::populate_dsims_groups(vector<string>& filenames)
{
	for (auto& filename : filenames)
	{
		filenames::dsims_t fn(filename);
		if (!fn.is_correct_type())	continue;
		files::dsims_t f(fn);
		if (!f.is_correct_type())	continue;
		if (try_insert_groups(fn,f,dsims_groups))	filename = ""; // delete from list
	}
}


