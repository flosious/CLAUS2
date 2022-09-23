/*
	Copyright (C) 2020 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef database_HPP
#define database_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h> 
#include <regex>

#include "tools.hpp"
#include "log.hpp"
// #include "measurement.hpp"
// #include "isotope.hpp"
// #include "sample.hpp"
// #include "matrix.hpp"
// #include "quantity.hpp"
// #include "element.hpp"
// #include "isotope.hpp"
// #include "ion.hpp"
// #include "msettings.hpp"
// #include "cluster.hpp"


/*FORWARD DECLARATIONS*/
// class sample_t;
// class measurements_;
// class measurements_::dsims_t;
/**********************/

// Check out this link. The easiest way is to do the locking yourself, 
// and to avoid sharing the connection between threads. 
// Another good resource can be found here, and it concludes with:
// 
//     Make sure you're compiling SQLite with -DTHREADSAFE=1.
// 
//     Make sure that each thread opens the database file and keeps
//     its own sqlite structure.
// 
//     Make sure you handle the likely possibility that one or more 
//     threads collide when they access the db file at the same time: handle SQLITE_BUSY appropriately.
// 
//     Make sure you enclose within transactions the commands that 
//     modify the database file, like INSERT, UPDATE, DELETE, and others.
// 

using namespace std;


/***********************/
/***   database_t    ***/
/***********************/

///this is a warpper for sqlite3
class database_t {
//	friend class config_t;
// 	friend class processor;
private:
    class_logger_t logger;
	static const int logger_verbosity_offset = 3;
    sqlite3* sql_handle=nullptr;
    bool close();
	/// returns -1 if error, or the id of the last entry in the table
    int get_last_autoID_from_table(string table);
	///just allow the processor to open and close
	
	
	///just allow the processor to open and close
	
public:
    string file_location = "/home/florian/projects/claus2/build/database.sqlite3";
	database_t(sqlite3* sql_handle);
	database_t(sqlite3* sql_handle, string filename);
	~database_t();
    bool open();
	bool execute_sql(string sql, int (*func_ptr)(void*,int,char**,char**)=NULL, void* func_arg=nullptr) const;
	
	/*call backs*/
	///general callback function -> populates *ptr -> matrix(vector(vector(string)))
	static int callback_lines_cols(void *ptr, int argc, char **argv, char **azColName);
	///general callback function -> populates *ptr -> map(colname,vector(string) lines ))
	static int callback_lines_map(void *ptr, int argc, char **argv, char **azColName);
};

/// sqlite3
extern Logger global_logger;
#endif // database_HPP
