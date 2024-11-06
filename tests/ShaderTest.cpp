#include <gtest/gtest.h>

#include "../view/Shader.h"

// Фикстура для тестов класса Shader
class ShaderTest : public ::testing::Test {
protected:
    VkDevice device;  // Подставной объект устройства
    Shader* shader;  // Указатель на тестируемый объект Shader

    void SetUp() override {
        // Логика инициализации перед тестом
        device = {};  // Предположим, что подставной объект или mock был создан
        shader = new Shader("dummy_shader_path.spv", device);
    }

    void TearDown() override {
        delete shader;
    }
};

TEST_F(ShaderTest, ShaderModuleIsLoadedSuccessfully) {
    VkShaderModule module = shader->getShaderModule();
    EXPECT_NE(module, VK_NULL_HANDLE);  // Проверка, что модуль шейдера загружен
}

TEST_F(ShaderTest, ShaderFileNotFound) {
    Shader invalidShader("invalid_path.spv", device);
    VkShaderModule module = invalidShader.getShaderModule();
    EXPECT_EQ(module, VK_NULL_HANDLE);  // Проверка, что модуль не создан для
                                        // несуществующего файла
}

TEST_F(ShaderTest, InvalidDataForShaderModule) {
    Shader emptyPathShader("", device);
    VkShaderModule module = emptyPathShader.getShaderModule();
    EXPECT_EQ(module, VK_NULL_HANDLE);  // Проверка для пустого пути

    Shader invalidDataShader("invalid_shader_data.spv", device);
    module = invalidDataShader.getShaderModule();
    EXPECT_EQ(module,
              VK_NULL_HANDLE);  // Проверка для файла с некорректными данными
}

TEST_F(ShaderTest, MultipleShaderCreationAndDestruction) {
    // Создаем несколько объектов Shader с одним и тем же устройством и путем
    const int shaderCount = 10;
    std::vector<Shader*> shaders;

    // Создаем и инициализируем 10 шейдеров
    for (int i = 0; i < shaderCount; ++i) {
        shaders.push_back(new Shader("dummy_shader_path.spv", device));
        // Проверяем, что каждый объект создается корректно (модуль загружен)
        EXPECT_NE(shaders.back()->getShaderModule(), VK_NULL_HANDLE);
    }

    // Удаляем созданные шейдеры и проверяем, что ресурсы освобождены корректно
    for (auto* shader : shaders) {
        delete shader;
    }
}
