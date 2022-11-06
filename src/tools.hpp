
#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "print.hpp"
#include <unordered_set>
#include <stdio.h>
#include <cmath>
#include <iomanip>      // std::get_time
#include <ctime>        // struct std::tm
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <string.h>
#include <numeric>
#include <map>
// #include "print.hpp"
#include <set>
#include <iostream>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <filesystem>

// #include <curl/curl.h> // libcurl4-gnutls-dev
// #include <gsl/gsl_sys.h>

#include "definitions.hpp"

// #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
// #define path_del "\\"
// #define line_del "\n"
// #elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
// #define path_del "/"
// #define line_del "\n"
// #endif

#ifdef __unix__
#include <unistd.h>

#else
#include <direct.h>
#include <windows.h>
#endif

#define FILETYPE_DELIMITER string(".")

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
    #include <conio.h>   //for getch(), needed in wait_for_key()
    #include <windows.h> //for Sleep()
    //  static void sleep(int i) { Sleep(i*1000); }
//     #define PATH_DELIMITER string("\\")
//     #define LINE_DELIMITER string("\n")
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
//     #define PATH_DELIMITER string("/")
//     #define LINE_DELIMITER string("\n")
#endif



class tools{
private:
public:
        template<typename O>
        static std::string to_string(O object)
	{
	        std::stringstream out;
		out << object;
		return out.str();
	}

	template<class In1, class In2>
	static std::string to_string(std::map<In1,In2>& I1_to_I2)
	{
	        std::stringstream out;
		for (auto& i : I1_to_I2)
		        out << to_string(i.first) << "->" << to_string(i.second) << ", ";
		return out.str();
	}
	///returns the pointer to key in V<keys>
	template <typename F,typename V>
	static const F* find_in_V(const F& f,const V& keys)
	{
	        for (auto& key : keys)
		{
		        if (key==f)
			        return &key;
		}
		return nullptr;
	}
	///returns the pointer to key in V<keys>
	template <typename F,typename V>
	static F* find_in_V(F& f,V& keys)
	{
	        for (auto& key : keys)
		{
		        if (key==f)
			        return &key;
		}
		return nullptr;
	}
	class web {
	public:
	        static const std::string get_contents(std::string url);
	};
    class str {
    public:
	        ///filter within strings for substrings / chars
// 		class filter_t
// 		{
// 		private:
// 			std::string filter_this;
// 			static std::set<std::string> special_characters;
// 		public:
// 			filter_t(const std::string std::string& filter_this);
// 			std::string escape_special_characters(std::string prefix_escape_string="\\");
// 		};
	        class filter_t
		{
		private:
		        std::string prefix_escape_string;
			static std::set<std::string> special_characters;
		public:
			filter_t(const std::string prefix_escape_string="\\");
			std::string escape_special_characters(std::string filter_this);
		};

