#include <cmath>
#include <fstream>
#include <iostream>
#include "Tools.hpp"
//#define STD_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

namespace StrikeEngine 
{
    namespace Tools
    {
		std::vector<char> GetBinaryFileContents(const std::string& filename)
		{
			std::ifstream file(filename, std::ios::binary);
			if (file.fail())
			{
				std::cout << "Could not open \"" << filename << "\"file" << std::endl;
				return std::vector<char>();
			}

			std::streampos begin, end;
			begin = file.tellg();
			file.seekg(0, std::ios::end);
			end = file.tellg();

			std::vector<char> res(static_cast<size_t>(end - begin));
			file.seekg(0, std::ios::beg);
			file.read(&res[0], end - begin);
			file.close();

			return res;
		}

    }
}

