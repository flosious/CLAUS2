#include "measurement_settings.hpp"


/****************************/
/****  Beam_settings_t  *****/
/****************************/

settings::beam_settings_t::beam_settings_t(energy_t energy_s, rastersize_t rastersize_s, ion_t ion_s) :energy(energy_s), raster_size(rastersize_s),ion(ion_s)
{
}




/********************/
/****  Sims_t  ******/
/********************/

settings::sims_t::sims_t(settings::beam_settings_t sputter_beam_s, string secondary_polarity_s) : sputter_beam(sputter_beam_s), secondary_polarity(secondary_polarity_s)
{
}

/********************/
/****  Dsims_t  *****/
/********************/

settings::dsims_t::dsims_t(settings::beam_settings_t sputter_beam_s, std::__cxx11::string secondary_polarity_s) : sims_t(sputter_beam_s,secondary_polarity_s)
{
}

/***********************/
/****  Tofsims_t  ******/
/***********************/

settings::tofsims_t::tofsims_t(settings::beam_settings_t sputter_beam_s, std::__cxx11::string secondary_polarity_s, settings::beam_settings_t analysis_beam_s) : sims_t(sputter_beam_s,secondary_polarity_s), analysis_beam(analysis_beam_s)
{
}