		static std::string to_ascii(std::string mainstring);
		static std::string remove_NOTchars_from_string(std::string mainstring, std::string NOTchars);
		/// checks wheter the input contains just empty strings ("") --> true or not --> false
		static bool is_empty(std::string input);
		static bool is_empty(std::vector<std::string> input);
		static bool is_empty(std::vector<std::vector<std::string>> input);
		static std::string get_string_between_string_A_and_next_B(const std::string *mainstring,std::string A, std::string B);
	static int is_number(std::string str);
	static double str_to_double(std::string mainstring);
	static int str_to_int(std::string mainstring);
	static std::vector<std::string> get_all_string_between_string_A_and_next_B(std::string *mainstring,std::string A, std::string B);
	static std::string get_string_between_string_A_and_last_B(std::string *mainstring,std::string A, std::string B);
	static std::vector<std::string> get_strings_between_delimiter(std::string mainstring, std::string delimiter, bool empty=false);
	static int levenshtein_distance(const std::string &s1, const std::string &s2);
	static void replace_NOTchars_from_string(std::string *mainstring, std::string replacethis_NOT, std::string replacewith);
	static void replace_chars(std::vector<std::vector<std::string>> *mat, std::string replacethis, std::string replacewith);
	static void replace_chars(std::string *mainstring, std::string replacethis, std::string replacewith);
	static void replace_chars(std::vector<std::string> *mainstring, std::string replacethis, std::string replacewith);
	static void remove_spaces_from_string_end(std::string *mainstring);
	static void remove_spaces_from_string_start(std::string *mainstring);
    static void remove_spaces(std::string* mainstring);
	static void remove_spaces(std::vector<std::string>* mainstring);
	        static void remove_spaces(std::vector<std::vector<std::string>>* mainstring);
		static std::string remove_linebreakers_from_string(std::string mainstring); // remove_linebreakers_from_string
	static std::string remove_chars_from_string(std::string mainstring, std::string chars);
	static bool replace_umlaute_to_windows_compatible(std::string *mainstring);
    static std::vector<std::string> args_to_vec(int    argc, char **argv);
	static void remove_duplicates(std::vector<std::string>* vec);
	/// returns the KGV of strings, that means, the least common multiple (which may be of course a single letter)
	static std::vector<std::string> remove_similarities(std::vector<std::string>* vec);
    static void to_lower(std::string* upper);
	static void to_lower(std::vector<std::string>* upper);
	/// extract all number chars into one integer ("A2B342c34" = "234234")
	static int extract_int_from_str(std::string str);
	static std::vector<std::string> find_all_substrings(std::vector<std::string>* vec, std::string find_this);
	        static std::vector<std::string> find_all_substrings(std::set<std::string>& search_these, std::string find_this);
        static void remove_substring_from_mainstring(std::string* mainstring, std::string substring);
    };
    class time {
    public:
	        static std::string tm_struct_to_string_time(tm tm, std::string time_format="%Y-%m-%d %H:%M:%S");
		static tm string_time_to_tm_structure(std::string time,std::string time_format="%Y-%m-%d %H:%M:%S");
		static std::string time_t_to_string(time_t ttime, std::string time_format="%Y-%m-%d %H:%M:%S");
		static std::string seconds_to_human_readable(double secs);
	static time_t string_to_time_t(std::string time,std::string time_format="%Y-%m-%d %H:%M:%S");
	static std::string get_time_now(std::string time_format="%Y-%m-%d %H:%M:%S");
    };
    class file {
    public:
	/// 2- folder, 1-file, 0-unknown, -1-error
	static int is_file_or_folder(const std::string file_or_folder);
	        static void remove_file_type_from_name(std::vector<std::string>* filenames, std::string filetye_delimiter=".");
        static void remove_file_type_from_name(std::string* filenames, std::string filetye_delimiter=".");
		static std::vector<std::vector<std::string>> load(std::string filename, std::string delimiter);
	static std::vector<std::string> filter_files_from_types(std::vector<std::string> * file_list, std::vector<std::string> * file_types, bool exclude=false);
	        static std::unordered_set<std::string> filter_files_from_types(std::unordered_set<std::string>& file_list, std::unordered_set<std::string>& file_types, bool exclude=false);
		static void remove_files(std::vector<std::string> files);
	static std::vector<std::string> load_file_to_vector_string(std::string filename_with_path);
	static std::string load_file_to_string(std::string filename_with_path);
	static bool write_mat_to_file(std::string filename_with_path,std::vector<std::vector<double>> *matrix, bool append, std::string column_delimiter, std::string line_delimiter=LINE_DELIMITER);
	static bool write_mat_to_file(std::string filename_with_path,std::vector<std::vector<std::string>> *matrix, bool append, std::string column_delimiter, std::string line_delimiter=LINE_DELIMITER);
	static bool write_to_file(std::string filename_with_path,const std::string *contents, bool append=true);
	        ///automatically creates folders along the path if they not already exist
	        static bool write_to_file_and_create_folders(std::string filename_with_path,const std::string *contents, bool append=true);
		static bool write_to_file_and_create_folders(std::string filename, std::string path,const std::string *contents, bool append=true);
        static std::string extract_filetype_ending(std::string filename,std::string filetye_delimiter=".",std::string path_delimiter=PATH_DELIMITER);
        static std::string extract_directory_from_filename(std::string filename,std::string path_delimiter=PATH_DELIMITER);
    static std::string extract_filename(std::string filename,std::string filetye_delimiter=".",std::string path_delimiter=PATH_DELIMITER);
    static std::vector<std::string> extract_filenames(std::vector<std::string> filenames,std::string filetye_delimiter=".",std::string path_delimiter=PATH_DELIMITER);
	static std::string check_directory_string(std::string add_directory, std::string root_dir);
	static std::string check_directory_string(std::string add_directory);
        #ifdef __unix__
	static int do_mkdir(const char *path, mode_t mode=0750);
	static int mkpath(std::string pfad, mode_t mode=0750);
        #else
    static int mkpath(std::string pfad, int delme=0);
        #endif
	static bool file_exists(std::string filename);
	        static bool directory_exists(std::string directory);
		static std::vector<std::string> get_file_list_from_dir_name(std::string dir_name, bool prepend_directory);
	        /// returns the creation date of the file (filename with path) in a timeinfo (tm) structure
	        static time_t creation_date(std::string filename);
    };
        class vec
	{
	public:
	///returns -1 if not found
	template<class T>
	static int get_index_position_by_comparing_pointers(const std::vector<T>& values, const T* search_val)
	{
	    for (unsigned int i=0;i<values.size();i++)
        {
            if (&values[i] == search_val)
            {
                return i;
            }
        }
	    return -1;
	}
	template<class T>
	static std::vector<T*> filter_as_pointers_from_indices(std::vector<T> values, std::vector<unsigned int> indices)
	{
	    std::vector<T*> filtered_values;
	    filtered_values.reserve(indices.size());
	    for (auto idx : indices)
	    {
		if (idx>=values.size()) continue;
		filtered_values.push_back(&values.at(idx));
	    }
	    return filtered_values;
	}
	template<class T>
	static std::vector<T> filter_as_copies_from_indices(std::vector<T> values, std::vector<unsigned int> indices)
	{
	    std::vector<T> filtered_values;
	    filtered_values.reserve(indices.size());
	    for (auto idx : indices)
	    {
		if (idx>=values.size()) continue;
		filtered_values.push_back(values.at(idx));
	    }
	    return filtered_values;
	}

