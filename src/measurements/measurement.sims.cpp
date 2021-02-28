#include "measurement.hpp"

measurements::sims_t::sims_t(files::sims_t* file, sample_t* sample) : measurement_t(file,sample)
{
	files.insert(file);
}

void measurements::sims_t::insert_file(files::sims_t* file_p)
{	
	files.insert(file_p);
	measurements::measurement_t::insert_file(file_p);
}

msettings::sims_t measurements::sims_t::settings()
{
	return msettings::sims_t(files);
}

std::__cxx11::string measurements::sims_t::to_string()
{
	stringstream ss;
	const string del =",";
	ss << measurement_t::to_string() << del;
// 	cout << "IN" << endl;
	ss << "clusters: " << clusters().size();
	return ss.str();
}

set<cluster_t *> measurements::sims_t::clusters()
{
	set<cluster_t*> cs;
	if (clusters_p.size()==0)
	{
		set<cluster_t> clusters_set;
		for (files::sims_t* f:files)
		{
			if (f->clusters().size()>0)
			{
// 				cout << "f->clusters().size()=" << f->clusters().size() << endl;
				for (auto& c : f->clusters())
					clusters_set.insert(c);
			}
		}
		clusters_p = vector<cluster_t>(clusters_set.begin(),clusters_set.end());
	}
	for (auto& c:clusters_p)
		cs.insert(&c);
	return cs;
}
