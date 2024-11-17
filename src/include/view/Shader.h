#pragma once

#include <string>
#include <vulkan/vulkan.h>

// Класс Shader отвечает за загрузку, хранение и управление шейдерами в Vulkan
class Shader {
public:
    // Конструктор принимает путь к файлу шейдера и устройство Vulkan,
    // чтобы инициализировать объект Shader и загрузить модуль шейдера
    Shader(const std::string& shaderPath, VkDevice device);

    // Метод для получения Vulkan модуля шейдера, который может быть
    // использован при настройке Vulkan пайплайнов и других операций.
    // Возвращает объект VkShaderModule, представляющий загруженный шейдер.
    [[nodiscard]] VkShaderModule getShaderModule() const;

    // Деструктор для очистки ресурсов
    ~Shader();

private:
    // Приватный метод, который выполняет основную работу по загрузке и
    // созданию модуля шейдера. Принимает путь к .spv файлу и
    // указатель на VkShaderModule, в который сохраняется результат.
    // Возвращает true при успешной загрузке, иначе false.
    bool loadShaderModule(const std::string& filePath,
                          VkShaderModule* outShaderModule);

    VkDevice _device;  // Устройство Vulkan, необходимое для создания шейдеров
    VkShaderModule _shaderModule;  // VkShaderModule, содержащий данные
                                   // загруженного шейдера
};