	template<class T>
	static std::vector<T> pointers_to_values(std::vector<T*> values)
	{
	    std::vector<T> pp;
	    pp.reserve(values.size());
	    for (int i=0;i<values.size();i++)
	    {
//                pp[i] = *values.at(i);
		pp.push_back(*values.at(i));
	    }
	    return pp;
	}
	template<class T>
	static std::vector<T*> pointers(std::vector<T> values)
	{
	    std::vector<T*> pp(values.size());
	    for (int i=0;i<values.size();i++)
	    {
		pp[i] = &values.at(i);
	    }
	    return pp;
	}
	template<class T>
	static std::vector<const T*> const_pointers(std::vector<T> values)
	{
	    std::vector<const T*> pp(values.size());
	    for (int i=0;i<values.size();i++)
	    {
		pp[i] = &values.at(i);
	    }
	    return pp;
	}

	        template<class In>
	        static std::string to_string(const std::vector<In>& I)
		{
		        std::stringstream out;
			for (auto& i : I)
			        out << tools::to_string(i) << ", ";
			return out.str();
		}
// 		template<class In>
// 		static std::vector<std::string> to_string(In& I)
// 		{
// 			std::vector<std::string> O;
// 			to_string(begin(I),end(I),back_inserter(O));
// 			return O;
// 		}

