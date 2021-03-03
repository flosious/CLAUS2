#include "file.hpp"

/**************/
/*** sims_t ***/
/**************/
files::sims_t::sims_t(filenames::sims_t& name,const string& delimiter,const set<string>& identifiers) : file_t(name,delimiter,identifiers), name(name)
{
}


vector<cluster_t> files::sims_t::clusters()
{
	logger::fatal("you should never be able to read this","virtual const vector<cluster_t> files::sims_t::clusters()");
	return {};
}
