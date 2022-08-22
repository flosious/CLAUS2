/*
    Copyright (C) 2021-2022 Florian Bärwolf
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

/***********************************/
/*******  processor::filter  *****/
/***********************************/

template class processor::filter::files_t<files_::dsims_t>;
template class processor::filter::files_t<files_::tofsims_t>;
template class processor::filter::files_t<files_::profilers_t::dektak6m_t>;
template class processor::filter::files_t<files_::jpg_t>;

template<class T>
processor::filter::files_t<T>::files_t(const std::vector<T*>& files) : files_p(files)
{
}

template<class T>
processor::filter::files_t<T>::files_t(const std::vector<T>& files)
{
    *this = filter::files_t<T>(tools::vec::pointers(files));
}

template<class T>
std::vector<T*> processor::filter::files_t<T>::files() const
{
    return files_p;
}

template<class T>
processor::filter::files_t<T> processor::filter::files_t<T>::by_olcdb(int olcdb)
{
    std::vector<T*> new_items;
    new_items.reserve(files().size());
    for (T* i : files())
    {
        if (i->name.olcdb() == olcdb)
        {
            new_items.push_back(i);
        }
    }
    return new_items;
}

template<class T>
processor::filter::files_t<T> processor::filter::files_t<T>::by_lot(std::string_view lot)
{
}

template<class T>
processor::filter::files_t<T> processor::filter::files_t<T>::by_wafer(int wafer)
{
}

/**************************************/


