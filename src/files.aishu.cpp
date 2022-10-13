/*
    Copyright (C) 2021, 2022 Florian Bärwolf
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

#include "files.hpp"

files_::aishu_t::aishu_t(std::string& filename, const std::string& contents_string)
    : logger(__FILE__,"files_::aishu_t"), name(filename), contents(filename, contents_string)
{
}

bool files_::aishu_t::operator<(const aishu_t& obj) const
{
    if (name.filename() < obj.name.filename()) return true;
    return false;
}

void files_::aishu_t::export_to_aishu_format(std::string export_filename_w_path)
{
    std::vector<std::vector<std::string>> lines;
    std::vector<std::string> line1, line2;
    std::vector<std::string> header,headerQs;
    auto blocks = contents.blocks();
    if (blocks.size()==0)
        return;
    header = {"T[°C]","chip_X_pos","chip_Y_pos"};
    ///line: Temp | Chip | <MEANS(Quantity)>
    for (auto& block : blocks)
    {
        std::string T (std::to_string(block.temperature().data().front()));

        std::string Chip1_x (std::to_string(block.chip1().pos_x));
        std::string Chip1_y (std::to_string(block.chip1().pos_y));
        line1 = {T,Chip1_x,Chip1_y};
        for (auto Q : block.chip1().quantities)
        {
            line1.push_back(std::to_string(Q.mean().data().front()));
            if (headerQs.size()<block.chip1().quantities.size())
            {
                headerQs.push_back({Q.name()+Q.unit().to_string()});
            }
        }


        std::string Chip2_x (std::to_string(block.chip2().pos_x));
        std::string Chip2_y (std::to_string(block.chip2().pos_y));
        line2 = {T,Chip2_x,Chip2_y};
        for (auto Q : block.chip2().quantities)
        {
            line2.push_back(std::to_string(Q.mean().data().front()));
        }
        lines.push_back(line1);
        lines.push_back(line2);
    }

    header.insert(header.end(),headerQs.begin(),headerQs.end());
    lines.insert(lines.begin(),header);
//    matrix = tools::mat::transpose_matrix(matrix);
    std::string out = tools::mat::format_matrix_to_string(&lines);
    if (export_filename_w_path=="")
    {
        export_filename_w_path = name.filename_with_path+"-summary.txt";
    }
    tools::file::write_to_file(export_filename_w_path,&out,false);
}

files_::aishu_t::name_t::name_t(std::string filename) : file_t::name_t(filename,"_",{".csv"},{})
{

}

files_::aishu_t::contents_t::contents_t(std::string& filename_with_path, const std::string& contents_string)
    : file_t::contents_t(filename_with_path,",",{"Circuit"},{0,1},contents_string),
      logger(__FILE__,"files_::aishu_t::contents_t")
{

}

std::vector<files_::aishu_t::contents_t::block_t> files_::aishu_t::contents_t::blocks()
{
    std::vector<files_::aishu_t::contents_t::block_t> blocks;
    if (raw_data_tensor().size() != raw_header_tensor().size())
    {
        logger.error(__func__,"this").if_statement("raw_data_tensor().size()",std::to_string(raw_data_tensor().size()),"=!","raw_header_tensor().size()",std::to_string(raw_header_tensor().size()));
        return {};
    }
    for (int i=0;i<raw_data_tensor().size();i++)
    {
        files_::aishu_t::contents_t::block_t B(raw_header_tensor().at(i),raw_data_tensor().at(i));
        blocks.push_back(B);
    }
    return blocks;
}

files_::aishu_t::contents_t::block_t::block_t(const std::vector<std::vector<std::string>>& header,
                                              const std::vector<std::vector<std::string>>& data)
    : logger(__FILE__,"files_::aishu_t::contents_t::block_t"), header(header), data(data)
{

}

const std::string files_::aishu_t::contents_t::block_t::header_to_string() const
{
    std::stringstream out;
    for (auto l : header)
    {
        for (auto c : l)
        {
            out << c;
        }
    }
    return out.str();
}

quantity::quantity_t files_::aishu_t::contents_t::block_t::temperature()
{
    std::smatch match;
    //Temp: 25°C
    std::string h = header_to_string();
    std::regex reg ("Temp: ([0-9]{1,3})");
    if (regex_search(h,match,reg))
    {
        double out = tools::str::str_to_double(match[1]);
        quantity::quantity_t T("temperature",{out},units::derived::celsius);
        logger.info(__func__,"temperature").value(T.to_string_short());
        return T;
    }
    logger.info(__func__,"temperature").value("not parsed",10,h);
}

files_::aishu_t::contents_t::block_t::chip_t files_::aishu_t::contents_t::block_t::chip(int chip_number)
{
    files_::aishu_t::contents_t::block_t::chip_t C;
    std::smatch match;
    std::regex reg ("Chip"+ std::to_string(chip_number)  +" x=([0-9]{1,2})y=([0-9]{1,2})");
    //Chip1 x=4y=2 Chip2 x=4y=4
    std::string h = header_to_string();
    if (regex_search(h,match,reg))
    {
        C.pos_x = tools::str::str_to_int(match[1]);
        logger.debug(__func__,"pos_x").value(std::to_string(C.pos_x));
        C.pos_y = tools::str::str_to_int(match[2]);
        logger.debug(__func__,"pos_y").value(std::to_string(C.pos_y));
    }
    else
        logger.debug(__func__,"reg").signal("not found");
    std::regex reg_col (std::to_string(chip_number));
    std::vector<int> chip_cols;
    for (int col=0; col<header.back().size();col++)
    {
        if (regex_search(header.back().at(col),match,reg_col))
        {
            chip_cols.push_back(col);
            logger.debug(__func__,"chip_col").value(std::to_string(col));
        }
    }
    std::vector<std::vector<std::string>> transposed_data_str_mat = tools::mat::transpose_matrix(data);
    for (auto col : chip_cols)
    {
        auto h = header.back().at(col);
        std::vector<double> d = tools::mat::str_vec_to_double_vec(transposed_data_str_mat.at(col));
        unit_t unit (tools::str::get_string_between_string_A_and_next_B(&h,"[","]"));
        std::string name (*(tools::str::get_strings_between_delimiter(h,"[").begin()));
        name = tools::str::remove_chars_from_string(name,std::to_string( chip_number));
        quantity::quantity_t Q(name,d,unit);
        logger.debug(__func__,"quantity").value(Q.to_string());
        C.quantities.push_back(Q);
    }
    return C;
}

files_::aishu_t::contents_t::block_t::chip_t files_::aishu_t::contents_t::block_t::chip1()
{
    logger.debug(__func__,"this").enter();
    auto C = chip(1);
    logger.debug(__func__,"this").exit();
    return C;
}
files_::aishu_t::contents_t::block_t::chip_t files_::aishu_t::contents_t::block_t::chip2()
{
    logger.debug(__func__,"this").enter();
    auto C = chip(2);
    logger.debug(__func__,"this").exit();
    return C;
}
