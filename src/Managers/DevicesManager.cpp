#include "DevicesManager.hpp"

#include <vector>
#include <set>

void DevicesManager::initWindow()
{
  m_window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGTH, "Vulkan Hello Triangle",
                              nullptr, nullptr);

  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
}

void DevicesManager::createSurface()
{
  if (glfwCreateWindowSurface(m_vkInstance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create the surface window.");
}

void DevicesManager::createPhysicalDevice()
{
  m_physicalDevice = VK_NULL_HANDLE;

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

  if (!deviceCount) throw std::runtime_error("ERROR: No compatible GPUs found!");

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (auto device : devices)
  {
    if (isDeviceSuitable(device))
    { // For now, just take the 1st suitable device we find.
      // TODO: Rate all available and select accordingly
      m_physicalDevice = device;
      break;
    }
  }

  if (m_physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("ERROR: No suitable GPUs found!");
}

void DevicesManager::createLogicalDevice(const std::vector<const char*>& _validationLayers)
{
  float queuePriority = 1.0; // TODO: Avoid magic numbers
  const QueueFamilyIndices_t queueFamilyIndices = findQueueFamilies();

  // How many queues we want for a single family (for now just one with graphics capabilities)
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {queueFamilyIndices.graphicsFamily.value(),
                                            queueFamilyIndices.presentFamily.value()};

  for(uint32_t queueFamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount       = 1; // TODO: Avoid magic numbers
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.emplace_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures = {}; // TODO: Add fancy stuff

  VkDeviceCreateInfo createInfo      = {};
  createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos       = queueCreateInfos.data();
  createInfo.queueCreateInfoCount    = queueCreateInfos.size();
  createInfo.pEnabledFeatures        = &deviceFeatures;
  createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();
  // NOTE: enabledExtensionCount and ppEnabledLayerNames are ignored in modern Vulkan implementations
  createInfo.enabledExtensionCount   = DEVICE_EXTENSIONS.size();
  if (!_validationLayers.empty())
  {
    createInfo.enabledLayerCount   = static_cast<uint32_t>(_validationLayers.size());
    createInfo.ppEnabledLayerNames = _validationLayers.data();
  }
  else createInfo.enabledLayerCount = 0;

  if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("ERROR: Failed to create logical device!");

  // DOUBT: Should I move this to another place?
  vkGetDeviceQueue(m_logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &m_presentQueue);
}

bool DevicesManager::isDeviceSuitable(const VkPhysicalDevice& _device)
{
  const QueueFamilyIndices_t queueFamilyIndices = findQueueFamiliesForPhysicalDevice(_device);

  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures   features;

  vkGetPhysicalDeviceProperties(_device, &properties);
  vkGetPhysicalDeviceFeatures(_device, &features);

  return features.geometryShader &&
         queueFamilyIndices.isComplete();
  // For some reason, my Nvidia GTX960m is not recognized as a discrete GPU :/
  //  primusrun might be masking the GPU as an integrated
  //     && properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

QueueFamilyIndices_t DevicesManager::findQueueFamiliesForPhysicalDevice(const VkPhysicalDevice& _device)
{
  QueueFamilyIndices_t result;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

  VkBool32 presentSupport = false;
  int i=0;
  for (const auto& family : queueFamilies)
  {
    if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) result.graphicsFamily = i;

    vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, getSurface(), &presentSupport);
    if (presentSupport) result.presentFamily = i;

    if (result.isComplete()) break;

    ++i;
  }

  m_graphicsQueueFamilyIdx = result.graphicsFamily.value();
  m_presentQueueFamilyIdx  = result.presentFamily.value();

  return result;
}

bool DevicesManager::checkExtensionSupportOfPhysicalDevice(const VkPhysicalDevice& _device)
{
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());

  extensionCount = 0;
  for (const char* extensionName : DEVICE_EXTENSIONS)
  {
    for (const VkExtensionProperties extension : availableExtensions)
      if (!strcmp(extensionName, extension.extensionName)) ++extensionCount;
  }

  return extensionCount == DEVICE_EXTENSIONS.size();
}

void DevicesManager::cleanUp()
{
  vkDestroyDevice(m_logicalDevice, nullptr);
  vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
  glfwDestroyWindow(m_window);
}