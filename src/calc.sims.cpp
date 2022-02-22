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
/************    cluster_relations_t     ***************/
/*******************************************************/

calc_t::sims_t::cluster_relations_t::cluster_relations_t(const measurements_::sims_t& measurement, 
														 const cluster_t& zaehler, 
														 const cluster_t& nenner) : 
														 measurement(measurement), 
														 zaehler(measurement.cluster(zaehler)), 
														 nenner(measurement.cluster(nenner))
{
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
		return quantity::substance_amount_t({0},units::SI::one);
	return substance_amount(cluster.isotopes.front());
}

const quantity::substance_amount_t calc_t::sims_t::Crel_t::substance_amount(const isotope_t& iso) const
{
	auto iso_mat = measurement.sample->matrix().isotope(iso);
	if (iso_mat==nullptr)
		return quantity::substance_amount_t({0},units::SI::one);
	return iso_mat->substance_amount;
}

const quantity::concentration_t calc_t::sims_t::Crel_t::from_sample_matrix_elements() const
{
	auto Crel_isotopical = from_sample_matrix_isotopes();
	quantity::substance_amount_t substance_amount_zero({0},units::SI::one);
	if (!Crel_isotopical.is_set())
		return {};
	if (Crel_isotopical.is_inf() || Crel_isotopical==substance_amount_zero) 
		return Crel_isotopical;
	const auto abu_Z = measurement.sample->matrix().isotope(zaehler->isotopes.front())->abundance;
	const auto abu_N = measurement.sample->matrix().isotope(nenner->isotopes.front())->abundance;
	
	if (!abu_Z.is_set() || !abu_N.is_set())
		return {};
	
	return Crel_isotopical*(abu_N/abu_Z);
}

const quantity::concentration_t calc_t::sims_t::Crel_t::from_sample_matrix_isotopes() const
{
	auto amount_zaehler = substance_amount(*zaehler);
	auto amount_nenner = substance_amount(*nenner);
	quantity::substance_amount_t substance_amount_zero({0},units::SI::one);
	if (amount_zaehler == amount_nenner && amount_nenner==substance_amount_zero)
	{
		logger::info(6,"calc_t::sims_t::Crel_t::from_sample_matrix_isotopes()","zaehler + nenner have 0 substance_amount in matrix",nenner->to_string()+" "+zaehler->to_string(), measurement.to_string_short());
		return {};
	}
	
	quantity::concentration_t Crel((amount_zaehler / (amount_nenner )).data,units::SI::one);
	return Crel;
}

/*******************************************************/
/**********************    Irel_t     ******************/
/*******************************************************/

