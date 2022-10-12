
#include "tools.hpp"

std::vector<std::vector<double>> tools::mat::cut_mat_double_rectengular(std::vector<std::vector<double>> mat_transposed) {
std::vector<std::vector<double>> mat;
int col_min;
if (mat_transposed.size()>0) col_min=mat_transposed[0].size();
else return mat;
for (int i=1;i<mat_transposed.size();i++) {
if (mat_transposed[i].size()<col_min) col_min = mat_transposed[i].size();
}
for (int i=0;i<mat_transposed.size();i++) {
std::vector<double> vec = mat_transposed[i];
vec.resize(col_min);
mat.push_back(vec);
}
return mat;
}

std::vector<std::vector<std::string>> tools::mat::cut_mat_rectengular(std::vector<std::vector<std::string>> mat_transposed) {
std::vector<std::vector<std::string>> mat;
int col_min;
if (mat_transposed.size()>0) col_min=mat_transposed[0].size();
else return mat;
for (int i=1;i<mat_transposed.size();i++) {
if (mat_transposed[i].size()<col_min) col_min = mat_transposed[i].size();
}
for (int i=0;i<mat_transposed.size();i++) {
std::vector<std::string> vec = mat_transposed[i];
vec.resize(col_min);
mat.push_back(vec);
}
return mat;
}

/*transforms matrix_XYY to tensor_{XY} */
std::vector<std::vector<std::vector<double>>> tools::mat::matrix_XYY_to_tensor(std::vector<std::vector<double>> matrix_XYY) {
std::vector<std::vector<std::vector<double>>> tensor;
std::vector<std::vector<double>> matrix_transposed = tools::mat::transpose_matrix_double(matrix_XYY);
for (int i=1;i<matrix_transposed.size();i++) {
tensor.push_back(tools::mat::transpose_matrix_double({matrix_transposed[0],matrix_transposed[i]}));
}
return tensor;
}


/*transforms matrix_XYXY to tensor_{XY} */
std::vector<std::vector<std::vector<double>>> tools::mat::matrix_XYXY_to_tensor(std::vector<std::vector<double>> matrix_XYXY) {
std::vector<std::vector<std::vector<double>>> tensor;
std::vector<std::vector<double>> matrix_transposed = tools::mat::transpose_matrix_double(matrix_XYXY);
for (int i=0;i<matrix_transposed.size();i=i+2) {
tensor.push_back(tools::mat::transpose_matrix_double({matrix_transposed[i],matrix_transposed[i+1]}));
}
return tensor;
}

std::vector<std::vector<double> > tools::mat::transpose_matrix_double(std::vector<std::vector<double> > matrix) {
    std::vector<std::vector<double> > matrix_T((matrix.at(0)).size(),std::vector<double>(matrix.size()));

    for (int i=0;i<matrix.size();i++) {
        for (int j=0;j<(matrix.at(i)).size();j++) {
            matrix_T[j][i]=(matrix)[i][j];
        }
    }
    return matrix_T;
}
std::vector<std::vector<double> > tools::mat::transpose_matrix(std::vector<std::vector<double> > matrix) {
    std::vector<std::vector<double> > matrix_T((matrix.at(0)).size(),std::vector<double>(matrix.size()));

    for (int i=0;i<matrix.size();i++) {
        for (int j=0;j<(matrix.at(i)).size();j++) {
            matrix_T[j][i]=(matrix)[i][j];
        }
    }
    return matrix_T;
}

std::vector<std::vector<std::string>> tools::mat::double_matrix_to_str_matrix(std::vector<std::vector<double>> mat_double) {
std::vector<std::vector<std::string>> mat_string;
std::vector<std::string> vec_string;
for (int i=0;i<mat_double.size();i++) {
vec_string.clear();
for (int j=0;j<mat_double.at(i).size();j++) {
vec_string.push_back(to_string(mat_double.at(i)[j]));
}
mat_string.push_back(vec_string);
}
return mat_string;
}

std::vector<std::vector<double>> tools::mat::str_matrix_to_double_matrix(std::vector<std::vector<std::string>> mat_string) {
std::vector<double> vec_ld;
std::vector<std::vector<double>> mat_ld;
for (int i=0;i<mat_string.size();i++) {
vec_ld.clear();
for (int j=0;j<mat_string.at(i).size();j++) {
if (mat_string.at(i)[j]!="")
vec_ld.push_back(tools::str::str_to_double(mat_string.at(i)[j]));
}
mat_ld.push_back(vec_ld);
}
mat_ld=tools::mat::cut_mat_double_rectengular(mat_ld);
return mat_ld;
}

