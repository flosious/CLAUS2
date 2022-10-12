#include "tools.hpp"

// static size_t WriteCallback(void *contents,std:: size_t size,std:: size_t nmemb,std:: void *userp)
// {
// std:: ((std::string*)userp)->append((char*)contents,std:: size * nmemb);
// std:: return size * nmemb;
// }
// 
// 
// std::string tools::web::get_contents(std::string url) 
// {
// 	std::string contents;
// std:: 	const char * site = url.c_str();
// std:: 	CURL *curl;
// 	CURLcode res;
// // 	std::string buffer;
// 	curl = curl_easy_init();
// 	if (curl) {
// 		curl_easy_setopt(curl,CURLOPT_URL,site);
// 		// Tell libcurl what function to call when it has data
// std:: 	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback); 
// 		/* example.com is redirected,std:: so we tell libcurl to follow redirection */ 
// std:: 		curl_easy_setopt(curl,std:: CURLOPT_FOLLOWLOCATION,std:: 1L);
// 		curl_easy_setopt(curl,std:: CURLOPT_WRITEDATA,std:: &contents);
// 		
// 		/* Perform the request,std:: res will get the return code */ 
// std:: 		res = curl_easy_perform(curl);
// 		/* always cleanup */ 
// std:: 		curl_easy_cleanup(curl);
// std:: 		/* Check for errors */ 
// std:: 		if(res != CURLE_OK)	{
// 			cerr std::<std::< "curl_easy_perform() failed: " std::<std::< curl_easy_strerror(res) std::<std::< std::endl; // 
// 			cout std::<std::< "Curl not OK" std::<std::< std::endl;
// 			return "";
// 		}
// 	}
// 	else {
// 	std:: 	cout std::<std::< "curl_easy_init() failed\n" std::<std::< std::endl;
// 		return "";
// 	}
// 	return contents; 
// }