processor::processor(vector<string> args_p) :   logger(global_logger,__FILE__,"processor"),
                                                database(sql),
                                                tofsims(unknown_filenames,samples_list_p,database,config_p),
                                                dsims(unknown_filenames,samples_list_p,database,config_p)
{	
    logger.debug(__func__,"claus").enter();
//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    config_p.load();
	/*connect to sqlite3 database*/
    if (!database.open())
    {
//		logger::error("processor::processor","could not connect to to database");
    }
	else // create tables    
	{
        sample_t::db_t::create_table(database);
// 		sample_t::db_t::migrate_claus1_db(sql_wrapper,"build/migrate.database.sqlite3");
	}
    /*****************************/
	
    //from command line
    filenames_collector_t filenames_collector;
    filenames_collector.add_files_or_folders(args_p);
    ///all filenames are unknown in the beginning
//    unknown_filenames = filenames_collector.filenames();
    parse_filenames(filenames_collector.filenames());
//    tofsims.add_all_files_to_measurements();
//    for (auto& F : tofsims.files())
//    {
//        logger.info(__func__,"tofsims files").value(F.name.filename());
//    }

//    std::vector<measurements_::tofsims_t> tms;
//    logger.info(__func__,"this").signal("HERE");

//	list<sample_t> samples_list;
//    dektak6m_t dektak6m(filenames_p,samples_list,sql_wrapper);
//    camera_t camera(filenames_p,sql_wrapper);
//    tofsims_t tofsims(filenames_p,samples_list,sql_wrapper);
//    dsims_t dsims(filenames_p,samples_list,sql_wrapper);
	
//	cout << "tofsims files: <" << tofsims.files().size() << ">" << endl;
//	for (auto& d : tofsims.files())
//	{
//		cout << "\t" << d.name.filename_with_path << endl;
//	}
//	cout << endl;
	
//	cout << "dsims files: <" << dsims.files().size() << ">" << endl;
//	for (auto& d : dsims.files())
//	{
//		cout << "\t" << d.name.filename_with_path << endl;
//	}
//	cout << endl;
	
	
//	cout << "dektak6m files: <" << dektak6m.files().size() << ">" << endl;
//	for (auto& d : dektak6m.files())
//	{
//		cout << "\t" << d.name.filename_with_path << endl;
//// 		cout << d.contents.linescan().xy.to_string_detailed() << endl;
//// 		d.contents.linescan().plot_now(0);
//	}
//	cout << endl;

//    cout << "not recognized: <" << filenames_p.size() << ">" << endl;
//    for (auto& f : filenames_p)
//        cout << "\t" << f << endl;
//	cout << endl;
	
//	cout << "dektak6m measurements: <" << dektak6m.measurements().size() << ">" << endl;
//	for (auto& d : dektak6m.measurements())
//		cout << "\t" << d.to_string() << endl;
//	cout << endl;
//	cout << "tofsims measurements: <" << tofsims.measurements().size() << ">" << endl;
//	for (auto& d : tofsims.measurements())
//		cout << "\t" << d.to_string() << endl;
//	cout << endl;
//	cout << "dsims measurements: <" << dsims.measurements().size() << ">" << endl;
//	for (auto& d : dsims.measurements())
//		cout << "\t" << d.to_string() << endl;
//	cout << endl;
//	cout << "samples: <" << samples_list.size() << ">" << endl;
//	for (auto& S : samples_list)
//	{
//		cout <<  "\t"  << S.to_string() << "; implants: " << endl;
//		for (auto& imp : S.implants())
//			cout << "\t\t" << imp.first.to_string() << ": " << imp.second.to_string() << endl;
//	}
//	cout << endl;
//	/*copy linescans to sims measurements*/
//// 	for (auto iter=dektak6m.measurements().begin();iter!=dektak6m.measurements().end();iter++)
//	for (measurements_::profilers_t::dektak6m_t&  pm : dektak6m.measurements())
//	{
//		for (measurements_::dsims_t& dm : dsims.measurements())
//		{
//			if (pm == dm)
//			{
//				dm.crater.linescans.push_back(pm.linescan());
//			}
//		}
//		for (auto& tm : tofsims.measurements())
//		{
//			if (pm == tm)
//			{
//				tm.crater.linescans.push_back(pm.linescan());
//			}
//		}
//	}
	
//	for (auto&  cf : camera.files())
//	{
//		measurements_::measurement_t cm(cf.name,samples_list,"camera",sql_wrapper);
//		for (measurements_::dsims_t& dm : dsims.measurements())
//		{
//			if (cm == dm)
//			{
//				dm.crater.total_sputter_depths << cf.name.total_sputter_depths();
//			}
//		}
//		for (auto& tm : tofsims.measurements())
//		{
//			if (cm == tm)
//			{
//				tm.crater.total_sputter_depths << cf.name.total_sputter_depths();
//			}
//		}
//	}
	
    /*tof sims*/
//    for (auto& MG : tofsims().mgroups())
//	{
//		MG.set_reference_isotopes_in_measurements();
//		MG.set_natural_abundances_in_matrix_clusters();
//		auto calc = MG.calc();
//		calc.matrices.median_const_from_reference_isotopes();
//		calc.SRs.from_crater_depths().SRs.from_implant_max();
//		if (MG.matrix_clusters().size()>1)
//		{
//			calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
//			auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
//			RSFs.plot_now(0);
//			for (auto& M:MG.measurements())
//			{
//				calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
//				const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
//				///copy the results - this is really ugly
//				for (auto C : M_with_Cs.clusters)
//				{
//					if (!C.concentration.is_set())
//						continue;
//					*M->cluster(C) = C;
//				}
//			}
//		}
//		// SR + SD
//		calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1}).SDs.from_SR();
//		// SF + RSF
//		calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
//		calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
//		// C
//		calc.concentrations.from_SF();
//		for (auto M : MG.measurements())
//		{
//			M->plot_now(0);
//		}
		/*exporting*/
//		MG.export_origin_ascii(config.tofsims_export_location);
//	}
	
	/*d sims*/
//	for (auto& MG : dsims.mgroups())
//	{
//		MG.set_reference_isotopes_in_measurements();
//		MG.set_natural_abundances_in_matrix_clusters();
//		auto calc = MG.calc();
//		calc.matrices.median_const_from_reference_isotopes();
//		calc.SRs.from_crater_depths().SRs.from_implant_max();
//		if (MG.matrix_clusters().size()>1)
//		{
//			calc_t::sims_t::matrix_t mat(MG.matrix_isotopes(),MG.measurements_copy());
//			auto RSFs = mat.RSFs().add_natural_abundances().remove_RSFs_below_gof_treshold(0.2).symmetrical_RSFs();
//			RSFs.plot_now(0);
//			for (auto& M:MG.measurements())
//			{
//				calc_t::sims_t::matrix_t::concentration_c Concentration(RSFs,*M);
//				const auto M_with_Cs = Concentration.concentrations_by_RSFs().concentrations_by_filling_up_all_concentrations_to_1().measurement();
//				///copy the results - this is really ugly
//				for (auto C : M_with_Cs.clusters)
//				{
//					if (!C.concentration.is_set())
//						continue;
//					*M->cluster(C) = C;
//				}
//			}
//		}
//		// SR
//		calc.SRs.copy_to_same_matrices().SRs.interpolate_from_known_sample_matrices({1,1});
//		// SD
//		calc.SDs.from_SR();
//		// SF + RSF
//		calc.SFs.from_implant_dose().SFs.from_implant_max().RSFs.from_SF_median_ref().RSFs.copy_to_same_matrices().RSFs.interpolate_from_known_sample_matrices();
//		calc.SFs.from_RSF_pbp_ref().SFs.from_implant_max();
//		// C
//		calc.concentrations.from_SF();
//		for (auto M : MG.measurements())
//		{
//			M->plot_now(0);
//		}
//		/*exporting*/
//		MG.export_origin_ascii(config.dsims_export_location);
//	}
	
    //logger::to_file("log.txt");
	
// 	if (!logger::instant_print_messages)
// 		logger::to_screen();
	
//	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//	std::cout << "Program runtime\t" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    logger.debug(__func__,"claus").exit();
}

