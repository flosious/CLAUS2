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

calc_t::sims_t::matrix_t::matrix_t(const vector<isotope_t>& matrix_isotopes, 
								   const vector<measurements_::sims_t>& measurements) : 
								   matrix_isotopes(matrix_isotopes),measurements(measurements),matrix_clusters(matrix_clusters_p()), RSFs(RSFs_elemental_median())
{
// 	for (auto& M:measurements)
// 		for (auto& i : M.sample->matrix().isotopes())
// 			cout << i.substance_amount.to_string_detailed() << endl;
}

const set<cluster_t> calc_t::sims_t::matrix_t::matrix_clusters_p() const
{
	set<cluster_t> matrix_clusters;
	set<cluster_t> clusters;

	for (const auto& M : measurements)
	{
		for (const auto& C : M.clusters)
		{
			clusters.insert(C);
		}
	}

	for (const auto& C : clusters)
	{
		if (C.isotopes.size()!=1) 
			continue;
		if (std::find(matrix_isotopes.begin(),matrix_isotopes.end(),C.isotopes.front())==matrix_isotopes.end())
			continue;
		matrix_clusters.insert(C);
	}

	return matrix_clusters;
}

const vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> calc_t::sims_t::matrix_t::RSFs_elemental_median() const
{
	logger::debug(13,"calc_t::sims_t::matrix_t::RSFs_elemental_median()","entering");
	vector<calc_t::sims_t::Crel_to_Irel_data_polynomial_fit_t> Crels_to_Irels;
	for (const auto& Z : matrix_clusters)
	{
		for (const auto& N : matrix_clusters)
		{
			if (Z==N)
				continue;
			calc_t::sims_t::Crel_to_Irel_data_collector_t Crel_to_Irel_data_collector(Z,N,measurements);
// 			cout << Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().Crel_to_Irel_data.first.to_string_detailed() << endl;
			Crels_to_Irels.push_back(Crel_to_Irel_data_collector.elemental_Crel().Irel_from_median().polynom({0,1},{0,1}));
		}
	}
	logger::debug(13,"calc_t::sims_t::matrix_t::RSFs_elemental_median()","leaving");
	return Crels_to_Irels;
}

quantity::concentration_t calc_t::sims_t::matrix_t::relative_concentration(const cluster_t& cluster, 
																		   const measurements_::sims_t& measurement)
{	
	quantity::quantity_t nenner;
	
	for (const Crel_to_Irel_data_polynomial_fit_t& RSF : RSFs)
	{
		if (cluster!=RSF.nenner)
			continue;
		if (!RSF.Crel(measurement).is_set())
			return {};
		if (!nenner.is_set())
			nenner = RSF.Crel(measurement);
		else
			nenner += RSF.Crel(measurement);
	}
	nenner += 1;
	
	return quantity::concentration_t((nenner.invert()*100).data(),units::derived::atom_percent);
}

measurements_::sims_t calc_t::sims_t::matrix_t::relative_concentrations(const vector<cluster_t>& clusters, measurements_::sims_t measurement)
{
	for (const auto& c : clusters)
	{
		if (measurement.cluster(c)==nullptr)
			continue;
		if (!relative_concentration(c,measurement).is_set())
			continue;
		measurement.cluster(c)->concentration = relative_concentration(c,measurement);
	}
	return measurement;
}

measurements_::sims_t calc_t::sims_t::matrix_t::relative_concentrations(measurements_::sims_t measurement)
{
	return relative_concentrations(vector<cluster_t>(matrix_clusters.begin(),matrix_clusters.end()),measurement);
}
