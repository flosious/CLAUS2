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
	filenames=args_p;
	
// 	for (auto& DM : dsims_measurements())
// 		cout << "DM.crater.linescans.size()=" << DM.crater.linescans.size() << endl;

	cout << "filenames.size()=" << filenames.size() << endl;
	cout << "dsims_files().size()=" << dsims_files().size() << endl;
	cout << "dsims_measurements.size()=" << dsims_measurements().size() << endl;
	cout << "dsims_mgroups.size()=" << dsims_mgroups().size() << endl;
	cout << "samples_list.size()=" << samples_list.size() << endl;
	
	
	
	
	
	
// 	cout << "samples_list.size()=" << samples_list.size() << endl;
// 	cout << "profiler_measurements.size()=" << profiler_measurements.size() << endl;
	
// 	
	for (auto& MG : dsims_mgroups())
		cout << MG.to_string() << endl;
	
// 	logger::to_screen();
// 	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}


vector<mgroups::dsims_t>& processor::dsims_mgroups()
{
	if (dsims_mgroups_p.size()>0)
		return dsims_mgroups_p;
	int old_size = 0;
	dsims_measurements();
	while (old_size!=dsims_measurements_p.size() && dsims_measurements_p.size() > 0)
	{
		dsims_mgroups_p.push_back(mgroups::dsims_t(dsims_measurements_p));
	}
	return dsims_mgroups_p;
}

void processor::populate_profiler_files()
{
// 	vector<files::profiler_t> dsims_profiler_files;
// 	vector<files::profiler_t> tofsims_profiler_files;
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		/*dsims_profiler_t*/
		files::profiler_t::name_t dpFN(*f);
		if (dpFN.is_correct_type())
		{
			files::profiler_t::contents_t F(*f);
			if (F.is_correct_type())
			{
				if (dpFN.method()=="dsims")
					dsims_profiler_files_p.push_back({dpFN,F});
				else if (dpFN.method()=="tofsims")
					tofsims_profiler_files_p.push_back({dpFN,F});
				else if (dpFN.method()=="")
				{
					tofsims_profiler_files_p.push_back({dpFN,F});
					dsims_profiler_files_p.push_back({dpFN,F});
				}
				else 
					logger::error("can not distinguish dsims / tofsims profiler measurements","expect wrong results");
				filenames.erase(f);
				f--;
			}
		}
	}
		
}

vector<files::profiler_t>& processor::dsims_profiler_files()
{
	if (dsims_profiler_files_p.size()==0) populate_profiler_files();
	return dsims_profiler_files_p;
}

vector<files::profiler_t>& processor::tofsims_profiler_files()
{
	if (tofsims_profiler_files_p.size()==0) populate_profiler_files();
	return tofsims_profiler_files_p;
}

vector<measurements_::dsims_t>& processor::dsims_measurements()
{
	if (dsims_measurements_p.size()>0)
		return dsims_measurements_p;
	
	dsims_files();
// 	int last_size = dsims_files().size();
// 	while (last_size!=dsims_measurements_p.size() && dsims_files().size()>0)
	for (vector<files::dsims_t>::iterator DF=dsims_files_p.begin();DF!=dsims_files_p.end();DF++)
	{
// 		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
// 		last_size=dsims_measurements_p.size();
		dsims_measurements_p.push_back({*DF,samples_list,&dsims_jpg_files(),&dsims_profiler_files()});
		dsims_files_p.erase(DF);
		DF--;
// 		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
// 		std::cout << "\tM-S deltaT\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	}
	return dsims_measurements_p;
}

vector<files::dsims_t> & processor::dsims_files()
{
	if (dsims_files_p.size()>0)
		return dsims_files_p;
	
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		files::dsims_t::name_t dFN(*f);
		if (dFN.is_correct_type())
		{
			files::dsims_t::contents_t F(*f);
			if (F.is_correct_type())
			{
				dsims_files_p.push_back({dFN,F});
// 				filenames.erase(f); // dont allow doubles
// 				f--;
			}
		}
	}
	return dsims_files_p;
}


void processor::populate_jpg_files()
{
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		files::jpg_t::name_t djFN(*f);
		if (djFN.is_correct_type())
		{
			if (djFN.method()=="dsims") dsims_jpg_files_p.push_back(*f);
			else if (djFN.method()=="tofsims") tofsims_jpg_files_p.push_back(*f);
			else
			{
				dsims_jpg_files_p.push_back(*f);
				tofsims_jpg_files_p.push_back(*f);
			}
// 			filenames.erase(f);
// 			f--;
		}
	}
}

vector<files::jpg_t>& processor::dsims_jpg_files()
{
	if (dsims_jpg_files_p.size()==0) populate_jpg_files();
	return dsims_jpg_files_p;
}

vector<files::jpg_t>& processor::tofsims_jpg_files()
{
	if (tofsims_jpg_files_p.size()==0) populate_jpg_files();
	return tofsims_jpg_files_p;
}

// vector<measurements_::profiler_t> & processor::dsims_profiler_measurements()
// {
// 	if (dsims_profiler_measurements_p.size()>0)
// 		return dsims_profiler_measurements_p;
// 	populate_profiler_measurements();
// 	
// 	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
// 	{
// 		/*profiler_t*/
// 		filenames::profiler_t pFN(*f);
// 		if (pFN.is_correct_type())
// 		{
// 			filecontents::profiler_t F(pFN);
// 			if (F.is_correct_type())
// 			{
// 				if (tofsims_profiler_measurements_p.size()>0)
// 					logger::error("tofsims_profiler_measurements_p.size()>0: can not distinguish between dsims and tofsims profiler measurements","expect wrong results");
// 				dsims_profiler_measurements_p.push_back({pFN,F,samples_list});
// 				filenames.erase(f);
// 				f--;
// 			}
// 		}
// 	}
// 	return dsims_profiler_measurements_p;
// }
// 
// vector<measurements_::profiler_t> & processor::tofsims_profiler_measurements()
// {
// 	if (tofsims_profiler_measurements_p.size()>0)
// 		return tofsims_profiler_measurements_p;
// 	populate_profiler_measurements();
// 	
// 	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
// 	{
// 		/*profiler_t*/
// 		filenames::profiler_t pFN(*f);
// 		if (pFN.is_correct_type())
// 		{
// 			filecontents::profiler_t F(pFN);
// 			if (F.is_correct_type())
// 			{
// 				if (dsims_profiler_measurements_p.size()>0)
// 					logger::error("tofsims_profiler_measurements_p.size()>0: can not distinguish between dsims and tofsims profiler measurements","expect wrong results");
// 				tofsims_profiler_measurements_p.push_back({pFN,F,samples_list});
// 				filenames.erase(f);
// 				f--;
// 			}
// 		}
// 	}
// 	return tofsims_profiler_measurements_p;
// }


