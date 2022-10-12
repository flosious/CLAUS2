/*
	Copyright (C) 2020 Florian Bärwolf
	floribaer@gmx.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PRINT_HPP
#define PRINT_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <set>
// #include "id.hpp"
// #include "quantity.hpp"
// #include "isotope.hpp"
// #include "measurement.hpp"



/// this class is essentially just for debugging...
class print {
private:
    
public:
//     static void measurement(measurement_t* measurement, std::string prefix="");
// 	static void id(identifier_t* id,std::string prefix="");
// 	static void quantity(quantity::quantity_t* quantity, std::string prefix="");
// 	static void cluster(cluster_t* cluster, std::string prefix="");
};

template <typename T>
void print(T ska) {
    std::cout << "\t"<< ska; std::cout << std::endl;
}

template <typename T>
void print(std::vector<T> ska) {
	for (int i=0; i<ska.size();i++) {
        std::cout << "[" << i << "]";
		print(ska[i]);
	}
}

template <typename T>
void print(std::set<T> ska) {
	int i=0;
	for (auto& s:ska)
	{
        std::cout << "[" << i << "]";
		print(s);
		i++;
	}
}

template <typename T>
void print(std::vector< std::vector<T> > mat, std::string delimiter="\t") {
    std::cout << std::endl;
	for (int i=0;i<mat.size();i++) {
		for (int j=0; j<(mat.at(i)).size();j++) {
            std::cout << "[" << i << "]";
            std::cout << "[" << j << "]";
            std::cout << "\t";
			print((mat)[i][j]);
		}
        std::cout << std::endl;
	}
}

template <typename T>
void print_not_empty(std::vector< std::vector<T> > mat, std::string delimiter="\t") {
    std::cout << std::endl;
	for (int i=0;i<mat.size();i++) {
		for (int j=0; j<(mat.at(i)).size();j++) 
		{
			if(mat.at(i)[j]!="") 
			{
                std::cout << "[" << i << "]";
                std::cout << "[" << j << "]";
                std::cout << "\t";
				print((mat)[i][j]);
			}
		}
        std::cout << std::endl;
	}
}

template <typename T>
void print(std::vector< std::vector< std::vector<T> > > ten) {
	for (int z=0;z<ten.size();z++) {
		for (int i=0;i<(ten.at(z)).size();i++) {
			for (int j=0; j<(ten.at(z)[i]).size();j++) {
//                 if ((ten)[z][i][j].size()>0) {
                std::cout << "[" << z << "]";
                std::cout << "[" << i << "]";
                std::cout << "[" << j << "]";
                std::cout << "\t";
				print((ten)[z][i][j]);
//                 }
			}
		}
	}
}

template <typename T>
void print(std::map<T, T>  mat) {
	for (auto& it : mat) {
        std::cout << it.first << "\t" << it.second << std::endl;
//         print(it.second);
    }
}

// template <typename T>
// void print(std::map<T, T>  mat) {
// 	for (auto& it : mat) {
// 		std::cout << it.first << ":\n";
//         print(it.second);
//     }
// }


// void print(std::map<std::string, quantity::quantity_t>  mat) {
// 	for (auto& it : mat) {
//         if (it.second.data.size()==1) std::cout << it.first << " = " << it.second.data[0] << "[" << it.second.unit << "]";
// 		
//         print(it.second);
//     }
// }

// void print(std::map<std::string, std::vector<std::string>>  mat) {
// 	for (std::map<std::string, std::vector<std::string>>::iterator it=mat.begin(); it!=mat.end(); ++it) {
//         std::cout << it->first << ":\n";
//         print(it->second);
//     }
// }

#endif // TOOLS_HPP
