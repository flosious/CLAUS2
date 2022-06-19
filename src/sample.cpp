#include "sample.hpp"

/*******************************/
/** sample_t::db_t::implant_s **/
/*******************************/

sample_t::implant_s::implant_s(quantity::dose_t dose, quantity::concentration_t concentration_maximum, quantity::depth_t depth_at_concentration_maxium) :
								dose(dose), concentration_maximum(concentration_maximum), depth_at_concentration_maxium(depth_at_concentration_maxium)
{
}



/******************************/
/***     sample_t::db_t     ***/
/******************************/

const string sample_t::db_t::tablename = "implanted_samples";

sample_t::db_t::db_t(const sample_t& sample, const database_t& sql_wrapper) : sample(sample), sql_wrapper(sql_wrapper)
{
// 	if (!create_table())
// 		logger::error("sample_t::db_t::db_t()","could not create sql table","","");
// 	else
// 		table_exists=true;
}

const map<isotope_t, sample_t::implant_s>& sample_t::db_t::implants()
{
	load_from_table();
	return implants_s;
}


///load entries from old db into this(new)
bool sample_t::db_t::migrate_claus1_db(database_t& sql_wrapper, const string filename)
{
	
	sqlite3* claus1_sql_handle=nullptr;
	database_t claus1(claus1_sql_handle,filename);
	if (!claus1.open())
	{
		logger::error("sample_t::db_t::migrate_claus1_db()","!claus1.open",filename);
		return false;
	}
	map<string,vector<string>> table_entries_s;
	stringstream sql;
	sql << "SELECT * FROM everything;";
	if (!claus1.execute_sql(sql.str(),database_t::callback_lines_map,&table_entries_s)) 
	{
		logger::error("sample_t::db_t::migrate_claus1_db()","!claus1.execute_sql",sql.str(),claus1.file_location);
		return false;
	}
	if (table_entries_s.size()==0)
	{
		logger::error("sample_t::db_t::migrate_claus1_db()","table_entries_s.size()==0");
		return false;
	}
	unsigned int size = table_entries_s.at("lot").size();
	logger::info(1,"sample_t::db_t::migrate_claus1_db()","table_entries=" + tools::to_string(size));
	for (int i=0;i<size;i++)
	{
		sql.str("");
		sql  << "INSERT INTO " << tablename; 
		sql << " (lot, wafer, lot_split, chip_x, chip_y, monitor, matrix, isotope, dose, maximum_concentration, depth_at_maximum_concentration, comments)";
		sql << " VALUES (";
		sql << "'" << table_entries_s.at("lot").at(i)<<"', ";
		sql << table_entries_s.at("wafer").at(i) << ", ";
		if (table_entries_s.at("lot_split").at(i)=="NULL")
			sql << "'', ";
		else
			sql << "'" << table_entries_s.at("lot_split").at(i) << "', ";
		if (table_entries_s.at("chip_x").at(i)!="-1") 
			sql << table_entries_s.at("chip_x").at(i) << ", ";
		else
			sql << "null, ";
		if (table_entries_s.at("chip_y").at(i)!="-1") 
			sql << table_entries_s.at("chip_y").at(i) << ", ";
		else
			sql << "null, ";
		sql << "'" << table_entries_s.at("monitor").at(i) << "', ";
		
		sql << "'" << table_entries_s.at("matrix_elements").at(i) << "', ";
		sql << "'" << table_entries_s.at("implant_isotope").at(i) << "', ";
		
		if (table_entries_s.at("dose").at(i)=="")
			sql << "null, ";
		else 
			sql << table_entries_s.at("dose").at(i) << ", ";
		
		if (table_entries_s.at("maximum_concentration").at(i)=="")
			sql << "null, ";
		else 
			sql << table_entries_s.at("maximum_concentration").at(i) << ", ";
		
		if (table_entries_s.at("depth_at_maximum_concentration").at(i)=="")
			sql << "null, ";
		else 
			sql << table_entries_s.at("depth_at_maximum_concentration").at(i) << ", ";
		
		sql << "'" << table_entries_s.at("comments").at(i) << "');";
		if (!sql_wrapper.execute_sql(sql.str()))
			logger::error("sample_t::db_t::migrate_claus1_db()","sql_wrapper.execute_sql",sql.str());
		else
			logger::info(1,"sample_t::db_t::migrate_claus1_db()","SUCCESS: ",sql.str());
	}
	return true;
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

bool sample_t::db_t::load_from_table()
{
	logger::debug(logger_verbosity_offset+6,"sample_t::db_t::load_from_table()","entering");
	if (implants_s.size()>0 || matrix_s.is_set()) // already loaded
		return true;
	map<string,vector<string>> table_entries_s;
			
	string sql1	= "SELECT * FROM " +tablename+ 	" WHERE " \
			"lot='" + sample.lot + "'" \
			" AND wafer=" +std::to_string(sample.wafer);
	
	if (sample.chip.x > -1)
		sql1 += " AND chip_x=" + std::to_string(sample.chip.x);
	if (sample.chip.y > -1)
		sql1 += " AND chip_y=" + std::to_string(sample.chip.y);
	if (sample.monitor != "")
		sql1 += " AND monitor='" + sample.monitor+"'";
	sql1 += ";";

	logger::debug(logger_verbosity_offset+5,"sample_t::db_t::load_from_table()","sql1=",sql1);

	if (!sql_wrapper.execute_sql(sql1,database_t::callback_lines_map,&table_entries_s)) 
	{
		logger::error("sample_t::db_t::load_from_table()","could not load db_t table to local map","","returning false");
		return false;
	}
	
	if (table_entries_s.size()==0)
	{
		logger::debug(logger_verbosity_offset+5,"sample_t::db_t::load_from_table()", sample.to_name() + " DB: no table entries","","returning false");
		return false;
	}
	
	/*set matrix*/
	if (!matrix_s.is_set())
	{
		for (auto& m : table_entries_s.at("matrix"))
		{
			if (m=="" || m=="NULL") continue;
			matrix_t M(m);
			if (!M.is_set())
			{
				logger::error("sample_t::db_t::load_from_table()","db matrix is not parseable",m,sample.to_name());
			}
			matrix_s = M;
			if (matrix_s.is_set())
				logger::info(3,"sample_t::db_t::load_from_table()",sample.to_name()+" DB: matrix:", matrix_s.to_string());
			break;
		}
	}
	
	int line=-1;
	/*set implants*/
	for (auto& i : table_entries_s.at("isotope"))
	{
		line++;
		if (i=="" || i=="NULL") continue;
		isotope_t iso(i);
		if (iso.symbol=="") continue;
		if (implants_s.find(iso)!=implants_s.end())
		{
			logger::warning(3,"sample_t::db_t::load_from_table()","more then 1 identical isotope implanted in sample: check database",sample.to_name() +"  " +iso.to_string(),"skipping");
			continue;
		}
		quantity::dose_t D;
		quantity::concentration_t C_max;
		quantity::depth_t SD_max;
		if (table_entries_s.at("dose").at(line)!="" && table_entries_s.at("dose").at(line)!="NULL")
			D = quantity::dose_t({tools::str::str_to_double(table_entries_s.at("dose").at(line))});
		if (table_entries_s.at("depth_at_maximum_concentration").at(line)!="" && table_entries_s.at("depth_at_maximum_concentration").at(line)!="NULL")
			SD_max = quantity::depth_t({tools::str::str_to_double(table_entries_s.at("depth_at_maximum_concentration").at(line))});
		if (table_entries_s.at("maximum_concentration").at(line)!="" && table_entries_s.at("maximum_concentration").at(line)!="NULL")
			C_max = quantity::concentration_t({tools::str::str_to_double(table_entries_s.at("maximum_concentration").at(line))});
		if (D.is_set())
			logger::info(3,"sample_t::db_t::load_from_table()",sample.to_name() +" DB: dose("+iso.to_string()+")="+D.to_string());
		if (SD_max.is_set())
			logger::info(3,"sample_t::db_t::load_from_table()",sample.to_name() + " DB: depth_at_maximum_concentration("+iso.to_string()+")="+SD_max.to_string());
		if (C_max.is_set())
			logger::info(3,"sample_t::db_t::load_from_table()",sample.to_name() + " DB: maximum_concentration("+iso.to_string()+")="+C_max.to_string());
		implant_s I{D,C_max,SD_max};
		implants_s.insert(pair<isotope_t,implant_s>(iso,I));
	}
	
	logger::debug(logger_verbosity_offset+6,"sample_t::db_t::load_from_table()","exiting");
	return true;
}

sample_t::matrix_t& sample_t::db_t::matrix()
{
	logger::debug(logger_verbosity_offset+6,"sample_t::db_t::matrix()","entering");
	if (matrix_s.is_set())
		return matrix_s;
	if (!load_from_table())
	{
		logger::debug(logger_verbosity_offset+4,"sample_t::db_t::matrix()","!load_from_table()","could not find " + sample.to_name()+ " in " +tablename,"returning empty");
		return  matrix_s;
	}

	return matrix_s;
}

sample_t::implant_s sample_t::db_t::implant(const isotope_t& isotope)
{
	if (implants().size()!=0 || load_from_table())
	{
		if (implants().find(isotope)!=implants().end())
			return implants().at(isotope);
	}
	return {};
}



/******************************/
/***   sample_t::chip_t     ***/
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

const string sample_t::chip_t::to_string(const string del) const
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

sample_t::implant_s sample_t::implant(const isotope_t& isotope)
{
	if (implants.size()==0)
		load_from_database();

	if (implants.find(isotope)!=implants.end())
		return implants.at(isotope);
	return {};
}


void sample_t::load_from_database()
{
	db_t db(*this,*sql_wrapper);
	if (!matrix_p.is_set())
		matrix_p = db.matrix();
	if (!matrix_p.is_set())
		return;
	if (implants.size()==0)
		implants = db.implants();
}

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


sample_t::matrix_t& sample_t::matrix()
{
	if (matrix_p.is_set()) return matrix_p;
	// do something to populate matrix_p --> look up Database
	load_from_database();
	
	return matrix_p;
}

bool sample_t::operator==(const sample_t& obj) const
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

bool sample_t::operator!=(const sample_t& obj) const
{
	return !operator==(obj);
}

string sample_t::to_string(const string del)
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
// 	if (simple_name!="")
// 		return simple_name;
// 		ss << "simple_name: " << simple_name << ",";
	return ss.str();
}

string sample_t::to_name(const string del) const
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
// 	if (simple_name!="")
// 		return simple_name;
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

