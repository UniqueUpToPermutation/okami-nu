#pragma once

#include <okami/input.hpp>
#include <okami/error.hpp>

struct GLFWwindow;

namespace okami {
    Key ConvertKeyFromGLFW(int key);
    int ConvertGLFWFromKey(Key key);

    MouseButton ConvertMouseButtonFromGLFW(int button);
    int ConvertGLFWFromMouseButton(MouseButton button);

    KeyAction ConvertKeyActionFromGLFW(int action);
    KeyModifiers ConvertKeyModifiersFromGLFW(int mods);

    std::array<bool, kKeyCount> KeyboardStateFromGLFW(GLFWwindow* window);
    std::array<bool, kMouseButtonCount> MouseStateFromGLFW(GLFWwindow* window);

    Error GLFWKeyHandler(StaticVector<KeyEvent, kKeyboardMaxEvents>& buffer,
        int key, int scancode, int action, int mods);
    Error GLFWMouseHandler(StaticVector<MouseEvent, kMouseMaxEvents>& buffer,
        int button, int action, int mods);
}