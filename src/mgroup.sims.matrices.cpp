#include "mgroup.hpp"


//mgroups_::sims_t::matrices_t::matrices_t(sims_t& MG) : MG(MG), log_c
//{

//}

//std::set<sample_t::matrix_t> mgroups_::sims_t::matrices_t::sample_matrices() const
//{
//    std::set<sample_t::matrix_t> mats;
//    for (auto& M : MG.measurements())
//        if (M->sample.matrix().is_set())
//            mats.insert(M->sample.matrix());
//    return mats;
//}

//std::vector<isotope_t> mgroups_::sims_t::matrices_t::isotopes() const
//{
//    log_f;
//    std::set<isotope_t> isos;
//    for (auto& M : MG.measurements())
//    {
////        const auto mat_isos = M->matrix_clusters().isotopes(); // this is wrong
//        const auto mat_isos = M->sample.matrix().isotopes();

//        isos.insert(mat_isos.begin(),mat_isos.end());
//    }
//    std::vector<isotope_t> isos_vec = {isos.begin(),isos.end()};

//    // keep abundance and/or substance_amount if there is only 1 known matrix with exactly 1 element from all reference samples within this group
////    if (sample_matrices().size()==1 && elements().size()==1)
////    {
////        return isos_vec;
////    }
//    // delete abundance and substance_amount because there are different ones in the group. calculate later
//    for (auto& I : isos_vec)
//    {
//        I.substance_amount.clear();
//        I.abundance.clear();
//    }

//    for (auto& mi : isos_vec)
//    {
//        //logger::info(3,"mgroups_::sims_t::matrix_isotopes()",mi.to_string());
//    }
//    if (isos.size()==0)
//    {
//        logger.warning("istopes").signal("none");
//        //logger::warning(1,"no matrix isotopes","");
//    }
//    std::stringstream out;
//    for (auto& iso : isos)
//        out << iso.to_string_short() +"; ";
//    logger.debug(MG.to_string()).value(out.str());
//    return isos_vec;
//}

//quantity::table_t mgroups_::sims_t::matrices_t::concentrations()
//{
//    quantity::table_t table;
//    for (const auto& I : isotopes())
//        table.add(concentrations(I));
//    return table;
//}

//quantity::table_t mgroups_::sims_t::matrices_t::concentrations(const isotope_t& isotope)
//{
//    quantity::table_t table;
//    data_collectors_t::sims_t dc(MG.measurements());
//    const auto col = dc.get_column_concentrations_from_sample_matrix(isotope);
//    table.add(col);
//    return table;
//}


//quantity::table_t mgroups_::sims_t::matrices_t::concentrations(const element_t& element)
//{
//    quantity::table_t table;
//    data_collectors_t::sims_t dc(MG.measurements());
//    const auto col = dc.get_column_concentrations_from_sample_matrix(element);
//    table.add(col);
//    return table;
//}

////quantity::table_t mgroups_::sims_t::matrices_t::sample_matrices_substance_amounts_to_cluster_intensities() const
////{
////    quantity::table_t table;
////    for (auto& M : MG.measurements())
////    {
////        quantity::table_t::column_t col(M->memory_address(),M->sa);
////    }

////    return table;
////}