std::vector<double> tools::mat::str_vec_to_double_vec(std::vector<std::string> vec_string) {
std::vector<double> vec_ld;
    vec_ld.clear();
for (int j=0;j<vec_string.size();j++) {
        if (vec_string[j]!="")
            vec_ld.push_back(tools::str::str_to_double(vec_string[j]));
    }
return vec_ld;
}

std::vector<std::string> tools::mat::double_vec_to_str_vec(std::vector<double> vec_double) {
std::vector<std::string> vec_ld(vec_double.size());
for (int j=0;j<vec_double.size();j++)
    {
// 		vec_ld[j]=(to_string(vec_double[j]));
        std::stringstream ss;
        ss << std::scientific << vec_double[j];
        vec_ld[j] = ss.str();
    }
return vec_ld;
}

std::string tools::mat::create_overview_string_from_matrix(std::vector< std::vector<std::string> > *overview) {
    std::string overview_string="";
    for (int i=0;i<overview->size();i++) {
        if ((overview->at(i)).size()==2) {
            overview_string.append((overview->at(i))[0]);
            overview_string.append("\t->\t");
            overview_string.append((overview->at(i))[1]);
            overview_string.append("\n");
        }
    }
    return overview_string;
}


std::vector<std::vector<std::string> > tools::mat::transform_tensor_unifying_columns_to_matrix(std::vector<std::vector<std::vector<std::string> > > *tensor) {
    std::vector<std::vector<std::string> > matrix,matrix_temp;
    matrix.clear();
    if (tensor->size()==0) return matrix;
    for (int i=0;i<tensor->size();i++) {
        if ((tensor->at(i).size())>0) { // matrix
        matrix_temp=tools::mat::transpose_matrix((tensor->at(i)));
        matrix.insert(matrix_temp.end(),matrix_temp.begin(),matrix_temp.end());
        }
    }
    return tools::mat::transpose_matrix(matrix_temp);
}

std::vector<std::vector<double> > tools::mat::transform_tensor_unifying_columns_to_matrix(std::vector<std::vector<std::vector<double> > > *tensor) {
    std::vector<std::vector<double> > matrix,matrix_temp;
    matrix.clear();
    if (tensor->size()==0) return matrix;
    for (int i=0;i<tensor->size();i++) {
        if ((tensor->at(i).size())>0) { // matrix
        matrix_temp=tools::mat::transpose_matrix((tensor->at(i)));
        matrix.insert(matrix_temp.end(),matrix_temp.begin(),matrix_temp.end());
        }
    }
    return tools::mat::transpose_matrix(matrix_temp);
}

std::vector<std::vector<std::string> > tools::mat::unify_2_vectors_to_matrix(std::vector<std::string> vector1,std::vector<std::string> vector2) {
    std::vector<std::vector<std::string> > matrix;
    matrix.clear();
    int max_size;
    if (vector1.size()<vector2.size()) max_size=vector2.size();
    else max_size=vector1.size();
    vector1.resize(max_size,"");
    vector2.resize(max_size,"");
    for (int i=0;i<max_size;i++) {
        matrix.push_back({vector1[i],vector2[i]});
    }
    return matrix;
}

std::vector<std::vector<std::string> > tools::mat::add_vector_to_matrix(std::vector<std::vector<std::string> > matrix_summand,std::vector<std::string> vec) {

    std::vector<std::vector<std::string> > matrix_T=tools::mat::transpose_matrix(matrix_summand);
    matrix_T.push_back(vec);
    return tools::mat::transpose_matrix(matrix_T);
}

std::vector<std::vector<std::string> > tools::mat::transform_tensor_unifying_lines_to_matrix(std::vector<std::vector<std::vector<std::string> > > *tensor) {
    std::vector<std::vector<std::string> > result_matrix;
    for (int i=0;i<tensor->size();i++) {
        result_matrix.insert(result_matrix.end(),(tensor->at(i)).begin(),(tensor->at(i)).end());
    }
    return result_matrix;
}

void tools::mat::remove_lines_or_columns_from_matrix(std::vector<std::vector<std::string> > *matrix,std::vector<int> *lines,std::vector<int> *columns) {

    if (lines==nullptr && columns==nullptr) return;
    if (lines!=nullptr) sort(lines->begin(),lines->end());
    if (columns!=nullptr) sort(columns->begin(),columns->end());
    if (lines!=nullptr && lines->size()>0) {
        for (int i=lines->size()-1;i>=0;i--) {
            if (lines->at(i)>=0 && matrix->size()>lines->at(i)) {
            matrix->erase(matrix->begin()+lines->at(i));
            }
        }
    }

    if (columns!=nullptr && columns->size()>0) {
        for (int i=0;i<matrix->size();i++) {
            for (int j=columns->size()-1;j>=0;j--) {
                if (columns->at(j)>=0 && (matrix->at(i)).size()>columns->at(j)) {
                (matrix->at(i)).erase((matrix->at(i)).begin()+columns->at(j));
                }
            }
        }
    }
}

