// #include "test.hpp"
// 
// Fn_t::Fn_t(const std::__cxx11::string& filename) : filename(filename)
// {
// }
// void Fn_t::print()
// {
// 	cout << filename << endl;
// }
// bool Fn_t::is_set()
// {
// 	if (filename!="") return true;
// 	return false;
// }
// bool Fn_t::operator==(const Fn_t& obj)
// {
// 	if (obj.filename == filename) return true;
// 	return false;
// }
// 
// 
// Fc_t::Fc_t(const std::__cxx11::string& filename) : filename(filename)
// {
// }
// void Fc_t::print()
// {
// 	cout << filename << endl;
// }
// bool Fc_t::is_set()
// {
// 	if (filename!="") return true;
// 	return false;
// }
// bool Fc_t::operator==(Fc_t& obj)
// {
// 	if (obj.filename == filename) return true;
// 	return false;
// }
// 
// F_t::F_t(const std::__cxx11::string& filename) : fn(filename), fc(filename)
// {
// }
// bool F_t::is_set()
// {
// 	if (fn().is_set() && fc().is_set()) return true;
// 	return false;
// }
// bool F_t::operator==(F_t& obj)
// {
// // 	if (fn() == obj.fn() && fc() == obj.fc()) return true;
// 	if (fn() == obj.fn())
// 		return true;
// 	return false;
// }
// 
// 
// M_t::M_t(vector<string>& filenames) 
// {
// 	for (auto& f: filenames)
// 	{
// 		if (f == "") continue;
// 		F_t F(f);
// 		if (!F.is_set()) continue;
// 		if (Fs.size()==0 || Fs.at(0)==F) 
// 		{
// 			Fs.push_back(F);
// 			f = ""; // clear it
// 		}
// 	}
// }
// 
// MG_t::MG_t(vector<string>& filenames)
// {
// 	for (auto& f: filenames)
// 	{
// 		if (f=="") continue;
// 		if (Ms.size()==0) Ms.push_back(filenames); // yes this is right, always the whole vector
// 	}
// }
