#include "measurement.hpp"


bool measurements::measurement_t::use_olcdb = true;
bool measurements::measurement_t::use_sample = true;
bool measurements::measurement_t::use_group = true;
bool measurements::measurement_t::use_repition = true;
bool measurements::measurement_t::use_settings = true;

/*******************/

// const int measurement_t::olcdb() const
// {
// 	if (!use_olcdb) return -1;
// 	if (files()->size()==0) return 0;
// 	return files()->front()->name->olcdb();
// }
// 
// const string measurement_t::group() const
// {
// 	if (!use_group) return "";
// 	if (files()->size()==0) return "";
// 	return files()->front()->name->group();
// }
// 
// const string measurement_t::repitition() const
// {
// 	if (!use_repition) return "";
// 	if (files()->size()==0) return "";
// 	return files()->front()->name->repetition();
// }

// const list<file_t *> * measurement_t::files() const
// {
// 	if (files_p.size()>0)
// 		return &files_p;
// 	return nullptr;
// }

// const sample_t * measurement_t::sample() const
// {
// // 	for (auto& s : *sample_list_global)
// 	for (list<sample_t>::iterator s; s!=sample_list_global->end(); s++)
// 	{
// 		for (auto& f:files_p)
// 		{
// // 			if (sample_t(*f)==*s)
// 				
// 		}
// 	}
// 	return nullptr;
// }
// 

/*operators*/
/*
const bool measurement_t::operator==(const measurement_t& obj) const
{
	if (use_sample)
	{
		//comparing pointers!
		if (sample()!=obj.sample()) return false;
	}
	
	if (use_olcdb)
	{
		if (olcdb()!=obj.olcdb()) return false;
	}
	
	if (use_group)
	{
		if (group()!=obj.group()) return false;
	}
	
	if (use_repition)
	{
		if (repitition()!=obj.repitition()) return false;
	}
	return true;
}

const bool measurement_t::operator!=(const measurement_t& obj) const
{
	return !(operator==(obj));
}

const bool measurement_t::operator<(const measurement_t& obj) const
{
	if (use_sample)
	{
		if (sample()<obj.sample()) return true;
		if (sample()>obj.sample()) return false;
	}
	
	if (use_olcdb)
	{
		if (olcdb()<obj.olcdb()) return true;
		if (olcdb()>obj.olcdb()) return false;
	}
	
	if (use_group)
	{
		if (group()<obj.group()) return true;
		if (group()>obj.group()) return false;
	}
	
	if (use_repition)
	{
		if (repitition()<obj.repitition()) return true;
		if (repitition()>obj.repitition()) return false;
	}
	
	return false;
}*/


