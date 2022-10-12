/*
	Copyright (C) 2020 Florian Bärwolf

std:: This program is free software: you can redistribute it and/or modify
std:: it under the terms of the GNU General Public License as published by
std:: the Free Software Foundation,std:: either version 3 of the License,std:: or
std:: (at your option) any later version.

std:: This program is distributed in the hope that it will be useful,
std:: but WITHOUT ANY WARRANTY; without even the implied warranty of
std:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. std:: See the
std:: GNU General Public License for more details.

std:: You should have received a copy of the GNU General Public License
std:: along with this program. std:: If not,std:: see std::<https://www.gnu.org/licenses/>.
*/
#include "tools.hpp"

// template std::<typename T> 
// static std::vector<T> add(std::vector<T> *start,std::vector<T> *ende)
// {
// 	std::vector<T> result={*start,*ende};
// // 	return start->insert(start->end(),ende->begin(),ende->end());
// 	return result;
// }

// template std::<typename T> 
// void tools::vec::add(std::vector<T> *result_vec,std::vector<T> adder)
// {
// 	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
// }

// std::string tools::vec::combine_vec_to_string(std::vector<std::string> vec,std::string delimiter)
// {
// 	std::string result="";
// 	for (int i=0;i<vec.size();i++)
// 		result+=delimiter+vec[i];
// 	if (delimiter!="" && result.length()>delimiter.length()) result.erase(0,delimiter.length());
// 	return result;
// }

std::string tools::vec::combine_vec_to_string(const std::vector<std::string>& vec,std::string delimiter)
{
	std::string result="";
	for (int i=0;i<vec.size();i++)
		result+=delimiter+vec[i];
	if (delimiter!="" && result.length()>delimiter.length()) result.erase(0,delimiter.length());
	return result;
}

void tools::vec::add(std::vector<std::string> *result_vec,std::vector<std::string> adder)
{
	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
}

void tools::vec::add(std::vector<double> *result_vec,std::vector<double> adder)
{
	result_vec->insert(result_vec->end(),adder.begin(),adder.end());
}

std::vector<std::string> tools::vec::add(const std::vector<std::string>& result_vec,const std::string& adder)
{
	if (adder=="")
		return result_vec;
	auto c = result_vec;
	c.push_back(adder);
	return c;
}

void tools::vec::remove(std::vector<double> *result_vec,std::vector<double> subtractors)
{
	for (auto& subtractor:subtractors)
	{
		for (int i=result_vec->size()-1;i>=0;i--)
		{
			if (result_vec->at(i)==subtractor) result_vec->erase(result_vec->begin()+i);
		}
	}
}

std::vector<std::string> tools::vec::double_to_string(const std::vector<double>& D)
{
    std::ostringstream sst;
	std::vector<std::string> out(D.size());
	for (int i=0;i<D.size();i++)
	{
        sst << D.at(i);
		out.at(i) = sst.str();
		sst.str("");
	}
	return out;
}

void tools::vec::split_map_to_vecs(const std::map<double,double>& XY_mat,std::vector<double> *X,std::vector<double>* Y)
{
	if (X!=nullptr && Y!=nullptr)
	{
		X->clear();
		Y->clear();
		X->resize(XY_mat.size());
		Y->resize(XY_mat.size());
		int i=0;
		for (auto& XY : XY_mat)
		{
			Y->at(i)=XY.second;
			X->at(i)=XY.first;
			i++;
		}
	}
// 	if (Y!=nullptr)
// 	{
// 		Y->clear();
// 		Y->resize(XY_mat.size());
// 		i=0;
// 		for (it=XY_mat.begin();it!=XY_mat.end();it++)
// 		{
// 			Y->at(i)=it->second;
// 			i++;
// 		}
// 	}
	
	/*
	for (it=XY_mat->begin();it!=XY_mat->end();it++)
	{
		X->at(i)=it->first;
		Y->at(i)=it->second;
		i++;
	}
	*/
	return;
}


void tools::vec::combine_vecs_to_map(const std::vector<double> *X,const std::vector<double>* Y,std::map<double,double>* XY_mat)
{
	int size=X->size();
	if (Y->size()<size) size = Y->size();
	
	XY_mat->clear();
	double x_shift = 1E-7;
	for (int i=0;i<size;i++)
	{
		/**************************************************
		std:: * inserting same X values,std:: will overwrite them std:: **
		std:: * ... so this a fast hack: std:: X->at(i)*1E-5 std:: **
		std:: *************************************************/
// 		if (XY_mat->find(X->at(i))!=XY_mat->end()) XY_mat->insert(std::pair<double,std:: double>(X->at(i)+X->at(i)*1E-5,Y->at(i)));
        if (XY_mat->find(X->at(i))!=XY_mat->end()) XY_mat->insert(std::pair<double,double>(X->at(i)+x_shift,Y->at(i)));
        else XY_mat->insert(std::pair<double,double>(X->at(i),Y->at(i)));
	}	
	return;
}

void tools::vec::combine_vecs_to_map(std::vector<double> X,const std::vector<double>* Y,std::map<double,double>* XY_mat)
{
	combine_vecs_to_map(&X,Y,XY_mat);
}
void tools::vec::combine_vecs_to_map(const std::vector<double> *X,std::vector<double> Y,std::map<double,double>* XY_mat)
{
	combine_vecs_to_map(X,&Y,XY_mat);
}
void tools::vec::combine_vecs_to_map(std::vector<double> X,std::vector<double> Y,std::map<double,double>* XY_mat)
{
	combine_vecs_to_map(&X,&Y,XY_mat);
}

std::map<double,double> tools::vec::combine_vecs_to_map(std::vector<double> X,std::vector<double> Y)
{
	std::map<double,double> XY_mat;
	tools::vec::combine_vecs_to_map(&X,&Y,&XY_mat);
	return XY_mat;
}