		/*from https://stackoverflow.com/questions/25371873/convert-vectordouble-to-vectorstring-elegant-way*/
		template<class IteratorIn, class IteratorOut>
		static void to_string(IteratorIn first, IteratorIn last, IteratorOut out)
		{
		        std::transform(first, last, out,
			        [](typename std::iterator_traits<IteratorIn>::value_type d) { return std::to_string(d); } );
		}
		///returns the pointer to key in keys_list
		template <typename F>
		static const F* find_in_vec(const F& key,const std::vector<F>& keys)
		{
		        return find_in_V(key,keys);
		}
		template <typename F>
		static std::vector<F> erase_copy(const std::vector<F>& erase_from, std::vector<unsigned int>& positions_to_erase)
		{
		        if (erase_from.size()==0) return {};
			if (positions_to_erase.size()==0) return erase_from;
			if (erase_from.size() == positions_to_erase.size()) return {};
			sort(positions_to_erase.begin(),positions_to_erase.end());
			std::vector<F> result(erase_from.size()-positions_to_erase.size());
			unsigned int result_idx = 0;
			unsigned int pos_start = 0;
			unsigned int pos_stop = erase_from.size();
			for (auto& pos : positions_to_erase)
			{
			        for (int i=pos_start;i<pos;i++)
				{
				        result.at(result_idx) = erase_from.at(i);
					result_idx++;
				}
				pos_start = pos + 1;
			}
			for (int i = pos_start;i<pos_stop;i++)
			{
			        result.at(result_idx) = erase_from.at(i);
				result_idx++;
			}

			return result;
		}
		template <typename F>
		static void erase(std::vector<F>& erase_from, std::vector<unsigned int> positions_to_erase)
	        {
	                if (erase_from.size()==0) return;
			if (positions_to_erase.size()==0) return;
// 			if (erase_from.size() == positions_to_erase.size()) return {};
			std::sort(positions_to_erase.begin(),positions_to_erase.end());
			std::reverse(positions_to_erase.begin(),positions_to_erase.end());
// 			for (auto it=erase_from.begin();it!=erase_from.end();)
			for (auto pos : positions_to_erase)
			{
// 				std::cout << "erase: " << pos  << std::endl;
			        erase_from.erase(erase_from.begin()+pos);
			}

			return;
	        }

// 		template <typename T> static std::vector<T> add(std::vector<T> *start,/*vector*/<T> *ende);
	        static void add(std::vector<std::string> *result_vec,std::vector<std::string> adder);
		static void add(std::vector<double> *result_vec,std::vector<double> adder);
		static std::vector<std::string> add(const std::vector<std::string>& result_vec,const std::string& adder);
		static void remove(std::vector<double> *result_vec,std::vector<double> subtractors);

		static std::vector<std::string> double_to_string(const std::vector<double>& D);

