#include "sample.hpp"



/****************************/
/***   sample_t::chip_t   ***/
/****************************/


bool sample_t::chip_t::operator==(const sample_t::chip_t& obj) const
{
	if (x != obj.x) return false;
	if (y != obj.y) return false;
	return true;
}
bool sample_t::chip_t::operator!=(const sample_t::chip_t& obj) const
{
	return !(operator==(obj));
}

sample_t::chip_t::chip_t(const int x, const int y) : x(x), y(y)
{
}

bool sample_t::chip_t::is_set()
{
	if (x>=0 && y>=0) return true;
	return false;
}

void sample_t::chip_t::to_screen(string prefix)
{
	cout << prefix << "chip\t" << "X:"<< x << ", Y:"<< y << endl;
}



// 
// 
// /***********************/




bool sample_t::use_lot=true;
bool sample_t::use_lot_split=true;
bool sample_t::use_wafer=true;
bool sample_t::use_monitor=true;
bool sample_t::use_chip=true;
bool sample_t::use_simple_name=true;
bool sample_t::use_matrix=false; //maybe not set for some 


/***********************/

vector<sample_t> sample_t::samples_list;

void sample_t::feed_samples_list(file_t& file)
{
	sample_t sample(file);
	for (auto& s : samples_list)
	{
		if (sample==s) 
		{
			s.files.insert(&file);
			return;
		}
	}
	samples_list.push_back(sample);
}

void sample_t::feed_samples_list(vector<file_t>& files)
{
	for (auto& f : files)
		feed_samples_list(f);
}


/*
vector<sample_t> sample_t::samples(vector<file_t>& files_s)
{
	vector<sample_t> sam;
	vector<sample_t>::iterator found;
	for (int i=0;i<files_s.size();i++)
	{
		sample_t s(files_s.at(i));
		found= find(sam.begin(),sam.end(),s);
		if (found==sam.end())
			sam.push_back(s);
		else
			found->files.insert(&files_s.at(i));
	}
	return sam;
}
*/

// vector<sample_t> sample_t::samples(vector<file_t>* files)
// {
// 	vector<sample_t> sam;
// 	vector<sample_t>::iterator found;
// 	for (int i=0;i<files->size();i++)
// 	{
// 		sample_t s(files->at(i));
// 		found= find(sam.begin(),sam.end(),s);
// 		if (found==sam.end())
// 			sam.push_back(s);
// 		else
// 			found->files.push_back(&files->at(i));
// 	}
// 	return sam;
// }



// void sample_t::add_to_list(file_t& file, vector<sample_t>& samples_list)
// {
// 	sample_t sample(file);
// 	vector<sample_t>::iterator s = find(samples_list.begin(),samples_list.end(),sample);
// 	if (s==samples_list.end())	samples_list.push_back(sample);
// 	else s->files.insert(&file);
// }





/********************************/
/*********   sample _t   ********/
/********************************/

// sample_t::sample_t(std::__cxx11::string lot, 
// 				   std::__cxx11::string lot_split, 
// 				   int wafer,
// 				   sample_t::chip_t chip, 
// 				   std::__cxx11::string monitor,
// 				   std::__cxx11::string simple_name) :
// 				   lot(lot),
// 				   lot_split(lot_split),
// 				   wafer(wafer),
// 				   chip(chip),
// 				   monitor(monitor),
// 				   simple_name(simple_name)
// {
// }

sample_t::sample_t(file_t& file_s)
{	
	files.insert(&file_s);
}


std::__cxx11::string sample_t::simple_name()
{
	if (!use_simple_name || files.size()==0) return "";
	for (auto& f : files)
	{
		if (f->name->simple_name()!="")
			return f->name->simple_name();
	}
	return "";
}


std::__cxx11::string sample_t::monitor()
{
	if (!use_monitor || files.size()==0) return "";
	for (auto& f : files)
	{
		if (f->name->monitor()!="")
			return f->name->monitor();
	}
	return "";
}