processor::~processor()
{
//    logger().debug("processor","claus","stop");
}

void processor::parse_unknown_filenames()
{
	parse_filenames(unknown_filenames);
}

void processor::parse_filenames(const std::vector<std::string>& filenames)
{
	for (auto& f : filenames)
		parse_filename(f);
}

void processor::parse_filename(string filename)
{
	// this is not efficient as for each try the file is closed and loaded;
	// it would be better to implement a general file.contents, 
	// which his loaded once and then tried for all method
    files_::tofsims_t TF(filename);
    if (TF.name.is_correct_type() && TF.contents.is_correct_type())
    {
        tofsims.add(TF);
        return;
    }

    files_::dsims_t DF(filename);
    if (DF.name.is_correct_type() && DF.contents.is_correct_type())
    {
        dsims.files.push_back(DF);
        return;
    }

    files_::aishu_t AF(filename);
    if (AF.name.is_correct_type() && AF.contents.is_correct_type())
    {
        aishu.files.push_back(AF);
        return;
    }

	unknown_filenames.push_back(filename);
}


/*****************************/
/**  processor::dsims_t    ***/
/*****************************/

processor::filter::files_t<files_::dsims_t> processor::dsims_t::filter_files(const std::vector<files_::dsims_t*>& files_s)
{
    return processor::filter::files_t(files_s);
}

processor::filter::files_t<files_::dsims_t> processor::dsims_t::filter_files()
{
    return filter::files_t(tools::vec::pointers(files));
}

processor::dsims_t::dsims_t(std::vector<string>& filenames_list, std::vector<sample_t>& samples_list, database_t& database, config_t& config)
    : logger(global_logger,__FILE__,"processor::dsims_t"), samples_list(&samples_list), database(&database), config(&config)
{
}

void processor::dsims_t::add_all_files_to_measurements()
{

    for (auto& F : files)
    {
        add_to_measurement(F);
    }
}
void processor::dsims_t::add_to_measurement(files_::dsims_t& F)
{
    measurements_::dsims_t DM(F,*database);
    measurements.push_back(DM);
}



/*****************************/
/**  processor::tofsims_t  ***/
/*****************************/

processor::tofsims_t::tofsims_t(vector<string>& filenames_list,
                                vector<sample_t>& samples_list,
                                database_t& database,
                                config_t& config	)
                                : filenames_list(&filenames_list)
                                , samples_list(&samples_list)
                                , database(&database)
                                , config(&config)
                                , logger(global_logger,__FILE__,"processor::tofsims_t")
{
}

