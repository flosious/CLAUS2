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



/***********************/
/*****   STATICS   *****/
/***********************/


bool sample_t::use_lot=true;
bool sample_t::use_lot_split=true; // identification by wafer is normaly sufficient 
bool sample_t::use_wafer=true;
bool sample_t::use_monitor=true;
bool sample_t::use_chip=true;
bool sample_t::use_simple_name=true;

// vector<sample_t> sample_t::samples_list_p;
// 
// vector<sample_t>* sample_t::samples_list()
// {
// 	bool inserted;
// 	for (auto& f: files::files_list())
// 	{
// 		sample_t sample(f);
// 		inserted = false;
// 		for (auto& s : samples_list_p)
// 		{
// 			if (s==sample)
// 			{
// 				s.files.insert(f);
// 				inserted = true;
// 			}
// 		}
// 		if (!inserted) samples_list_p.push_back(sample);
// 	}
// 	if (samples_list_p.size()==0) return nullptr;
// 	return &samples_list_p;
// }


/***********************/

// sample_t::sample_t(int& wafer, string& monitor, string& lot, string& lot_split, chip_t chip, string& simple_name, matrix_t& matrix) :
// 		wafer(wafer),monitor(monitor), lot(lot), lot_split(lot_split), chip(chip),simple_name(simple_name), matrix_p(matrix)
// {	
// }

sample_t::sample_t(files::file_t::name_t& fn,files::file_t::contents_t& f) : 
																wafer(fn.wafer()), 
																monitor(fn.monitor()),
																lot(fn.lot()),
																lot_split(fn.lot_split()),
																chip(fn.chip_x(),fn.chip_y()),
																simple_name(fn.simple_name()),
																matrix_p(f.matrix())
{
}

sample_t::sample_t(files::file_t::name_t& fn) : wafer(fn.wafer()), 
																monitor(fn.monitor()),
																lot(fn.lot()),
																lot_split(fn.lot_split()),
																chip(fn.chip_x(),fn.chip_y()),
																simple_name(fn.simple_name())
{
}

matrix_t& sample_t::matrix()
{
	if (matrix_p.is_set()) return matrix_p;
	// do something to populate matrix_p --> look up Database
	return matrix_p;
}

// void sample_t::to_screen(std::__cxx11::string prefix)
// {
// 	cout << prefix << "lot\t" << lot() << endl;
// 	cout << prefix << "lot_split\t" << lot_split() << endl;
// 	cout << prefix << "wafer\t" << wafer() << endl;
// 	chip().to_screen(prefix);
// 	cout << prefix << "monitor\t" << monitor() << endl;
// 	cout << prefix << "simple_name\t" << simple_name() << endl;
// 	cout << prefix << "matrix\t" << matrix().to_string() << endl;
// }

/*
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
	
	return false;
}*/

bool sample_t::operator==(sample_t& obj)
{
// 	to_screen();
// 	obj.to_screen();
	if (use_lot)
	{
// 		cout << "use_lot" << endl;
		if (lot!=obj.lot)
		{
// 			cout << "sample lot different" << endl;
			return false;
		}
	}
	
	if (use_lot_split)
	{
// 		cout << "use_lot_split" << endl;
		if (lot_split!=obj.lot_split) return false;
	}
	
	if (use_wafer)
	{
// 		cout << "use_wafer" << endl;
		if (wafer!=obj.wafer) return false;
	}
	
	if (use_monitor)
	{
// 		cout << "use_monitor" << endl;
		if (monitor!=obj.monitor) return false;
	}
	
	if (use_chip)
	{
// 		cout << "use_chip" << endl;
		if (chip!=obj.chip) return false;
	}
	
	if (use_simple_name)
	{
// 		cout << "use_simple_name" << endl;
		if (simple_name!=obj.simple_name) return false;
	}
	return true;
}

bool sample_t::operator!=(sample_t& obj)
{
	return !operator==(obj);
}

std::__cxx11::string sample_t::to_string(const string del)
{
	stringstream ss;
	ss << "lot: " << lot << ",";
	ss << "lot_split: " << lot_split << ",";
	ss << "wafer: " << wafer << ",";
	ss << "chip: " << "x:" << chip.x << " y:" <<chip.y << ",";
	ss << "monitor: " << monitor << ",";
	ss << "simple_name: " << simple_name << ",";
	ss << "matrix: ";
	if (matrix().is_set())
		ss << matrix().to_string();
	else 
		ss << "uknown";
	return ss.str();
}

// const string sample_t::db_tablename {"samples"};
// bool sample_t::create_table()
// {
//     std::string sql;
//     sql = "CREATE TABLE IF NOT EXISTS " + db_tablename + "(" \
//          "sample_id INTEGER PRIMARY KEY, " \
//          "lot                   	 TEXT, " \
//          "wafer                  	 INT, " \
//          "lot_split            	     TEXT, " \
//          "chip_x                  	 INT, " \
//          "chip_y                  	 INT, " \
//          "monitor                	 TEXT, " \
// 		 "matrix	                 TEXT, " \
// 		 "matrix_thickness      	 REAL, " \
//          "isotope                  	 TEXT, " \
//          // 2E15 in at/scm
//          "dose	                	 INT);";
//     return db.execute_sql(sql);
// }

