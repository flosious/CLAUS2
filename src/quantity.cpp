#include "quantity.hpp"



/*******************************/
quantity_t::quantity_t(std::__cxx11::string name_s, vector<double> data_s, unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
	data_p = data_s;
}

quantity_t::quantity_t(string name_s,unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
}

quantity_t::quantity_t(vector<double> data_s,unit_t unit_s)
{
	data_p = data_s;
	unit_p = unit_s;
}

quantity_t::quantity_t()
{

}




const vector<double> quantity_t::data() const
{
	if (resolution_p<=0) return data_p;

// 	const double min = statistics::get_min_from_Y(data_p);
// 	const double max = statistics::get_max_from_Y(data_p);
// 	int size = floor(abs(max-min)/resolution_p)+1;
	set<double> sort_data(data_p.begin(),data_p.end());
	cout << *sort_data.begin() << "\t" << *sort_data.rbegin() <<endl;
	cout << *data_p.begin() << "\t" << *data_p.rbegin() <<endl;
	int size = floor(abs(*sort_data.begin()-*sort_data.rbegin())/resolution_p)+1;
	vector<double> new_data(size);
	for (int i=0;i<size;i++)
		new_data[i] =i*resolution_p + *sort_data.begin();
// 		new_data[i] =i*resolution_p + min;
	return new_data;
}


void quantity_t::change_resolution(double resolution_s)
{
	resolution_p = resolution_s;
}

void quantity_t::reset_resolution()
{
	resolution_p = -1;
}

const quantity_t quantity_t::resolution() const
{
	if (resolution_p > 0) return {name()+"_resolution", {resolution_p},unit()};
	
	if (data_p.size()<2) return {};
	double res_d = abs((*data_p.rbegin()-*data_p.begin())/(data_p.size()-1));
	quantity_t res(name()+"_resolution",{res_d},unit());
	return res;
}

const std::__cxx11::string quantity_t::to_string() const
{
	stringstream out;
	if (data_p.size()!=1)
		out <<  name() << " = " <<"<" << data_p.size() << ">" << " [" << unit().name() << "]";
	else
		out <<  name() << " = " << data_p[0] << " [" << unit().name() << "]";
	return out.str();
}

quantity_t quantity_t::mean() const
{
	quantity_t mean_p{"mean("+name()+")",{statistics::get_mean_from_Y(data_p)},unit()};
	return mean_p;
}

quantity_t quantity_t::median() const
{
	quantity_t median_p{"median("+name()+")",{statistics::get_median_from_Y(data_p)},unit()};
	return median_p;
}

bool quantity_t::is_set() const
{
	if (data_p.size()==0) return false;
	return true;
}

const std::__cxx11::string quantity_t::name() const
{
	return name_p;
}

unit_t quantity_t::unit() const
{
	return unit_p;
}


const quantity_t quantity_t::change_unit(unit_t target_unit) const
{	
	quantity_t copy = *this;
	double factor = copy.unit().change_unit(target_unit);
	for (int i=0;i<data_p.size();i++)
		copy.data_p[i] *= factor ;
	return copy;
}


bool quantity_t::operator==(const quantity_t& obj) const
{
	if (unit()!=obj.unit()) return false;
	if (data_p.size()!=obj.data_p.size())	 return false;
	for (int i=0;i<data_p.size();i++)
		if (data_p[i]!=obj.data_p[i]) return false;
	return true;
}

bool quantity_t::operator!=(const quantity_t& obj) const
{
	return !(operator==(obj));
}

void quantity_t::operator<<(const quantity_t& obj)
{
	if (!obj.is_set()) return;
	if (!is_set())
	{
		*this = obj;
		return;
	}
	quantity_t adder = obj.change_unit(unit());
	tools::vec::add(&data_p,adder.data_p);
}

quantity_t quantity_t::operator+(const quantity_t& quantity) const
{
	if (name()!=quantity.name()) return {}; // only allow addition within the same Größenart
	if (!is_set() || !quantity.is_set()) return {};
	if (unit() != quantity.unit()) return {};
	quantity_t sum=*this;
	if (data_p.size() == quantity.data_p.size())
	{
		sum.data_p.resize(data_p.size());
		for (int i=0; i< data_p.size()&&i<quantity.data_p.size();i++)
		{
			sum.data_p[i] = data_p[i] + quantity.data_p[i];
		}
	} 
	else if (data_p.size()==1)
	{
		sum.data_p.resize(quantity.data_p.size());
		for (int i=0; i< sum.data_p.size();i++)
		{
			sum.data_p[i] = data_p[0] + quantity.data_p[i];
		}
	}
	else if (quantity.data_p.size()==1)
	{
		sum.data_p.resize(data_p.size());
		for (int i=0; i< sum.data_p.size();i++)
		{
			sum.data_p[i] = quantity.data_p[0] + data_p[i];
		}
	}
	return sum;
}

