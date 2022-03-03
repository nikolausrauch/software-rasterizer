#include "gl_window.h"

#include "detail/shader.h"
#include "detail/mesh.h"
#include "detail/texture.h"

#include <iostream>
#include <exception>

const std::string vertex_shader_code = R"END(
    #version 330 core

    /* attributes of vertex */
    layout(location = 0) in vec3 aPosition;
    layout(location = 1) in vec2 aUV;

    /* color and UV are additional outputs of this shader (can be used in the fragment shader) */
    out vec2 tUV;

    void main()
    {
        gl_Position = vec4(aPosition, 1.0);
        tUV = aUV;
    }
)END";


const std::string fragment_shader_code = R"END(
    #version 330 core

    /* interpolated UV coordinates */
    in vec2 tUV;

    uniform sampler2D uTexture;

    void main()
    {
        gl_FragColor = texture(uTexture, tUV);
    }
)END";

void glfw_error_callback(int error, const char* description)
{
    std::cerr << "GLFW Error: " <<  description << std::endl;
}

void key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if(window->m_keyboard_cb) window->m_keyboard_cb(*window, key, mods, action == GLFW_PRESS);

    /* close window on escape */
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(handle, true);
    }
}

void window_resize_callback(GLFWwindow* handle, int width, int height)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if(window->m_resize_cb) window->m_resize_cb(*window, width, height);

    /* on window size changes - re-assign viewport mapping */
    glViewport(0, 0, width, height);
}

void window_mouse_position(GLFWwindow* handle, double x, double y)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    window->m_input.mousePosition = Vec2(x, y);
}

void window_mouse_button(GLFWwindow* handle, int button, int action, int mods)
{
    Window* window = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if(window->m_mouse_cb) window->m_mouse_cb(*window, window->m_input.mousePosition, button, mods, action == GLFW_PRESS);
}

Window::Window(const std::string& title, unsigned int width, unsigned int height)
    : m_program(0), m_quad_vao(0), m_quad_vbo(0), m_texture(0), m_fps_counter({ 0, 0, 0 })
{
    /*-------------- init glfw ----------------*/
    if(!glfwInit())
    {
        throw std::runtime_error("[Window] Coudn't initialize GLEW");
    }
    glfwSetErrorCallback(glfw_error_callback);


    /*-------------- create window ----------------*/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* create window and its opengl context */
    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(m_window == nullptr)
    {
        std::cerr << "Couldn't create Window (GL context)" << std::endl;
        glfwTerminate();

        throw std::runtime_error("[Window] Coudn't create Window");
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, key_callback);
    glfwSetWindowSizeCallback(m_window, window_resize_callback);
    glfwSetMouseButtonCallback(m_window, window_mouse_button);
    glfwSetCursorPosCallback(m_window, window_mouse_position);

    /*-------------- init glad ----------------*/
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cerr << "Couldn't initialize GLAD" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();

        throw std::runtime_error("[Window] Coudn't initialize GLAD");
    }

    /*-------------- init opengl stuff ----------------*/
    m_program = shaderCreate(vertex_shader_code, fragment_shader_code);
    m_texture = textureCreate(width, height);
    std::tie(m_quad_vao, m_quad_vbo) = meshCreate(
                {
                    {{-1.0, -1.0, 0.0}, {0.0, 0.0}},
                    {{-1.0,  1.0, 0.0}, {0.0, 1.0}},
                    {{ 1.0,  1.0, 0.0}, {1.0, 1.0}},
                    {{ 1.0,  1.0, 0.0}, {1.0, 1.0}},
                    {{ 1.0, -1.0, 0.0}, {1.0, 0.0}},
                    {{-1.0, -1.0, 0.0}, {0.0, 0.0}}
                });
}

Window::~Window()
{
    glDeleteProgram(m_program);

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::onKeyboard(const KeyboardCallback &cb)
{
    m_keyboard_cb = cb;
}

void Window::onMouse(const MouseCallback &cb)
{
    m_mouse_cb = cb;
}

void Window::onDraw(const DrawCallback &cb)
{
    m_draw_cb = cb;
}

void Window::onUpdate(const UpdateCallback &cb)
{
    m_update_cb = cb;
}

void Window::onResize(const ResizeCallback &cb)
{
    m_resize_cb = cb;
}

void Window::swap(Framebuffer &framebuffer)
{
    auto& buffer = framebuffer.color();

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer.width(), buffer.height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer.ptr());
}

void Window::run()
{
    /* Timer init */
    double previousTime = 0.0;
    glfwSetTime(previousTime);
    m_fps_counter.fps = 60;
    m_fps_counter.frames = 0;

    /*=============== main loop ===============*/
    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        /* Timer and simple fps counter*/
        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;

        m_fps_counter.accumTime += elapsed;
        m_fps_counter.frames++;
        if (m_fps_counter.accumTime >= 1.0)
        {
            m_fps_counter.fps = static_cast<double>(m_fps_counter.frames) * 0.5 + m_fps_counter.fps * 0.5;
            m_fps_counter.accumTime -= 1.0f;
            m_fps_counter.frames = 0;
        }

        if(m_update_cb) m_update_cb(*this, elapsed);
        if(m_draw_cb) m_draw_cb(*this, elapsed);

        /* draw screen quad with framebuffer of software rasterizer set as texture */
        glUseProgram(m_program);
        glUniform1i(glGetUniformLocation(m_program, "uTexture"), 0);
        glBindVertexArray(m_quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(m_window);
    }
}

Vec2 Window::mousePosition() const
{
    return m_input.mousePosition;
}
