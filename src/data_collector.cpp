#include "data_collector.hpp"

data_collectors_t::sims_t::sims_t(const std::vector<measurements_::sims_t*>& Ms_s)  : logger(__FILE__,"data_collectors_t::sims_t")
{
	measurements.reserve(Ms_s.size());
	for (const auto M : Ms_s)
	{
		measurements.push_back(M);
	}
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_concentrations_from_sample_matrix(const isotope_t& iso)
{
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
        const auto mat_concentration = M->sample.matrix().concentration(iso);
		if (mat_concentration.is_set())
		{
			col.add_quantity_entry(M->memory_address(),mat_concentration);
		}
	}
	col.name = "matrix";
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_concentrations_from_sample_matrix(const element_t& ele)
{
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
        const auto mat_concentration = M->sample.matrix().concentration(ele);
		if (mat_concentration.is_set())
		{
			col.add_quantity_entry(M->memory_address(),mat_concentration);
		}
	}
	col.name = "matrix";
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_concentrations_from_cluster(const cluster_t& cluster)
{
	if (!cluster.is_set())
		return {};
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		col.add_quantity_entry(M->memory_address(),M->concentration(cluster));
	}
	col.name = cluster.to_string();
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_sputter_rates()
{
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		if (M->crater.SR.is_set())
		{
			col.add_quantity_entry(M->memory_address(),M->crater.SR);
		}
	}
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_SFs_from_cluster(const cluster_t& cluster)
{	
	if (!cluster.is_set())
		return {};
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		col.add_quantity_entry(M->memory_address(),M->SF(cluster));
	}
	col.name = cluster.to_string();
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_RSFs_from_cluster(const cluster_t& cluster)
{
	if (!cluster.is_set())
		return {};
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		col.add_quantity_entry(M->memory_address(),M->RSF(cluster));
	}
	col.name = cluster.to_string();
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_intensities_from_cluster(const cluster_t& cluster)
{
	if (!cluster.is_set())
		return {};
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		col.add_quantity_entry(M->memory_address(),M->intensity(cluster));
	}
	col.name = cluster.to_string();
	return col;
}

quantity::table_t::column_t data_collectors_t::sims_t::get_column_Q_from_cluster(const cluster_t& cluster, quantity::quantity_t (measurements_::sims_t::*q)(const cluster_t& cluster) const)
{
	if (!cluster.is_set())
		return {};
	quantity::table_t::column_t col;
	for (auto& M : measurements)
	{
		col.add_quantity_entry(M->memory_address(),(M->*q)(cluster));
	}
	col.name = cluster.to_string();
	return col;
}


quantity::table_t data_collectors_t::sims_t::measurement_line_names() const
{
	quantity::table_t T;
	for (auto& M : measurements)
	{
		T.line_names.insert({M->memory_address(),M->to_string_short()});
	}
	return T;
}
