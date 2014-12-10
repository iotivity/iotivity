#ifndef _OCANDROID_H_
#define _OCANDROID_H_

#ifdef __ANDROID__
#include <string>

// FIXME : ugly hack
// Android missing functions
namespace std {
	int stoi(const std::string& s);
	double stod(const std::string& s);
	long long stoll(const std::string& s);
	unsigned long long stoull(const std::string& s);
	long double stold(const string& s);

	std::string to_string(int i);
	std::string to_string(std::uint32_t i);
	std::string to_string(double d);
}

#endif


#endif
