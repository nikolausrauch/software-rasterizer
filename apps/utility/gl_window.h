#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <framebuffer.h>

#include <string>
#include <functional>

struct Window
{
    typedef std::function<void(Window &window, int key, int mod, bool press)> KeyboardCallback;
    typedef std::function<void(Window &window, const Vec2i& position, int button, int mod, bool press)> MouseCallback;
    typedef std::function<void(Window &window, double dt)> DrawCallback;
    typedef std::function<void(Window &window, double dt)> UpdateCallback;
    typedef std::function<void(Window &window, unsigned int width, unsigned int height)> ResizeCallback;


    Window(const std::string &title, unsigned int width, unsigned int height);
    Window(const std::string &title, unsigned int width, unsigned int height, unsigned int fb_width, unsigned int fb_height);
    ~Window();

    void onKeyboard(const KeyboardCallback& cb);
    void onMouse(const MouseCallback& cb);
    void onDraw(const DrawCallback& cb);
    void onUpdate(const UpdateCallback& cb);
    void onResize(const ResizeCallback& cb);

    void swap(DefaultFramebuffer& framebuffer);

    void run();

    Vec2 mousePosition() const;

private:
    GLFWwindow* m_window;

    GLuint m_program;
    GLuint m_quad_vao;
    GLuint m_quad_vbo;
    GLuint m_texture;

    KeyboardCallback m_keyboard_cb;
    MouseCallback m_mouse_cb;
    DrawCallback m_draw_cb;
    UpdateCallback m_update_cb;
    ResizeCallback m_resize_cb;

    struct
    {
        float accumTime;
        unsigned int frames;
        float fps;
    } m_fps_counter;

    struct
    {
        Vec2 mousePosition;
    } m_input;

    friend void key_callback(GLFWwindow* handle, int key, int scancode, int action, int mods);
    friend void window_resize_callback(GLFWwindow* handle, int width, int height);
    friend void window_mouse_position(GLFWwindow* handle, double x, double y);
    friend void window_mouse_button(GLFWwindow* handle, int button, int action, int mods);
};