		static void split_map_to_vecs(const std::map<double,double>& XY_mat, std::vector<double> *X, std::vector<double>* Y);
		static void combine_vecs_to_map(const std::vector<double> *X, const std::vector<double>* Y, std::map<double,double>* XY_mat);
		static void combine_vecs_to_map(const std::vector<double> *X, std::vector<double> Y, std::map<double,double>* XY_mat);
		static void combine_vecs_to_map(std::vector<double> X, const std::vector<double> *Y, std::map<double,double>* XY_mat);
		static void combine_vecs_to_map(std::vector<double> X, std::vector<double> Y, std::map<double,double>* XY_mat);
		static std::map<double,double> combine_vecs_to_map(std::vector<double> X, std::vector<double> Y);
		static std::string combine_vec_to_string(const std::vector<std::string>& vec, std::string delimiter="");
// 		static std::string combine_vec_to_string(std::vector<std::string> vec, std::string delimiter="");

	};
    class mat {
    public:
	        static int is_in_vec(std::vector<std::string>* vec, std::string find_this);
		static int find_in_vec(std::vector<std::string>* vec, std::string find_this);
		static int find_in_vec(std::vector<std::string> vec, std::string find_this);
		static void cut_matrix(std::vector<std::vector<std::string>>* mat);
		static bool find_str_in_mat(std::vector<std::vector<std::string>>* matrix, std::string find_this);
	        static bool find_str_in_mat(std::vector<std::string>* vec, std::string find_this);
		static bool find_str_in_mat(std::vector<std::string> vec, std::string find_this);
		/// returns -1 if not found, otherwise returns the column
		static int find_in_matrix_xy_return_y(std::vector<std::vector<std::string>>* matrix, std::string find_this);
	static std::vector<std::vector<std::vector<double>>> matrix_XYXY_to_tensor(std::vector<std::vector<double>> matrix_XYXY);
	static std::vector<std::vector<std::vector<double>>> matrix_XYY_to_tensor(std::vector<std::vector<double>> matrix_XYY);
	static std::vector<std::vector<double>> cut_mat_double_rectengular(std::vector<std::vector<double>> mat_transposed);
	        static std::vector<std::vector<std::string>> cut_mat_rectengular(std::vector<std::vector<std::string>> mat_transposed);
		static std::vector<std::vector<double>> str_matrix_to_double_matrix(std::vector<std::vector<std::string>> mat_string);
	        static std::vector<double> str_vec_to_double_vec(std::vector<std::string> vec_string);
		static std::vector<std::string> double_vec_to_str_vec(std::vector<double> vec_double);
	static std::vector<std::vector<std::string>> double_matrix_to_str_matrix(std::vector<std::vector<double>> mat_double);
	static std::vector<std::vector<double> > transpose_matrix_double(std::vector<std::vector<double> > matrix);
	static std::vector<int> get_empty_columns_from_matrix(std::vector<std::vector<std::string>> *mat);
	static std::vector<std::vector<std::string> > unify_2_vectors_to_matrix(std::vector<std::string> vector1, std::vector<std::string> vector2);
	static std::vector<std::vector<std::string> > transform_tensor_unifying_lines_to_matrix(std::vector<std::vector<std::vector<std::string> > > *tensor);
	static std::string create_overview_string_from_matrix(std::vector< std::vector<std::string> > *overview);
	static std::vector<std::vector<std::string> > transform_tensor_unifying_columns_to_matrix(std::vector < std::vector < std::vector < std::string > > > *tensor);
	static std::vector<std::vector<double> > transform_tensor_unifying_columns_to_matrix(std::vector < std::vector < std::vector < double > > > *tensor);
	static void remove_lines_or_columns_from_matrix(std::vector<std::vector<std::string> > *matrix,std::vector<int> *lines=nullptr, std::vector<int> *columns=nullptr);
	static void remove_empty_cols_from_matrix(std::vector<std::vector<std::string>> *mat);
	static void remove_empty_lines_from_matrix(std::vector<std::vector<std::string>> *mat);
	static std::vector<std::vector<std::string> > transpose_matrix(std::vector<std::vector<std::string> > matrix);
	static std::vector<std::vector<double> > transpose_matrix(std::vector<std::vector<double> > matrix);
	static void fillup_matrix(std::vector<std::vector<std::string> > *matrix);
//         static std::vector<std::vector<std::string> > format_string_to_matrix(std::string *mainstring, std::string line_delimiter=LINE_DELIMITER, std::string column_delimiter="\t");
// 		static std::vector<std::vector<std::string> > format_string_to_matrix(std::string mainstring, std::string line_delimiter=LINE_DELIMITER, std::string column_delimiter="\t");
	static std::vector<std::vector<std::string> > format_string_to_matrix(const std::string& mainstring, std::string line_delimiter=LINE_DELIMITER, std::string column_delimiter="\t");
    static std::vector<std::vector<std::string> > format_string_to_matrix(std::string* mainstring, std::string line_delimiter=LINE_DELIMITER, std::string column_delimiter="\t");
	static std::string format_matrix_to_string(std::vector<std::vector<std::string> > *matrix, std::string line_delimiter=LINE_DELIMITER, std::string column_delimiter="\t");
	static std::vector<std::vector<std::string> > add_vector_to_matrix(std::vector<std::vector<std::string> > matrix_summand, std::vector<std::string> vec);
    };
        class math
	{
	public:
	        /*=values * factor*/
	        static std::vector<double> multiply(std::vector<double>& values, double factor);
		/*=values + summand*/
		static std::vector<double> add(std::vector<double>& values, double summand);
		static long long unsigned int hexTOint(const std::string& hex);
		static double hexTOdouble(const std::string& hex);
	};


// static bool create_directory(std::string directory,std::string root_dir, std::string path_delimiter);

static void wait_for_key();

static bool transform_string_to_xml(std::string *content, std::string *header,std::string *result);
static bool transform_xml_to_string(std::string *content, std::string *header,std::string *xml,int iter=0);
static bool transform_xml_to_vector_string(std::vector<std::string> *content, std::vector<std::string> *header,std::string *xml);




};


#endif // TOOLS_HPP
