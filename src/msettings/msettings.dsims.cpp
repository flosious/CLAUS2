#include "msettings.hpp"

/********************/
/****  Dsims_t  *****/
/********************/

msettings::dsims_t::dsims_t()
{
}

msettings::dsims_t::dsims_t(set<files::dsims_t*> files_p) : sims_t(*files_p.begin()), files_p(files_p)
{
	for (files::dsims_t* f: files_p)
		msettings::sims_t::files_p.insert(f);
}

string& msettings::dsims_t::secondary_polarity()
{
	for (files::dsims_t* F : files_p)
		if (F->secondary_polarity()!="" )
		{
			if (sims_t::secondary_polarity()!="" && F->secondary_polarity() !=sims_t::secondary_polarity())
				logger::warning("msettings::dsims_t::secondary_polarity file name differs from its contents, using contents",F->secondary_polarity());
			secondary_polarity_p = F->secondary_polarity();
		}
	return secondary_polarity_p;
}
secondary_voltage_t& msettings::dsims_t::secondary_voltage()
{
// 	cout << "msettings::dsims_t::secondary_voltage called" << endl;
	for (files::dsims_t* F : files_p)
		if (F->sputter_energy().is_set() )
		{
			if (sims_t::secondary_voltage().is_set() && F->sputter_energy().data()[0]!=sims_t::secondary_voltage().data()[0])
				logger::warning("msettings::dsims_t::secondary_voltage file name differs from its contents, using contents",F->sputter_energy().to_string());
			secondary_voltage_p = secondary_voltage_t(F->sputter_energy().data());
		}
	return secondary_voltage_p;
}
ion_t& msettings::dsims_t::sputter_ion()
{
	for (files::dsims_t* F : files_p)	
		if (F->sputter_element().is_set())
		{
			if (sims_t::sputter_ion().is_set() && F->sputter_element().symbol() != sims_t::sputter_ion().elements()[0].symbol())
				logger::warning("msettings::dsims_t::sputter_element file name differs from its contents, using contents",F->sputter_element().symbol());
			sputter_ion_p = ion_t({F->sputter_element()},{{1}});
		}
	return sputter_ion_p;
}


const rastersize_t msettings::dsims_t::analyzed_area()
{
	for (files::dsims_t* f: files_p)
		if (f->analyzed_area().is_set()) 
			return f->analyzed_area();
	return {};
}

const rastersize_t msettings::dsims_t::sputter_rastersize()
{
	for (files::dsims_t* f: files_p)
		if (f->sputter_rastersize().is_set()) 
			return f->sputter_rastersize();
	return {};
}


const quantity_t msettings::dsims_t::chamber_pressure()
{
	for (files::dsims_t* f: files_p)
		if (f->chamber_pressure().is_set()) 
			return f->chamber_pressure();
	return {};
}

const quantity_t msettings::dsims_t::contrast_aperture()
{
	for (files::dsims_t* f: files_p)
		if (f->contrast_aperture().is_set()) 
			return f->contrast_aperture();
	return {};
}

const quantity_t msettings::dsims_t::egate()
{
	for (files::dsims_t* f: files_p)
		if (f->egate().is_set()) 
			return f->egate();
	return {};
}

const quantity_t msettings::dsims_t::em_voltage()
{
	for (files::dsims_t* f: files_p)
		if (f->em_voltage().is_set()) 
			return f->em_voltage();
	return {};
}

const quantity_t msettings::dsims_t::em_yield()
{
	for (files::dsims_t* f: files_p)
		if (f->em_yield().is_set()) 
			return f->em_yield();
	return {};
}

const quantity_t msettings::dsims_t::energy_window()
{
	for (files::dsims_t* f: files_p)
		if (f->energy_window().is_set()) 
			return f->energy_window();
	return {};
}

const quantity_t msettings::dsims_t::entrance_slit()
{
	for (files::dsims_t* f: files_p)
		if (f->entrance_slit().is_set()) 
			return f->entrance_slit();
	return {};
}

const quantity_t msettings::dsims_t::exit_slit()
{
	for (files::dsims_t* f: files_p)
		if (f->exit_slit().is_set()) 
			return f->exit_slit();
	return {};
}

const quantity_t msettings::dsims_t::field_aperture()
{
	for (files::dsims_t* f: files_p)
		if (f->field_aperture().is_set()) 
			return f->field_aperture();
	return {};
}

const quantity_t msettings::dsims_t::mass_resolution()
{
	for (files::dsims_t* f: files_p)
		if (f->mass_resolution().is_set()) 
			return f->mass_resolution();
	return {};
}

std::__cxx11::string msettings::dsims_t::to_string()
{
	stringstream ss;
	const string del = ",";
	ss << sims_t::to_string() << del;
	ss << chamber_pressure().to_string() << del;
	ss << egate().to_string() << del;
	ss << mass_resolution().to_string() << del;
	ss << field_aperture().to_string() << del;
	ss << contrast_aperture().to_string() << del;
	ss << entrance_slit().to_string() << del;
	ss << exit_slit().to_string() << del;
	ss << energy_window().to_string() << del;
	ss << em_yield().to_string() << del;
	ss << em_voltage().to_string() << del;
	return ss.str();
}

