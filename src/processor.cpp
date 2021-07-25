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
		logger::error("processor::processor","could not connec to to database");
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
// 		MG.calc().SR_SF_from_implants_maxima().RSFs.from_SF_median_ref().SFs.from_RSF_median_ref().SRs.to_others().SDs.from_SR().concentrations.from_SF();
		MG.calc().SR_SF_from_implants_maxima().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().SFs.from_RSF_median_ref().concentrations.from_SF().SRs.copy_to_same_matrices().SDs.from_SR();
// 		MG.calc().matrices.median_const_from_db();
		//get matrices
		//get/calculate RSF from references
		//set/calculate matrix
		//set RSFs depending on matrix
		
		for (auto& M : MG.measurements())
		{
// 			logger::debug(11,"processor::processor()","sample="+M->sample->to_string(), M->sample->matrix().to_string());
			for (auto& C : M->clusters)
			{
				if (C.RSF.is_set())
				{
					cout << endl << "C.SF=" << C.SF.to_string() << endl;
					cout << "C.RSF=" << C.RSF.to_string() << endl;
					cout << "C.concentration=" << C.concentration.to_string() << endl;
				}
// 				isotope_t I = M->isotope_corresponding_to_cluster(C);
// 				logger::debug(11,"processor::processor()","cluster="+C.to_string(),"corresponding isotope="+I.to_string());
			}
// 			M->calc().SR.from_crater_depths().SR.from_implant_max().SR.from_ref_fit().SD.from_SR().SF.from_db_dose().SF.from_db_max().SF.from_ref_fit().concentration.from_SF();
// 			M->calc().SR.from_implant_max().SD.from_SR().SF.from_db_max().concentration.from_SF();
			M->plot_now(0);
		}
	}
	
	if (!logger::instant_print_messages)
		logger::to_screen();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}

/*****************************/
/***  processor::dsims_t  ****/
/*****************************/

processor::dsims_t::dsims_t(vector<std::__cxx11::string>& filenames, 
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

processor::camera_t::camera_t(vector<std::__cxx11::string>& filenames, database_t& sql_wrapper) : filenames(filenames), sql_wrapper(sql_wrapper)
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

processor::profiler_t::profiler_t(vector<std::__cxx11::string>& filenames, list<sample_t>& samples_list,database_t& sql_wrapper) : filenames(filenames), samples_list(samples_list), sql_wrapper(sql_wrapper)
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
