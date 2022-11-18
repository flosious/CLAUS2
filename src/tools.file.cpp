#include "tools.hpp"



int tools::file::is_file_or_folder(const std::string file_or_folder)
{
    if (std::filesystem::is_directory(file_or_folder))
        return 2;
    return 1;
//    struct stat s;
//    if (stat(file_or_folder.c_str(),&s) == 0)
//    {
//        if (s.st_mode & S_IFDIR)
//            return 2; // directory
//        else if (s.st_mode & S_IFREG)
//            return 1; // file
//        else
//            return 0; // sth else
//    }
//    return -1; // error
}

time_t tools::file::creation_date(std::string filename)
{
	struct stat t_stat;
// 	std::cout << filename << std::endl;
	stat(filename.c_str(),&t_stat);
// 	stat("data/51398_SJZ649_w4_450VO+_2.dp_rpc_asc",&t_stat);
	struct tm *creation_date = localtime(&t_stat.st_ctime);
// 	asctime(creation_date);
// 	printf("File time and date: %s",    asctime(creation_date));
	return t_stat.st_ctime;
// 	time_t test = 0;
// 	return test;
}

std::vector<std::string> tools::file::filter_files_from_types(std::vector<std::string> * file_list,std::vector<std::string> * file_types,    bool exclude)
{
    std::vector<std::string> included_files,    excluded_files;
    bool found=false;
    for (int i=0;i<file_list->size();i++) { // iterate the file_list
    found = false;
    for (int j=0;j<file_types->size();j++) { // iterate the config file endings (cfg or conf ...)
    if (file_list->at(i).find(file_types->at(j),file_list->at(i).length()-file_types->at(j).length())!=std::string::npos) {
    included_files.push_back(file_list->at(i));
    found = true;
    break;
    }
    }
    if (!found) excluded_files.push_back(file_list->at(i));
    }
    if (exclude) return excluded_files;
    return included_files;
}

std::unordered_set<std::string> tools::file::filter_files_from_types(std::unordered_set<std::string>& file_list,std::unordered_set<std::string>& file_types,    bool exclude)
{
std::unordered_set<std::string> included_files,    excluded_files;
    bool found=false;
	for (auto& file:file_list)
	{ 
    found = false;
		for (auto& file_type:file_types)
		{
    if (file.find(file_type,file.length()-file_type.length())!=std::string::npos)
			{
    included_files.insert(file);
    found = true;
    break;
    }
    }
    if (!found) excluded_files.insert(file);
    }
    if (exclude) return excluded_files;
    return included_files;
}

void tools::file::remove_file_type_from_name(std::string* filename,std::string filetye_delimiter)
{
	if (filetye_delimiter=="") return;
	std::vector<std::string> file_parts=tools::str::get_strings_between_delimiter(*filename,filetye_delimiter);
	*filename = file_parts[0];
}

void tools::file::remove_file_type_from_name(std::vector<std::string>* filenames,std::string filetye_delimiter)
{
	for (int i=0;i<filenames->size();i++)
	{
		remove_file_type_from_name(&filenames->at(i),filetye_delimiter);
	}
}


/*remove path structure and optinally file ending from the name*/
std::string tools::file::extract_filetype_ending(std::string filename,std::string filetye_delimiter,std::string path_delimiter)
{
	if (filetye_delimiter=="") return "";
    std::string filename_with_type=(tools::str::get_strings_between_delimiter(filename,path_delimiter)).back();
	std::string filename_type=tools::str::get_strings_between_delimiter(filename_with_type,filetye_delimiter).back();
    return filename_type;
}

/*remove path structure and optinally file ending from the name*/
std::string tools::file::extract_filename(std::string filename,std::string filetye_delimiter,std::string path_delimiter) {
    std::string filename_with_type=(tools::str::get_strings_between_delimiter(filename,path_delimiter)).back();
	if (filetye_delimiter=="") return filename_with_type;
	std::vector<std::string> file_parts=tools::str::get_strings_between_delimiter(filename_with_type,filetye_delimiter);
	
	if (file_parts.size()<2) return filename;
    return file_parts[0];
}

/*remove path structure and optinally file ending from the name*/
std::vector<std::string> tools::file::extract_filenames(std::vector<std::string> filenames,std::string filetye_delimiter,std::string path_delimiter) {
std::vector<std::string> extraced_filenames;
	for (int i=0;i<filenames.size();i++) {
    extraced_filenames.push_back(extract_filename(filenames[i],filetye_delimiter,path_delimiter));
    }
	return extraced_filenames;
}

std::string tools::file::extract_directory_from_filename(std::string filename,std::string path_delimiter) {

	std::string directory="";
    std::vector<std::string> directory_parts = tools::str::get_strings_between_delimiter(filename,path_delimiter);
	if (directory_parts.size()<2) {
        directory=std::string(".")+path_delimiter;
		return directory;
	}
	for (int i=0; i<directory_parts.size()-1;i++) {
		directory.append(directory_parts[i]);
        directory.append(path_delimiter);
	}
    if (directory.compare(directory.size()-path_delimiter.size(),path_delimiter.size(),path_delimiter)!=0) {
        directory.append(path_delimiter);
	}
	return directory;
}


