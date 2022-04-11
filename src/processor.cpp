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

#include "processor.hpp"

//global var

// database_t db(sql_handle);

processor::processor(vector<string> args_p) : sql_wrapper(sql)
{	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	cout << "processor start" << endl;
	
	/*tests*/

	/*******/
	
	filenames=args_p;
	
	/*connect to sqlite3 database*/
	if (!sql_wrapper.open())
		logger::error("processor::processor","could not connect to to database");
	else // create tables    
	{
		sample_t::db_t::create_table(sql_wrapper);
// 		sample_t::db_t::migrate_claus1_db(sql_wrapper,"build/migrate.database.sqlite3");
	}
	/*****************************/
	
	list<sample_t> samples_list;
	profiler_t profiler(filenames,samples_list,sql_wrapper);
	camera_t camera(filenames,sql_wrapper);
	dsims_t dsims(filenames,samples_list,profiler,camera,sql_wrapper);
	
	
	
// 	cout << "filenames.size()=" << filenames.size() << endl;
// 	cout << "dsims.files().size()=" << dsims.files().size() << endl;
// 	cout << "dsims.measurements().size()=" << dsims.measurements().size() << endl;
// 	cout << "dsims.mgroups().size()=" << dsims.mgroups().size() << endl;
	
	cout << "dsims files:" << endl;
	for (auto& d : dsims.files())
		cout << "\t" << d.name.filename_without_crater_depths() << endl;
	cout << endl;
	
	
	for (auto& MG : dsims.mgroups())
	{	
		calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
// 		mat.RSFs();
// 		auto MG_copy = MG;
// 		MG_copy.calc().matrices.median_const_from_db();
// 		MG.calc().matrices.median_const_from_db().matrices.interpolate({0,1},true);
// 		auto proportional_fit_Crels_to_Irels = MG.calc().proportional_fit_Crels_to_Irels(MG.matrix_clusters());
// 		calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
		for (auto& M:MG.measurements_copy())
		{
			calc_t::sims_t::matrix_t::concentration_c Concentration(mat.RSFs(),M);
			Concentration.measurement().plot_now(0);
// 			for (auto& cluster : Concentration.measurement().clusters)
// 			{
// 				cout << cluster.concentration.to_string() << endl;
// 			}
		}
// 		Crel_to_Irel_lin_fits
		
		
// 		cout << "mat.Crel_to_Irel_lin_fits.size()=" << mat.RSFs().Crel_to_Irel_lin_fits().size() << endl;
// 		for (auto& fit : mat.RSFs().Crel_to_Irel_lin_fits())
// 			cout << "mat.Crel_to_Irel_lin_fit: " << fit.to_string() << endl;

// 		for (const auto& RSF : mat.RSFs)
// 		{
// 			cout << RSF.polynom.to_string() << endl;
// 			for (auto& d : RSF.Crel_to_Irel_data.first.data)
// 				cout << "c=" << d << endl;
// 			for (auto& d : RSF.Crel_to_Irel_data.second.data)
// 				cout << "i=" << d << endl;
// 			if (RSF.Crel_to_Irel_data.first.data().size()<2)
// 				continue;
// 			cout << RSF.to_string() << ":" << endl;
// 			cout << RSF.Crel_to_Irel_data.first.to_string_detailed() << endl;
// 			cout << RSF.Crel_to_Irel_data.second.to_string_detailed() << endl;
				
// 			RSF.plot_to_screen(0);
// 		}
	}
	
	if (!logger::instant_print_messages)
		logger::to_screen();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}

/*****************************/
/***  processor::dsims_t  ****/
/*****************************/

processor::dsims_t::dsims_t(vector<string>& filenames, 
							list<sample_t>& samples_list, 
							processor::profiler_t& profiler, 
							processor::camera_t& cam,
							database_t& sql_wrapper	) : filenames(filenames),samples_list(samples_list),profiler(profiler),camera(cam), sql_wrapper(sql_wrapper)
{
}