calc_t::sims_t::Irel_t::Irel_t(const measurements_::sims_t& measurement, const cluster_t& zaehler, const cluster_t& nenner) :
								cluster_relations_t(measurement,zaehler,nenner)
{
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


/*******************************************************/
/*****    Crel_to_Irel_data_fit_routine_template   *****/
/*******************************************************/

calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::Crel_to_Irel_data_fit_routine_template(const cluster_t& zaehler, 
																							   const cluster_t& nenner, 
																							   const pair<quantity::concentration_t, quantity::intensity_t> Crel_to_Irel_data) :
																							   zaehler(zaehler.isotopes), nenner(nenner.isotopes),Crel_to_Irel_data(Crel_to_Irel_data)
{
}

calc_t::sims_t::Crel_to_Irel_data_fit_routine_template::~Crel_to_Irel_data_fit_routine_template()
{
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

/*******************************************************/
/*****      Crel_to_Irel_data_polynomial_fit_t     *****/
/*******************************************************/
calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel_to_Irel_data_polynomial_fit_t(const cluster_t& zaehler, 
																					   const cluster_t& nenner, 
																					   const pair<quantity::concentration_t, 
																					   quantity::intensity_t> Crel_to_Irel_data, 
																					   fit_functions::polynom_t polynom) : 
										Crel_to_Irel_data_fit_routine_template(zaehler,nenner,Crel_to_Irel_data), 
										polynom(polynom)
{
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel(const quantity::intensity_t Irel) const
{
	if (!Irel.is_set())
		return {};
	if (!polynom.successfully_fitted())
		return {};
	return quantity::concentration_t(polynom.y_data(Irel.data),units::SI::one);
}

const quantity::concentration_t calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel(const measurements_::sims_t& measurement) const
{
	if (!measurement.are_intensities_of_clusters_set({zaehler,nenner}))
	{
		logger::warning(3,"calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::Crel()","!measurement.are_intensities_of_clusters_set( "+measurement.to_string_short() +" )","returning empty");
		return {};
	}
	quantity::intensity_t Irel = measurement.cluster(zaehler)->intensity / measurement.cluster(nenner)->intensity;
	if (!Irel.is_set())
		return {};
	return Crel(Irel);
}

void calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t::plot_to_screen(double sleep_sec) const
{
	plot_t plot(false,false);
	stringstream plot_title;
// 	if (!Crel_to_Irel_data.first.is_set() || Crel_to_Irel_data.second.is_set())
// 		return;
	quantity::map_t XY(Crel_to_Irel_data.second,Crel_to_Irel_data.first);
	auto XYr = XY.remove_inf();
	cout << "XYr.data_map().size()=" << XYr.data_map().size() << endl; 
	cout << XYr.X.to_string_detailed() << endl;
	cout << XYr.Y.to_string_detailed() << endl;
	
	plot.Y1.add_points(XYr,"data"," ro");
	if (polynom.successfully_fitted())
	{
// 		cout << "IN" << endl;
		///create some points on the curve to be plotted
		auto Irel_fit = Crel_to_Irel_data.second.change_resolution(Crel_to_Irel_data.first.data.size()*50);
		quantity::concentration_t Crel_fit(polynom.y_data(Irel_fit.data),Crel_to_Irel_data.first.unit());
		plot.Y1.add_curve(Irel_fit,Crel_fit,polynom.to_string());
	}
	plot_title << "{" << zaehler.to_string() << "} / {" << nenner.to_string() <<  "}";
	plot.to_screen(plot_title.str(),sleep_sec);
}


/*******************************************************/
/*****          Crel_to_Irel_data_fits_t           *****/
/*******************************************************/

calc_t::sims_t::Crel_to_Irel_data_fits_t::Crel_to_Irel_data_fits_t(const pair<quantity::concentration_t, quantity::intensity_t> Crel_to_Irel_data, const cluster_t& zaehler, const cluster_t& nenner) :
													Crel_to_Irel_data(Crel_to_Irel_data), zaehler(zaehler),nenner(nenner)
{
}

const calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t calc_t::sims_t::Crel_to_Irel_data_fits_t::polynom(const vector<unsigned int> polynom_rank, const vector<double> polynom_start_parameters) const
{
	const auto data = tools::vec::combine_vecs_to_map(Crel_to_Irel_data.second.data, Crel_to_Irel_data.first.data);
	fit_functions::polynom_t poly(polynom_rank,polynom_start_parameters,data);
	Crel_to_Irel_data_polynomial_fit_t crel_irel_polyfit(zaehler,nenner,Crel_to_Irel_data,poly);
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

const calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t calc_t::sims_t::Crel_to_Irel_data_collector_t::elemental_Crel()
{
	map<const quantity::concentration_t, const calc_t::sims_t::Irel_t> Crel_data_to_Irels;
	for (const auto& M : measurements)
	{
		calc_t::sims_t::Crel_t Crel(M,zaehler,nenner);
		if (!Crel.from_sample_matrix_elements().is_set())
			continue;
		calc_t::sims_t::Irel_t Irel(M,zaehler,nenner);
		Crel_data_to_Irels.insert(pair<const quantity::concentration_t, const calc_t::sims_t::Irel_t> (Crel.from_sample_matrix_elements(),Irel));
	}
	return {Crel_data_to_Irels,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t calc_t::sims_t::Crel_to_Irel_data_collector_t::isotopical_Crel()
{
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
/*******     Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t      ********/
/******************************************************************************/

calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Crel_data_to_Irel_t(const std::map< const quantity::concentration_t, const calc_t::sims_t::Irel_t>& Crel_data_to_Irels, 
																						const cluster_t& zaehler, 
																						const cluster_t& nenner) :
																						Crel_data_to_Irels(Crel_data_to_Irels), zaehler(zaehler), nenner(nenner)
{
}

/*static*/
bool calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::add_Crel_Irel_to_pair(pair<quantity::concentration_t, quantity::intensity_t>& Crel_to_Irel_pair, const quantity::concentration_t& Crel, const quantity::intensity_t& Irel)
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
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Irel_from_mean() const
{
	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_mean_ratios();
		auto Crel = C_to_I.first;
		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {Crel_to_Irel_data,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Irel_from_median() const
{
	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_median_ratios();
		auto Crel = C_to_I.first;
		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {Crel_to_Irel_data,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Irel_from_cut_mean(float cut) const
{
	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_cut_mean_ratios(cut);
		auto Crel = C_to_I.first;
		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {Crel_to_Irel_data,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Irel_from_cut_median(float cut) const
{
	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_cut_median_ratios(cut);
		auto Crel = C_to_I.first;
		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {Crel_to_Irel_data,zaehler,nenner};
}
const calc_t::sims_t::Crel_to_Irel_data_fits_t calc_t::sims_t::Crel_to_Irel_data_collector_t::Crel_data_to_Irel_t::Irel_from_trimmed_mean(float trim) const
{
	pair<quantity::concentration_t,quantity::intensity_t> Crel_to_Irel_data;
	for (const auto& C_to_I : Crel_data_to_Irels)
	{
		auto Irel = C_to_I.second.from_clusters_trimmed_mean_ratios(trim);
		auto Crel = C_to_I.first;
		add_Crel_Irel_to_pair(Crel_to_Irel_data,Crel,Irel);
	}
	return {Crel_to_Irel_data,zaehler,nenner};
}


