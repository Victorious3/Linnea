#include "common.h"

#include <chrono>
#include <fstream>

int64 common::time_nano() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int64 common::time_milli() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void common::str_trim(std::string& str) {
	while (str.length() > 0 && (str[str.length() - 1] == ' ' || str[str.length() - 1] == '\t'))
		str.erase(str.length() - 1, 1);

	while (str.length() > 0 && (str[0] == ' ' || str[0] == '\t'))
		str.erase(0, 1);
}

bool common::file_exists(std::string& filename) {
	ifstream f(filename);
	bool good = f.good();
	f.close();
	return good;
}

string common::syserror() {
	char sys_msg[64];
	strerror_s(sys_msg, errno);
	return string(sys_msg);
}