#include "quantity.hpp"



/*******************************/

bool quantity_t::is_set() const
{
	if (data.size()==0 || unit().name()=="") return false;
	return true;
}


std::__cxx11::string quantity_t::name() const
{
	return name_p;
}

quantity_t::quantity_t(string name_s,unit_t unit_s)
{
	name_p = name_s;
	unit_p = unit_s;
}

unit_t quantity_t::unit() const
{
	return unit_p;
}


quantity_t quantity_t::change_unit(unit_t target_unit) const
{	
	return quantity_t{name(),target_unit};
}


bool quantity_t::operator==(const quantity_t& obj) const
{
	if (unit()!=obj.unit()) return false;
	if (data.size()!=obj.data.size())	 return false;
	for (int i=0;i<data.size();i++)
		if (data[i]!=obj.data[i]) return false;
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
	tools::vec::add(&data,adder.data);
}

quantity_t quantity_t::operator+(const quantity_t& quantity) const
{
	if (name()!=quantity.name()) return {}; // only allow addition within the same Größenart
	if (!is_set() || !quantity.is_set()) return {};
	if (unit() != quantity.unit()) return {};
	quantity_t sum=*this;
	if (data.size() == quantity.data.size())
	{
		sum.data.resize(data.size());
		for (int i=0; i< data.size()&&i<quantity.data.size();i++)
		{
			sum.data[i] = data[i] + quantity.data[i];
		}
	} 
	else if (data.size()==1)
	{
		sum.data.resize(quantity.data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = data[0] + quantity.data[i];
		}
	}
	else if (quantity.data.size()==1)
	{
		sum.data.resize(data.size());
		for (int i=0; i< sum.data.size();i++)
		{
			sum.data[i] = quantity.data[0] + data[i];
		}
	}
	return sum;
}

quantity_t quantity_t::operator+(const double summand) const
{
	if (!is_set()) return {};
	quantity_t sum=*this;

	for (int i=0; i< data.size()&&i<data.size();i++)
	{
		sum.data[i] += summand;
	}
	return sum;
}

quantity_t quantity_t::operator-(const quantity_t& quantity) const
{
	return {*this + quantity*(-1)};
}


quantity_t quantity_t::operator * (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};
	quantity_t produkt{name() + "*" + quantity_p.name(),{unit()*quantity_p.unit()}};
	
	if (data.size()==1)
	{
		produkt.data=quantity_p.data;
		for (auto& d:produkt.data)
			d*=data[0];
	}
	else if (quantity_p.data.size()==1)
	{
		produkt.data=data;
		for (auto& d:produkt.data)
			d*=quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		produkt.data=data;
		for (int i=0;i<data.size();i++)
			produkt.data[i] *= quantity_p.data[i];
	}
	else return {};
	return produkt;
}

quantity_t quantity_t::operator / (const quantity_t& quantity_p) const
{
	if (!is_set() || !quantity_p.is_set()) return {};
	quantity_t quotient{name() + " / (" + quantity_p.name() +")",{unit() / quantity_p.unit()}};
	
	if (quantity_p.data.size()==1)
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			quotient.data[i]=data[i]/quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			quotient.data[i]=data[i]/quantity_p.data[i];
	}
	else if (data.size()==1)
	{
		quotient.data.resize(quantity_p.data.size());
		for (int i=0;i<quotient.data.size();i++)
			quotient.data[i]=data[0]/quantity_p.data[i];
	}
	else
	{
		cout << ">>>ERROR<<<: " <<  quotient.name() << endl;
		cout << name() << ".data.size()= " << data.size() << " divide by " << quantity_p.name() << ".data.size()= " << quantity_p.data.size() << endl;
		return {};
	}
	return quotient;
}

quantity_t quantity_t::operator/ (const double divisor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]/divisor;
	}

	return quotient;
}

quantity_t quantity_t::operator * (const double factor) const
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i] *= factor;
	}

	return quotient;
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



sputter_energy_t::sputter_energy_t(unit_t unit_s,vector<double> data_s) : quantity_t("sputter_energy",unit_s,data_s){}
sputter_energy_t::sputter_energy_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

sputter_rastersize_t::sputter_rastersize_t(unit_t unit_s,vector<double> data_s) : quantity_t( "sputter_rastersize",unit_s,data_s){}
sputter_rastersize_t::sputter_rastersize_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

sputter_depth_t::sputter_depth_t(unit_t unit_s,vector<double> data_s) : quantity_t("sputter_depth",unit_s,data_s){}
sputter_depth_t::sputter_depth_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

total_sputter_depth_t::total_sputter_depth_t(unit_t unit_s,vector<double> data_s) : quantity_t("total_sputter_depth",unit_s,data_s){}
total_sputter_depth_t::total_sputter_depth_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

sputter_time_t::sputter_time_t(unit_t unit_s,vector<double> data_s) : quantity_t("sputter_time",unit_s,data_s){}
sputter_time_t::sputter_time_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

total_sputter_time_t::total_sputter_time_t(unit_t unit_s,vector<double> data_s) : quantity_t("total_sputter_time",unit_s,data_s){}
total_sputter_time_t::total_sputter_time_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

analysis_energy_t::analysis_energy_t(unit_t unit_s,vector<double> data_s) : quantity_t("analysis_energy",unit_s,data_s){}
analysis_energy_t::analysis_energy_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

analysis_rastersize_t::analysis_rastersize_t(unit_t unit_s,vector<double> data_s) : quantity_t("analysis_rastersize",unit_s,data_s){}
analysis_rastersize_t::analysis_rastersize_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

intensity_t::intensity_t(unit_t unit_s,vector<double> data_s) : quantity_t("intensity",unit_s,data_s){}
intensity_t::intensity_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

concentration_t::concentration_t(unit_t unit_s,vector<double> data_s) : quantity_t("concentration",unit_s,data_s){}
concentration_t::concentration_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

dose_t::dose_t(unit_t unit_s,vector<double> data_s) : quantity_t("dose",unit_s,data_s){}
dose_t::dose_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

SF_t::SF_t(unit_t unit_s,vector<double> data_s) : quantity_t("sensitivity_factor",unit_s,data_s){}
SF_t::SF_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

RSF_t::RSF_t(unit_t unit_s,vector<double> data_s) : quantity_t("relative_sensitivity_factor",unit_s,data_s){}
RSF_t::RSF_t(quantity_t quantity_s) : quantity_t(quantity_s) {}

SR_t::SR_t(unit_t unit_s,vector<double> data_s) : quantity_t("sputter_rate",unit_s,data_s){}
SR_t::SR_t(quantity_t quantity_s) : quantity_t(quantity_s) {}
