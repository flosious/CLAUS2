#include "quantity.hpp"

/****************************************************/
/*********  quantity::table_t::column_t  ************/
/****************************************************/

quantity::table_t::column_t::column_t() : name(""), comment("")
{
}

quantity::table_t::column_t::column_t(long long unsigned int line_id,const quantity_t& q, string name, string comment) : name(name), comment(comment)
{
	quantity_entries_p.insert(pair<long long unsigned int, quantity::quantity_t>(line_id,q));
	if (!q.is_set())
	{
		logger::debug(7,"quantity::table_t::column_t::column_t()","q is not set!",q.to_string());
	}
}

const quantity::quantity_t quantity::table_t::column_t::header() const
{
	for (const auto& entry : quantity_entries())
	{
		if (entry.second.is_set())
		{
			auto h = entry.second;
			return h.clear();
		}
	}
	if (quantity_entries_p.size()>0)
	{
		auto h = quantity_entries().begin()->second;
		return h;
	}
	return {};
}

bool quantity::table_t::column_t::is_set() const
{
	//at least 1 quantity entry line should be set
	for (const auto& entry : quantity_entries())
	{
		if (entry.second.is_set())
			return true;
	}
	return false;
}

bool quantity::table_t::column_t::operator==(const column_t& col) const
{
	if (name != col.name)
		return false;
	if (!is_set() || !col.is_set())
		return false;
	if (header().name()!=col.header().name())
		return false;
	if (header().dimension()!=col.header().dimension())
		return false;
	if (header().unit()!=col.header().unit())
		return false;
	return true;
}

quantity::quantity_t quantity::table_t::column_t::get_quantity(long long unsigned int line_id) const
{
	if (quantity_entries_p.find(line_id)==quantity_entries_p.end()) // not found
		return {};
	return quantity_entries_p.at(line_id);
}

bool quantity::table_t::column_t::operator!=(const column_t& col) const
{
	return !operator==(col);
}

string quantity::table_t::column_t::to_string() const
{
	stringstream out;
	out << "name: '" << name << "'; ";
	out << "quantity: '" << header().name() << " [" << header().unit().to_string() << "]" << " {" << header().dimension().to_string() << "}";
	if (comment!="") out << "; comment: '" << comment << "'";
	return out.str();
}

bool quantity::table_t::column_t::add_entries(const column_t& col)
{
	if (*this != col) //nope
		return false;
	for (const auto& entry : col.quantity_entries())
	{
		if (quantity_entries_p.find(entry.first)==quantity_entries_p.end()) //entry/line is not in column
		{
			add_quantity_entry(entry.first,entry.second.change_unit(header().unit()));
		}
		else //entry / line is already in column
		{
			auto& Q_in_line = quantity_entries_p.at(entry.first);
			if (!Q_in_line.is_set()) // if the quantity of this entry/line is not set, then overwrite it with the new one
				Q_in_line = entry.second.change_unit(header().unit());
		}
		//do nothing if the entry/line already exists in this column and its quantity is set
	}
	return true;
}

bool quantity::table_t::column_t::add_quantity_entry(long long unsigned int line_id, quantity::quantity_t q)
{
	if (header().is_set())
		q = q.change_unit(header().unit());
	if (!q.is_set())
		return false;
	if (quantity_entries().find(line_id)!=quantity_entries().end())
	{
		logger::error("quantity::table_t::column_t::add_quantity_entry","line_id already exists","quantity_entries.at("+ ::to_string(line_id)+")="+quantity_entries().at(line_id).to_string(),"returning false");
		return false;
	}
	quantity_entries_p.insert(pair<long long unsigned int,quantity::quantity_t>(line_id,q));
	return true;
}

const map<long long unsigned int, quantity::quantity_t>& quantity::table_t::column_t::quantity_entries() const
{
	return quantity_entries_p;
}

bool quantity::table_t::column_t::unset_quantity_entry(long long unsigned int line_id)
{
	if (quantity_entries().find(line_id)==quantity_entries().end())
	{
		logger::error("quantity::table_t::column_t::unset_quantity_entry()","could not find line_id=" + ::to_string(line_id),"returning false");
		return false;
	}
	quantity_entries_p.at(line_id).clear();
	return true;
}

bool quantity::table_t::column_t::unset_quantities(const quantity_t& q)
{
	const auto line_ids = get_line_ids(q);
	bool success=true;
	for (const auto& id : line_ids)
	{
		if (!unset_quantity_entry(id))
		{
			logger::error("quantity::table_t::column_t::unset_quantities()",q.to_string(),"keep on going and returning false at the end");
			success=false;
		}
	}
	return success;
}

