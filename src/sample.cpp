#include "sample.hpp"

/******************************/
/***   sample_t::database   ***/
/******************************/

const string sample_t::database::tablename = "implanted_samples";

sample_t::database::database(sample_t& sample, database_t& sql_wrapper) : sample(sample), sql_wrapper(sql_wrapper)
{
// 	if (!create_table())
// 		logger::error("sample_t::database::database()","could not create sql table","","");
// 	else
// 		table_exists=true;
}

bool sample_t::database::create_table(database_t& sql_wrapper)
{
	std::string sql;
	sql = "CREATE TABLE IF NOT EXISTS " + tablename + "(" \
        "id INTEGER PRIMARY KEY, " \
        "lot                   	 TEXT, " \
        "wafer                  	 INT, " \
        "lot_split            	 TEXT, " \
        "chip_x                  	 INT, " \
        "chip_y                  	 INT, " \
        "monitor                	 TEXT, " \
        "matrix			      	 TEXT, " \
        // 31P1
        "implanted_isotope          	 TEXT, " \
        // 2E15 in at/scm
        "dose	           	 INT, " \
        "maximum_concentration                 INT, " \
        "depth_at_maximum_concentration	INT, "  \
		"comments         			 TEXT);";
   return sql_wrapper.execute_sql(sql);
}

map<string,vector<string>> sample_t::database::load_from_table(isotope_t isotope)
{
	if (!table_exists)
		return {};
	map<string,vector<string>> matching_table_entries;
	string sql1 = "SELECT * FROM " +tablename+ 	" WHERE " \
			"lot='" + sample.lot + "' AND " \
			"wafer=" +std::to_string(sample.wafer)+ " AND " \
			"chip_x=" +std::to_string(sample.chip.x)+ " AND " \
			"chip_y=" +std::to_string(sample.chip.y)+ " AND ";
	if (isotope.symbol=="")
		sql1 += "monitor=" + sample.monitor+"';";
	else
	{
		sql1 += "monitor=" + sample.monitor+" AND " \
				"implanted_isotope=" + isotope.to_string() +	"';";
	}
	if (!sql_wrapper.execute_sql(sql1,database_t::callback_lines_map,&matching_table_entries)) 
		logger::error("sample_t::database::load_from_table()","could not load database table to local map","","returning empty");
	return matching_table_entries;
}

matrix_t sample_t::database::matrix()
{
	map<string,vector<string>> table_entries = load_from_table();
	if (table_entries.size()==0)
	{
		logger::error("sample_t::database::matrix()","table_entries.size()==0","could not find sample in database table " +tablename,"returning empty");
		return  {};
	}
// 	if (table_entries.size()>1)
// 		logger::warning(3,"sample_t::database::matrix()","table_entries.size()>1","multiple entries of same sample in table " +tablename,"using 1st entry");
	return matrix_t(table_entries.at("matrix").at(1));
}

/***************************************************/
/****  sample_t::database::implant_parameters_t ****/
/***************************************************/

dose_t sample_t::database::dose(const isotope_t isotope)
{
	map<string,vector<string>> table_entries = load_from_table();
	if (table_entries.size()==0)
	{
		logger::error("sample_t::database::dose()","table_entries.size()==0","could not find sample " +sample.to_string()+ " and isotope " +isotope.to_string()+  " in database table " +tablename,"returning empty");
		return  {};
	}
	if (table_entries.size()>1)
		logger::error("sample_t::database::dose()","table_entries.size()>1",sample.to_string() + " " + isotope.to_string(),"returning empty");
	
	return dose_t({tools::str::str_to_double(table_entries.at("dose").at(1))});
}


/******************************/
/***   sample_t::database   ***/
/******************************/


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

bool sample_t::chip_t::is_set() const
{
	if (x>=0 && y>=0) return true;
	return false;
}

void sample_t::chip_t::to_screen(string prefix)
{
	cout << prefix << "chip\t" << "X:"<< x << ", Y:"<< y << endl;
}

bool sample_t::chip_t::operator<(const sample_t::chip_t& obj) const
{
	if (x < obj.x) return true;
	if (x > obj.x) return false;
	
	if (y < obj.y) return true;
	if (y > obj.y) return false;
	return false;
}

