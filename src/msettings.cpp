// #include "msettings.hpp"
// 
// 
// /****************************/
// /****  Beam_settings_t  *****/
// /****************************/
// 
// msettings::beam_settings_t::beam_settings_t(energy_t energy_s, rastersize_t rastersize_s, ion_t ion_s) :energy(energy_s), raster_size(rastersize_s),ion(ion_s)
// {
// }
// 
// 
// 
// 
// /********************/
// /****  Sims_t  ******/
// /********************/
// 
// msettings::sims_t::sims_t()
// {
// }
// 
// msettings::sims_t::sims_t(files_::sims_t* file_p) : files_p({file_p})
// {
// }
// 
// msettings::sims_t::sims_t(set<files_::sims_t *> file_p) : files_p(file_p)
// {
// }
// 
// std::__cxx11::string msettings::sims_t::to_string()
// {
// 	stringstream ss;
// 	const string del = ",";
// 	ss << "sputter_ion: " << sputter_ion().to_string() << del;
// // 	for (auto& ele : sputter_ion().elements())
// // 		ss << ele.symbol() << static_cast<int>(sputter_ion().electric_charge.data()[0]) << " ";
// // 	ss << "\t";
// 	ss << secondary_voltage().to_string() << del;
// 	ss << "secondary_polarity: " << secondary_polarity();
// 	return ss.str();
// }
// 
// 
// bool msettings::sims_t::is_set()
// {
// 	if (files_p.size()==0) return false;
// 	if (secondary_polarity()!="" || secondary_voltage().is_set() || sputter_ion().is_set())
// 		return true;
// 	return false;
// }
// 
// std::__cxx11::string& msettings::sims_t::secondary_polarity()
// {
// 	if (secondary_polarity_p!="")
// 		return secondary_polarity_p;
// 	for (files_::sims_t* f: files_p)
// 		if (f->name.secondary_polarity()!="") 
// 		{
// 			secondary_polarity_p= f->name.secondary_polarity();
// 			break;
// 		}
// 	return secondary_polarity_p;
// }
// 
// secondary_voltage_t& msettings::sims_t::secondary_voltage()
// {
// 	if (secondary_voltage_p.is_set())
// 		return secondary_voltage_p;
// 	for (files_::sims_t* f: files_p)
// 		if (f->name.secondary_voltage().is_set()) 
// 		{
// 			secondary_voltage_p=f->name.secondary_voltage();
// 			break;
// 		}
// 	return secondary_voltage_p;
// }
// 
// ion_t& msettings::sims_t::sputter_ion()
// {
// 	if (sputter_ion_p.is_set())
// 		return sputter_ion_p;
// 	for (files_::sims_t* f: files_p)
// 		if (f->name.sputter_element().is_set()) 
// 		{
// 			element_t ele = f->name.sputter_element();
// 			if (secondary_polarity()=="+") // O2 +
// 				ele.substance_amount() = substance_amount_t({2});
// 			sputter_ion_p = ion_t{ele,{{1}}}; // Cs +
// 		}
// 	return sputter_ion_p;
// }
// 
// bool msettings::sims_t::operator==(msettings::sims_t obj)
// {
// 	if (secondary_polarity()!=obj.secondary_polarity()) return false;
// 	if (secondary_voltage() != obj.secondary_voltage()) return false;
// 	if (sputter_ion() != obj.sputter_ion()) return false;
// 	return true;
// }
// 
// bool msettings::sims_t::operator!=(msettings::sims_t obj)
// {
// 	return !operator==(obj);
// }
// 
// 
// 
// /***********************/
// /****  Tofsims_t  ******/
// /***********************/
// 
// // msettings::tofsims_t::tofsims_t(settings::beam_settings_t sputter_beam_s, std::__cxx11::string secondary_polarity_s, settings::beam_settings_t analysis_beam_s) : sims_t(sputter_beam_s,secondary_polarity_s), analysis_beam(analysis_beam_s)
// // {
// // }
// // 