void tools::file::remove_files(std::vector<std::string> files) {
    for (int i=0;i<files.size();i++) {
    remove(    files[i].c_str() );
    }
    return;
}

std::vector<std::string> tools::file::load_file_to_vector_string(std::string filename_with_path) {
std::vector<std::string> contents;
    #ifdef __unix__
    #else
	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
    #endif
    std::ifstream myfile;
	std::string line;
    myfile.open (filename_with_path.c_str(),std::ios::in);
	if (!myfile.is_open()) return contents;
	contents.clear();
    while (    getline (myfile,line) ) {
		contents.push_back(tools::str::remove_linebreakers_from_string(line));
	}
	myfile.close();
	return contents;
}

// OBSOLET
// std::string tools::file::load_file_to_string(std::string filename_with_path) {
// std::string contents="";
//     #ifdef __unix__
//     #else
// 	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
//     #endif
// 	streampos size;
// 	char * memblock;
// 	ifstream myfile;
// 	myfile.clear();
// std::string line="";
// 	myfile.open (filename_with_path.c_str(),    ios::in|ios::binary|ios::ate);
// 	if (!myfile.is_open()) return contents;
// 	size = myfile.tellg();
// 	
//     memblock = new char [size];
// 	
//     myfile.seekg (0,    ios::beg);
//     myfile.read (memblock,    size);
//     myfile.close();
// 	/// ERROR ///
// 	contents = std::string(memblock);
// 	std::cout << "contents.size()="<std::<contents.size() << std::endl;
// 	/////////////
// 	delete[] memblock;
// 	return contents;
// }

std::string tools::file::load_file_to_string(std::string filename_with_path)
{
//    filename_with_path.insert(0,"\"");
//    filename_with_path.append("\"");
	std::ifstream in(filename_with_path,std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0,std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0,std::ios::beg);
        in.read(&contents[0],    contents.size());
		in.close();
		return(contents);
	}
// 	throw(errno);
	return "";
}


std::vector<std::vector<std::string>> tools::file::load(std::string filename,std::string delimiter)
{

//    filename.insert(0,"\"");
//    filename.append("\"");
    std::vector<std::vector<std::string>> mat;
    std::ifstream myfile;
    std::string line;
	myfile.open (filename.c_str());
	if (!myfile.is_open()) return mat;
    while (    getline (myfile,line) )     {
		line = tools::str::remove_linebreakers_from_string(line);
    mat.push_back(tools::str::get_strings_between_delimiter(line,delimiter));
    }
    myfile.close();
    return mat;
}

bool tools::file::write_mat_to_file(std::string filename_with_path,std::vector<std::vector<double>> *matrix,    bool append,std::string column_delimiter,std::string line_delimiter) {
    std::ofstream myfile;
    if (append) myfile.open (filename_with_path.c_str(),    std::ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) return false;
    for (int l=0;l<matrix->size();l++) {
    for (int c=0;c<matrix->at(l).size()-1;c++) {
    myfile << to_string(matrix->at(l)[c]);
    myfile << column_delimiter;
    }
    myfile << to_string((matrix->at(l)).back());
    myfile << line_delimiter;
    }
	myfile.close();
	return true;
}

bool tools::file::write_mat_to_file(std::string filename_with_path,std::vector<std::vector<std::string>> *matrix,    bool append,std::string column_delimiter,std::string line_delimiter) {
    std::ofstream myfile;
    if (append) myfile.open (filename_with_path.c_str(),    std::ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) return false;
    for (int l=0;l<matrix->size();l++) {
    for (int c=0;c<matrix->at(l).size()-1;c++) {
    myfile << matrix->at(l)[c];
    myfile << column_delimiter;
    }
    myfile << (matrix->at(l)).back();
    myfile << line_delimiter;
    }
	myfile.close();
	return true;
}

bool tools::file::write_to_file(std::string filename_with_path,    const std::string *contents,    bool append)
{
    #ifdef __unix__
#else
	tools::str::replace_umlaute_to_windows_compatible(&filename_with_path);
#endif
    std::ofstream myfile;
    if (append) myfile.open (filename_with_path.c_str(),   std::ios::app);
	else myfile.open (filename_with_path.c_str());
	if (!myfile.is_open()) 
		return false;
    myfile << *contents;
	myfile.close();
	return true;
}

bool tools::file::write_to_file_and_create_folders(std::string filename_with_path,    const std::string *contents,    bool append)
{
	auto parts = tools::str::get_strings_between_delimiter(filename_with_path,PATH_DELIMITER);
	std::string filename = parts.back();
	std::stringstream path;
	for (int i=0;i<parts.size()-1;i++)
        path << parts[i] << PATH_DELIMITER;
	std::string path_s = path.str();
// 	std::cout << std::endl << path_s << " " << filename << std::endl;
	return write_to_file_and_create_folders(filename,path_s,contents,append);
}


