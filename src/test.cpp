#include "test.hpp"

/***** FILE  ******/

const std::__cxx11::string & Tfile_t::Tcontents_t::contents_string()
{
	if (contents_string_p=="")
	{
		contents_string_p=tools::file::load_file_to_string(filename_with_path);
		cout << "reading file" << endl;
	}
	return contents_string_p;
}


bool Tfile_t::operator==(Tfile_t& obj)
{
	if (name==obj.name)
	{
		return true;
	}
	return false;
}

bool Tfile_t::Tname_t::operator==(Tfile_t::Tname_t& obj)
{
	if (filename_with_path==obj.filename_with_path)
		return true;
	return false;
}

Tfile_t::Tcontents_t::Tcontents_t(const std::__cxx11::string& filename_with_path_s, 
								  const std::__cxx11::string delimiter_s, 
								  const set<std::__cxx11::string> identifiers_s
								  ) 
									:
								  filename_with_path(filename_with_path_s),
								  delimiter(delimiter_s),
								  identifiers(identifiers_s)
{
}

Tfile_t::Tname_t::Tname_t(const std::__cxx11::string& filename_with_path_s, 
						  const std::__cxx11::string delimiter_s, 
						  const set<std::__cxx11::string> identifiers_s) 
							:
						  filename_with_path(filename_with_path_s),
						  delimiter(delimiter_s),
						  identifiers(identifiers_s)
{
}

Tfile_t::Tfile_t(Tfile_t::Tname_t& name_s, 
				 Tfile_t::Tcontents_t& contents_s) 
					: 
				 name(name_s), 
				 contents(contents_s)
{
}


/***** SIMS *******/

Tfiles::Tsims_t::Tcontents_t::Tcontents_t(const std::__cxx11::string& filename_with_path_s, 
										  const std::__cxx11::string delimiter_s, 
										  const set<std::__cxx11::string> identifiers_s
										  )
											:
											Tfile_t::Tcontents_t(filename_with_path_s,delimiter_s,identifiers_s)
											
{
}

Tfiles::Tsims_t::Tname_t::Tname_t(const std::__cxx11::string& filename_with_path_s, 
								  const std::__cxx11::string delimiter_s, 
								  const set<std::__cxx11::string> identifiers_s)
									:
									Tfile_t::Tname_t(filename_with_path_s, delimiter_s, identifiers_s)
{
}

Tfiles::Tsims_t::Tsims_t(Tfiles::Tsims_t::Tname_t& name_s, 
						 Tfiles::Tsims_t::Tcontents_t& contents_s) 
								:
								Tfile_t(name_s,contents_s),name(name_s),contents(contents_s)
{
}

/*** DSIMS ****/

Tfiles::Tdsims_dp_rpc_asc_t::Tcontents_t::Tcontents_t(const std::__cxx11::string& filename_with_path_s
													  )
														:
					Tfiles::Tsims_t::Tcontents_t(filename_with_path_s,"dsims contents delimiter",{"dsims contents identifiers"})
{
}

Tfiles::Tdsims_dp_rpc_asc_t::Tname_t::Tname_t(const std::__cxx11::string& filename_with_path_s) :
			Tfiles::Tsims_t::Tname_t(filename_with_path_s,"dsims name delimiter",{"dsims name identifiers"})
{
}


Tfiles::Tdsims_dp_rpc_asc_t::Tdsims_dp_rpc_asc_t(Tname_t& name_s, Tcontents_t& contents_s) :
								name(name_s), contents(contents_s), Tfiles::Tsims_t(name_s,contents_s)
// 												 contents(filename_with_path_s,contents_s),
// 												 name(filename_with_path_s)
// 												 Tfiles::Tsims_t(name,contents)
{
// 	Tfiles::Tsims_t::contents = &contents;
// 	Tfiles::Tsims_t::name = &name;
// 	Tfile_t::contents = &contents;
// 	Tfile_t::name = &name;
}

Tfiles::Tdsims::Tdsims(std::__cxx11::string filename_with_path_s) :
			contents(filename_with_path_s),name(filename_with_path_s), dsims_dp_rpc_asc(name,contents)
{
}



