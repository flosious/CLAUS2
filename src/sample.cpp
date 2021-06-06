#include "sample.hpp"

/******************************/
/***   sample_t::db_t   ***/
/******************************/

const string sample_t::db_t::tablename = "implanted_samples";

sample_t::db_t::db_t(const sample_t& sample, const database_t& sql_wrapper) : sample(sample), sql_wrapper(sql_wrapper)
{
// 	if (!create_table())
// 		logger::error("sample_t::db_t::db_t()","could not create sql table","","");
// 	else
// 		table_exists=true;
}

bool sample_t::db_t::create_table(database_t& sql_wrapper)
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
        "isotope          	 TEXT, " \
        // 2E15 in at/scm
        "dose	           	 INT, " \
        "maximum_concentration                 INT, " \
        "depth_at_maximum_concentration	INT, "  \
		"comments         			 TEXT);";
   return sql_wrapper.execute_sql(sql);
}

const map<string,vector<string>>& sample_t::db_t::load_from_table()
{
	logger::debug(9,"sample_t::db_t::load_from_table()","entering");
	if (table_entries_s.size()>0)
	{
		logger::debug(9,"sample_t::db_t::load_from_table()","table_entries_s.size()>0");
		return table_entries_s;
	}

	string sql1 = "SELECT * FROM " +tablename+ 	" WHERE " \
			"lot='" + sample.lot + "' AND " \
			"wafer=" +std::to_string(sample.wafer)+ " AND " \
			"chip_x=" +std::to_string(sample.chip.x)+ " AND " \
			"chip_y=" +std::to_string(sample.chip.y)+ " AND " \
			"monitor='" + sample.monitor+"';";

	logger::debug(11,"sample_t::db_t::load_from_table()","sql1=",sql1);

	if (!sql_wrapper.execute_sql(sql1,database_t::callback_lines_map,&table_entries_s)) 
		logger::error("sample_t::db_t::load_from_table()","could not load db_t table to local map","","returning empty");
	logger::debug(9,"sample_t::db_t::load_from_table()","exiting");
	return table_entries_s;
}

matrix_t sample_t::db_t::matrix()
{
	logger::debug(9,"sample_t::db_t::matrix()","entering");

	if (load_from_table().size()==0 || load_from_table().at("lot").size()==0)
	{
		logger::info(3,"sample_t::db_t::matrix()","table_entries.size()==0","could not find sample in db_t table " +tablename,"returning empty");
		return  {};
	}

	for (auto& matrix_str:load_from_table().at("matrix"))
	{
		if (matrix_str!="")
			return matrix_t(matrix_str);
	}
	logger::info(3,"sample_t::db_t::matrix()","no db entry with matrix set",tablename,"returning empty");
	return {};
}

sample_t::db_t::implant_t sample_t::db_t::implant(const isotope_t& isotope)
{
// 	map<string,vector<string>> table_entries = load_from_table();
	if (load_from_table().size()==0)
	{
		logger::error("sample_t::db_t::implant()","table_entries.size()==0","could not find sample " +sample.lot + "w"+ sample.wafer_string()+ " and isotope " +isotope.to_string()+  " in database table " +tablename,"returning empty");
		return  {};
	}
	if (load_from_table().at("dose").size()>1)
		logger::error("sample_t::db_t::implant()","load_from_table().at('dose').size()>1",sample.lot + "_w"+ sample.wafer_string() + " " + isotope.to_string(),"returning empty");
	
// 	implant.dose = dose_t({tools::str::str_to_double(table_entries.at("dose").at(1))});
	
	return implant_t(isotope,load_from_table());
}

/************************************************/
/***   sample_t::db_t::implant_t::implant_t   ***/
/************************************************/

sample_t::db_t::implant_t::implant_t(const isotope_t& isotope, const map<std::__cxx11::string, vector<std::__cxx11::string> >& table_entries_s)
{
	if (table_entries_s.size()==0)
	{
		logger::error("","");
		return;
	}
	if (table_entries_s.at("isotope").size()==0)
		return;
	int table_entry_row;
	for (table_entry_row=0;table_entry_row<table_entries_s.at("isotope").size();table_entry_row++)
		if (isotope_t(table_entries_s.at("isotope").at(table_entry_row)) == isotope)
			break;
	
	if (table_entries_s.find("dose")!=table_entries_s.end()) 
		dose = dose_t({tools::str::str_to_double(table_entries_s.at("dose").at(table_entry_row))});
	else
		logger::error("sample_t::db_t::implant_t::implant_t()","could not find 'dose' in table",tablename,"skipping");
	
	if (table_entries_s.find("depth_at_maximum_concentration")!=table_entries_s.end()) 
		depth_at_concentration_maxium = sputter_depth_t({tools::str::str_to_double(table_entries_s.at("depth_at_maximum_concentration").at(table_entry_row))});
	else
		logger::error("sample_t::db_t::implant_t::implant_t()","could not find 'depth_at_maximum_concentration' in table",tablename,"skipping");
	
	if (table_entries_s.find("maximum_concentration")!=table_entries_s.end()) 
		concentration_maximum = concentration_t({tools::str::str_to_double(table_entries_s.at("maximum_concentration").at(table_entry_row))});
	else
		logger::error("sample_t::db_t::implant_t::implant_t()","could not find 'maximum_concentration' in table",tablename,"skipping");
}

sample_t::db_t::implant_t::implant_t()
{
}


/******************************/
/***   sample_t::database_t   ***/
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
	if (x<0 && y < 0)
		return "";
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


sample_t::db_t sample_t::database() const
{
	return db_t(*this,*sql_wrapper);
}



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
																sql_wrapper(&sql_wrapper)
// 																database(*this,sql_wrapper)
{
// 	cout << "f.matrix() = " << f.matrix().to_string() << endl;
}

sample_t::sample_t(files_::file_t::name_t& fn, database_t& sql_wrapper) : wafer(fn.wafer()), 
																monitor(fn.monitor()),
																lot(fn.lot()),
																lot_split(fn.lot_split()),
																chip(fn.chip_x(),fn.chip_y()),
																simple_name(fn.simple_name()),
																sql_wrapper(&sql_wrapper)
// 																database(*this,sql_wrapper)
{
}

const string sample_t::wafer_string() const
{
	stringstream ss;
	if (wafer<1)
		return "";
	if (wafer<10)
		ss << "0";
	ss << wafer;
	return ss.str();
}

const matrix_t& sample_t::matrix()
{
	if (matrix_p.is_set()) return matrix_p;
	// do something to populate matrix_p --> look up Database
	if (database().matrix().is_set())
		matrix_p = database().matrix();
// 		return matrix_t(database.matrix());
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
	ss << "lot: " << lot;
	if (lot_split!="")
		ss << lot_split;
	ss << ", wafer: " << wafer;
	if (chip.to_string()!="")
		ss << ", chip: " << chip.to_string();
	if (monitor!="")
		ss << ", monitor: " << monitor;
	if (matrix().is_set())
		ss << ", matrix: " << matrix().to_string();
	if (simple_name!="")
		return simple_name;
// 		ss << "simple_name: " << simple_name << ",";
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

