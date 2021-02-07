#include "sample.hpp"



/****************************/
/***   sample_t::chip_t   ***/
/****************************/


// bool sample_t::chip_t::operator==(const sample_t::chip_t& obj) const
// {
// 	if (x != obj.x) return false;
// 	if (y != obj.y) return false;
// 	return true;
// }
// bool sample_t::chip_t::operator!=(const sample_t::chip_t& obj) const
// {
// 	return !(operator==(obj));
// }
// 
// sample_t::chip_t::chip_t(const int x, const int y) : x(x), y(y)
// {
// }
// 
// 
// /***********************/



// // bool sample_t::definition_t::lot_split=false;
// // bool sample_t::definition_t::monitor=true;
// // bool sample_t::definition_t::chip=true;
// // bool sample_t::definition_t::simple_name=true;
// 
// 
// /***********************/
// 
// 
// 
// 
// 








/********************************/
/*********   sample _t   ********/
/********************************/

sample_t::sample_t(std::__cxx11::string lot, 
				   std::__cxx11::string lot_split, 
				   int wafer,
				   sample_t::chip_t chip, 
				   std::__cxx11::string monitor,
				   std::__cxx11::string simple_name) :
				   lot(lot),
				   lot_split(lot_split),
				   wafer(wafer),
				   chip(chip),
				   monitor(monitor),
				   simple_name(simple_name)
{
}


// void sample_t::set_default_definitions()
// {
// 	definitions.wafer=true;
// }
// 
// 
// sample_t::sample_t(file_t file_s) : 	lot(file_s.name.lot()),
// 										lot_split(file_s.name.lot_split()),
// 										wafer(file_s.name.wafer()),
// 										monitor(file_s.name.monitor()),
// 										chip(file_s.name.chip_x(), file_s.name.chip_y()),
// 										simple_name(file_s.name.simple_name()),
// 										olcdbid(file_s.name.olcdb()),
// 										group(file_s.name.group()),
// 										repition(file_s.name.repetition())
// {
// }
// 
// sample_t::sample_t(files::dsims_dp_rpc_asc_t file_s) : sample_t(file_s.file())
// {}
// 
// 
// 
// 
// void sample_t::to_screen(std::__cxx11::string prefix)
// {
// 	cout << prefix << "lot\t" << lot << endl;
// 	cout << prefix << "lot_split\t" << lot_split << endl;
// 	cout << prefix << "wafer\t" << wafer << endl;
// 	cout << prefix << "chip\tX" << chip.x << " Y" << chip.y << endl;
// 	cout << prefix << "monitor\t" << monitor << endl;
// 	cout << prefix << "matrix\t" << matrix.to_string() << endl;
// 	cout << prefix << "simple_name\t" << simple_name << endl;
// }
// 
// 
// bool sample_t::operator==(const sample_t& obj) const
// {
// 	
// 	if (definitions.lot) 			if (lot != obj.lot) return false;
// 	if (definitions.lot_split)	 	if (lot_split != obj.lot_split) return false;
// 	if (definitions.monitor) 		if (monitor != obj.monitor) return false;
// 	if (definitions.simple_name) 	if (simple_name != obj.simple_name) return false;
// 	if (definitions.wafer) 			if (wafer != obj.wafer) return false;
// 	if (definitions.chip) 			if (chip != obj.chip) return false;
// 	
// 	if (definitions.olcdbid) 		if (olcdbid != obj.olcdbid) return false;
// 	if (definitions.group) 			if (group != obj.group) return false;
// 	if (definitions.repition) 	if (repition != obj.repition) return false;
// 	return true;
// }
// 
// bool sample_t::operator!=(const sample_t& obj) const
// {
// 	return !(operator==(obj));
// }


void sample_t::to_screen(std::__cxx11::string prefix)
{
// 	matrix = matrix_t();
}


bool sample_t::use_lot=true;
bool sample_t::use_lot_split=true;
bool sample_t::use_wafer=true;
bool sample_t::use_monitor=true;
bool sample_t::use_chip=true;
bool sample_t::use_simple_name=true;

bool sample_t::operator<(const sample_t& obj) const
{	
	if (use_lot)
	{
		if (lot<obj.lot) return true;
		if (lot>obj.lot) return false;
	}
	
	if (use_lot_split)
	{
		if (lot_split<obj.lot_split) return true;
		if (lot_split>obj.lot_split) return false;
	}
	
	if (use_wafer)
	{
		if (wafer<obj.wafer) return true;
		if (wafer>obj.wafer) return false;
	}
	
	if (use_monitor)
	{
		if (monitor<obj.monitor) return true;
		if (monitor>obj.monitor) return false;
	}
	
	if (use_chip)
	{
		if (chip.x<obj.chip.x) return true;
		if (chip.x>obj.chip.x) return false;
		if (chip.y<obj.chip.y) return true;
		if (chip.y>obj.chip.y) return false;
	}
	
	if (use_simple_name)
	{
		if (simple_name<obj.simple_name) return true;
		if (simple_name>obj.simple_name) return false;
	}
	
	
	return false;
}




