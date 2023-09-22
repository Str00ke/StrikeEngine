//#ifndef VK_FUNCTIONS_HEADER
//#define VK_FUNCTIONS_HEADER
//
//#include "vulkan/vulkan.h"
//
//#define VK_EXPORTED_FUNCTION(func)			extern PFN_##func func;
//#define VK_GLOBAL_LEVEL_FUNCTION(func)		extern PFN_##func func;
//#define VK_INSTANCE_LEVEL_FUNCTION(func)	extern PFN_##func func;
//#define VK_DEVICE_LEVEL_FUNCTION(func)		extern PFN_##func func;
//
//#include "VkFuncList.inl"
//
//#endif
//

//#ifndef VK_NO_PROTOTYPES
//#define VK_NO_PROTOTYPES
//#endif

#if !defined(VULKAN_FUNCTIONS_HEADER)
#define VULKAN_FUNCTIONS_HEADER

#include "vulkan.h"

//#include "../Externals/vulkan.h"

namespace StrikeEngine 
{
#define VK_EXPORTED_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION( fun) extern PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION( fun ) extern PFN_##fun fun;

#include "Vulkan/VkFuncList.inl"
}

#endif