std::vector<std::vector<std::string> > tools::mat::transpose_matrix(std::vector<std::vector<std::string> > matrix) {

    tools::mat::fillup_matrix(&matrix);
    std::vector<std::vector<std::string> > matrix_T((matrix.at(0)).size(),std::vector<std::string>(matrix.size()));

    for (int i=0;i<matrix.size();i++) {
        for (int j=0;j<(matrix.at(i)).size();j++) {
            matrix_T[j][i]=(matrix)[i][j];
        }
    }
    return matrix_T;
}

bool tools::mat::find_str_in_mat(std::vector<std::vector<std::string>>* matrix,std::string find_this) {
for (int x=0;x<matrix->size();x++) {
for (int y=0;y<matrix->at(x).size();y++) {
if (matrix->at(x)[y].find(find_this)!=std::string::npos) return true;
}
}
return false;
}
bool tools::mat::find_str_in_mat(std::vector<std::string>* matrix,std::string find_this) {
for (int x=0;x<matrix->size();x++)
    {
if (matrix->at(x).find(find_this)!=std::string::npos) return true;
}
return false;
}
bool tools::mat::find_str_in_mat(std::vector<std::string> matrix,std::string find_this) {
for (int x=0;x<matrix.size();x++)
    {
if (matrix.at(x).find(find_this)!=std::string::npos) return true;
}
return false;
}

int tools::mat::find_in_matrix_xy_return_y(std::vector<std::vector<std::string>>* matrix,std::string find_this) {
int y=-1;
for (int x=0;x<matrix->size();x++) {
for (int y=0;y<matrix->at(x).size();y++) {
if (matrix->at(x)[y].find(find_this)!=std::string::npos) return y;
}
}
return y;
}

int tools::mat::find_in_vec(std::vector<std::string>* vec,std::string find_this) {
for (int x=0;x<vec->size();x++) {
        if (vec->at(x).find(find_this)!=std::string::npos) return x;
}
return -1;
}
int tools::mat::find_in_vec(std::vector<std::string> vec,std::string find_this) {
for (int x=0;x<vec.size();x++) {
        if (vec.at(x).find(find_this)!=std::string::npos) return x;
}
return -1;
}

int tools::mat::is_in_vec(std::vector<std::string>* vec,std::string find_this) {
for (int x=0;x<vec->size();x++) {
        if (vec->at(x)==find_this) return x;
}
return -1;
}

void tools::mat::fillup_matrix(std::vector<std::vector<std::string> > *matrix) {
    int max_column_size=0;
    int size;
    for (int i=0;i<matrix->size();i++) {
        size = (matrix->at(i)).size();
        if (size>max_column_size) max_column_size=size;
    }
    for (int i=0;i<matrix->size();i++) {
        (matrix->at(i)).resize(max_column_size,"");
    }
}

std::vector<int> tools::mat::get_empty_columns_from_matrix(std::vector<std::vector<std::string>> *mat) {
std::vector<int> cols;
if (mat->size()==0) return cols;
for (int j=0;j<mat->at(0).size();j++) {
bool empty=false;
for (int i=0;i<mat->size();i++) {
if (mat->at(i)[j]=="") empty=true;
            else {
            empty = false;
            break;
            }
}
if (empty) cols.push_back(j);
}
return cols;
}

void tools::mat::cut_matrix(std::vector<std::vector<std::string>>* mat) {
// 	for (int i=mat->size()-1;i>=0;i--) {
// 		bool empty=false;
// 		for (int j=0;j<mat->at(i).size();j++) {
// 			if (std::map*t->at(i)[j]=="") empty=true;
// 			else {
// 			empty = false;
// 			break;
// 			}
// 		}
// 		if (empty) mat->erase(std::map*t->begin()+i);
// 	}
}

void tools::mat::remove_empty_lines_from_matrix(std::vector<std::vector<std::string>> *mat) {
    for (int i=mat->size()-1;i>=0;i--) {
        bool empty=false;
        for (int j=0;j<mat->at(i).size();j++) {
            if (mat->at(i)[j]=="") empty=true;
            else {
            empty = false;
            break;
            }
        }
        if (empty) mat->erase(mat->begin()+i);
    }
}

void tools::mat::remove_empty_cols_from_matrix(std::vector<std::vector<std::string>> *mat) {
std::vector<std::vector<std::string>> mat_rem;
mat_rem=tools::mat::transpose_matrix(*mat);
tools::mat::remove_empty_lines_from_matrix(&mat_rem);
*mat=tools::mat::transpose_matrix(*mat);
}