quantity_t quantity_t::operator+(const double summand) const
{
	if (!is_set()) return {};
	quantity_t sum=*this;

	for (int i=0; i< data_p.size()&&i<data_p.size();i++)
	{
		sum.data_p[i] += summand;
	}
	return sum;
}

quantity_t quantity_t::operator-(const quantity_t& quantity) const
{
	return {*this + quantity*(-1)};
}
quantity_t quantity_t::operator-(const double summand) const
{
	return {*this + summand*(-1)};
}

quantity_t quantity_t::operator * (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};

	quantity_t produkt{name() + "*" + quantity_p.name(),{unit()*quantity_p.unit()}};
	
	if (data_p.size()==1)
	{
		produkt.data_p=quantity_p.data_p;
		for (auto& d:produkt.data_p)
			d*=data_p[0];
	}
	else if (quantity_p.data_p.size()==1)
	{
		produkt.data_p=data_p;
		for (auto& d:produkt.data_p)
			d*=quantity_p.data_p[0];
	}
	else if (quantity_p.data_p.size()==data_p.size())
	{
		produkt.data_p=data_p;
		for (int i=0;i<data_p.size();i++)
			produkt.data_p[i] *= quantity_p.data_p[i];
	}
	else return {};
	return produkt;
}

quantity_t quantity_t::operator / (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};
	quantity_t quotient{name() + " / (" + quantity_p.name() +")",{unit() / quantity_p.unit()}};
	
	if (quantity_p.data_p.size()==1)
	{
		quotient.data_p.resize(data_p.size());
		for (int i=0;i<data_p.size();i++)
			quotient.data_p[i]=data_p[i]/quantity_p.data_p[0];
	}
	else if (quantity_p.data_p.size()==data_p.size())
	{
		quotient.data_p.resize(data_p.size());
		for (int i=0;i<data_p.size();i++)
			quotient.data_p[i]=data_p[i]/quantity_p.data_p[i];
	}
	else if (data_p.size()==1)
	{
		quotient.data_p.resize(quantity_p.data_p.size());
		for (int i=0;i<quotient.data_p.size();i++)
			quotient.data_p[i]=data_p[0]/quantity_p.data_p[i];
	}
	else
	{
		cout << ">>>ERROR<<<: " <<  quotient.name() << endl;
		cout << name() << ".data_p.size()= " << data_p.size() << " divide by " << quantity_p.name() << ".data_p.size()= " << quantity_p.data_p.size() << endl;
		return {};
	}
	return quotient;
}

quantity_t quantity_t::operator/ (const double divisor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data_p.size();i++)
	{
		quotient.data_p[i]=data_p[i]/divisor;
	}

	return quotient;
}

quantity_t quantity_t::operator * (const double factor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data_p.size();i++)
	{
		quotient.data_p[i] *= factor;
	}

	return quotient;
}

void quantity_t::operator+=(const double summand)
{
	*this = *this + summand;
}

void quantity_t::operator+=(const quantity_t& quantity_p)
{
	*this = *this + quantity_p;
}


/******************************************/
/*STATICS*/
// quantity_t quantity_t::sputter_energy()
// {
// 	return quantity_t{"sputter_energy",{"eV"}};
// }
// 
// quantity_t quantity_t::analysis_energy()
// {
// 	return quantity_t{"analysis_energy",{"keV"}};
// }
// 
// quantity_t quantity_t::analysis_rastersize()
// {
// 	return quantity_t{"analysis_rastersize",{"um"}};
// }
// 
// quantity_t quantity_t::concentration()
// {
// 	return quantity_t{"concentration",{"at/cm^3"}};
// }
// 
// quantity_t quantity_t::dose()
// {
// 	return quantity_t{"dose",{"at/cm^2"}};
// }



/*********************/

mass_t::mass_t(vector<double> data_s,unit_t unit_s) : quantity_t("mass",data_s,unit_s){}
mass_t::mass_t(quantity_t quantity_s) : mass_t(quantity_s.data(),quantity_s.unit()) {}

abundance_t::abundance_t(vector<double> data_s,unit_t unit_s) : quantity_t("abundance",data_s,unit_s){}
abundance_t::abundance_t(quantity_t quantity_s) : abundance_t(quantity_s.data(),quantity_s.unit()) {}

energy_t::energy_t(vector<double> data_s,unit_t unit_s) : quantity_t("energy",data_s,unit_s){}
energy_t::energy_t(quantity_t quantity_s) : energy_t(quantity_s.data(),quantity_s.unit()) {}

rastersize_t::rastersize_t(vector<double> data_s,unit_t unit_s) : quantity_t( "rastersize",data_s,unit_s){}
rastersize_t::rastersize_t(quantity_t quantity_s) : rastersize_t(quantity_s.data(),quantity_s.unit()) {}

depth_t::depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("depth",data_s,unit_s){}
depth_t::depth_t(quantity_t quantity_s) : depth_t(quantity_s.data(),quantity_s.unit()) {}

