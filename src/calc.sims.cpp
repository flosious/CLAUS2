/*
	Copyright (C) 2022 Florian Bärwolf
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

#include "calc.hpp"

calc_t::sims_t::sims_t(const vector<measurements_::sims_t>& measurements) : measurements(measurements)
{
}
/*******************************************************/
/********    cluster_relations_copies_t     ************/
/*******************************************************/

calc_t::sims_t::cluster_relations_copies_t::cluster_relations_copies_t(const cluster_t& Z, const cluster_t& N, const quantity::abundance_t& abundance_ratio) : zaehler_p(Z), nenner_p(N), abundance_ratio(abundance_ratio)
{
}

const cluster_t& calc_t::sims_t::cluster_relations_copies_t::zaehler() const
{
	return zaehler_p;
}
const cluster_t& calc_t::sims_t::cluster_relations_copies_t::nenner() const
{
	return nenner_p;
}

/*******************************************************/
/************    cluster_relations_t     ***************/
/*******************************************************/

calc_t::sims_t::cluster_relations_t::cluster_relations_t(const measurements_::sims_t& measurement, 
														 const cluster_t& zaehler_s, 
														 const cluster_t& nenner_s) : 
														 measurement(measurement), 
														 zaehler(measurement.cluster(zaehler_s)), 
														 nenner(measurement.cluster(nenner_s))
{
	if (zaehler==nullptr)
		logger::fatal("calc_t::sims_t::cluster_relations_t::cluster_relations_t()","zaehler==nullptr", zaehler_s.to_string());
	if (nenner == nullptr)
		logger::fatal("calc_t::sims_t::cluster_relations_t::cluster_relations_t()","nenner==nullptr", nenner_s.to_string());
}

bool calc_t::sims_t::cluster_relations_t::clusters_exist() const
{
	if (zaehler==nullptr) return false;
	if (nenner==nullptr) return false;
	return true;
}

/*******************************************************/
/**********************    Crel_t     ******************/
/*******************************************************/


calc_t::sims_t::Crel_t::Crel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner) :
							cluster_relations_t(measurement,zaehler,nenner)
{
}

const quantity::substance_amount_t calc_t::sims_t::Crel_t::substance_amount(const cluster_t& cluster) const
{
	if (cluster.isotopes.size()!=1)
		return quantity::substance_amount_t({0},units::derived::atom_percent,quantity::dimensions::SI::relative);
	//ATTENTION asumes matrix_cluster consists just out of 1 isotope
	return substance_amount(cluster.isotopes.front());
}

const quantity::substance_amount_t calc_t::sims_t::Crel_t::substance_amount(const isotope_t& iso) const
{
	auto iso_mat = measurement.sample->matrix().isotope(iso);
	if (iso_mat==nullptr)
		return quantity::substance_amount_t({0},units::derived::atom_percent,quantity::dimensions::SI::relative);
// 	cout << "substance_amount:" << iso_mat->substance_amount.to_string_detailed() << endl;
	return iso_mat->substance_amount;
}

const quantity::concentration_t calc_t::sims_t::Crel_t::from_sample_matrix_elements() const
{
	auto Crel_isotopical = from_sample_matrix_isotopes();
// 	cout << endl << "from_sample_matrix_isotopes: " << Crel_isotopical.to_string_detailed() << endl;
	if (!Crel_isotopical.is_set())
		return {};
	if (Crel_isotopical.is_inf() || Crel_isotopical==0) 
		return Crel_isotopical;
	if (!measurement.sample->matrix().is_set())
		return {};
	if (zaehler==nullptr || nenner == nullptr)
		return {};
	//ATTENTION zaehler could be nullptr
	if (measurement.sample->matrix().isotope(zaehler->isotopes.front())==nullptr)
		return {};
	//ATTENTION nenner could be nullptr
	if (measurement.sample->matrix().isotope(nenner->isotopes.front())==nullptr)
		return {};
	const auto abu_Z = measurement.sample->matrix().isotope(zaehler->isotopes.front())->abundance;
	const auto abu_N = measurement.sample->matrix().isotope(nenner->isotopes.front())->abundance;
	
	if (!abu_Z.is_set() || !abu_N.is_set())
		return {};
	
	return Crel_isotopical*(abu_N/abu_Z);
}

