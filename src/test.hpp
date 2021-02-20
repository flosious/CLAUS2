

#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include "quantity.hpp"
#include "element.hpp"
#include "cluster.hpp"

using namespace std;

/*

filenames --> fileS --> measurementS --> MGS

MG<dsims>
	- RSF
	- measurementS<dsims>
		- fileS
		- crater
		- clusters
		- sample
			- matrix

			
BSP:
MG(filename 1:9) -> M(filename 1:9)	-> files:	-> file(filename 1)
												-> file(filename 4)
													-> contents(filename1)
														-> matrix
														-> clusters : cluster -> isotope	-> concentration 
																							-> intensity
														-> settings
															-> sputter_energy
															-> sputter_ion
													-> name(filename1)
														-> lot
														-> lot_split
														-> wafer
														-> chip
														-> monitor
														-> sputter_energy
														-> sputter_ion
									-> crater(contents):
												-> sputter_time
												-> sputter_depth
												-> total_sputter_depth
												-> total_sputter_time
												-> sputter_rate
						sample is an interface to results outside of this particular M or MG
									-> sample:	-> isotopes : isotope(cluster)
												-> matrix
												
									-> isotopes : isotope(cluster)
								
				-> M(filename 2:9)	->files:	->file(filename 2)
												->file(filename 3)
												->file(filename 9)
												
				-> M(filename 3:9)	->files:	->file(filename 5)
												->file(filename 6)
												->file(filename 7)
												->file(filename 8)
*/







class Fn_t
{
private:
	const string& filename;
public:
	bool is_set();
	Fn_t(const string& filename);
	void print();
	bool operator==(const Fn_t &obj);
};

class Fc_t
{
private:
	const string& filename;
public:
	bool is_set();
	void print();
	Fc_t(const string& filename);
	bool operator==(Fc_t &obj);
};

class F_t
{
private:
	const string& filename;
	Fn_t fn_p;
	Fc_t fc_p;
public:
	Fn_t fn();
	Fc_t fc();
	bool is_set();
	F_t(const string& filename);
	bool operator==(F_t &obj);
};

///sample
class S_t
{
public:
// 	S_t(Fn_)
};

class C_t
{
private:
	vector<isotope_t> isotopes;
public:
	C_t(const string clustername);
	bool is_set();
};

///will use the [0] non empty entry of filenames for initialization
class M_t
{
	vector< C_t > clusters_p; // all cluster
public:
	M_t(vector<string>& filenames);
	C_t& reference_cluster(); // --> returns the corresponding matrix cluster
	vector<C_t>& clusters(); // --> all clusters, except matrix cluster
	bool operator==(M_t &obj);
	vector<F_t> Fs;
};

class MG_t
{
public:
	// RSF_t const_RSF("mad","quantil:,25,50"); --> applies const RSF method to all measurements
	MG_t(vector<string>& filenames);
	vector<M_t> Ms;
};

#endif // TEST_HPP