//string processor::tofsims_t::export_path="";
void processor::tofsims_t::add(mgroups_::tofsims_t& MG)
{
	mgroups_p.push_back(MG);
}
void processor::tofsims_t::add(measurements_::tofsims_t& M)
{	
    logger.debug(__func__,"measurement").enter();
    if (measurements_p.size()>0)
    {
        logger.debug(__func__,"measurements_p.back()").value(measurements_p.back().to_string_short(),10,measurements_p.back().to_string());
    }
    measurements_p.push_back(M);
    logger.debug(__func__,"measurements_p").value("pushed back",10,M.to_string());
    logger.debug(__func__,"measurements_p.back()").value(measurements_p.back().to_string_short(),10,measurements_p.back().to_string());
    logger.debug(__func__,"measurement").exit();
}
void processor::tofsims_t::add( std::vector<measurements_::tofsims_t>& Ms)
{
// 	measurements_p.reserve(measurements_p.size()+Ms.size());
// 	for (const auto& M : Ms)
// 		add(M);
	measurements_p.insert(measurements_p.end(),Ms.begin(),Ms.end());
}
void processor::tofsims_t::add( files_::tofsims_t& M)
{
	files_p.push_back(M);
}
void processor::tofsims_t::add( std::vector<files_::tofsims_t>& Fs)
{
// 	files_p.reserve(files_p.size()+Fs.size());
// 	for (const auto& F : Fs)
// 		add(F);
	files_p.insert(files_p.end(),Fs.begin(),Fs.end());
}
void processor::tofsims_t::remove(measurements_::tofsims_t& M)
{
	auto iter = std::find(measurements_p.begin(), measurements_p.end(), M);
	if (iter != measurements_p.end())
		measurements_p.erase(iter);
}
void processor::tofsims_t::remove(files_::tofsims_t& F)
{
	auto iter = std::find(files_p.begin(), files_p.end(), F);
	if (iter != files_p.end())
		files_p.erase(iter);
}

void processor::tofsims_t::add_all_files_to_measurements()
{
    for (auto& F : files())
    {
        add_to_measurement(F);
    }
}

void processor::tofsims_t::add_to_measurement(files_::tofsims_t& F)
{
    logger.debug(__func__,"this").enter();
    measurements_::tofsims_t TM(F,*database);
    logger.debug(__func__,"file=").value(F.name.filename_with_path);
    logger.debug(__func__,"measurement=").value(TM.to_string_short(),10,TM.to_string());
    add(TM);
    logger.debug(__func__,"this").exit();
}

vector<files_::tofsims_t>& processor::tofsims_t::files()
{
// 	files_p.reserve(filenames_list->size());
// 	for (auto& f : *filenames_list)
//     {
//         files_::tofsims_t F(f);
//         if (F.name.is_correct_type() && F.contents.is_correct_type())
//         {
// 			logger::debug(3,"processor::tofsims_t::files()",f + "\t->\t" + "tofsims_t");
// 			files_p.push_back(F);
//         }
//     }
//     
//     // erase names from the list of unknown filenames (filenames_list)
//     for (auto& F : files_p)
//     {
// 		auto iter = std::find(filenames_list->begin(),filenames_list->end(),F.name.filename_with_path);
//         filenames_list->erase(iter);
//     }

    return files_p;
}
vector<measurements_::tofsims_t>& processor::tofsims_t::measurements()
{
	return measurements_p;
}
vector<mgroups_::tofsims_t>& processor::tofsims_t::mgroups()
{
	return mgroups_p;
}

vector<sample_t> processor::tofsims_t::samples() const
{
	if (samples_list->size()==0)
		return {};
    vector<sample_t> S;
	S.reserve(samples_list->size()); //over estimation, but ok
	for (auto& M : measurements_p)
	{
		S.push_back(M.sample);
	}
	for (auto& MG : mgroups_p)
	{
		for (auto& M : measurements_p)
		{
			S.push_back(M.sample);
		}
	}
	return S;
}


//std::map<std::string, files_::tofsims_t>& processor::tofsims_t::filenames_with_path_to_files()
//{
//    for (auto& f : *filenames_list)
//    {
//        if (filenames_with_path_to_files_p.find(f) != filenames_with_path_to_files_p.end())
//        {
//            continue;
//        }
//        files_::tofsims_t F(f);
//        if (F.name.is_correct_type() && F.contents.is_correct_type())
//        {
//            logger::debug(3,"processor::tofsims_t::files()",f + "\t->\t" + "tofsims_t");
//            filenames_with_path_to_files_p.insert(std::pair<std::string,files_::tofsims_t>(f,F));
//        }
//    }
	
    ///erase names from the list of unknown filenames (filenames_list)
