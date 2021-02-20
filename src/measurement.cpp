#include "measurement.hpp"


bool measurements::measurement_t::use_olcdb = true;
bool measurements::measurement_t::use_sample = true;
bool measurements::measurement_t::use_group = true;
bool measurements::measurement_t::use_repition = true;
bool measurements::measurement_t::use_settings = true;

/*******************/

measurements::measurement_t::measurement_t(files::file_t::name_t& filename_p, 
										   files::file_t::contents_t& filecontents_p, 
										   list<sample_t>& samples_list_p)
{
	filenames.insert(&filename_p);
	filecontents.insert(&filecontents_p);
	samples_list = &samples_list_p;
}

const sample_t * measurements::measurement_t::sample()
{
	if (sample_p!=nullptr)
		return sample_p;
	if (filecontents.size()==0) return nullptr;
	/*extract the sample and save its position in the list*/
	sample_t sample(**filenames.begin(),**filecontents.begin());
	for (auto& s : *samples_list)
		if (s==sample)
		{
			sample_p = &s;
			return sample_p;
		}
	samples_list->push_back(sample);
	sample_p = &samples_list->back();
	return sample_p;
}


const int measurements::measurement_t::olcdb() const
{
	if (!use_olcdb) return -1;
	if (filenames.size()==0) return 0;
	return (*filenames.begin())->olcdb();
}

const string measurements::measurement_t::group() const
{
	if (!use_group) return "";
	if (filenames.size()==0) return "";
	return (*filenames.begin())->group();
}

const string measurements::measurement_t::repitition() const
{
	if (!use_repition) return "";
	if (filenames.size()==0) return "";
	return (*filenames.begin())->repetition();
}

const bool measurements::measurement_t::operator==(measurements::measurement_t& obj)
{
	if (use_sample)
	{
		//pointer comparison
		if (sample()!=obj.sample()) return false;
	}
	
	if (use_repition)
	{
		if (repitition()!=obj.repitition()) return false;
	}
	
	if (use_olcdb)
	{
		if (olcdb()!=obj.olcdb()) return false;
	}
	
	if (use_group)
	{
		if (group()!=obj.group()) return false;
	}
	
// 	if (use_settings)
// 	{
// 		if (simple_name()!=obj.simple_name()) return false;
// 	}
	return false;
}


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


