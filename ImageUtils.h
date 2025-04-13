#pragma once

#include "lodepng.h"

#include <vector>
#include <iostream>

bool tryDecodeImage(const char* filename, std::vector<unsigned char>& image, unsigned int& width, unsigned int& height);
bool tryEncodeImage(const std::string filename, std::vector<unsigned char>& image, unsigned width, unsigned height);