std::__cxx11::string sample_t::lot_split()
{
	if (!use_lot_split || files.size()==0) return "";
	for (auto& f : files)
	{
		if (f->name->lot_split()!="")
			return f->name->lot_split();
	}
	return "";
}

string sample_t::lot()
{
	if (!use_lot || files.size()==0) return "";
	for (auto& f : files)
	{
		if (f->name->lot()!="")
			return f->name->lot();
	}
	return "";
}

sample_t::chip_t sample_t::chip()
{
	if (!use_chip || files.size()==0) return {-1,-1};
	for (auto& f : files)
	{
		if (f->name->chip_x()>=0 && f->name->chip_y()>=0)
			return {f->name->chip_x(),f->name->chip_y()};
	}
	return {-1,-1};
}

int sample_t::wafer()
{
	if (!use_wafer || files.size()==0) return -1;
	for (auto& f : files)
	{
		//can there be a wafer "0" ? - no
		if (f->name->wafer()>0)
			return f->name->wafer();
	}
	return -1;
}

matrix_t sample_t::matrix()
{
	if (!use_matrix || files.size()==0) return {};
	for (auto& f : files)
	{
		if (f->contents->matrix().is_set())
			return f->contents->matrix();
	}
	return {};
}

void sample_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "files\t<" << files.size() << ">" << endl;
	cout << prefix << "lot\t" << lot() << endl;
	cout << prefix << "lot_split\t" << lot_split() << endl;
	cout << prefix << "wafer\t" << wafer() << endl;
	chip().to_screen(prefix);
	cout << prefix << "monitor\t" << monitor() << endl;
	cout << prefix << "simple_name\t" << simple_name() << endl;
}

// bool sample_t::try_add(file_t& file_s)
// {
// 	sample_t s(file_s);
// 	if (*this == s)
// 	{
// 		files.push_back(&file_s);
// 		return true;
// 	}
// 	return false;
// }
// 



bool sample_t::operator<(sample_t& obj)
{	
	if (use_lot)
	{
		if (lot()<obj.lot()) return true;
		if (lot()>obj.lot()) return false;
	}
	
	if (use_lot_split)
	{
		if (lot_split()<obj.lot_split()) return true;
		if (lot_split()>obj.lot_split()) return false;
	}
	
	if (use_wafer)
	{
		if (wafer()<obj.wafer()) return true;
		if (wafer()>obj.wafer()) return false;
	}
	
	if (use_monitor)
	{
		if (monitor()<obj.monitor()) return true;
		if (monitor()>obj.monitor()) return false;
	}
	
	if (use_chip)
	{
		if (chip().x<obj.chip().x) return true;
		if (chip().x>obj.chip().x) return false;
		if (chip().y<obj.chip().y) return true;
		if (chip().y>obj.chip().y) return false;
	}
	
	if (use_simple_name)
	{
		if (simple_name()<obj.simple_name()) return true;
		if (simple_name()>obj.simple_name()) return false;
	}

	if (use_matrix)
	{
		if (matrix() < obj.matrix()) return true;
	}
	
	return false;
}

bool sample_t::operator==(sample_t& obj)
{
	if (use_lot)
	{
		if (lot()!=obj.lot()) return false;
	}
	
	if (use_lot_split)
	{
		if (lot_split()!=obj.lot_split()) return false;
	}
	
	if (use_wafer)
	{
		if (wafer()!=obj.wafer()) return false;
	}
	
	if (use_monitor)
	{
		if (monitor()!=obj.monitor()) return false;
	}
	
	if (use_chip)
	{
		if (chip()!=obj.chip()) return false;
	}
	
	if (use_simple_name)
	{
		if (simple_name()!=obj.simple_name()) return false;
	}
	return false;
}

bool sample_t::operator!=(sample_t& obj)
{
	return !operator==(obj);
}



