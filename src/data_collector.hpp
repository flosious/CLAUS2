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

#pragma once

#ifndef DATA_COLLECTOR_T_HPP
#define DATA_COLLECTOR_T_HPP

#include "quantity.hpp"
#include "measurement.hpp"

class data_collectors_t
{
public:
	class sims_t
	{
	private:
        class_logger_t logger;
		///Ms are unique, so it is effectively a set
		///sort data using measurement as reference column to sort lines   	/*************************/
		vector<const measurements_::sims_t*> measurements;							// M1* // Q1 // Q2 // .. //
	public:
// 		cluster_t::RSF_t& RSFs(const measurements_::sims_t* M, ) const;
		sims_t(const vector<measurements_::sims_t*>& Ms_s);
		///scalars
		quantity::table_t::column_t get_column_concentrations_from_sample_matrix(const isotope_t& iso);
		quantity::table_t::column_t get_column_concentrations_from_sample_matrix(const element_t& ele);
		///probably vectors
		quantity::table_t::column_t get_column_concentrations_from_cluster(const cluster_t& cluster);
		///vectors or scalars
		quantity::table_t::column_t get_column_sputter_rates();
		///vectors or scalars
		quantity::table_t::column_t get_column_SFs_from_cluster(const cluster_t& cluster);
		///vectors or scalars
		quantity::table_t::column_t get_column_RSFs_from_cluster(const cluster_t& cluster);
		///vectors
		quantity::table_t::column_t get_column_intensities_from_cluster(const cluster_t& cluster);
		///not useable
		quantity::table_t::column_t get_column_Q_from_cluster(const cluster_t& cluster, quantity::quantity_t (measurements_::sims_t::*q)(const cluster_t& cluster) const);
		///id->measurement.to_string()
		quantity::table_t measurement_line_names() const;
	};
};
extern Logger global_logger;
#endif // DATA_COLLECTOR_T_HPP
