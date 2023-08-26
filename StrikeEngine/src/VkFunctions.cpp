//#include "vulkan/vulkan.h"
//
//
//#define VK_EXPORTED_FUNCTION(func)			PFN_##func func;
//#define VK_GLOBAL_LEVEL_FUNCTION(func)		PFN_##func func;
//#define VK_INSTANCE_LEVEL_FUNCTION(func)	PFN_##func func;
//#define VK_DEVICE_LEVEL_FUNCTION(func)		PFN_##func func;
//
//#include "VkFuncList.inl"

//#ifndef VK_NO_PROTOTYPES
//#define VK_NO_PROTOTYPES
//#endif

//#include "../Externals/vulkan.h"
#include "vulkan/vulkan.h"


namespace StrikeEngine
{
#define VK_EXPORTED_FUNCTION( fun ) PFN_##fun fun;
#define VK_GLOBAL_LEVEL_FUNCTION( fun ) PFN_##fun fun;
#define VK_INSTANCE_LEVEL_FUNCTION( fun ) PFN_##fun fun;
#define VK_DEVICE_LEVEL_FUNCTION( fun ) PFN_##fun fun;

#include "VkFuncList.inl"

}