const quantity::concentration_t calc_t::sims_t::Crel_t::from_sample_matrix_isotopes() const
{
	logger::debug(21,"calc_t::sims_t::Crel_t::from_sample_matrix_isotopes()","entering");
	if (zaehler == nullptr || nenner==nullptr)
		return {};
	auto amount_zaehler = substance_amount(*zaehler);
	auto amount_nenner = substance_amount(*nenner);
	
// 	cout << zaehler->to_string() << " amount_zaehler:" << amount_zaehler.to_string_detailed() << endl;
// 	cout << nenner->to_string() << " amount_nenner:" << amount_nenner.to_string_detailed() << endl;
	if (amount_zaehler == amount_nenner && amount_nenner==0)
	{
		logger::info(6,"calc_t::sims_t::Crel_t::from_sample_matrix_isotopes()","zaehler + nenner have 0 substance_amount in matrix",nenner->to_string()+" "+zaehler->to_string(), measurement.to_string_short());
		return {};
	}
	
// 	quantity::concentration_t Crel((amount_zaehler / (amount_nenner )).data(),units::SI::one);
	return amount_zaehler / amount_nenner;
}

/*******************************************************/
/**********************    Irel_t     ******************/
/*******************************************************/

calc_t::sims_t::Irel_t::Irel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner) :
								cluster_relations_t(measurement,zaehler,nenner)
{
// 	zaehler_p;
}

