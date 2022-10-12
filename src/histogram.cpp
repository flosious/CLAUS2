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

#include "histogram.hpp"




/********************************************/
/******* 	histogram_builder_t	    *********/
/********************************************/

// histogram_builder_t::histogram_builder_t(const quantity::quantity_t& start, const quantity::quantity_t& stop) :
// 		start(start),stop(stop)
// {
// }

histogram_builder_t::histogram_builder_t(quantity::quantity_t qs) :
        /*start(qs.min()),stop(qs.max()),*/ qs(qs.sort()), logger(global_logger,__FILE__,"histogram_builder_t")
{
}

histogram_t histogram_builder_t::equally_linearly_distanced_bins(unsigned int number_of_bins) const
{
	std::vector<histogram_t::bin_t> bins;
	bins.reserve(number_of_bins);
	const auto start = qs.min();
	const auto stop = qs.max();
	/*calc the range of each bin*/
	const auto delta = (stop - start) / number_of_bins;
	for (int i=0;i<number_of_bins;i++)
	{
		histogram_t::bin_t bin(start + delta*i, start + delta*(i+1));
		bins.push_back(bin);
	}
	
	histogram_t histogram(bins);
	histogram.add(qs);
	return histogram;
}

histogram_t histogram_builder_t::equally_log10_distanced_bins(unsigned int number_of_bins) const
{
	std::vector<histogram_t::bin_t> bins;
	bins.reserve(number_of_bins);
	const auto QS = qs.remove_data_smaller_than(0).remove_data_equal_to(0);
	const auto start = QS.min();
	const auto stop = QS.max();
	auto delta = ((stop.log10()-start.log10())/number_of_bins).data().front();
	for (int i=0;i<number_of_bins;i++)
	{
		histogram_t::bin_t bin(start*pow(10,delta*i), start*pow(10,delta*(i+1)));
		bins.push_back(bin);
	}
	histogram_t histogram(bins);
	histogram.add(QS);
	return histogram;
}

histogram_t histogram_builder_t::equally_summand_distanced_bins(quantity::quantity_t q_summand) const
{
	q_summand = q_summand.change_unit(qs.unit());
	if (!q_summand.is_set())
	{
        //logger::error("histogram_builder_t::equally_summand_distanced_bins","units not changeable", q_summand.to_string(), qs.to_string());
		return histogram_t{{}};
	}
	std::vector<histogram_t::bin_t> bins;
	/*calcualte the number_of_bins to cover the whole min-max-range*/
	unsigned int number_of_bins = ceil(((qs.max()-qs.min())/q_summand).data().front());
	bins.reserve(number_of_bins);
	const auto start = qs.min();
	for (int i=0;i<number_of_bins;i++)
	{
		histogram_t::bin_t bin(start + q_summand*i, start + q_summand*(i+1));
		bins.push_back(bin);
	}
	
	histogram_t histogram(bins);
	histogram.add(qs);
	return histogram;
}

histogram_t histogram_builder_t::equally_factor_distanced_bins(quantity::quantity_t q_factor) const
{
	q_factor = q_factor.change_unit(qs.unit());
	if (!q_factor.is_set())
	{
        //logger::error("histogram_builder_t::equally_factor_distanced_bins","units not changeable", q_factor.to_string(), qs.to_string());
		return histogram_t{{}};
	}
	std::vector<histogram_t::bin_t> bins;
	/*calcualte the number_of_bins to cover the whole min-max-range*/
	double delta = qs.max().data().front()-qs.min().data().front();
	unsigned int number_of_bins = ceil(pow(delta,1/q_factor.data().front()));
	bins.reserve(number_of_bins);
	const auto start = qs.min();
	for (int i=0;i<number_of_bins;i++)
	{
		histogram_t::bin_t bin(start * pow(q_factor.data().front(),i), start * pow(q_factor.data().front(),i+1));
		bins.push_back(bin);
	}
	
	histogram_t histogram(bins);
	histogram.add(qs);
	return histogram;
}

