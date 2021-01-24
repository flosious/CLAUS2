#include "sample.hpp"



/**************************/


bool sample_t::chip_t::operator==(const sample_t::chip_t& obj) const
{
	if (x() != obj.x()) return false;
	if (y() != obj.y()) return false;
	return true;
}
bool sample_t::chip_t::operator!=(const sample_t::chip_t& obj) const
{
	return !(operator==(obj));
}

int sample_t::chip_t::x() const
{
	return x_p;
}

int sample_t::chip_t::y() const
{
	return y_p;
}



/***********************/


bool sample_t::definition_t::wafer_number=true;
bool sample_t::definition_t::lot=true;
bool sample_t::definition_t::lot_split=false;
bool sample_t::definition_t::monitor=true;
bool sample_t::definition_t::chip=true;
bool sample_t::definition_t::simple_name=true;


/***********************/

sample_t::sample_t(std::__cxx11::string lot_s, int wafer_number_s, std::__cxx11::string lot_split_s, std::__cxx11::string monitor_s, int chip_x_s, int chip_y_s, std::__cxx11::string simple_name_s)
{
	lot_p = lot_s;
	lot_split_p = lot_split_s;
	wafer_number_p = wafer_number_s;
	monitor_p = monitor_s;
	chip.x_p = chip_x_s;
	chip.y_p = chip_y_s;
	simple_name_p = simple_name_s;
}


// sample_t::sample_t(files::file_t file_s)
// {	
// 	if ((file_s.name.lot()=="") && file_s.name.not_parseable_filename_parts().size()>0) simple_name_p=tools::vec::combine_vec_to_string(file_s.name.not_parseable_filename_parts(), "_");
// 	else if ((file_s.name.wafer()<0)) simple_name_p=file_s.name.lot() +file_s.name.lot_split() + tools::vec::combine_vec_to_string(file_s.name.not_parseable_filename_parts(), "_");
// 	else simple_name_p="";
// 	*this = sample_t{file_s.name.lot(),file_s.name.wafer(),file_s.name.lot_split(),file_s.name.monitor(),file_s.name.chip_x(),file_s.name.chip_y(),simple_name_p};
// }

sample_t::sample_t(files::dsims_dp_rpc_asc_t file_s)
{
// 	*this = sample_t{file_s.file()};
}

sample_t::sample_t(files::tofsims_TXT_t file_s)
{
// 	*this = sample_t{file_s.file()};
}

std::__cxx11::string sample_t::lot() const
{
	return lot_p;
}
std::__cxx11::string sample_t::lot_split() const
{
	return lot_split_p;
}
std::__cxx11::string sample_t::monitor() const
{
	return monitor_p;
}
std::__cxx11::string sample_t::simple_name() const
{
	return simple_name_p;
}
int sample_t::wafer_number() const
{
	return wafer_number_p;
}
matrix_t sample_t::matrix() const
{
	return matrix_p;
}


void sample_t::to_screen(std::__cxx11::string prefix) const
{
	cout << prefix << "lot\t" << lot() << endl;
	cout << prefix << "lot_split\t" << lot_split() << endl;
	cout << prefix << "wafer\t" << wafer_number() << endl;
	cout << prefix << "chip\t" << chip.x() << " " << chip.y() << endl;
	cout << prefix << "monitor\t" << monitor() << endl;
	cout << prefix << "matrix\t" << matrix().to_string() << endl;
	cout << prefix << "simple_name\t" << simple_name() << endl;
}


bool sample_t::operator==(const sample_t& obj) const
{
	if (definition.lot) if (lot_p != obj.lot_p) return false;
	if (definition.lot_split) if (lot_split_p != obj.lot_split_p) return false;
	if (definition.monitor) if (monitor_p != obj.monitor_p) return false;
	if (definition.simple_name) if (simple_name_p != obj.simple_name_p) return false;
	if (definition.wafer_number) if (wafer_number_p != obj.wafer_number_p) return false;
	if (definition.chip) if (chip != obj.chip) return false;

	return true;
}

bool sample_t::operator!=(const sample_t& obj) const
{
	return !(operator==(obj));
}

bool sample_t::operator<(const sample_t& obj) const
{	
	if (lot()<obj.lot()) return true;
	if (lot()>obj.lot()) return false;
	
	if (lot_split()<obj.lot_split()) return true;
	if (lot_split()>obj.lot_split()) return false;
	
	if (wafer_number()<obj.wafer_number()) return true;
	if (wafer_number()>obj.wafer_number()) return false;
	
	if (monitor()<obj.monitor()) return true;
	if (monitor()>obj.monitor()) return false;
	
	if (chip.x()<obj.chip.x()) return true;
	if (chip.x()>obj.chip.x()) return false;
	if (chip.y()<obj.chip.y()) return true;
	if (chip.y()>obj.chip.y()) return false;
	
	if (simple_name()<obj.simple_name()) return true;
	if (simple_name()>obj.simple_name()) return false;
	
	return false;
}




