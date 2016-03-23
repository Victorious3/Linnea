#pragma once

#include <spdlog/spdlog.h>
#include "Linnea.h"

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long uint64;
typedef signed long long int64;

#define LOG (Linnea::instance()->logger())

namespace common {

	int64 time_nano();

	int64 time_milli();

	void str_trim(std::string& str);

	bool file_exists(std::string& filename);

	string syserror();
}