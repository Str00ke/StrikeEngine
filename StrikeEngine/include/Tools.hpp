#ifndef TOOLS_HEADER
#define TOOLS_HEADER

#include <string>
#include <vector>
#include <array>
#include "../Externals/vulkan.h"

namespace StrikeEngine 
{
	namespace Tools 
	{
		template<class T, class F>
		class AutoDeleter
		{
		public:
			AutoDeleter() :
				Object(VK_NULL_HANDLE),
				Deleter(nullptr),
				Device(VK_NULL_HANDLE)
			{}

			AutoDeleter(T object, F deleter, VkDevice device) :
				Object(object),
				Deleter(deleter),
				Device(device)
			{}

			AutoDeleter(AutoDeleter&& other)
			{
				*this = std::move(other);
			}

			~AutoDeleter()
			{
				if ((Object != VK_NULL_HANDLE) && (Deleter != nullptr) && (Device != VK_NULL_HANDLE))
				{
					Deleter(Device, Object, nullptr);
				}
			}

			AutoDeleter& operator=(AutoDeleter&& other)
			{
				if (this != other)
				{
					Object = other.Object;
					Deleter = other.Deleter;
					Device = other.Device;
					other.Object = VK_NULL_HANDLE;
				}
				
				return *this;
			}

			T Get()
			{
				return Object;
			}

			bool operator !() const
			{
				return Object == VK_NULL_HANDLE;
			}

		private:
			AutoDeleter(const AutoDeleter&);
			AutoDeleter& operator=(const AutoDeleter&);
			T Object;
			F Deleter;
			VkDevice Device;
		};

		std::vector<char> GetBinaryFileContents(const std::string& filename);
	}

}


#endif //TOOLS_HEADER


