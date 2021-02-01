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


/***********************/


bool sample_t::definition_t::wafer_number=true;
bool sample_t::definition_t::lot=true;
bool sample_t::definition_t::lot_split=false;
bool sample_t::definition_t::monitor=true;
bool sample_t::definition_t::chip=true;
bool sample_t::definition_t::simple_name=true;


/***********************/










/****************************/
/***  sample_t::matrix_t  ***/
/****************************/

sample_t::matrix_t::matrix_t(set<element_t> elements_s) : elements_p(elements_s)
{
}

const set<element_t>& sample_t::matrix_t::elements()
{
// 	if (elements_p.size()==0) // look up database
	return elements_p;
}

bool sample_t::matrix_t::operator==(matrix_t& obj)
{
	if (elements().size()!=obj.elements().size()) return false;
// 	if (isotopes.size()!=obj.isotopes.size()) return false;
	
	for (auto& element:elements())
		if (obj.elements().find(element)!=obj.elements().end()) 
			return false;
// 	for (auto& isotope:isotopes)
// 		if (obj.isotopes.find(isotope)!=obj.isotopes.end()) 
// 			return false;
		
	return true;
}
bool sample_t::matrix_t::operator!=(matrix_t& obj) 
{
	return !(operator==(obj));
}
const string sample_t::matrix_t::to_string()
{
	stringstream out;
	out << "elements:";
	for (auto& element:elements())
		out << "\t" << element.to_string() ;
	return out.str();
}

/********************************/
/*********   sample _t   ********/
/********************************/



sample_t::sample_t(std::__cxx11::string lot_s, 
				   int wafer_number_s, 
				   std::__cxx11::string lot_split_s, 
				   std::__cxx11::string monitor_s, 
				   int chip_x_s, int chip_y_s, 
				   std::__cxx11::string simple_name_s, 
				   set<element_t> elements_s) 	: 			lot(lot_s),
															wafer_number(wafer_number_s),
															lot_split(lot_split_s),
															monitor(monitor_s),
															simple_name(simple_name_s),
															chip(chip_x_s,chip_y_s),
															matrix(elements_s)
{
}

// sample_t::sample_t(files::file_t file_s)
// {	
// 	if ((file_s.name.lot()=="") && file_s.name.not_parseable_filename_parts().size()>0) simple_name_p=tools::vec::combine_vec_to_string(file_s.name.not_parseable_filename_parts(), "_");
// 	else if ((file_s.name.wafer()<0)) simple_name_p=file_s.name.lot() +file_s.name.lot_split() + tools::vec::combine_vec_to_string(file_s.name.not_parseable_filename_parts(), "_");
// 	else simple_name_p="";
// 	*this = sample_t{file_s.name.lot(),file_s.name.wafer(),file_s.name.lot_split(),file_s.name.monitor(),file_s.name.chip_x(),file_s.name.chip_y(),simple_name_p};
// }

sample_t::sample_t(files::file_t file_s) : 	sample_t(file_s.name.lot(),
													file_s.name.wafer(),
													file_s.name.lot_split(),
													file_s.name.monitor(),
													file_s.name.chip_x(),
													file_s.name.chip_y(),
													file_s.name.simple_name(),
													file_s.contents.matrix_elements())
{
}

sample_t::sample_t(files::dsims_dp_rpc_asc_t file_s) : sample_t(file_s.file())
{}




void sample_t::to_screen(std::__cxx11::string prefix)
{
	cout << prefix << "lot\t" << lot << endl;
	cout << prefix << "lot_split\t" << lot_split << endl;
	cout << prefix << "wafer\t" << wafer_number << endl;
	cout << prefix << "chip\t" << chip.x << " " << chip.y << endl;
	cout << prefix << "monitor\t" << monitor << endl;
	cout << prefix << "matrix\t" << matrix.to_string() << endl;
	cout << prefix << "simple_name\t" << simple_name << endl;
}


bool sample_t::operator==(const sample_t& obj) const
{
	if (definition.lot) if (lot != obj.lot) return false;
	if (definition.lot_split) if (lot_split != obj.lot_split) return false;
	if (definition.monitor) if (monitor != obj.monitor) return false;
	if (definition.simple_name) if (simple_name != obj.simple_name) return false;
	if (definition.wafer_number) if (wafer_number != obj.wafer_number) return false;
	if (definition.chip) if (chip != obj.chip) return false;
	return true;
}

bool sample_t::operator!=(const sample_t& obj) const
{
	return !(operator==(obj));
}

bool sample_t::operator<(const sample_t& obj) const
{	
	if (lot<obj.lot) return true;
	if (lot>obj.lot) return false;
	
	if (lot_split<obj.lot_split) return true;
	if (lot_split>obj.lot_split) return false;
	
	if (wafer_number<obj.wafer_number) return true;
	if (wafer_number>obj.wafer_number) return false;
	
	if (monitor<obj.monitor) return true;
	if (monitor>obj.monitor) return false;
	
	if (chip.x<obj.chip.x) return true;
	if (chip.x>obj.chip.x) return false;
	if (chip.y<obj.chip.y) return true;
	if (chip.y>obj.chip.y) return false;
	
	if (simple_name<obj.simple_name) return true;
	if (simple_name>obj.simple_name) return false;
	
	return false;
}