//    for (auto& F : filenames_with_path_to_files_p)
//    {
//        auto iter = std::find(filenames_list->begin(),filenames_list->end(),F.first);
//        filenames_list->erase(iter);
//    }

//    return filenames_with_path_to_files_p;

//    for (size_t f=0;f<filenames_list->size();f++)
//	{
//        auto& ff = filenames_list->at(f);
//        ///filename already in the list? --> skip and delete from the filenames_list
//        if (filename_with_path_to_file.find(ff) != filename_with_path_to_file.end())
//        {
//            filenames_list->erase(filenames_list->begin()+f); // dont allow doubles
//            f--;
//            continue;
//        }
//        files_::tofsims_t F(ff);
//		if (F.name.is_correct_type() && F.contents.is_correct_type())
//		{
//            logger::debug(3,"processor::tofsims_t::files()",filenames_list->at(f) + "\t->\t" + "tofsims_t");
//			files_p.push_back(F);
//            filenames_list->erase(filenames_list->begin()+f); // dont allow doubles
//			f--;
//		}
//	}
//	sort(files_p.begin(),files_p.end());
//	return files_p;
//}

//vector<measurements_::tofsims_t> processor::tofsims_t::measurements()
//{
//	if (measurements_p.size()>0)
//		return measurements_p;
//	if (files().size()==0) // populate files_p
//		return measurements_p;
//    (*database).open();
//	for (vector<files_::tofsims_t>::iterator DF=files_p.begin();DF!=files_p.end();DF++)
//	{
//        measurements_::tofsims_t M(*DF,*samples_list,*database);
//	}
	
//	if (measurements_p.size()<2)
//		return measurements_p;
	
//	/*clear duplicates*/
//	sort(measurements_p.begin(),measurements_p.end());
//	for (vector<measurements_::tofsims_t>::iterator DM=measurements_p.begin();DM!=measurements_p.end();DM++)
//	{
//		for (vector<measurements_::tofsims_t>::iterator DM2=DM+1;DM2!=measurements_p.end();DM2++)
//		{
//			if (DM->add(*DM2))
//			{
//				measurements_p.erase(DM2);
//				DM2--;
//			}
//		}
//	}
	
//	return measurements_p;
//}

// std::vector<measurements_::tofsims_t>& processor::tofsims_t::measurements()
// {
//     database->open();
//     measurements_p.reserve(files().size());
//     for (auto f : files())
//     {
//         measurements_::tofsims_t M(f,*samples_list,*database);
//         if (std::find(measurements_p.begin(),measurements_p.end(),M) != measurements_p.end())
//             continue;
//         measurements_p.push_back(M);
//     }
//     return measurements_p;
// }
// 
// vector<mgroups_::tofsims_t>& processor::tofsims_t::mgroups()
// {
// 	if (mgroups_p.size()>0)
// 		return mgroups_p;
// 	
// 	int old_size = 0;
// 	measurements();
// 	//populate groups with measurements, until measurement_p is empty
// 	while (old_size!=measurements_p.size() && measurements_p.size() > 0)
// 	{
// 		mgroups_p.push_back(mgroups_::tofsims_t(measurements_p));
// 	}
// 	return mgroups_p;
// }

/*****************************/
/***  processor::dsims_t  ****/
/*****************************/

//processor::dsims_t::dsims_t(vector<string>& filenames,
//							list<sample_t>& samples_list,
//                            database_t& sql_wrapper	) : filenames(filenames),samples_list(samples_list), sql_wrapper(sql_wrapper)
//{
//}

//string processor::dsims_t::export_path="";

//vector<files_::dsims_t> & processor::dsims_t::files()
//{
//	if (files_p.size()>0)
//		return files_p;
	
//    for (int f=0;f<filenames.size();f++)
//	{
//        files_::dsims_t dsims_file(filenames.at(f));
//		if (dsims_file.name.is_correct_type() && dsims_file.contents.is_correct_type())
//		{
//            logger::debug(3,"processor::dsims_t::files()",filenames.at(f) + "\t->\t" + "dsims_t");
//			files_p.push_back(dsims_file);
//            filenames.erase(filenames.begin()+f); // dont allow doubles
//			f--;
//		}
//	}
//	sort(files_p.begin(),files_p.end());
//	return files_p;
//}

