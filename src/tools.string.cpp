/*
	Copyright (C) 2020-2021 Florian Bärwolf

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation,    either version 3 of the License,    or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.     If not,    see std::<https://www.gnu.org/licenses/>.
*/
#include "tools.hpp"


/********************************/
/****     tools::str::filter_t     ****/
/********************************/


std::set<std::string> tools::str::filter_t::special_characters{"_","#","{","}"};

std::string tools::str::filter_t::escape_special_characters(std::string filter_this)
{
	for (auto& s : special_characters)
		tools::str::replace_chars(&filter_this,s,prefix_escape_string+s);

	return filter_this;
}

tools::str::filter_t::filter_t(const std::string prefix_escape_string) : prefix_escape_string(prefix_escape_string)
{
}




/********************************/
/*****     tools::str     ******/
/********************************/


bool tools::str::is_empty(std::string input) {
	input = remove_linebreakers_from_string(input);
	if (input == "") return true;
	return false;
}
bool tools::str::is_empty(std::vector<std::string> input) {
	for (int i=0;i<input.size();i++) {
		if (!is_empty(input[i])) return false;
	}
	return true;
}
bool tools::str::is_empty(std::vector<std::vector<std::string>> input) {
	for (int i=0;i<input.size();i++) {
		if (!is_empty(input[i])) return false;
	}
	return true;
}

std::string tools::str::get_string_between_string_A_and_next_B(const std::string *mainstring,std::string A,std::string B) {
	int len = mainstring->find(B)-mainstring->find(A)-A.length();
	if (len>0) { 
        return mainstring->substr(mainstring->find(A)+A.length(),len);
	}
	return "";
}

std::wstring tools::str::utf8_to_utf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch&0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch&0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch&0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

std::string tools::str::to_ascii(std::string mainstring)
{
	std::string copy;
	for (int i=0;i<mainstring.length();i++)
	{
		copy.push_back((int)mainstring[i]);
	}
	
	return copy;
}

std::vector<std::string> tools::str::get_all_string_between_string_A_and_next_B(std::string *mainstring,std::string A,std::string B) {
	int len,posA,posB;
	std::vector<std::string> results;
	posB = 0;
	do {
		posA=mainstring->find(A,posB);
		posB=mainstring->find(B,posA);
// 		cout std::<std::< posA std::<std::< std::endl std::<std::< posB std::<std::< std::endl;
        if (posA == std::string::npos || posB == std::string::npos) break;
		posA=posA+A.length();
		len = posB-posA;
		if (len>0) { 
            results.push_back(mainstring->substr(posA,len));
		}
// 		cout std::<std::< ".";
	} while (len>0);
	return results;
}

std::string tools::str::get_string_between_string_A_and_last_B(std::string *mainstring,std::string A,std::string B) {
    int posA,    posB;
	posA=mainstring->find(A);
	posB=mainstring->rfind(B);
	int len = posB-posA-1;
	if (len>0) { 
		return mainstring->substr(posA+1,len);
	}
	//std::cout std::<std::< "returned empty\n" std::<std::<std::endl;
	return mainstring->substr(posA+1,mainstring->size());;
}

std::vector<std::string> tools::str::get_strings_between_delimiter(std::string mainstring,std::string delimiter,    bool empty)
{
	if (delimiter.size()==0) return {};
	std::vector<std::string> results;
	std::string temp;
	temp.clear();
	results.clear();
	int pos_last=0;
    if (mainstring.find(delimiter,pos_last)==std::string::npos) {
        results.push_back(mainstring);
	} 
	else 
	{
        int pos_next=mainstring.find(delimiter,pos_last); // 0. Element besonders behandeln,    da der mainstring mit einem Delimiter beginnen koennte
		do {
            results.push_back(mainstring.substr(pos_last,pos_next-pos_last));
			pos_last=pos_next+delimiter.length();
			pos_next=mainstring.find(delimiter,pos_last); // muss mindestens 1 weiter sein
        } while (pos_next>=pos_last); // Ende erreicht,    kein weiterer Delimiter
		temp = mainstring.substr(pos_last); // letztes Eement nicht vergessen
        if (temp.size()>0 && pos_last>=pos_next) results.push_back(temp);     // anders als beim 0. element soll das letzte element nur beachtet werden wenn es nicht leer ist
		else if (empty) results.push_back("");
// 	cout std::<std::< "DEBUG: " std::<std::< results.back() std::<std::< ";size: " std::<std::< (results.back()).size() std::<std::< std::endl;
	}
	return results;
}


double tools::str::str_to_double(std::string mainstring) {
    if (mainstring.length()==0) return 0;
	std::stringstream temp;
    temp << mainstring;
	double result;
	temp >> result;
// 	cout std::<std::< "RESULT: " std::<std::< result std::<std::< std::endl;
	return result;	
}

