// glpp.cpp : 定义静态库的函数。
//
#include <glad/glad.h>
#include "glpp.hpp"
#include <stdexcept>
#include <thread>
#include <cassert>
#include "stb_image.h"

namespace glpp {
    GlobalContext::~GlobalContext() {
        glfwTerminate();
    }
    GlobalContext::GlobalContext() {
        glfwInit();
    }
    static bool initialized = false;
    static std::thread::id main_thread_id = std::this_thread::get_id();
#define ENSURE_MAIN_THREAD()\
    assert(main_thread_id == std::this_thread::get_id())

    GlobalContext GlobalContext::init() {
        ENSURE_MAIN_THREAD();
        
        assert(!initialized);

        if (!glfwInit())
            throw std::runtime_error("Failed to call glfwInit()");
        initialized = true;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        return {};
    }

    static void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }
    void Window::setShouldClose(int v) {
        glfwSetWindowShouldClose(m_win, v);
    }
    int Window::getShouldClose() const {
        return glfwWindowShouldClose(m_win);
    }
    void Window::swapBuffer() {
        glfwSwapBuffers(m_win);
    }
    int Window::getKey(int key) {
        return glfwGetKey(m_win, GLFW_KEY_ESCAPE);
    }

    Window Window::make(rect_scale_t rect, ntstr_view title, std::optional<std::reference_wrapper<Monitor>> monitor, std::optional<std::reference_wrapper<Window>> share) {
        auto pmonitor = monitor.has_value() ? monitor.value().get().m_monitor : nullptr;
        auto pshare = share.has_value() ? share.value().get().m_win : nullptr;
        auto pwin = glfwCreateWindow(rect.width, rect.height, title.c_str(), pmonitor, pshare);
        if (!pwin) throw std::runtime_error("Failed to create GLFW window");

        glfwMakeContextCurrent(pwin);
        glfwSetFramebufferSizeCallback(pwin, framebuffer_size_callback);
        return Window(pwin);
     }

    Shader::~Shader() {
        if (m_id != 0)
            glDeleteShader(m_id);
    }

    Shader Shader::make(GLuint  type, GLsizei  n, const char* const* sources, const GLint* lengths) {
        GLuint id = glCreateShader(type);

        glShaderSource(id, n, sources, lengths);
        glCompileShader(id);
        int  success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLint size = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &size);

            std::string infoLog;
            infoLog.resize(size);
            glGetShaderInfoLog(id, size, &size, infoLog.data());
            infoLog.resize(size);
            glDeleteShader(id);
            throw std::runtime_error(infoLog);
        }

        return Shader{ id };
    }
    Program::~Program() {
        if (m_id != 0)
            glDeleteProgram(m_id);
    }
    void Program::use() {
        assert(valid());
        glUseProgram(m_id);
    }
    int Program::index_of_uniform(ntstr_view param) {
        return glGetUniformLocation(m_id, param.c_str());
    }

    ProgramBuilder::ProgramBuilder() : m_id(glCreateProgram()){
    }
    ProgramBuilder& ProgramBuilder::renew() {
        if (m_id) glDeleteProgram(m_id);
        m_id = glCreateProgram();
        return *this;
    }
    ProgramBuilder& ProgramBuilder::addShader(const Shader& rhs) {
        assert(valid());
        glAttachShader(m_id, rhs.m_id);
        return *this;
    }

    Program ProgramBuilder::build() {
        assert(valid());
        auto id = m_id;
        m_id = 0;

        glLinkProgram(id);
        int  success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            GLint size = 0;
            glGetProgramiv(id, GL_INFO_LOG_LENGTH, &size);

            std::string infoLog;
            infoLog.resize(size);
            glGetProgramInfoLog(id, size, &size, infoLog.data());
            infoLog.resize(size);
            glDeleteProgram(id);
            throw std::runtime_error(infoLog);
        }
        
        return Program(id);
    }

    Image loadImage(ntstr_view path) {
        //stbi_set_flip_vertically_on_load(true);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data)
            throw std::runtime_error(std::string("Failed to load image file: ") + path.c_str() );
        data_ptr pdata(reinterpret_cast<std::byte*>(data), stbi_image_free);
        return { width, height, nrChannels , std::move(pdata)};
    }
}