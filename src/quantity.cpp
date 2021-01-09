#include "quantity.hpp"



/*******************************/

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
	quantity_t adder = obj.change_unit(unit());
	tools::vec::add(&data,adder.data);
}

quantity_t quantity_t::operator+(const quantity_t& quantity)
{
	quantity_t sum;
	if (!is_set()) return sum;
	if (dimension != quantity.dimension) return sum;
	if (unit != quantity.unit) return sum;
	if (data.size()==0 || quantity.data.size()==0 ) return sum;
	sum.dimension=dimension;
	sum.unit = unit;
	sum.name = name;
	
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

quantity_t quantity_t::operator+(const double summand)
{
	quantity_t sum;
	if (!is_set()) return sum;
	sum.dimension=dimension;
	sum.unit = unit;
	sum.name = name;
	sum.data.resize(data.size());
	for (int i=0; i< data.size()&&i<data.size();i++)
	{
		sum.data[i] = data[i] + summand;
	}
	return sum;
}

quantity_t quantity_t::operator-(const quantity_t& quantity)
{
	quantity_t difference;
	if (!is_set()) return difference;
	if (dimension != quantity.dimension) return difference;
	if (unit != quantity.unit) return difference;
	if (data.size()==0 || quantity.data.size()==0 ) return difference;
	difference.dimension=dimension;
	difference.unit = unit;
	difference.name = name;
	if (data.size() == quantity.data.size())
	{
		difference.data.resize(data.size());
		for (int i=0; i< data.size()&&i<quantity.data.size();i++)
		{
			difference.data[i] = data[i] - quantity.data[i];
		}
	} 
	else if (data.size()==1)
	{
		difference.data.resize(quantity.data.size());
		for (int i=0; i< difference.data.size();i++)
		{
			difference.data[i] = data[0] - quantity.data[i];
		}
	}
	else if (quantity.data.size()==1)
	{
		difference.data.resize(data.size());
		for (int i=0; i< difference.data.size();i++)
		{
			difference.data[i] = quantity.data[0] - data[i];
		}
	}
	return difference;
}


quantity_t quantity_t::operator * (const quantity_t& quantity_p)
{
	quantity_t produkt;
	
	produkt.name=name + "*" + quantity_p.name;
	produkt.dimension=dimension + "*" + quantity_p.dimension;
	produkt.unit=unit + "*" + quantity_p.unit;
	
	if (data.size()==1)
	{
		produkt.data=quantity_p.data;
		for (auto& d:produkt.data)
			d=d*data[0];
	}
	else if (quantity_p.data.size()==1)
	{
		produkt.data=data;
		for (auto& d:produkt.data)
			d=d*quantity_p.data[0];
	}
	else if (quantity_p.data.size()==data.size())
	{
		produkt.data.resize(data.size());
		for (int i=0;i<data.size();i++)
			produkt.data[i]=data[i]*quantity_p.data[i];
	}
	else return quantity_t();
	return produkt;
}

quantity_t quantity_t::operator / (const quantity_t& quantity_p)
{
	quantity_t quotient;
	quotient.name_p=name() + " / (" + quantity_p.name() +")";
	quotient.unit_p=unit_t{ unit().prefix()+" / " + quantity_p.unit().prefix(),unit().base() + " / " + quantity_p.unit().base()};
	
	if (quantity_p.data.size()==1)
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
		{
// 			cout << "A: " << data[i] << "/" << quantity_p.data[0] << endl;
			quotient.data[i]=data[i]/quantity_p.data[0];
		}
	}
	else if (quantity_p.data.size()==data.size())
	{
		quotient.data.resize(data.size());
		for (int i=0;i<data.size();i++)
		{
// 			cout << "B: " << data[i] << "/" << quantity_p.data[i] << endl;
			quotient.data[i]=data[i]/quantity_p.data[i];
		}
	}
	else if (data.size()==1)
	{
		quotient.data.resize(quantity_p.data.size());
		for (int i=0;i<quotient.data.size();i++)
		{
// 			cout << "B: " << data[i] << "/" << quantity_p.data[i] << endl;
			quotient.data[i]=data[0]/quantity_p.data[i];
		}
	}
	else
	{
		cout << ">>>ERROR<<<: " <<  quotient.name() << endl;
		cout << name() << ".data.size()= " << data.size() << " divide by " << quantity_p.name() << ".data.size()= " << quantity_p.data.size() << endl;
	}
	return quotient;
}

quantity_t quantity_t::operator / (const double divisor)
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]/divisor;
	}

	return quotient;
}
quantity_t quantity_t::operator * (const double factor)
{
	quantity_t quotient=*this;
	for (int i=0;i<data.size();i++)
	{
		quotient.data[i]=data[i]*factor;
	}

	return quotient;
}


/******************************************/

quantity_t quantity_t::sputter_energy()
{
	return quantity_t{"sputter_energy",{"","eV"}};
}

quantity_t quantity_t::analysis_energy()
{
	return quantity_t{"analysis_energy",{"k","eV"}};
}

quantity_t quantity_t::analysis_rastersize()
{
	return quantity_t{"analysis_rastersize",{"u","m"}};
}

quantity_t quantity_t::concentration()
{
	return quantity_t{"concentration",{"","at/cm^3"}};
}

quantity_t quantity_t::dose()
{
	return quantity_t{"dose",{"","at/cm^2"}};
}



/*********************/



sputter_energy_t::sputter_energy_t(unit_t unit_s) : quantity_t("sputter_energy",unit_s){}
sputter_energy_t::sputter_energy_t(quantity_t quantity_s) : quantity_t(quantity_s) {}


sputter_rastersize_t::sputter_rastersize_t(unit_t unit_s) : quantity_t( "sputter_rastersize",unit_s)
{
}

sputter_depth_t::sputter_depth_t(unit_t unit_s) : quantity_t("sputter_depth",unit_s)
{
}

total_sputter_depth_t::total_sputter_depth_t(unit_t unit_s) : quantity_t("total_sputter_depth",unit_s)
{
}

sputter_time_t::sputter_time_t(unit_t unit_s) : quantity_t("sputter_time",unit_s)
{
}

total_sputter_time_t::total_sputter_time_t(unit_t unit_s) : quantity_t("total_sputter_time",unit_s)
{
}

analysis_energy_t::analysis_energy_t(unit_t unit_s) : quantity_t("analysis_energy",unit_s)
{
}

analysis_rastersize_t::analysis_rastersize_t(unit_t unit_s) : quantity_t("analysis_rastersize",unit_s)
{
}

intensity_t::intensity_t(unit_t unit_s) : quantity_t("intensity",unit_s)
{
}

concentration_t::concentration_t(unit_t unit_s) : quantity_t("concentration",unit_s)
{
}
