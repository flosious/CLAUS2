#include "file.hpp"

files::tofsims_TXT_t::tofsims_TXT_t(const string& filename_with_path_s, string* contents_s) : 	sims_t(filename_with_path_s, contents_s),
																										name(filename_with_path_s), 
																										contents(filename_with_path_s,contents_s)
{
}

files::tofsims_TXT_t::contents_t::contents_t(const string& filename_with_path_s,string* contents_s) : sims_t::contents_t(filename_with_path_s,contents_s)
{
}

files::tofsims_TXT_t::name_t::name_t(const string& filename_with_path_s) : sims_t::name_t(filename_with_path_s)
{
}