bool calc_t::sims_t::Irel_t::intensities_are_set_in_clusters() const
{
	if (!clusters_exist()) 
		return false;
	if (!nenner->intensity.is_set()) 
		return false;
	if (!zaehler->intensity.is_set()) 
		return false;
	return true;
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_mean_ratios() const
{
	if (!intensities_are_set_in_clusters())
		return {};
	return quantity::intensity_t(zaehler->intensity.mean() / nenner->intensity.mean());
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_median_ratios() const
{
	if (!intensities_are_set_in_clusters())
		return {};
	return quantity::intensity_t(zaehler->intensity.median() / nenner->intensity.median());
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_trimmed_mean_ratios(float trim) const
{
	if (!intensities_are_set_in_clusters())
		return {};
	return quantity::intensity_t(zaehler->intensity.trimmed_mean(trim) / nenner->intensity.trimmed_mean(trim));
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_cut_median_ratios(float cut) const
{
	if (!intensities_are_set_in_clusters())
		return {};
	if (cut>=1 || cut <= 0)
		return {};
	return zaehler->intensity.cut_median(cut) / nenner->intensity.cut_median(cut);
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_cut_mean_ratios(float cut) const
{
	if (!intensities_are_set_in_clusters())
		return {};
	if (cut>=1 || cut <= 0)
		return {};
	return zaehler->intensity.cut_mean(cut) / nenner->intensity.cut_mean(cut);
}

const quantity::intensity_t calc_t::sims_t::Irel_t::from_clusters_pbp() const
{
	if (!intensities_are_set_in_clusters())
		return {};
	return quantity::intensity_t(zaehler->intensity / nenner->intensity);
}

/*******************************************************/
/*****    Crel_to_Irel_data_fit_routine_template   *****/
/*******************************************************/

// calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, 
// 																							   const cluster_t& nenner, 
// 																							   const pair<quantity::concentration_t, quantity::intensity_t> Crel_to_Irel_data) :
// 																							   zaehler(zaehler.isotopes), nenner(nenner.isotopes),Crel_to_Irel_data(Crel_to_Irel_data)
// {
// }

calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, 
																							   const cluster_t& nenner, 
																							   const quantity::map_t& Crel_to_Irel_map) :
// 																							   zaehler_p(zaehler.isotopes), nenner_p(nenner.isotopes),
																							   Crel_to_Irel_map(Crel_to_Irel_map), cluster_relations_copies_t(zaehler,nenner)
{
}

calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::~Crel_to_Irel_data_fit_routine_template()
{
}

// const cluster_t& calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::zaehler() const
// {
// 	return zaehler_p;
// }
// const cluster_t& calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::nenner() const
// {
// 	return nenner_p;
// }
// const quantity::map_t& calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel_to_Irel_map const
// {
// 	return Crel_to_Irel_map_p;
// }

bool calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::operator<(const Crel_to_Irel_data_fit_routine_template& obj) const
{
	if (zaehler().to_string() > obj.zaehler().to_string())
		return false;
	if (zaehler().to_string() > obj.nenner().to_string())
		return false;
	return true;
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel(const measurements_::sims_t& measurement) const
{
	logger::error("calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel(measurement)","you should never read this, tell Florian","return empty","");
	return {};
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel(const quantity::intensity_t Irel) const
{
	logger::error("calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel(Irel)","you should never read this, tell Florian","return empty");
	return {};
}

void calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::plot_to_screen(double sleep_sec) const
{
	logger::error("calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::plot_to_screen()","you should never read this, tell Florian","return");
	return;
}

string calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::to_string() const
{
	stringstream ss;
	ss << "Z:" << zaehler().to_string() << "; N:" << nenner().to_string() << "; Crel_to_Irel_data.size()=" << Crel_to_Irel_map.X().data().size();
	return ss.str();
}

/*******************************************************/
/*****      Crel_to_Irel_data_polynomial_fit_t     *****/
/*******************************************************/
calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel_to_Irel_data_polynomial_fit_t(const cluster_t& zaehler, 
																					   const cluster_t& nenner, 
// 																					   const pair<quantity::concentration_t, quantity::intensity_t> Crel_to_Irel_data, 
																					   const quantity::map_t& Crel_to_Irel_map,
																					   fit_functions::polynom_t polynom) : 
										Crel_to_Irel_data_fit_routine_template(zaehler,nenner,Crel_to_Irel_map), 
										polynom_p(polynom)
{
}

const fit_functions::polynom_t& calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::polynom() const
{
	return polynom_p;
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel(const quantity::intensity_t Irel) const
{
	if (!Irel.is_set())
		return {};
	if (!polynom().successfully_fitted())
		return {};
// 	cout << "Crel_to_Irel_map.polyfit=" << Crel_to_Irel_map.polyfit(Irel,polynom).to_string_detailed() << endl;
	return Crel_to_Irel_map.polyfit(Irel,polynom());
// 	return polynom.y_data(Irel.data());
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel(const measurements_::sims_t& measurement) const
{
	if (!measurement.are_intensities_of_clusters_set({zaehler(),nenner()}))
	{
		logger::warning(3,"calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel()","!measurement.are_intensities_of_clusters_set( "+measurement.to_string_short() +" )","returning empty");
		return {};
	}
	auto Irel = measurement.cluster(zaehler())->intensity / measurement.cluster(nenner())->intensity;
	
	return Crel(Irel);
}

void calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::plot_to_screen(double sleep_sec) const
{
	plot_t plot(false,false);
	stringstream plot_title;

	auto Crel_to_Irel_map_withot_infs = Crel_to_Irel_map.remove_inf();
// 	cout << endl << Crel_to_Irel_map_withot_infs.to_string_list() << endl;
	plot.Y1.add_points(Crel_to_Irel_map_withot_infs,zaehler().to_string() + " / " + nenner().to_string()," ro");
	if (polynom().successfully_fitted())
	{
// 		plot.Y1.add_polynom(Crel_to_Irel_map,polynom(),polynom().to_string()); 
		plot.Y1.add_polynom(Crel_to_Irel_map_withot_infs,polynom(),polynom().to_string()); 
	}
	plot_title << "{" << zaehler().to_string() << "} / {" << nenner().to_string() <<  "}";
	plot.to_screen(plot_title.str(),sleep_sec);
}


/*******************************************************/
/*****          Crel_to_Irel_data_fits_t           *****/
/*******************************************************/

calc_t::sims_t::Crel_to_Irel_data_fits_t::Crel_to_Irel_data_fits_t(const quantity::map_t& Crel_to_Irel_map, const cluster_t& zaehler, const cluster_t& nenner) :
													Crel_to_Irel_map(Crel_to_Irel_map), 
													cluster_relations_copies_t(zaehler,nenner)
// 													zaehler_p(zaehler.isotopes),nenner_p(nenner.isotopes)
{
}


// const quantity::map_t& calc_t::sims_t::Crel_to_Irel_data_fits_t::Crel_to_Irel_map const
// {
// 	return Crel_to_Irel_map_p;
// }

const calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t calc_t::sims_t::Crel_to_Irel_data_fits_t::polynom(const vector<unsigned int> polynom_rank, const vector<double> polynom_start_parameters) const
{
	const auto P = Crel_to_Irel_map.remove_inf().remove_nan().polynom(polynom_rank,polynom_start_parameters);
	Crel_to_Irel_data_polynomial_fit_t crel_irel_polyfit(zaehler(),nenner(),Crel_to_Irel_map,P);
	return crel_irel_polyfit;
}

/*******************************************************/
/*******     Crel_to_Irel_data_collector_t      ********/
/*******************************************************/

calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_to_Irel_data_collector_t(const cluster_t& zaehler, 
																				   const cluster_t& nenner,
																				   const vector<measurements_::sims_t>& measurements) :
																				   zaehler(zaehler), nenner(nenner),measurements(measurements)
{
}

const calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t calc_t::sims_t::Crel_to_Irel_data_collector_t::elemental_Crel()
{
	logger::debug(21,"calc_t::sims_t::Crel_to_Irel_data_collector_t::elemental_Crel()","entering...");
	map<const quantity::concentration_t, const calc_t::sims_t::Irel_t> Crel_data_to_Irels;
	for (const auto& M : measurements)
	{
		calc_t::sims_t::Crel_t Crel(M,zaehler,nenner);
// 		cout << "Crel.from_sample_matrix_elements " << Crel.from_sample_matrix_elements().to_string() << endl;
		if (!Crel.from_sample_matrix_elements().is_set())
			continue;
		calc_t::sims_t::Irel_t Irel(M,zaehler,nenner);
		Crel_data_to_Irels.insert(pair<const quantity::concentration_t, const calc_t::sims_t::Irel_t> (Crel.from_sample_matrix_elements(),Irel));
	}
	return {Crel_data_to_Irels,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t calc_t::sims_t::Crel_to_Irel_data_collector_t::isotopical_Crel()
{
	logger::debug(21,"calc_t::sims_t::Crel_to_Irel_data_collector_t::isotopical_Crel()","entering...");
	map<const quantity::concentration_t, const calc_t::sims_t::Irel_t> Crel_data_to_Irels;
	for (const auto& M : measurements)
	{
		calc_t::sims_t::Crel_t Crel(M,zaehler,nenner);
		if (!Crel.from_sample_matrix_elements().is_set())
			continue;
		calc_t::sims_t::Irel_t Irel(M,zaehler,nenner);
		Crel_data_to_Irels.insert(pair<const quantity::concentration_t, const calc_t::sims_t::Irel_t> (Crel.from_sample_matrix_isotopes(),Irel));
	}
	return {Crel_data_to_Irels,zaehler,nenner};
}


/******************************************************************************/
/*******        Crel_to_Irel_data_collector_t::collect_Irels_t         ********/
/******************************************************************************/

calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::collect_Irels_t(const std::map< const quantity::concentration_t, const calc_t::sims_t::Irel_t>& Crel_data_to_Irels, 
																						const cluster_t& zaehler, 
																						const cluster_t& nenner) :
																						Crel_data_to_Irels(Crel_data_to_Irels), zaehler(zaehler), nenner(nenner)
{
// 	logger::debug(13,"calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::collect_Irels_t()","entering");
}

/*static*/
bool calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::add_Crel_Irel_to_pair(pair<quantity::concentration_t, quantity::intensity_t>& Crel_to_Irel_pair, const quantity::concentration_t& Crel, const quantity::intensity_t& Irel)
{
	if (Crel.is_set() && Irel.is_set())
	{
		Crel_to_Irel_pair.first << Crel;
		Crel_to_Irel_pair.second << Irel;
		return true;
	} 
	return false;
}

/*templates would be nice here ... but I dont know how to give a class/object-function as template argument :(*/
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::Irel_from_mean() const
{
// 	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	quantity::quantity_t X;
	quantity::quantity_t Y;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_mean_ratios();
		auto Crel = C_to_I.first;
		X << Irel;
		Y << Crel;
// 		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {{X,Y},zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::Irel_from_median() const
{
// 	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	quantity::quantity_t X;
	quantity::quantity_t Y;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_median_ratios();
		auto Crel = C_to_I.first;
		X << Irel;
		Y << Crel;
// 		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
// 	return {Crel_to_Irel_data,zaehler,nenner};
	return {{X,Y},zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::Irel_from_cut_mean(float cut) const
{
// 	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	quantity::quantity_t X;
	quantity::quantity_t Y;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_cut_mean_ratios(cut);
		auto Crel = C_to_I.first;
		X << Irel;
		Y << Crel;
// 		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
// 	return {Crel_to_Irel_data,zaehler,nenner};
	return {{X,Y},zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::Irel_from_cut_median(float cut) const
{
// 	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	quantity::quantity_t X;
	quantity::quantity_t Y;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_cut_median_ratios(cut);
		auto Crel = C_to_I.first;
		X << Irel;
		Y << Crel;
// 		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
// 	return {Crel_to_Irel_data,zaehler,nenner};
	return {{X,Y},zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::collect_Irels_t::Irel_from_trimmed_mean(float trim) const
{
// 	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	quantity::quantity_t X;
	quantity::quantity_t Y;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_trimmed_mean_ratios(trim);
		auto Crel = C_to_I.first;
		X << Irel;
		Y << Crel;
// 		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
// 	return {Crel_to_Irel_data,zaehler,nenner};
	return {{X,Y},zaehler,nenner};
}



