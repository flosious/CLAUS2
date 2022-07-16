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
	
	/*config*/
	config.load();
	
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
	dektak6m_t dektak6m(filenames,samples_list,sql_wrapper);
	camera_t camera(filenames,sql_wrapper);
	tofsims_t tofsims(filenames,samples_list,sql_wrapper);
	dsims_t dsims(filenames,samples_list,sql_wrapper);
	
	cout << "tofsims files:" << endl;
	for (auto& d : tofsims.files())
	{
		cout << "\t" << d.name.filename_with_path << endl;
	}
	cout << endl;
	
	cout << "dsims files:" << endl;
	for (auto& d : dsims.files())
	{
		cout << "\t" << d.name.filename_with_path << endl;
	}
	cout << endl;
	
	
	cout << "dektak6m files:" << endl;
	for (auto& d : dektak6m.files())
	{
		cout << "\t" << d.name.filename_with_path << endl;
// 		cout << d.contents.linescan().xy.to_string_detailed() << endl;
// 		d.contents.linescan().plot_now(0);
	}
	cout << endl;

	cout << "not recognized: " << endl;
	for (auto& f : filenames)
		cout << "\t" << f << endl;
	cout << endl;
	
// 	cout << "dektak6m measurements:" << endl;
// 	for (auto& d : dektak6m.measurements())
// 	{
// 		cout << "\t" << d.to_string() << endl;
// 	}
// 	cout << endl;
	
	
	/*copy linescans to sims measurements*/
// 	for (auto iter=dektak6m.measurements().begin();iter!=dektak6m.measurements().end();iter++)
	for (measurements_::profilers_t::dektak6m_t&  pm : dektak6m.measurements())
	{
		for (measurements_::dsims_t& dm : dsims.measurements())
		{
			if (pm == dm)
			{
				dm.crater.linescans.push_back(pm.linescan());
			}
		}
		for (auto& tm : tofsims.measurements())
		{
			if (pm == tm)
			{
				tm.crater.linescans.push_back(pm.linescan());
			}
		}
	}
	
	for (auto&  cf : camera.files())
	{
		measurements_::measurement_t cm(cf.name,samples_list,"camera",sql_wrapper);
		for (measurements_::dsims_t& dm : dsims.measurements())
		{
			if (cm == dm)
			{
				dm.crater.total_sputter_depths << cf.name.total_sputter_depths();
			}
		}
		for (auto& tm : tofsims.measurements())
		{
			if (cm == tm)
			{
				tm.crater.total_sputter_depths << cf.name.total_sputter_depths();
			}
		}
	}
	
	/*tof sims*/
	for (auto& MG : tofsims.mgroups())
	{
		MG.set_reference_isotopes_in_measurements();
		MG.set_natural_abundances_in_matrix_clusters();
		auto calc = MG.calc();
		calc.matrices.median_const_from_reference_isotopes();
		calc.SRs.from_crater_depths().SRs.from_implant_max();
		if (MG.matrix_clusters().size()>1)
		{
			calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
			auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
			RSFs.plot_now(0);
			for (auto& M:MG.measurements())
			{
				calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
				const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
				///copy the results - this is really ugly
				for (auto C : M_with_Cs.clusters)
				{
					if (!C.concentration.is_set()) 
						continue;
					*M->cluster(C) = C;
				}
			}
		}
		// SR + SD
		calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1}).SDs.from_SR();
		// SF + RSF
		calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
		calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
		// C
		calc.concentrations.from_SF();
		for (auto M : MG.measurements())
		{
			M->plot_now(0);
		}
		/*exporting*/
		MG.export_origin_ascii(config.tofsims_export_location);
	}
	
	/*d sims*/
	for (auto& MG : dsims.mgroups())
	{
		MG.set_reference_isotopes_in_measurements();
		MG.set_natural_abundances_in_matrix_clusters();
		auto calc = MG.calc();
		calc.matrices.median_const_from_reference_isotopes();
		calc.SRs.from_crater_depths().SRs.from_implant_max();
		if (MG.matrix_clusters().size()>1)
		{
			calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
			auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
			RSFs.plot_now(0);
			for (auto& M:MG.measurements())
			{
				calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
				const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
				///copy the results - this is really ugly
				for (auto C : M_with_Cs.clusters)
				{
					if (!C.concentration.is_set()) 
						continue;
					*M->cluster(C) = C;
				}
			}
		}
		// SR
		calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1});
		// SD
		calc.SDs.from_SR();
		// SF + RSF
		calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
		calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
		// C
		calc.concentrations.from_SF();
		for (auto M : MG.measurements())
		{
			M->plot_now(0);
		}
		/*exporting*/
		MG.export_origin_ascii(config.dsims_export_location);
	}
	
	cout << "samples: " << samples_list.size() << endl;
	for (auto& S : samples_list)
		cout << S.to_string() << endl;
	
	if (!logger::instant_print_messages)
		logger::to_screen();
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
}


/*****************************/
/**  processor::tofsims_t  ***/
/*****************************/