/***********************************************************/
/***      			 histogram_t   			           *****/
/***********************************************************/

histogram_t::histogram_t(const std::vector<bin_t>& bins) :bins_p(bins), logger(global_logger,__FILE__,"histogram_t")
{
}

unsigned int histogram_t::quantities_in_bins() const
{
	unsigned int c = 0;
	for (const auto& bin : bins())
		c += bin.quantities_in_bin().data().size();
	return c;
}

histogram_t& histogram_t::add(const quantity::quantity_t& q)
{
	if (bins().size()==0)
	{
		return *this;
	}
	auto Q = q.change_unit(bins().front().start().unit());
	if (!Q.is_set())
	{
		return *this;
	}
	Q.sort();
	auto start_bin = bins_p.begin();
	for (auto& d : Q.data()) // this asumes, that smalles bin[0] = q[start,stop) is at the beginning
	{
		for (auto bin = start_bin;bin != bins_p.end();++bin)
		{
			if ((*bin) << d) //successfully added
				break;
			else
				start_bin = bin;
		}
	}
	/*this is more inefficient*/
// 	for (auto& bin : bins_p)
// 	{
// 		bin << q;
// 	}
	return *this;
}

histogram_t& histogram_t::operator<<(const quantity::quantity_t& q)
{
	return add(q);
}

const std::vector<histogram_t::bin_t> histogram_t::bins() const
{
	return bins_p;
}

std::string histogram_t::to_string() const
{
	std::stringstream out;
	for (int i=0; i<bins().size();i++)
	{
		out << "[" << i << "]\t" << "[" << bins_p.at(i).start().data().front() << "," << bins_p.at(i).stop().data().front() << ")\t\t"  << bins_p.at(i).quantities_in_bin().data().size() << std::endl;
	}
	return out.str();
}

const histogram_builder_t histogram_t::rebuild() const
{
	quantity::quantity_t qs;
	for (const auto& bin : bins_p)
		qs << bin.quantities_in_bin();
	return histogram_builder_t(qs);
}

const histogram_builder_t histogram_t::rebuild(const std::vector<bin_t>& bins) const
{
	quantity::quantity_t qs;
	for (const auto& bin : bins)
		qs << bin.quantities_in_bin();
	return histogram_builder_t(qs);
}

const histogram_builder_t histogram_t::rebuild(const bin_t& bin) const
{
	return histogram_builder_t(bin.quantities_in_bin());
}

const std::map<unsigned int, histogram_t::bin_t> histogram_t::quantity_data_size_to_its_bin() const
{
	std::map<unsigned int, histogram_t::bin_t> result;
	for (const auto& bin : bins())
	{
		result.insert({bin.quantities_in_bin().data().size(),bin});
	}
	return result;
}

/***********************************************************/
/*****  		histogram_t::bin_t      			 *******/
/***********************************************************/

histogram_t::bin_t::bin_t(const quantity::quantity_t& start, const quantity::quantity_t& stop)
    : start_p(start), stop_p(stop),
      logger(global_logger,__FILE__,"histogram_t::bin_t")
{
}

const  quantity::quantity_t& histogram_t::bin_t::quantities_in_bin() const
{
	return quantities_in_bin_p;
}

const quantity::quantity_t& histogram_t::bin_t::start() const
{
	return start_p;
}
const quantity::quantity_t& histogram_t::bin_t::stop() const
{
	return stop_p;
}

bool histogram_t::bin_t::operator<<(const double d)
{
	if (d >= start().data().front() && d < stop().data().front())
	{
		quantities_in_bin_p << quantity::quantity_t(start(),d);
		return true;
	}
	return false;
}

int histogram_t::bin_t::operator<<(const quantity::quantity_t& q)
{
	int out=0;
	const auto Q = q.change_unit(start().unit());
	if (!Q.is_set())
		return 0;
	
	for (auto& d : Q.data())
	{
		if (*this << d)
			out++;
	}
	return out;
}
