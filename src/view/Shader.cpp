#include "Shader.h"

#include <fmt/core.h>
#include <fstream>
#include <vector>

// Конструктор, принимающий путь к шейдеру и устройство
Shader::Shader(const std::string& shaderPath, VkDevice device)
    : _device(device), _shaderModule(VK_NULL_HANDLE) {
    // Попытка загрузить шейдер в модуль. Если неудачно, выводим ошибку.
    if (!loadShaderModule(shaderPath, &_shaderModule)) {
        fmt::print("Error when building the shader from {}\n", shaderPath);
    }
}

// Метод, загружающий бинарные данные шейдера и создающий VkShaderModule
bool Shader::loadShaderModule(const std::string& filePath,
                              VkShaderModule* outShaderModule) {
    // Открываем файл в бинарном режиме и позиционируем курсор в конце
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        fmt::print("Failed to open shader file {}\n", filePath);
        return false;
    }

    // Определяем размер файла и читаем его содержимое в буфер
    size_t fileSize = (size_t)file.tellg();  // Размер файла
    std::vector<char> buffer(fileSize);      // Буфер для данных

    // Считываем данные файла в буфер
    file.seekg(0);  // Возвращаем курсор в начало файла
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));  // Чтение данных в буфер
    file.close();  // Закрытие файла после чтения

    // Настройка структуры VkShaderModuleCreateInfo для создания модуля
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    // Создаем модуль шейдера. Если не удается, возвращаем ошибку
    if (vkCreateShaderModule(_device, &createInfo, nullptr, outShaderModule) !=
        VK_SUCCESS) {
        fmt::print("Failed to create shader module\n");
        return false;
    }

    return true;  // Успешная загрузка и создание модуля
}

// Метод для доступа к созданному модулю шейдера
VkShaderModule Shader::getShaderModule() const {
    return _shaderModule;
}

// Деструктор для очистки ресурсов
Shader::~Shader() {
    if (_shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_device, _shaderModule, nullptr);
    }
}
