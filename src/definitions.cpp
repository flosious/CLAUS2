#include "definitions.hpp"


// definitions_t::definitions_t(bool olcdbid_s, 
// 							 bool lot_s, 
// 							 bool lot_split_s,
// 							 bool wafer_s, 
// 							 bool monitor_s, 
// 							 bool chip_s, 
// 							 bool group_s, 
// 							 bool repetition_s,
// 							 bool simple_name_s) 
// 								: 
// 							 olcdbid(olcdbid_s),
// 							 lot(lot_s),
// 							 lot_split(lot_split_s),
// 							 wafer(wafer_s),
// 							 monitor(monitor_s),
// 							 chip(chip_s),
// 							 group(group_s),
// 							 repition(repetition_s),
// 							 simple_name(simple_name_s)
// {
// }

// template<typename T> const bool definitions_t::operator==(const T& obj1, const T& obj2) const
// {
// 	//TODO filename_id_structure!
// 	if (lot)
// 	{
// 		if (lot<obj.lot) return true;
// 		if (lot>obj.lot) return false;
// 	}
// 	
// 	if (lot_split)
// 	{
// 		if (lot_split<obj.lot_split) return true;
// 		if (lot_split>obj.lot_split) return false;
// 	}
// 	
// 	if (wafer)
// 	{
// 		if (wafer<obj.wafer) return true;
// 		if (wafer>obj.wafer) return false;
// 	}
// 	
// 	if (monitor)
// 	{
// 		if (monitor<obj.monitor) return true;
// 		if (monitor>obj.monitor) return false;
// 	}
// 	
// 	if (chip)
// 	{
// 		if (chip.x<obj.chip.x) return true;
// 		if (chip.x>obj.chip.x) return false;
// 		if (chip.y<obj.chip.y) return true;
// 		if (chip.y>obj.chip.y) return false;
// 	}
// 	
// 	if (simple_name)
// 	{
// 		if (simple_name<obj.simple_name) return true;
// 		if (simple_name>obj.simple_name) return false;
// 	}
// 	
// 	if (olcdbid)
// 	{
// 		if (olcdbid<obj.olcdbid) return true;
// 		if (olcdbid>obj.olcdbid) return false;
// 	}
// 	
// 	if (group)
// 	{
// 		if (group<obj.group) return true;
// 		if (group>obj.group) return false;
// 	}
// 	
// 	if (repition)
// 	{
// 		if (repition<obj.repition) return true;
// 		if (repition>obj.repition) return false;
// 	}
// 	
// 	return false;
// }