// std::vector<std::vector<std::string> > tools::mat::format_string_to_matrix(std::string *mainstring,std::string line_delimiter,std::string column_delimiter)
// {
// 	std::string line;
// 	std::vector<std::string> columns_in_line;
// 	std::vector<std::vector< std::string >> matrix;
// 	std::vector<std::string> lines = tools::str::get_strings_between_delimiter(*mainstring,line_delimiter);
// 	for (int i=0;i<lines.size();i++) {
// 		if (lines[i].length()==0) continue;
// 		line = lines[i];
// 		tools::str::replace_chars(&(line),"\r",""); // win
// 		if (line.length()>0) {
// 			columns_in_line = tools::str::get_strings_between_delimiter(line,column_delimiter,true);
// 			matrix.push_back(columns_in_line);
// 		}
// 	}
// 	return matrix;
// }
// std::vector<std::vector<std::string> > tools::mat::format_string_to_matrix(std::string mainstring,std::string line_delimiter,std::string column_delimiter)
// {
// 	std::string line;
// 	std::vector<std::string> columns_in_line;
// 	std::vector<std::vector< std::string >> matrix;
// 	std::vector<std::string> lines = tools::str::get_strings_between_delimiter(std::map*instring,line_delimiter);
// 	for (int i=0;i<lines.size();i++) {
// 		if (lines[i].length()==0) continue;
// 		line = lines[i];
// 		tools::str::replace_chars(&(line),"\r",""); // win
// 		if (line.length()>0) {
// 			columns_in_line = tools::str::get_strings_between_delimiter(line,column_delimiter,true);
// 			matrix.push_back(columns_in_line);
// 		}
// 	}
// 	return matrix;
// }

std::vector<std::vector<std::string> > tools::mat::format_string_to_matrix(const std::string& mainstring,std::string line_delimiter,std::string column_delimiter)
{
    std::string line;
    std::vector<std::string> columns_in_line;
    std::vector<std::vector< std::string >> matrix;
    std::vector<std::string> lines = tools::str::get_strings_between_delimiter(mainstring,line_delimiter);
    for (int i=0;i<lines.size();i++) {
        if (lines[i].length()==0) continue;
        line = lines[i];
        tools::str::replace_chars(&(line),"\r",""); // win
        if (line.length()>0) {
            columns_in_line = tools::str::get_strings_between_delimiter(line,column_delimiter,true);
            matrix.push_back(columns_in_line);
        }
    }
    return matrix;
}


std::vector<std::vector<std::string> > tools::mat::format_string_to_matrix(std::string* mainstring,std::string line_delimiter,std::string column_delimiter)
{
    std::string line;
    std::vector<std::string> columns_in_line;
    std::vector<std::vector< std::string >> matrix;
    std::vector<std::string> lines = tools::str::get_strings_between_delimiter(*mainstring,line_delimiter);
    for (int i=0;i<lines.size();i++) {
        if (lines[i].length()==0) continue;
        line = lines[i];
        tools::str::replace_chars(&(line),"\r",""); // win
        if (line.length()>0)
        {
            columns_in_line = tools::str::get_strings_between_delimiter(line,column_delimiter,true);
            matrix.push_back(columns_in_line);
        }
    }
    return matrix;
}


std::string tools::mat::format_matrix_to_string(std::vector<std::vector<std::string> > *matrix,std::string line_delimiter,std::string column_delimiter)
{
    std::stringstream ss;
    for (int i=0;i<matrix->size();i++) {
        if (i!=0) ss << line_delimiter;
        for (int j=0;j<(matrix->at(i)).size();j++) {
            if (((*matrix)[i][j]).size()>0) {
                if (j!=0) ss << column_delimiter;// content=content +column_delimiter;
                ss << (*matrix)[i][j];
            } else {
                if (j!=0) ss << column_delimiter; // content=content +column_delimiter;
                ss << "";	// content=content+"";
            }
        }
    }
    return ss.str();
}

// std::string tools::mat::format_matrix_to_string(std::vector<std::vector<std::string> > *matrix,std::string line_delimiter,std::string column_delimiter) {
// 	std::string content;
// 	for (int i=0;i<matrix->size();i++) {
// 		if (i!=0) content=content+(line_delimiter);
// 		for (int j=0;j<(matrix->at(i)).size();j++) {
// 			if (((*matrix)[i][j]).size()>0) {
// 				if (j!=0) content=content +column_delimiter;
// 				content=content+((*matrix)[i][j]);
// 			} else {
// 				if (j!=0) content=content +column_delimiter;
// 				content=content+"";
// 			}
// 		}
//
// 	}
// 	return content;
// }
