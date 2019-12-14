#ifndef HELLO_TRIANGLE_HPP
#define HELLO_TRIANGLE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// Error management
#include <stdexcept>
#include <iostream>
// Lambdas
#include <functional>
// EXIT_SUCCESS and EXIT_FAILURES
#include <cstdlib>
// Loading files
#include <fstream>
// Max int values
#include <cstdint>
// General use
#include <string.h>
#include <cstring>
#include <set>
#include <vector>
#include <optional>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#include "Managers/DevicesManager.hpp"

static const int WIDTH  = 800;
static const int HEIGTH = 600;

const std::vector<const char*> VALIDATION_LAYERS = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> DEVICE_EXTENSIONS = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NDEBUG
  const bool ENABLE_VALIDATION_LAYERS = false;
#else
  const bool ENABLE_VALIDATION_LAYERS = true;
#endif

typedef struct
{
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool IsComplete() const
  {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }

} QueueFamilyIndices_t;

typedef struct
{
  VkSurfaceCapabilitiesKHR        capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR>   presentModes;

} SwapChainDetails_t;

class HelloTriangle
{
public:
  HelloTriangle();
  ~HelloTriangle();
  void Run();

private:
  GLFWwindow*  m_window;
  VkSurfaceKHR m_surface;

  VkInstance       m_vkInstance;
  VkPhysicalDevice m_physicalDevice; // Implicitly destroyed alongside m_vkInstance
  VkDevice         m_logicalDevice;
  VkQueue          m_graphicsQueue; // Implicitly destroyed alongside m_logicalDevice
  VkQueue          m_presentQueue; // Implicitly destroyed alongside m_logicalDevice

  VkSwapchainKHR           m_swapChain;
  VkFormat                 m_swapChainImageFormat;
  VkExtent2D               m_swapChainExtent;
  std::vector<VkImage>     m_swapChainImages; // Implicitly destroyed alongside m_swapChain
  std::vector<VkImageView> m_swapChainImageViews;

  VkRenderPass     m_renderPass;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline       m_graphicsPipeline;

  VkDebugUtilsMessengerEXT m_debugMessenger;

  void InitWindow();
  void CreateVkInstance();
  void InitVulkan();
  void MainLoop();
  void Cleanup();

  void CreateSurface();

  // Device management TODO: Move to an independent manager
  void GetPhysicalDevice();
  bool IsDeviceSuitable(VkPhysicalDevice device);
  void CreateLogicalDevice();

  QueueFamilyIndices_t FindQueueFamilies(VkPhysicalDevice device);

  // Validation layers and extensions
  bool CheckValidationSupport();
  bool CheckExtensionSupport(VkPhysicalDevice device);
  std::vector<const char*> GetRequiredExtensions();
  void PopulateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
  void InitDebugMessenger();

  // Swapchain
  SwapChainDetails_t QuerySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
  VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
  VkExtent2D         ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
  void               CreateSwapChain();
  void               CreateImageViews();

  // Pipeline
  void CreateRenderPass();
  void CreateGraphicsPipeline();

  // Shaders
  VkShaderModule CreateShaderModule(const std::vector<char>& _code);

  static std::vector<char> ReadShaderFile(const char* _fileName)
  {
    // Read the file from the end and as a binary file
    std::ifstream file(_fileName, std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("ERROR: Couldn't open file"); //%s", _fileName);

    size_t fileSize = static_cast<size_t>(file.tellg());
    // We use a vector of chars instead of a char* or a string for more simplicity during the shader module creation
    std::vector<char> buffer(fileSize);

    // Go back to the beginning of the gile and read all the bytes at once
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
  };

  static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT _messageType,
      const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
      void* _pUserData)
  {
    if (_messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
      std::cerr << "Validation layer: " << _pCallbackData->pMessage << std::endl;
      // Just to avoid compilation errors, not real functionality yet.
      std::cerr << "Message Type: " << _messageType << std::endl;
      if (_pUserData) std::cerr << "User Data exists!" << std::endl;
    }

    return VK_FALSE;
  };
};
#endif