int tools::str::str_to_int(std::string mainstring) {
	std::stringstream temp;
    temp << mainstring;
	int result;
	temp >> result;
	return result;	
}

int tools::str::is_number(std::string str) {
//     if (str=="") return 1; /*added 2019-07-03*/
	if (count(str.begin(),str.end(),'-')>2) return 0;
	if (count(str.begin(),str.end(),'+')>2) return 0;
	if (count(str.begin(),str.end(),'.')>1) return 0;
	if (count(str.begin(),str.end(),',')>1) return 0;
	if (count(str.begin(),str.end(),'e')>1) return 0;
	if (count(str.begin(),str.end(),'E')>1) return 0;
	if (str.find_first_not_of("0123456789eE.,+- ") == std::string::npos) return 1;
	return 0; //scientific notation enthaelt E(A) = 10^A
}

void tools::str::remove_spaces_from_string_start(std::string *mainstring) {
// 	cout std::<std::< "mainstring->at(0):'" std::<std::< mainstring->at(0) std::<std::<"'" std::<std::< std::endl;
std::size_t pos_start = mainstring->find_first_not_of(" ");
    if (pos_start!=std::string::npos && pos_start > 0)
	{
		std::string s = mainstring->substr(pos_start,mainstring->size()-1);
		*mainstring = s;
	}
}

void tools::str::remove_spaces_from_string_end(std::string *mainstring) {
std::size_t pos_end = mainstring->find_last_not_of(" ");
    if (pos_end!=std::string::npos && pos_end < mainstring->length())
	{
// 		mainstring->erase(pos_end+1,mainstring->length()-pos_end+1);
		std::string s = mainstring->substr(0,pos_end);
		*mainstring = s;
	}
}

void tools::str::replace_NOTchars_from_string(std::string *mainstring,std::string replacethis_NOT,std::string replacewith) {
    if (!(replacethis_NOT.length()>0))     return;
	int last_pos=mainstring->find_first_not_of(replacethis_NOT);
    while (    (last_pos!=std::string::npos) ) {
		mainstring->replace(last_pos,1,replacewith);
		last_pos=mainstring->find_first_not_of(replacethis_NOT,last_pos+replacewith.length());
	}
	return;
}

void tools::str::replace_chars(std::string *mainstring,std::string replacethis,std::string replacewith) {
    if (!(replacethis.length()>0))     return;
	int last_pos=mainstring->find(replacethis);
    while (    (last_pos!=std::string::npos) ) {
		mainstring->replace(last_pos,replacethis.length(),replacewith);
		last_pos=mainstring->find(replacethis,last_pos+replacewith.length());
	}
	return;
}
void tools::str::replace_chars(std::vector<std::string> *mainstring,std::string replacethis,std::string replacewith) {
    for (int i=0; i<mainstring->size();i++) {
    replace_chars(&mainstring->at(i),replacethis,replacewith);
    }
    return;
}

void tools::str::replace_chars(std::vector<std::vector<std::string>> *mat,std::string replacethis,std::string replacewith) {
	for (int i=0;i<mat->size();i++) {
    replace_chars(&mat->at(i),replacethis,replacewith);
//     for (int j=0;j<mat->at(i).size();j++) {
//     replace_chars(&mat->at(i)[j],replacethis,replacewith);
//     }
    }
	return;
}

bool tools::str::replace_umlaute_to_windows_compatible(std::string *mainstring) {
    replace_chars(mainstring,"Ä","\x00c4");
    replace_chars(mainstring,"Ü","\x00dc");
    replace_chars(mainstring,"Ö","\x00d6");
    replace_chars(mainstring,"ä","\x00E4");
    replace_chars(mainstring,"ü","\x00fc");
    replace_chars(mainstring,"ö","\x00f6");
    replace_chars(mainstring,"ß","\x00df");
//    replace_chars(mainstring,"µ","\u03bc");
	return true;
}

void tools::str::remove_spaces(std::string *mainstring)
{
// 	for (int i=0;i<mainstring->length();i++)
// 	{
// 		if (isspace(mainstring->at(i))) mainstring->erase(i);
// 	}
// 	remove_spaces_from_string_start(mainstring);
// 	remove_spaces_from_string_end(mainstring);
    replace_chars(mainstring,"\n","");
    replace_chars(mainstring,"\r","");
    replace_chars(mainstring," ","");
    replace_chars(mainstring,"\t","");
// 	replace_chars(mainstring,"\f","");
// 	replace_chars(mainstring,"\v","");
	return;
}
void tools::str::remove_spaces(std::vector<std::string> * mainstring) {
	for (int i=0;i<mainstring->size();i++) {
    tools::str::remove_spaces(&mainstring->at(i));
    }
	return;
}
void tools::str::remove_spaces(std::vector<std::vector<std::string>> * mainstring) {
	for (int i=0;i<mainstring->size();i++) {
    tools::str::remove_spaces(&mainstring->at(i));
    }
	return;
}