bool sample_t::chip_t::operator>(const chip_t& obj) const
{
	if (!operator<(obj) && operator!=(obj)) return true;
	return false;
}

const std::__cxx11::string sample_t::chip_t::to_string(const std::__cxx11::string del) const
{
	stringstream out;
	out << "X"<<x << "Y" << y;
	return out.str();
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

/***********************/

// sample_t::sample_t(int& wafer, string& monitor, string& lot, string& lot_split, chip_t chip, string& simple_name, matrix_t& matrix) :
// 		wafer(wafer),monitor(monitor), lot(lot), lot_split(lot_split), chip(chip),simple_name(simple_name), matrix_p(matrix)
// {	
// }

sample_t::sample_t(files_::file_t::name_t& fn,files_::file_t::contents_t& f,database_t& sql_wrapper) : 
																wafer(fn.wafer()), 
																monitor(fn.monitor()),
																lot(fn.lot()),
																lot_split(fn.lot_split()),
																chip(fn.chip_x(),fn.chip_y()),
																simple_name(fn.simple_name()),
																matrix_p(f.matrix()),
																DB(*this,sql_wrapper)
{
// 	cout << "f.matrix() = " << f.matrix().to_string() << endl;
}

sample_t::sample_t(files_::file_t::name_t& fn, database_t& sql_wrapper) : wafer(fn.wafer()), 
																monitor(fn.monitor()),
																lot(fn.lot()),
																lot_split(fn.lot_split()),
																chip(fn.chip_x(),fn.chip_y()),
																simple_name(fn.simple_name()),
																DB(*this,sql_wrapper)
{
}

string sample_t::wafer_string()
{
	stringstream ss;
	if (wafer<1)
		return "";
	if (wafer<10)
		ss << "0";
	ss << wafer;
	return ss.str();
}

matrix_t& sample_t::matrix()
{
	if (matrix_p.is_set()) return matrix_p;
	// do something to populate matrix_p --> look up Database
	if (DB.matrix().is_set())
		matrix_p = DB.matrix();
	return matrix_p;
}

bool sample_t::operator==(sample_t& obj)
{
// 	to_screen();
// 	obj.to_screen();
	if (use_simple_name)
	{
		if (simple_name!=obj.simple_name) return false;
	}
	if (use_lot)
	{
		if (lot!=obj.lot)	return false;
	}
	
	if (use_lot_split)
	{
		if (lot_split!=obj.lot_split) return false;
	}
	
	if (use_wafer)
	{
		if (wafer!=obj.wafer) return false;
	}
	
	if (use_monitor)
	{
		if (monitor!=obj.monitor) return false;
	}
	
	if (use_chip)
	{
		if (chip!=obj.chip) return false;
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
	ss << "chip: " << chip.to_string() << ",";
	ss << "monitor: " << monitor << ",";
	ss << "simple_name: " << simple_name << ",";
	ss << "matrix: ";
	if (matrix().is_set())
		ss << matrix().to_string();
	else 
		ss << "uknown";
	return ss.str();
}

bool sample_t::operator<(sample_t& obj)
{	
	if (use_simple_name)
	{
		if (simple_name < obj.simple_name) return true;
		if (simple_name > obj.simple_name) return false;
	}
	if (use_lot)
	{
		if (lot < obj.lot) return true;
		if (lot > obj.lot) return false;
	}
	if (use_lot_split)
	{
		if (lot_split < obj.lot_split) return true;
		if (lot_split > obj.lot_split) return false;
	}
	if (use_wafer)
	{
		if (wafer < obj.wafer) return true;
		if (wafer > obj.wafer) return false;
	}
	if (use_monitor)
	{
		if (monitor < obj.monitor) return true;
		if (monitor > obj.monitor) return false;
	}
	if (use_chip)
	{
		if (chip < obj.chip) return true;
		if (chip > obj.chip) return false;
	}
	
	return false;
}

bool sample_t::operator>(sample_t& obj)
{
	if (!operator<(obj) && operator!=(obj)) return true;
	return false;
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

