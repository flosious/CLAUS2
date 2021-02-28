#include "lists.hpp"



list<sample_t> lists::samples;

/*filenames*/
list<filenames::dsims_t> lists::dsims_filenames;
// list<filenames::tofsims_t> lists::tofsims_filenames;

/*files*/
list<files::dsims_t> lists::dsims_files;
// list<files::tofsims_t> lists::tofsims_files;

/*measurements*/
list<measurements::dsims_t> lists::dsims_measurements;
// list<measurements::tofsims_t> lists::tofsims_measurements;

/*measurement_groups*/
list<mgroups::dsims_t> lists::dsims_groups;
// list<mgroups::tofsims_t> lists::tofsims_groups;

void lists::update_measurement_pointers_in_samples()
{
	for (auto& M : dsims_measurements)
		M.sample()->dsims.insert(&M);
// 	for (auto& TM : lists::measurements::tofsims)
// 		TM.sample()->tofsims.insert(&TM);
}


// void lists::feed_measurements_list_dsims(list<files::dsims_t>& file, list< measurements::dsims_t >& measurements)
// {
// 	for (auto& f: file)
// 		feed_measurements_list_dsims(f,measurements);
// }
// 
// void lists::feed_measurements_list_dsims(files::dsims_t& file, list< measurements::dsims_t >& measurements)
// {
// 	measurements::dsims_t* M_p=nullptr;
// 	sample_t* S_p=nullptr;
// 	feed_measurements_list(file, measurements,&M_p,&S_p);
// 	if (M_p!=nullptr && S_p!=nullptr) S_p->dsims.insert(M_p);
// }
