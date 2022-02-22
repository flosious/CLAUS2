/* 
 * 	CLever AUtomated Scientist
 * 	Version 2022-02-11_2
 * 	Florian Bärwolf 2022
 *	
 * 
 *  Copyright (C) 2020-2022 Florian Bärwolf
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
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
                        *********************************************************************************************
                        *                                                                                           *
                        *                                       DEDICATION                                          *
                        *                                                                                           *
                        *   This program is dedicated to my uncle, Klaus Bärwolf, who never hesitated to help me.   *
                        *                                                                                           *
                        *********************************************************************************************
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 * Purpose:
 * 		This software mimics the solution of the daily tasks and routines of material scientists within a semiconductor fab: Using very few, but similar methods over and over again.
 * 
 * Motivation:
 * 		This software was created in the hope to be useful in automating calculation of raw data measurements (e.g. intensities) to characteristic material parameters (e.g. concentrations).
 * 		While doing that, it decreases operator errors, saves time and opens new opportunities in decreasing measurement uncertainties.
 * 
 * Functionality:
 * 		fast, modular and reliable
 * 
 * Input:
 * 		exported, raw measurement files of (different) measurement tools
 * 
 * Output:
 * 		evaluated measurement results as files
 *		graphs
 * 
 * Necessities:
 * 		config file(s) (config.conf)
 * 		periodic table of elements (pse.csv)
 *		sqlite3 database filled with reference measurements / samples / ...
 
 */

#include "processor.hpp"

// void handler(int sig) {
//   void *array[10];
//   size_t size;
// 
//   // get void*'s for all entries on the stack
//   size = backtrace(array, 10);
// 
//   // print out all the frames to stderr
//   fprintf(stderr, "Error: signal %d:\n", sig);
//   backtrace_symbols_fd(array, size, STDERR_FILENO);
//   exit(1);
// }


int main( int    argc, char **argv )
{ 
// 	signal(SIGSEGV, handler);
    processor Klaus(tools::str::args_to_vec(argc,argv));
    return 0;
}
