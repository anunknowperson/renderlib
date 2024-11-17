#include "view/Shader.h"

#include <fmt/core.h>
#include <fstream>
#include <vector>

// Конструктор, принимает путь к файлу шейдера и устройство Vulkan
Shader::Shader(const std::string& shaderPath, VkDevice device)
    : _device(device), _shaderModule(VK_NULL_HANDLE) {
    if (!loadShaderModule(shaderPath, &_shaderModule)) {
        fmt::print("Ошибка при создании шейдера из {}\n", shaderPath);
    }
}

// Загрузка данных шейдера и создание VkShaderModule
bool Shader::loadShaderModule(const std::string& filePath, VkShaderModule* outShaderModule) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        fmt::print("Не удалось открыть файл шейдера {}\n", filePath);
        return false;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    if (vkCreateShaderModule(_device, &createInfo, nullptr, outShaderModule) != VK_SUCCESS) {
        fmt::print("Не удалось создать модуль шейдера\n");
        return false;
    }

    return true;
}

// Получение созданного модуля шейдера
VkShaderModule Shader::getShaderModule() const {
    return _shaderModule;
}

// Деструктор для освобождения ресурсов
Shader::~Shader() {
    if (_shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_device, _shaderModule, nullptr);
    }
}