processor::tofsims_t::tofsims_t(vector<string>& filenames, 
							list<sample_t>& samples_list, 
							database_t& sql_wrapper	) : filenames(filenames),samples_list(samples_list), sql_wrapper(sql_wrapper)
{
}

string processor::tofsims_t::export_path="";

vector<files_::tofsims_t> & processor::tofsims_t::files()
{
	if (files_p.size()>0)
		return files_p;
	
	for (int f=0;f<filenames.size();f++)
	{
		files_::tofsims_t F(filenames.at(f));
		if (F.name.is_correct_type() && F.contents.is_correct_type())
		{
			logger::debug(3,"processor::tofsims_t::files()",filenames.at(f) + "\t->\t" + "tofsims_t");
			files_p.push_back(F);
			filenames.erase(filenames.begin()+f); // dont allow doubles
			f--;
		}
	}
	sort(files_p.begin(),files_p.end());
	return files_p;
}
vector<measurements_::tofsims_t>& processor::tofsims_t::measurements()
{
	if (measurements_p.size()>0)
		return measurements_p;
	if (files().size()==0) // populate files_p
		return measurements_p;
	
	for (vector<files_::tofsims_t>::iterator DF=files_p.begin();DF!=files_p.end();DF++)
	{
		measurements_p.push_back({*DF,samples_list,sql_wrapper});
	}
	
	if (measurements_p.size()<2)
		return measurements_p;
	
	/*clear duplicates*/
	sort(measurements_p.begin(),measurements_p.end());
	for (vector<measurements_::tofsims_t>::iterator DM=measurements_p.begin();DM!=measurements_p.end();DM++)
	{
		for (vector<measurements_::tofsims_t>::iterator DM2=DM+1;DM2!=measurements_p.end();DM2++)
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

vector<mgroups_::tofsims_t>& processor::tofsims_t::mgroups()
{
	if (mgroups_p.size()>0)
		return mgroups_p;
	
	int old_size = 0;
	measurements();
	//populate groups with measurements, until measurement_p is empty
	while (old_size!=measurements_p.size() && measurements_p.size() > 0)
	{
		mgroups_p.push_back(mgroups_::tofsims_t(measurements_p));
	}
	return mgroups_p;
}

/*****************************/
/***  processor::dsims_t  ****/
/*****************************/

processor::dsims_t::dsims_t(vector<string>& filenames, 
							list<sample_t>& samples_list, 
							database_t& sql_wrapper	) : filenames(filenames),samples_list(samples_list), sql_wrapper(sql_wrapper)
{
}

string processor::dsims_t::export_path="";

vector<files_::dsims_t> & processor::dsims_t::files()
{
	if (files_p.size()>0)
		return files_p;
	
	for (int f=0;f<filenames.size();f++)
	{
		files_::dsims_t dsims_file(filenames.at(f));
		if (dsims_file.name.is_correct_type() && dsims_file.contents.is_correct_type())
		{
			logger::debug(3,"processor::dsims_t::files()",filenames.at(f) + "\t->\t" + "dsims_t");
			files_p.push_back(dsims_file);
			filenames.erase(filenames.begin()+f); // dont allow doubles
			f--;
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
		measurements_p.push_back(measurements_::dsims_t{*DF,samples_list,sql_wrapper});
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
				logger::warning(3,"processor::dsims_t::measurements()","cleared duplicate of: " + DM->to_string());
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
		files_::jpg_t jpg_file(*f);
		if (jpg_file.name.is_correct_type())
		{
			logger::debug(3,"processor::jpg_t::files()",*f + "\t->\t" + "jpg_t");
			files_p.push_back(jpg_file);
			filenames.erase(f);
			f--;
		}
	}
	return files_p;
}

/**********************************/
/***   processor::dektak6m_t   ****/
/**********************************/

processor::dektak6m_t::dektak6m_t(vector<string>& filenames, list<sample_t>& samples_list,database_t& sql_wrapper) : 
	filenames(filenames), samples_list(samples_list), sql_wrapper(sql_wrapper)
{
}

vector<files_::profilers_t::dektak6m_t>& processor::dektak6m_t::files()
{
	if (files_p.size()>0)
		return files_p;
	for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
	{
		/*dsims_profiler_t*/
		files_::profilers_t::dektak6m_t  dektak6m_file(*f);
		if (dektak6m_file.name.is_correct_type() && dektak6m_file.contents.is_correct_type())
		{
			logger::debug(3,"processor::dektak6m_t::files()",*f + "\t->\t" + "dektak6m_t");
			files_p.push_back(dektak6m_file);
			filenames.erase(f);
			f--;
		}
	}
	return files_p;
}

vector<measurements_::profilers_t::dektak6m_t>& processor::dektak6m_t::measurements()
{
	if (measurements_p.size()>0)
		return measurements_p;
	if (files().size()==0) 
		return measurements_p;
	
	for (vector<files_::profilers_t::dektak6m_t>::iterator PM=files_p.begin();PM!=files_p.end();PM++)
	{
		measurements_p.push_back({*PM,samples_list,sql_wrapper});
		files_p.erase(PM);
		PM--;
	}
	return measurements_p;
}
