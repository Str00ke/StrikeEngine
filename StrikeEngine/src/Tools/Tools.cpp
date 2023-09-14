#include <cmath>
#include <fstream>
#include <iostream>
#include "Tools/Tools.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../Externals/stb_image.h"

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

		std::vector<char> GetImageData(const std::string& filename, int requestedComponents, int* width, int* height, int* components, int* dataSize)
		{
			std::vector<char> fileData = Tools::GetBinaryFileContents(filename);
			if (fileData.size() == 0)
				return std::vector<char>();

			int tmpWidth = 0, tmpHeight = 0, tmpComponents = 0;
			unsigned char* imgData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(&fileData[0]), static_cast<int>(fileData.size()), &tmpWidth, &tmpHeight, &tmpComponents, requestedComponents);
			if ((imgData == nullptr) || (tmpWidth <= 0) || (tmpHeight <= 0) || (tmpComponents <= 0))
			{
				std::cout << "Could not read image data" << std::endl;
				return std::vector<char>();
			}

			int size = (tmpWidth) * (tmpHeight) * (requestedComponents <= 0 ? tmpComponents : requestedComponents);

			if (dataSize)
				*dataSize = size;

			if (width)
				*width = tmpWidth;

			if (height)
				*height = tmpHeight;

			if (components)
				*components = tmpComponents;

			std::vector<char> output(size);
			memcpy(&output[0], imgData, size);

			stbi_image_free(imgData);
			return output;


		}

		std::array<float, 16> GetPerspectiveProjectionMatrix(const float aspectRatio, const float fieldOfView, const float nearClip, const float farClip)
		{
			float f = 1.0f / std::tan(fieldOfView * 0.5f * 0.01745329251994329576923690768489f);

			return
			{
				f / aspectRatio,
				0.0f,
				0.0f,
				0.0f,

				0.0f,
				-f,
				0.0f,
				0.0f,

				0.0f,
				0.0f,
				farClip / (nearClip - farClip),
				-1.0f,

				0.0f,
				0.0f,
				(nearClip * farClip) / (nearClip - farClip),
				0.0f,
			};
		}

		std::array<float, 16> GetOrthographicProjectionMatrix(const float leftPlane, const float rightPlane, const float topPlane, const float bottomPlane, const float nearPlane, const float farPlane)
		{
			return
			{
				2.0f / (rightPlane - leftPlane),
				0.0f,
				0.0f,
				0.0f,

				0.0f,
				2.0f / (bottomPlane - topPlane),
				0.0f,
				0.0f,

				0.0f,
				0.0f,
				1.0f / (nearPlane - farPlane),
				0.0f,

				-(rightPlane + leftPlane) / (rightPlane - leftPlane),
				-(bottomPlane + topPlane) / (bottomPlane - topPlane),
				nearPlane / (nearPlane - farPlane),
				1.0f
			};
		}



	}
}