vector<files_::dsims_t> & processor::dsims_t::files()
{
	if (files_p.size()>0)
		return files_p;
	
	for (int f=0;f<filenames.size();f++)
	{
		files_::dsims_t::name_t dFN(filenames.at(f));
		if (dFN.is_correct_type())
		{
			files_::dsims_t::contents_t F(filenames.at(f));
			if (F.is_correct_type())
			{
				files_::dsims_t D{dFN,F};
				files_p.push_back(D);
				filenames.erase(filenames.begin()+f); // dont allow doubles
				f--;
			}
		}
	}
	sort(files_p.begin(),files_p.end());
	return files_p;
}

vector<measurements_::dsims_t>& processor::dsims_t::measurements()
{
	if (measurements_p.size()>0)
		return measurements_p;
	if (files().size()==0) // populate files_p
		return measurements_p;
	
	for (vector<files_::dsims_t>::iterator DF=files_p.begin();DF!=files_p.end();DF++)
	{
		measurements_p.push_back({*DF,samples_list,sql_wrapper,&camera.files(),&profiler.files()});
	}
	
	if (measurements_p.size()<2)
		return measurements_p;
	
	/*clear duplicates*/
	sort(measurements_p.begin(),measurements_p.end());
	for (vector<measurements_::dsims_t>::iterator DM=measurements_p.begin();DM!=measurements_p.end();DM++)
	{
		for (vector<measurements_::dsims_t>::iterator DM2=DM+1;DM2!=measurements_p.end();DM2++)
		{
			if (DM->add(*DM2))
			{
				measurements_p.erase(DM2);
				DM2--;
			}
		}
	}
	
	return measurements_p;
}

vector<mgroups_::dsims_t>& processor::dsims_t::mgroups()
{
	if (mgroups_p.size()>0)
		return mgroups_p;
	
	int old_size = 0;
	measurements();
	//populate groups with measurements, until measurement_p is empty
	while (old_size!=measurements_p.size() && measurements_p.size() > 0)
	{
		mgroups_p.push_back(mgroups_::dsims_t(measurements_p));
	}
	return mgroups_p;
}



/*****************************/
/**  processor::camera_t  ****/
/*****************************/

processor::camera_t::camera_t(vector<string>& filenames, database_t& sql_wrapper) : filenames(filenames), sql_wrapper(sql_wrapper)
{
}


vector<files_::jpg_t>& processor::camera_t::files()
{
	if (files_p.size()>0)
		return files_p;
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		files_::jpg_t::name_t djFN(*f);
		if (djFN.is_correct_type())
		{
			files_p.push_back(djFN);
			filenames.erase(f);
			f--;
		}
	}
	/*check for distinguishability*/
	set<string> different_methods;
	for (auto& PF : files_p)
	{
		different_methods.insert(PF.name.method());
	}
	different_methods.erase("");
	if (different_methods.size()>1)
		logger::error("can not distinguish dsims / tofsims jpg files","expect wrong results");
	return files_p;
}

/**********************************/
/***   processor::profiler_t   ****/
/**********************************/

processor::profiler_t::profiler_t(vector<string>& filenames, list<sample_t>& samples_list,database_t& sql_wrapper) : filenames(filenames), samples_list(samples_list), sql_wrapper(sql_wrapper)
{
}

vector<files_::profiler_t>& processor::profiler_t::files()
{
	if (files_p.size()>0)
		return files_p;
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		/*dsims_profiler_t*/
		files_::profiler_t::name_t dpFN(*f);
		if (dpFN.is_correct_type())
		{
			files_::profiler_t::contents_t F(*f);
			if (F.is_correct_type())
			{
				files_p.push_back({dpFN,F});
				filenames.erase(f);
				f--;
			}
		}
	}
	/*check for distinguishability*/
	set<string> different_methods;
	for (auto& PF : files_p)
	{
		different_methods.insert(PF.name.method());
	}
	different_methods.erase("");
	if (different_methods.size()>1)
		logger::error("can not distinguish dsims / tofsims profiler files","expect wrong results");
	return files_p;
}

vector<measurements_::profiler_t> & processor::profiler_t::measurements()
{
	if (measurements_p.size()>0)
		return measurements_p;
	if (files().size()==0) 
		return measurements_p;
	
	for (vector<files_::profiler_t>::iterator PM=files_p.begin();PM!=files_p.end();PM++)
	{
		measurements_p.push_back({*PM,samples_list,sql_wrapper});
		files_p.erase(PM);
		PM--;
	}
	return measurements_p;
}
