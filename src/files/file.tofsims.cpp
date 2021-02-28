#include "file.hpp"

/************************************/
/*****  files::tofsims_t         ****/
/************************************/

files::tofsims_t::tofsims_t(filenames::tofsims_t& name) : sims_t(name,"\t",{"tofsims filde identifier"}), name(name)
{
}
