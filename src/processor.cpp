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
                                                dsims(unknown_filenames,samples_list_p,database,config_p),
                                                dektak6m(unknown_filenames,samples_list_p,database,config_p),
                                                jpg(unknown_filenames,samples_list_p,database,config_p),
                                                aishu(unknown_filenames,samples_list_p,database,config_p)
{	
    logger.debug(__func__,"claus").enter();

    /*testing*/
//    std::vector<int> veci(19);
//    for (int i=0;i<veci.size();i++)
//    {
//        veci[i] = i*i;
//    }
//    int* idx_pointer = &veci[5];
//    for (int i=0;i<veci.size();i++)
//    {
//        if (&veci[i]==idx_pointer)
//            std::cout << "FOUND idx=" <<i << std::endl;
//    }
//    exit(1);
    /*********/

//	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    config_p.load();
	/*connect to sqlite3 database*/
    if (!database.open())
    {
        logger.error(__func__,"database").signal("can NOT open",15,database.file_location);
//		logger::error("processor::processor","could not connect to to database");
    }
	else // create tables    
	{
        sample_t::db_t::create_table(database);
        logger.info(__func__,"database").signal("created tables",15,database.file_location);
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
//    jpg_t camera(filenames_p,sql_wrapper);
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
        tofsims.files.push_back(TF);
        return;
    }

    files_::dsims_t DF(filename, TF.contents.contents_string());
    if (DF.name.is_correct_type() && DF.contents.is_correct_type())
    {
        dsims.files.push_back(DF);
        return;
    }

    files_::profilers_t::dektak6m_t DeF(filename,DF.contents.contents_string());
    if (DeF.name.is_correct_type() && DeF.contents.is_correct_type())
    {
        dektak6m.files.push_back(DeF);
        return;
    }

    files_::jpg_t JF(filename);
    if (JF.name.is_correct_type())
    {
        jpg.files.push_back(JF);
        return;
    }

    files_::aishu_t AF(filename, DeF.contents.contents_string());
    if (AF.name.is_correct_type() && AF.contents.is_correct_type())
    {
        aishu.files.push_back(AF);
        return;
    }

	unknown_filenames.push_back(filename);
}

/**************************************
 *
 *  instanciation of templates
 *
 * ************************************/


//template class processor::filter::files_t<files_::dsims_t>;
//template class processor::filter::files_t<files_::tofsims_t>;
//template class processor::filter::files_t<files_::profilers_t::dektak6m_t>;
//template class processor::filter::files_t<files_::jpg_t>;

//template class processor::tool_with_files<files_::dsims_t>;
//template class processor::tool_with_files<files_::tofsims_t>;
//template class processor::tool_with_files<files_::profilers_t::dektak6m_t>;
//template class processor::tool_with_files<files_::jpg_t>;
