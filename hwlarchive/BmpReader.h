#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <map>
#include <vector>

class BmpReader {

public:
	uint16_t *Read24or32BitsFileTo16Bits(std::string fileName, uint32_t &outWidth, uint32_t &outHeight);
};