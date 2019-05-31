#pragma once

#include <stdio.h>
#include <tchar.h>

#include "HWLContainer.h"
#include "BmpReader.h"
#include <cstring>
#include <vector>
#include <filesystem>

namespace zlib {
	#define ZLIB_WINAPI
	#include <zlib.h>
}
