#include "measurement.hpp"

measurements::dsims_t::dsims_t(files::dsims_t* file, sample_t* sample) : sims_t(file,sample)
{
	files.insert(file);
// 	cout << "files.push_back(file) files.back()=" << files.back()<< endl;
// 	if (sample != nullptr)
// 		sample->dsims.insert(this);
}

void measurements::dsims_t::insert_file(files::dsims_t* file_p)
{
	files.insert(file_p);
	measurements::sims_t::insert_file(file_p);	
}

msettings::dsims_t measurements::dsims_t::settings()
{
	return msettings::dsims_t(files);
}

std::__cxx11::string measurements::dsims_t::to_string()
{
	stringstream ss;
	const string del =",";
	ss << measurement_t::to_string() << del;
	ss << settings().to_string();
	return ss.str();
}

files::dsims_t::Ipr_t* measurements::dsims_t::Ipr()
{
	if (Ipr_p.sputter_current.is_set())
		return &Ipr_p;
	for (files::dsims_t* f : files)
	{
		if (f->Ipr().sputter_current.is_set())
		{
			Ipr_p = f->Ipr();
		}
	}
	if (!Ipr_p.sputter_current.is_set())
		return nullptr;
	return &Ipr_p;
}