sputter_depth_t::sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_depth",data_s,unit_s){}
sputter_depth_t::sputter_depth_t(quantity_t quantity_s) : sputter_depth_t(quantity_s.data(),quantity_s.unit()) {}

total_sputter_depth_t::total_sputter_depth_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_depth",data_s,unit_s){}
total_sputter_depth_t::total_sputter_depth_t(quantity_t quantity_s) : total_sputter_depth_t(quantity_s.data(),quantity_s.unit()) {}

sputter_time_t::sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_time",data_s,unit_s){}
sputter_time_t::sputter_time_t(quantity_t quantity_s) : sputter_time_t(quantity_s.data(),quantity_s.unit()) {}

total_sputter_time_t::total_sputter_time_t(vector<double> data_s,unit_t unit_s) : quantity_t("total_sputter_time",data_s,unit_s){}
total_sputter_time_t::total_sputter_time_t(quantity_t quantity_s) : total_sputter_time_t(quantity_s.data(),quantity_s.unit()) {}

// analysis_energy_t::analysis_energy_t(vector<double> data_s,unit_t unit_s) : quantity_t("analysis_energy",data_s,unit_s){}
// analysis_energy_t::analysis_energy_t(quantity_t quantity_s) : analysis_energy_t(quantity_s.data(),quantity_s.unit()) {}
// 
// analysis_rastersize_t::analysis_rastersize_t(vector<double> data_s,unit_t unit_s) : quantity_t("analysis_rastersize",data_s,unit_s){}
// analysis_rastersize_t::analysis_rastersize_t(quantity_t quantity_s) : analysis_rastersize_t(quantity_s.data(),quantity_s.unit()) {}

intensity_t::intensity_t(vector<double> data_s,unit_t unit_s) : quantity_t("intensity",data_s,unit_s){}
intensity_t::intensity_t(quantity_t quantity_s) : intensity_t(quantity_s.data(),quantity_s.unit()) {}

current_t::current_t(vector<double> data_s,unit_t unit_s) : quantity_t("current",data_s,unit_s){}
current_t::current_t(quantity_t quantity_s) : current_t(quantity_s.data(),quantity_s.unit()) {}

sputter_current_t::sputter_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("primary_sputter_current",data_s,unit_s){}
sputter_current_t::sputter_current_t(quantity_t quantity_s) : quantity_t(quantity_s.data(),quantity_s.unit()) {}

analysis_current_t::analysis_current_t(vector<double> data_s,unit_t unit_s) : quantity_t("primary_analysis_current",data_s,unit_s){}
analysis_current_t::analysis_current_t(quantity_t quantity_s) : quantity_t(quantity_s.data(),quantity_s.unit()) {}

concentration_t::concentration_t(vector<double> data_s,unit_t unit_s) : quantity_t("concentration",data_s,unit_s){}
concentration_t::concentration_t(quantity_t quantity_s) : concentration_t(quantity_s.data(),quantity_s.unit()) {}

dose_t::dose_t(vector<double> data_s,unit_t unit_s) : quantity_t("dose",data_s,unit_s){}
dose_t::dose_t(quantity_t quantity_s) : dose_t(quantity_s.data(),quantity_s.unit()) {}

SF_t::SF_t(vector<double> data_s,unit_t unit_s) : quantity_t("sensitivity_factor",data_s,unit_s){}
SF_t::SF_t(quantity_t quantity_s) : SF_t(quantity_s.data(),quantity_s.unit()) {}

RSF_t::RSF_t(vector<double> data_s,unit_t unit_s) : quantity_t("relative_sensitivity_factor",data_s,unit_s){}
RSF_t::RSF_t(quantity_t quantity_s) : RSF_t(quantity_s.data(),quantity_s.unit()) {}

SR_t::SR_t(vector<double> data_s,unit_t unit_s) : quantity_t("sputter_rate",data_s,unit_s){}
SR_t::SR_t(quantity_t quantity_s) : SR_t(quantity_s.data(),quantity_s.unit()) {}

secondary_voltage_t::secondary_voltage_t(vector<double> data_s, unit_t unit_s) : quantity_t("secondary_voltage",data_s,unit_s) {}
secondary_voltage_t::secondary_voltage_t(quantity_t quantity_s) :secondary_voltage_t(quantity_s.data(),quantity_s.unit()) {}

substance_amount_t::substance_amount_t(vector<double> data_s, unit_t unit_s) : quantity_t("substance_amount",data_s,unit_s) {}
substance_amount_t::substance_amount_t(quantity_t quantity_s) :substance_amount_t(quantity_s.data(),quantity_s.unit()) {}

electrical_charge_t::electrical_charge_t(vector<double> data_s, unit_t unit_s) : quantity_t("electrical_charge",data_s,unit_s) {}
electrical_charge_t::electrical_charge_t(quantity_t quantity_s) :electrical_charge_t(quantity_s.data(),quantity_s.unit()) {}