bool quantity::table_t::column_t::replace_quantity_entry(long long unsigned int line_id, const quantity_t& q_replacement)
{
	if (quantity_entries().find(line_id)==quantity_entries().end())
	{
		logger::error("quantity::table_t::column_t::replace_quantity_entry()","could not find line_id=" + ::to_string(line_id),"returning false");
		return false;
	}
	quantity_entries_p.at(line_id)=q_replacement;
	return true;
}

vector<long long unsigned int> quantity::table_t::column_t::get_line_ids(quantity_t q) const
{
	vector<long long unsigned int> findings;
	q = q.change_unit(header().unit());
	if (!q.is_set())
		return {};
	for (const auto& entry : quantity_entries_p)
	{
		if (entry.second==q)
			findings.push_back(entry.first);
	}
	return findings;
}

bool quantity::table_t::column_t::erase_quantity_entry(long long unsigned int line_id)
{
	if (quantity_entries_p.find(line_id)==quantity_entries_p.end())
	{
		logger::debug(7,"quantity::table_t::column_t::erase_quantity_entry()","could not find line_id=" + ::to_string(line_id), to_string(),"returning false");
		return false;
	}
	quantity_entries_p.erase(line_id);
	return true;
}

quantity::table_t::column_t quantity::table_t::column_t::median() const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.median();
	}
	copy.comment ="median";
	return copy;
}

quantity::table_t::column_t quantity::table_t::column_t::mean() const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.mean();
	}
	copy.comment ="mean";
	return copy;
}

quantity::table_t::column_t quantity::table_t::column_t::geo_mean() const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.geo_mean();
	}
	copy.comment ="geo_mean";
	return copy;
}

quantity::table_t::column_t quantity::table_t::column_t::trimmed_mean(float alpha) const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.trimmed_mean(alpha);
	}
	copy.comment ="trimmed_mean("+ tools::to_string(alpha) +")";
	return copy;
}

quantity::table_t::column_t quantity::table_t::column_t::cut_mean(float alpha) const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.cut_mean(alpha);
	}
	copy.comment ="cut_mean("+ tools::to_string(alpha) +")";
	return copy;
}

quantity::table_t::column_t quantity::table_t::column_t::cut_median(float alpha) const
{
	auto copy = *this;
	for (auto& Q : copy.quantity_entries_p)
	{
		Q.second = Q.second.cut_median(alpha);
	}
	copy.comment ="cut_median("+ tools::to_string(alpha) +")";
	return copy;
}

/******************************************/
/*********  quantity::table_t  ************/
/******************************************/

const set<long long unsigned int> quantity::table_t::line_ids() const
{
	set<long long unsigned int> ids;
	
	for (const auto& col : columns())
	{
		for (const auto& entry : col.quantity_entries())
			ids.insert(entry.first);
	}
	
	for (const auto& line : line_names)
		ids.insert(line.first);
		
	return ids;
}

long long unsigned int quantity::table_t::get_new_random_line_id() const
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	long long unsigned int R = rand();
	while (line_ids().find(R)!=line_ids().end())
		R = rand();
	return R;
#else
	long long unsigned int R = random();
	while (line_ids().find(R)!=line_ids().end())
		R = random();
	return R;
#endif
}

long long unsigned int quantity::table_t::get_new_sequential_line_id() const
{
	long long unsigned int R = 0;
	while (line_ids().find(R)!=line_ids().end())
		R++;
	return R;
}

bool quantity::table_t::add(const table_t& table)
{
	if (!add(table.columns()))
		return false;
	line_names.insert(table.line_names.begin(),table.line_names.end());
	return true;
}

bool quantity::table_t::add(const quantity::quantity_t& Q, string col_name)
{
	return add(Q,col_name,get_new_sequential_line_id());
}
bool quantity::table_t::add(const quantity::quantity_t& Q, string col_name, long long unsigned int line_id)
{
	if (!Q.is_set())
	{
		logger::error("quantity::table_t::add()","Q not set",Q.to_string(),"returning false");
		return false;
	}
	column_t col(line_id,Q,col_name);
	return add(col);
}
bool quantity::table_t::add(const column_t& col)
{
	if (!col.is_set())
		return false;
	if (find(columns_p.begin(),columns_p.end(),col)==columns_p.end()) ///col NOT yet in table
	{
		logger::debug(7,"quantity::table_t::add()","col not found in table",col.to_string(),"adding col");
		//add the column to the table
		columns_p.push_back(col);
	}
	else //col is already in table
	{
		auto col_in_table = column(col); // the column within the table
		if (col_in_table==nullptr)
		{
			logger::error("quantity::table_t::add()","column not found in table",col.to_string(),"returning false");
			return false;
		}
		if (!col_in_table->add_entries(col))
		{
			logger::error("quantity::table_t::add()","add_entries() returned false",col.to_string(),"returning false");
			return false;
		}
	}
	return true;
}
bool quantity::table_t::add(const vector<column_t>& cols)
{
	bool success=true;
	for (const auto& col : cols)
	{
		if (!add(col))
		{
			logger::error("quantity::table_t::add()",col.to_string(),"keep on going and returning false at the end");
			success=false;
		}
	}
	return success;
}