//vector<measurements_::dsims_t>& processor::dsims_t::measurements()
//{
//	if (measurements_p.size()>0)
//		return measurements_p;
//	if (files().size()==0) // populate files_p
//		return measurements_p;
	
//	for (vector<files_::dsims_t>::iterator DF=files_p.begin();DF!=files_p.end();DF++)
//	{
//		measurements_p.push_back(measurements_::dsims_t{*DF,samples_list,sql_wrapper});
//	}
	
//	if (measurements_p.size()<2)
//		return measurements_p;
	
//	/*clear duplicates*/
//	sort(measurements_p.begin(),measurements_p.end());
//	for (vector<measurements_::dsims_t>::iterator DM=measurements_p.begin();DM!=measurements_p.end();DM++)
//	{
//		for (vector<measurements_::dsims_t>::iterator DM2=DM+1;DM2!=measurements_p.end();DM2++)
//		{
//			if (DM->add(*DM2))
//			{
//				logger::warning(3,"processor::dsims_t::measurements()","cleared duplicate of: " + DM->to_string());
//				measurements_p.erase(DM2);
//				DM2--;
//			}
//		}
//	}
	
//	return measurements_p;
//}

//vector<mgroups_::dsims_t>& processor::dsims_t::mgroups()
//{
//	if (mgroups_p.size()>0)
//		return mgroups_p;
	
//	int old_size = 0;
//	measurements();
//	//populate groups with measurements, until measurement_p is empty
//	while (old_size!=measurements_p.size() && measurements_p.size() > 0)
//	{
//		mgroups_p.push_back(mgroups_::dsims_t(measurements_p));
//	}
//	return mgroups_p;
//}



/*****************************/
/**  processor::camera_t  ****/
/*****************************/

//processor::camera_t::camera_t(vector<string>& filenames, database_t& sql_wrapper) : filenames(filenames), sql_wrapper(sql_wrapper)
//{
//}

//vector<files_::jpg_t>& processor::camera_t::files()
//{
//	if (files_p.size()>0)
//		return files_p;
//    for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
//	{
//		files_::jpg_t jpg_file(*f);
//		if (jpg_file.name.is_correct_type())
//		{
//			logger::debug(3,"processor::jpg_t::files()",*f + "\t->\t" + "jpg_t");
//			files_p.push_back(jpg_file);
//            filenames.erase(f);
//			f--;
//		}
//	}
//	return files_p;
//}

/**********************************/
/***   processor::dektak6m_t   ****/
/**********************************/

//processor::dektak6m_t::dektak6m_t(vector<string>& filenames, list<sample_t>& samples_list,database_t& sql_wrapper) :
//    filenames(filenames), samples_list(samples_list), sql_wrapper(sql_wrapper)
//{
//}

//vector<files_::profilers_t::dektak6m_t>& processor::dektak6m_t::files()
//{
//	if (files_p.size()>0)
//		return files_p;
//    for (vector<string>::iterator f=filenames.begin();f!=filenames.end();++f)
//	{
//		/*dsims_profiler_t*/
//		files_::profilers_t::dektak6m_t  dektak6m_file(*f);
//		if (dektak6m_file.name.is_correct_type() && dektak6m_file.contents.is_correct_type())
//		{
//			logger::debug(3,"processor::dektak6m_t::files()",*f + "\t->\t" + "dektak6m_t");
//			files_p.push_back(dektak6m_file);
//            filenames.erase(f);
//			f--;
//		}
//	}
//	return files_p;
//}

//vector<measurements_::profilers_t::dektak6m_t>& processor::dektak6m_t::measurements()
//{
//	if (measurements_p.size()>0)
//		return measurements_p;
//	if (files().size()==0)
//		return measurements_p;
	
//	for (vector<files_::profilers_t::dektak6m_t>::iterator PM=files_p.begin();PM!=files_p.end();PM++)
//	{
//		measurements_p.push_back({*PM,samples_list,sql_wrapper});
//		files_p.erase(PM);
//		PM--;
//	}
//	return measurements_p;
//}