bool tools::file::write_to_file_and_create_folders(std::string filename,std::string path,    const std::string *contents,    bool append)
{
	path = tools::file::check_directory_string(path);
	tools::file::mkpath(path,0750);
	return tools::file::write_to_file(path+filename,contents,append);
}

std::string tools::file::check_directory_string(std::string add_directory,std::string root_dir) {
    /*if empty,    then return the working dir*/
	if (add_directory=="" && root_dir=="") return "";

    /*if empty,    then return the root dir*/
	if (add_directory=="") return tools::file::check_directory_string(root_dir,"");
	
    /*add path_delimiter at end of add_directory,    if not exists*/
    if (add_directory.compare(add_directory.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) {
    add_directory.append(PATH_DELIMITER);
    }

    /*if add_directory is a directory structure or root_dir is "",    then return add_directory*/
    if (add_directory.compare(0,PATH_DELIMITER.size(),PATH_DELIMITER)==0 || add_directory.find(":")!=std::string::npos || root_dir=="")
    return add_directory;

    /*add PATH_DELIMITER at end of root_directory,    if not exists*/
    if (root_dir.compare(root_dir.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) {
    root_dir.append(PATH_DELIMITER);
    }

    /*so add_directory is NOT a directory structure,    then return root_dir + add_directory*/
	return root_dir+add_directory;
}

std::string tools::file::check_directory_string(std::string directory) {
	if (directory=="") 
	{
		directory.append(".");
		directory.append(PATH_DELIMITER);
		return directory;
	}
	/*add path delimiter at the end if not exist*/
	if (directory.compare(directory.size()-PATH_DELIMITER.size(),PATH_DELIMITER.size(),PATH_DELIMITER)!=0) 
	{
		directory.append(PATH_DELIMITER);
	}
	return directory;
}


#ifdef __unix__
typedef struct stat Stat;

int tools::file::do_mkdir(const char *path,    mode_t mode)
{
    Stat     st;
    int     status = 0;

    if (stat(path,    &st) != 0)
    {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(path,    mode) != 0 && errno != EEXIST)
    status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
    errno = ENOTDIR;
    status = -1;
    }

    return(status);
}
/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists,    rather than optimistically creating
** the last element and working backwards.
*/
int tools::file::mkpath(std::string pfad,    mode_t mode)
{
    const char *path = pfad.c_str();
    char     *pp;
    char     *sp;
    int     status;
    char     *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp,    '/')) != 0)
    {
    if (sp != pp)
    {
    /* Neither root nor double slash in path */
    *sp = '\0';
    status = do_mkdir(copypath,    mode);
    *sp = '/';
    }
    pp = sp + 1;
    }
    if (status == 0)
    status = do_mkdir(path,    mode);
    free(copypath);
    return (status);
}
#else
int tools::file::mkpath(std::string pfad,    int delme) {
    tools::str::replace_umlaute_to_windows_compatible(&pfad);
    pfad = "\"" + pfad + "\"";
//    std::string cmd = std::string("mkdir ") + pfad + std::string(" 2>nul");
    std::string cmd = std::string("mkdir ") + pfad;
	system(cmd.c_str() );
	return 1;
}
#endif

bool tools::file::directory_exists(std::string directory)
{
    return std::filesystem::exists(directory);
//	struct stat info;

//    if(    stat(    directory.c_str(),    &info ) != 0 )
//        // 		printf(    "cannot access %s\n",    pathname );
//		return false;
//    else if(    info.st_mode & S_IFDIR )     // S_ISDIR() doesn't exist on my windows
//// 		printf(    "%s is a directory\n",    pathname );
//		return true;
//	else
//// 		printf(    "%s is no directory\n",    pathname );
//		return false;

//	return true;
}

bool tools::file::file_exists(std::string filename) {
//    filename.insert(0,"\"");
//    filename.append("\"");
    return std::filesystem::exists(filename);
//    std::cout << filename << std::endl;
//    std::ifstream f(filename.c_str());
//	if (f.good())
//	{
//		f.close();
//		return true;
//	}
//	else
//	{
//		f.close();
//// 		return false;
//	}
//	return false;
}

std::vector<std::string> tools::file::get_file_list_from_dir_name(std::string dir_name,    bool prepend_directory) {
	dir_name = check_directory_string(dir_name);
	std::vector<std::string> filelist;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dir_name.c_str())) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			if (strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0)
			{
                /*testing*/
//                ent->d_name;
                /*********/
				if (prepend_directory) 	filelist.push_back(dir_name +ent->d_name);
				else filelist.push_back(ent->d_name);
			}
		}
		closedir (dir);
	}
	return filelist;
}