quantity::table_t::column_t* quantity::table_t::column(const quantity::table_t::column_t& col)
{
	for (auto& col_in_table : columns_p)
	{
		if (col_in_table==col)
			return &col_in_table;
	}
	return nullptr;
}

const vector<quantity::table_t::column_t>& quantity::table_t::columns() const
{
	return columns_p;
}
// bool quantity::table_t::add_or_overwrite(const ::map<long long unsigned int,string>& line_names)
// {
// 	for (auto& l : line_names)
// 		add_or_overwrite(l.first,l.second);
// 	return true;
// }
// 
// bool quantity::table_t::add_or_overwrite(long long unsigned int line_id, string line_name)
// {
// 	line_names_p[line_id] = line_name;
// 	return true;
// }

string quantity::table_t::to_string() const
{
	stringstream out;
	out << "columns().size()=" << columns().size() << "; line_ids.size()="<< line_ids().size();;
	return out.str();
}

quantity::table_t& quantity::table_t::erase_line(long long unsigned int line_id)
{
	for (auto& col : columns_p)
	{
		col.erase_quantity_entry(line_id);
	}
	line_names.erase(line_id);
	return *this;
}

quantity::table_t& quantity::table_t::erase_lines(set<long long unsigned int> line_ids)
{
	for (auto& id : line_ids)
		erase_line(id);
	return *this;
}

const quantity::table_t::column_t* quantity::table_t::get_col(const column_t& col) const
{
	const auto c = find(columns_p.begin(),columns_p.end(),col);
	if (c!=columns_p.end())
		return &(*c);
	return nullptr;
}

const quantity::table_t::column_t* quantity::table_t::get_col(const quantity_t& col_header, string col_name) const
{
	column_t col(0,col_header, col_name);
	return get_col(col);
}

quantity::map_t quantity::table_t::get_map(const column_t& col_X, const column_t& col_Y) const
{
	table_t T=*this;
	T.remove_empty_lines();
	const auto col_X_in_table = T.get_col(col_X);
	const auto col_Y_in_table = T.get_col(col_Y);

	auto q_X( col_X_in_table->header());
	auto q_Y( col_Y_in_table->header());
	
	for (auto line : T.line_ids())
	{
		q_X << col_X_in_table->get_quantity(line);
		q_Y << col_Y_in_table->get_quantity(line);
	}
	return map_t(q_X,q_Y);
}

quantity::map_t quantity::table_t::get_map() const
{
	if (columns().size()>1)
		return get_map(0,1);
	logger::debug(7,"quantity::table_t::get_map()","columns().size()<2");
	return {};
}

quantity::map_t quantity::table_t::get_map(int X_col_idx, int Y_col_idx) const
{
	if (X_col_idx > columns().size() || Y_col_idx > columns().size())
	{
		logger::debug(6,"quantity::table_t::get_map(X_idx,Y_idx)","X_col_idx < columns().size() || Y_col_idx < columns().size()");
		return {};
	}
	return get_map(columns_p.at(X_col_idx),columns_p.at(Y_col_idx));
}
		
quantity::table_t& quantity::table_t::remove_empty_lines()
{
	set<long long unsigned int> erase_ids;
	//get line_ids with
	for (const auto line_id : line_ids())
	{
		for (const auto& col : columns())
		{
			if (!col.get_quantity(line_id).is_set())
			{
				erase_ids.insert(line_id);	
				break;
			}
		}
	}
	erase_lines(erase_ids);
	return *this;
}

quantity::table_t& quantity::table_t::remove_empty_columns()
{
	for (auto ci = columns_p.begin();ci!=columns_p.end();++ci)
		if (!ci->is_set())
			columns_p.erase(ci);
	return *this;
}

bool quantity::table_t::is_set() const
{
	for (const auto& col : columns())
		if (col.is_set())
			return true;
	return false;
}