std::string tools::str::remove_linebreakers_from_string(std::string mainstring) {
	std::string copystring=mainstring;
	replace_chars(&copystring,"\n","");
	replace_chars(&copystring,"\r","");
	return copystring;
}

std::string tools::str::remove_NOTchars_from_string(std::string mainstring,std::string NOTchars) 
{
	std::string filtered_mainstring;
	for (int i=0;i<mainstring.length();i++)
	{
// 		bool found=false;
		for (int j=0;j<NOTchars.length();j++)
		{
            if (mainstring[i]==NOTchars[j])
			{
                filtered_mainstring.push_back(mainstring[i]);
				break;
			}
		}
// 		if (!found) filtered_mainstring.push_back(mainstring[i]);
		
	}
	return filtered_mainstring;
}

std::string tools::str::remove_chars_from_string(std::string mainstring,std::string chars)
{
	std::string result = mainstring;
	for (int j=0;j<chars.length();j++) {
		for (int i=0;i<result.length();i++) {
			if (result[i] == chars[j]) { 
				result.erase(i,1);		
			}
		}
	}
	return result;
}

void tools::str::remove_substring_from_mainstring(std::string* mainstring,std::string substring)
{
	size_t found = mainstring->find(substring);
    while (found!=std::string::npos)
	{
		mainstring->erase(found,substring.length());
		found = mainstring->find(substring);
	}
	return;
}

//https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
int tools::str::levenshtein_distance(const std::string &s1,    const std::string &s2)
{
    // To change the type this function manipulates and returns,    change
	// the return type and the types of the two variables below.
	int s1len = s1.size();
	int s2len = s2.size();
	
	auto column_start = (decltype(s1len))1;
	
	auto column = new decltype(s1len)[s1len + 1];
    std::iota(column + column_start,    column + s1len + 1,    column_start);
	
    for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
        for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	return result;
}

std::vector<std::string> tools::str::args_to_vec(int     argc,    char **argv) {
std::vector<std::string> vec;
    for (int i=1;i<argc;i++) {
    vec.push_back(argv[i]);
    }
    return vec;
}

void tools::str::remove_duplicates(std::vector<std::string>* vec) {
std::set<std::string> s(    vec->begin(),    vec->end() );
    vec->assign(    s.begin(),    s.end() );
    return;
}
std::vector<std::string> tools::str::remove_similarities(std::vector<std::string>* vec) {
    sort(vec->begin(),    vec->end());
//     reverse(vec->begin(),vec->end());
	int last_found_pos;
std::vector<std::string> results;
    remove_duplicates(vec);

    for (int i=0;i<vec->size();i++) {
			last_found_pos = i;
			results.push_back(vec->at(i));
    for (int j=i+1;j<vec->size();j++) {
    if (vec->at(j).find(vec->at(i))!=std::string::npos) last_found_pos=j;
				else break;
				
    }
    i = last_found_pos;
//     if (i std::< vec->size()) results.push_back(vec->at(i));
    }

    return results;
}

void tools::str::to_lower(std::string* upper) {
    transform(upper->begin(),upper->end(),upper->begin(),[](unsigned char c) { return tolower(c); });
}
void tools::str::to_lower(std::vector<std::string>* uppers) {
    for (int i=0;i<uppers->size();i++) {
    to_lower(&uppers->at(i));
    }
}

int tools::str::extract_int_from_str(std::string str) {
std::string numbers_str=str;
//     for (int i=0; i<str.length();i++) {
//     if (str.find_first_of("0123456789") != std::string::npos) {
//     numbers_str += str[i];
//     }
//     }
//     cout std::<std::< numbers_str std::<std::< std::endl;
    tools::str::replace_NOTchars_from_string(&numbers_str,"1234567890","");
    return tools::str::str_to_int(numbers_str);
}

std::vector<std::string> tools::str::find_all_substrings(std::vector<std::string>* vec,std::string find_this) {
std::vector<std::string> results;
    for (int i=0;i<vec->size(); i++) {
    if (vec->at(i).find(find_this)!=std::string::npos)
    results.push_back(vec->at(i));
    }
    return results;
}


std::vector<std::string> tools::str::find_all_substrings(std::set<std::string>& search_these,std::string find_this)
{
	std::set<std::string> result_set;
    for (auto& search_this:search_these)
	{
    if (search_this.find(find_this)!=std::string::npos)
    result_set.insert(search_this);
    }
    return std::vector<std::string>(result_set.begin(),result_set.end());
}
























































