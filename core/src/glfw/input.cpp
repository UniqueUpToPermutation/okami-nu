#include <okami/glfw/input.hpp>

#include <GLFW/glfw3.h>

using namespace okami;

#define CASE_CONV_F(x, y) case x: return y
#define CASE_CONV_R(x, y) case y: return x

MouseButton okami::ConvertMouseButtonFromGLFW(int button) {
    switch (button) {
        CASE_CONV_F(GLFW_MOUSE_BUTTON_1, MouseButton::LEFT);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_2, MouseButton::RIGHT);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_3, MouseButton::MIDDLE);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_4, MouseButton::BUTTON4);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_5, MouseButton::BUTTON5);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_6, MouseButton::BUTTON6);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_7, MouseButton::BUTTON7);
        CASE_CONV_F(GLFW_MOUSE_BUTTON_8, MouseButton::BUTTON8);
        default:
            return MouseButton::BUTTON8;
    }
}

int okami::ConvertGLFWFromMouseButton(MouseButton button) {
    switch (button) {
        CASE_CONV_R(GLFW_MOUSE_BUTTON_1, MouseButton::LEFT);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_2, MouseButton::RIGHT);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_3, MouseButton::MIDDLE);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_4, MouseButton::BUTTON4);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_5, MouseButton::BUTTON5);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_6, MouseButton::BUTTON6);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_7, MouseButton::BUTTON7);
        CASE_CONV_R(GLFW_MOUSE_BUTTON_8, MouseButton::BUTTON8);
        default:
            return GLFW_MOUSE_BUTTON_8;
    }
}

Key okami::ConvertKeyFromGLFW(int key) {
    switch (key) {
        CASE_CONV_F(GLFW_KEY_UNKNOWN,Key::UNKNOWN);
        CASE_CONV_F(GLFW_KEY_SPACE,Key::SPACE);
        CASE_CONV_F(GLFW_KEY_APOSTROPHE,Key::APOSTROPHE);
        CASE_CONV_F(GLFW_KEY_COMMA,Key::COMMA);
        CASE_CONV_F(GLFW_KEY_MINUS,Key::MINUS);
        CASE_CONV_F(GLFW_KEY_PERIOD,Key::PERIOD);
        CASE_CONV_F(GLFW_KEY_SLASH,Key::SLASH);
        CASE_CONV_F(GLFW_KEY_0,Key::_0);
        CASE_CONV_F(GLFW_KEY_1,Key::_1);
        CASE_CONV_F(GLFW_KEY_2,Key::_2);
        CASE_CONV_F(GLFW_KEY_3,Key::_3);
        CASE_CONV_F(GLFW_KEY_4,Key::_4);
        CASE_CONV_F(GLFW_KEY_5,Key::_5);
        CASE_CONV_F(GLFW_KEY_6,Key::_6);
        CASE_CONV_F(GLFW_KEY_7, Key::_7);
        CASE_CONV_F(GLFW_KEY_8, Key::_8);
        CASE_CONV_F(GLFW_KEY_9, Key::_9);
        CASE_CONV_F(GLFW_KEY_SEMICOLON, Key::SEMICOLON);
        CASE_CONV_F(GLFW_KEY_EQUAL, Key::EQUAL);
        CASE_CONV_F(GLFW_KEY_A, Key::A);
        CASE_CONV_F(GLFW_KEY_B, Key::B);
        CASE_CONV_F(GLFW_KEY_C, Key::C);
        CASE_CONV_F(GLFW_KEY_D, Key::D);
        CASE_CONV_F(GLFW_KEY_E, Key::E);
        CASE_CONV_F(GLFW_KEY_F, Key::F);
        CASE_CONV_F(GLFW_KEY_G, Key::G);
        CASE_CONV_F(GLFW_KEY_H, Key::H);
        CASE_CONV_F(GLFW_KEY_I, Key::I);
        CASE_CONV_F(GLFW_KEY_J, Key::J);
        CASE_CONV_F(GLFW_KEY_K, Key::K);
        CASE_CONV_F(GLFW_KEY_L, Key::L);
        CASE_CONV_F(GLFW_KEY_M, Key::M);
        CASE_CONV_F(GLFW_KEY_N, Key::N);
        CASE_CONV_F(GLFW_KEY_O, Key::O);
        CASE_CONV_F(GLFW_KEY_P, Key::P);
        CASE_CONV_F(GLFW_KEY_Q, Key::Q);
        CASE_CONV_F(GLFW_KEY_R, Key::R);
        CASE_CONV_F(GLFW_KEY_S, Key::S);
        CASE_CONV_F(GLFW_KEY_T, Key::T);
        CASE_CONV_F(GLFW_KEY_U, Key::U);
        CASE_CONV_F(GLFW_KEY_V, Key::V);
        CASE_CONV_F(GLFW_KEY_W, Key::W);
        CASE_CONV_F(GLFW_KEY_X, Key::X);
        CASE_CONV_F(GLFW_KEY_Y, Key::Y);
        CASE_CONV_F(GLFW_KEY_Z, Key::Z);
        CASE_CONV_F(GLFW_KEY_LEFT_BRACKET, Key::LEFT_BRACKET);
        CASE_CONV_F(GLFW_KEY_BACKSLASH, Key::BACKSLASH);
        CASE_CONV_F(GLFW_KEY_RIGHT_BRACKET, Key::RIGHT_BRACKET);
        CASE_CONV_F(GLFW_KEY_GRAVE_ACCENT, Key::GRAVE_ACCENT);
        CASE_CONV_F(GLFW_KEY_WORLD_1, Key::WORLD1);
        CASE_CONV_F(GLFW_KEY_WORLD_2, Key::WORLD2);
        CASE_CONV_F(GLFW_KEY_ESCAPE, Key::ESCAPE);
        CASE_CONV_F(GLFW_KEY_ENTER, Key::ENTER);
        CASE_CONV_F(GLFW_KEY_TAB, Key::TAB);
        CASE_CONV_F(GLFW_KEY_BACKSPACE, Key::BACKSPACE);
        CASE_CONV_F(GLFW_KEY_INSERT, Key::INSERT);
        CASE_CONV_F(GLFW_KEY_DELETE, Key::DELETE);
        CASE_CONV_F(GLFW_KEY_RIGHT, Key::RIGHT);
        CASE_CONV_F(GLFW_KEY_LEFT, Key::LEFT);
        CASE_CONV_F(GLFW_KEY_DOWN, Key::DOWN);
        CASE_CONV_F(GLFW_KEY_UP, Key::UP);
        CASE_CONV_F(GLFW_KEY_PAGE_UP, Key::PAGE_UP);
        CASE_CONV_F(GLFW_KEY_PAGE_DOWN, Key::PAGE_DOWN);
        CASE_CONV_F(GLFW_KEY_HOME, Key::HOME);
        CASE_CONV_F(GLFW_KEY_END, Key::END);
        CASE_CONV_F(GLFW_KEY_CAPS_LOCK, Key::CAPS_LOCK);
        CASE_CONV_F(GLFW_KEY_SCROLL_LOCK, Key::SCROLL_LOCK);
        CASE_CONV_F(GLFW_KEY_NUM_LOCK, Key::NUM_LOCK);
        CASE_CONV_F(GLFW_KEY_PRINT_SCREEN, Key::PRINT_SCREEN);
        CASE_CONV_F(GLFW_KEY_PAUSE, Key::PAUSE);
        CASE_CONV_F(GLFW_KEY_F1, Key::F1);
        CASE_CONV_F(GLFW_KEY_F2, Key::F2);
        CASE_CONV_F(GLFW_KEY_F3, Key::F3);
        CASE_CONV_F(GLFW_KEY_F4, Key::F4);
        CASE_CONV_F(GLFW_KEY_F5, Key::F5);
        CASE_CONV_F(GLFW_KEY_F6, Key::F6);
        CASE_CONV_F(GLFW_KEY_F7, Key::F7);
        CASE_CONV_F(GLFW_KEY_F8, Key::F8);
        CASE_CONV_F(GLFW_KEY_F9, Key::F9);
        CASE_CONV_F(GLFW_KEY_F10, Key::F10);
        CASE_CONV_F(GLFW_KEY_F11, Key::F11);
        CASE_CONV_F(GLFW_KEY_F12, Key::F12);
        CASE_CONV_F(GLFW_KEY_F13, Key::F13);
        CASE_CONV_F(GLFW_KEY_F14, Key::F14);
        CASE_CONV_F(GLFW_KEY_F15, Key::F15);
        CASE_CONV_F(GLFW_KEY_F16, Key::F16);
        CASE_CONV_F(GLFW_KEY_F17, Key::F17);
        CASE_CONV_F(GLFW_KEY_F18, Key::F18);
        CASE_CONV_F(GLFW_KEY_F19, Key::F19);
        CASE_CONV_F(GLFW_KEY_F20, Key::F20);
        CASE_CONV_F(GLFW_KEY_F21, Key::F21);
        CASE_CONV_F(GLFW_KEY_F22, Key::F22);
        CASE_CONV_F(GLFW_KEY_F23, Key::F23);
        CASE_CONV_F(GLFW_KEY_F24, Key::F24);
        CASE_CONV_F(GLFW_KEY_F25, Key::F25);
        CASE_CONV_F(GLFW_KEY_KP_0, Key::NUMPAD0);
        CASE_CONV_F(GLFW_KEY_KP_1, Key::NUMPAD1);
        CASE_CONV_F(GLFW_KEY_KP_2, Key::NUMPAD2);
        CASE_CONV_F(GLFW_KEY_KP_3, Key::NUMPAD3);
        CASE_CONV_F(GLFW_KEY_KP_4, Key::NUMPAD4);
        CASE_CONV_F(GLFW_KEY_KP_5, Key::NUMPAD5);
        CASE_CONV_F(GLFW_KEY_KP_6, Key::NUMPAD6);
        CASE_CONV_F(GLFW_KEY_KP_7, Key::NUMPAD7);
        CASE_CONV_F(GLFW_KEY_KP_8, Key::NUMPAD8);
        CASE_CONV_F(GLFW_KEY_KP_9, Key::NUMPAD9);
        CASE_CONV_F(GLFW_KEY_KP_DECIMAL, Key::NUMPAD_DECIMAL);
        CASE_CONV_F(GLFW_KEY_KP_DIVIDE, Key::NUMPAD_DIVIDE);
        CASE_CONV_F(GLFW_KEY_KP_MULTIPLY, Key::NUMPAD_MULTIPLY);
        CASE_CONV_F(GLFW_KEY_KP_SUBTRACT, Key::NUMPAD_SUBSTRACT);
        CASE_CONV_F(GLFW_KEY_KP_ADD, Key::NUMPAD_ADD);
        CASE_CONV_F(GLFW_KEY_KP_ENTER, Key::NUMPAD_ENTER);
        CASE_CONV_F(GLFW_KEY_KP_EQUAL, Key::NUMPAD_EQUAL);
        CASE_CONV_F(GLFW_KEY_LEFT_SHIFT, Key::LEFT_SHIFT);
        CASE_CONV_F(GLFW_KEY_LEFT_CONTROL, Key::LEFT_CONTROL);
        CASE_CONV_F(GLFW_KEY_LEFT_ALT, Key::LEFT_ALT);
        CASE_CONV_F(GLFW_KEY_LEFT_SUPER, Key::LEFT_SUPER);
        CASE_CONV_F(GLFW_KEY_RIGHT_SHIFT, Key::RIGHT_SHIFT);
        CASE_CONV_F(GLFW_KEY_RIGHT_CONTROL, Key::RIGHT_CONTROL);
        CASE_CONV_F(GLFW_KEY_RIGHT_ALT, Key::RIGHT_ALT);
        CASE_CONV_F(GLFW_KEY_RIGHT_SUPER, Key::RIGHT_SUPER);
        CASE_CONV_F(GLFW_KEY_MENU, Key::MENU);
        default: 
            return Key::UNKNOWN;
    }
}

int okami::ConvertGLFWFromKey(Key key) {
    switch (key) {
        CASE_CONV_R(GLFW_KEY_UNKNOWN,Key::UNKNOWN);
        CASE_CONV_R(GLFW_KEY_SPACE,Key::SPACE);
        CASE_CONV_R(GLFW_KEY_APOSTROPHE,Key::APOSTROPHE);
        CASE_CONV_R(GLFW_KEY_COMMA,Key::COMMA);
        CASE_CONV_R(GLFW_KEY_MINUS,Key::MINUS);
        CASE_CONV_R(GLFW_KEY_PERIOD,Key::PERIOD);
        CASE_CONV_R(GLFW_KEY_SLASH,Key::SLASH);
        CASE_CONV_R(GLFW_KEY_0,Key::_0);
        CASE_CONV_R(GLFW_KEY_1,Key::_1);
        CASE_CONV_R(GLFW_KEY_2,Key::_2);
        CASE_CONV_R(GLFW_KEY_3,Key::_3);
        CASE_CONV_R(GLFW_KEY_4,Key::_4);
        CASE_CONV_R(GLFW_KEY_5,Key::_5);
        CASE_CONV_R(GLFW_KEY_6,Key::_6);
        CASE_CONV_R(GLFW_KEY_7, Key::_7);
        CASE_CONV_R(GLFW_KEY_8, Key::_8);
        CASE_CONV_R(GLFW_KEY_9, Key::_9);
        CASE_CONV_R(GLFW_KEY_SEMICOLON, Key::SEMICOLON);
        CASE_CONV_R(GLFW_KEY_EQUAL, Key::EQUAL);
        CASE_CONV_R(GLFW_KEY_A, Key::A);
        CASE_CONV_R(GLFW_KEY_B, Key::B);
        CASE_CONV_R(GLFW_KEY_C, Key::C);
        CASE_CONV_R(GLFW_KEY_D, Key::D);
        CASE_CONV_R(GLFW_KEY_E, Key::E);
        CASE_CONV_R(GLFW_KEY_F, Key::F);
        CASE_CONV_R(GLFW_KEY_G, Key::G);
        CASE_CONV_R(GLFW_KEY_H, Key::H);
        CASE_CONV_R(GLFW_KEY_I, Key::I);
        CASE_CONV_R(GLFW_KEY_J, Key::J);
        CASE_CONV_R(GLFW_KEY_K, Key::K);
        CASE_CONV_R(GLFW_KEY_L, Key::L);
        CASE_CONV_R(GLFW_KEY_M, Key::M);
        CASE_CONV_R(GLFW_KEY_N, Key::N);
        CASE_CONV_R(GLFW_KEY_O, Key::O);
        CASE_CONV_R(GLFW_KEY_P, Key::P);
        CASE_CONV_R(GLFW_KEY_Q, Key::Q);
        CASE_CONV_R(GLFW_KEY_R, Key::R);
        CASE_CONV_R(GLFW_KEY_S, Key::S);
        CASE_CONV_R(GLFW_KEY_T, Key::T);
        CASE_CONV_R(GLFW_KEY_U, Key::U);
        CASE_CONV_R(GLFW_KEY_V, Key::V);
        CASE_CONV_R(GLFW_KEY_W, Key::W);
        CASE_CONV_R(GLFW_KEY_X, Key::X);
        CASE_CONV_R(GLFW_KEY_Y, Key::Y);
        CASE_CONV_R(GLFW_KEY_Z, Key::Z);
        CASE_CONV_R(GLFW_KEY_LEFT_BRACKET, Key::LEFT_BRACKET);
        CASE_CONV_R(GLFW_KEY_BACKSLASH, Key::BACKSLASH);
        CASE_CONV_R(GLFW_KEY_RIGHT_BRACKET, Key::RIGHT_BRACKET);
        CASE_CONV_R(GLFW_KEY_GRAVE_ACCENT, Key::GRAVE_ACCENT);
        CASE_CONV_R(GLFW_KEY_WORLD_1, Key::WORLD1);
        CASE_CONV_R(GLFW_KEY_WORLD_2, Key::WORLD2);
        CASE_CONV_R(GLFW_KEY_ESCAPE, Key::ESCAPE);
        CASE_CONV_R(GLFW_KEY_ENTER, Key::ENTER);
        CASE_CONV_R(GLFW_KEY_TAB, Key::TAB);
        CASE_CONV_R(GLFW_KEY_BACKSPACE, Key::BACKSPACE);
        CASE_CONV_R(GLFW_KEY_INSERT, Key::INSERT);
        CASE_CONV_R(GLFW_KEY_DELETE, Key::DELETE);
        CASE_CONV_R(GLFW_KEY_RIGHT, Key::RIGHT);
        CASE_CONV_R(GLFW_KEY_LEFT, Key::LEFT);
        CASE_CONV_R(GLFW_KEY_DOWN, Key::DOWN);
        CASE_CONV_R(GLFW_KEY_UP, Key::UP);
        CASE_CONV_R(GLFW_KEY_PAGE_UP, Key::PAGE_UP);
        CASE_CONV_R(GLFW_KEY_PAGE_DOWN, Key::PAGE_DOWN);
        CASE_CONV_R(GLFW_KEY_HOME, Key::HOME);
        CASE_CONV_R(GLFW_KEY_END, Key::END);
        CASE_CONV_R(GLFW_KEY_CAPS_LOCK, Key::CAPS_LOCK);
        CASE_CONV_R(GLFW_KEY_SCROLL_LOCK, Key::SCROLL_LOCK);
        CASE_CONV_R(GLFW_KEY_NUM_LOCK, Key::NUM_LOCK);
        CASE_CONV_R(GLFW_KEY_PRINT_SCREEN, Key::PRINT_SCREEN);
        CASE_CONV_R(GLFW_KEY_PAUSE, Key::PAUSE);
        CASE_CONV_R(GLFW_KEY_F1, Key::F1);
        CASE_CONV_R(GLFW_KEY_F2, Key::F2);
        CASE_CONV_R(GLFW_KEY_F3, Key::F3);
        CASE_CONV_R(GLFW_KEY_F4, Key::F4);
        CASE_CONV_R(GLFW_KEY_F5, Key::F5);
        CASE_CONV_R(GLFW_KEY_F6, Key::F6);
        CASE_CONV_R(GLFW_KEY_F7, Key::F7);
        CASE_CONV_R(GLFW_KEY_F8, Key::F8);
        CASE_CONV_R(GLFW_KEY_F9, Key::F9);
        CASE_CONV_R(GLFW_KEY_F10, Key::F10);
        CASE_CONV_R(GLFW_KEY_F11, Key::F11);
        CASE_CONV_R(GLFW_KEY_F12, Key::F12);
        CASE_CONV_R(GLFW_KEY_F13, Key::F13);
        CASE_CONV_R(GLFW_KEY_F14, Key::F14);
        CASE_CONV_R(GLFW_KEY_F15, Key::F15);
        CASE_CONV_R(GLFW_KEY_F16, Key::F16);
        CASE_CONV_R(GLFW_KEY_F17, Key::F17);
        CASE_CONV_R(GLFW_KEY_F18, Key::F18);
        CASE_CONV_R(GLFW_KEY_F19, Key::F19);
        CASE_CONV_R(GLFW_KEY_F20, Key::F20);
        CASE_CONV_R(GLFW_KEY_F21, Key::F21);
        CASE_CONV_R(GLFW_KEY_F22, Key::F22);
        CASE_CONV_R(GLFW_KEY_F23, Key::F23);
        CASE_CONV_R(GLFW_KEY_F24, Key::F24);
        CASE_CONV_R(GLFW_KEY_F25, Key::F25);
        CASE_CONV_R(GLFW_KEY_KP_0, Key::NUMPAD0);
        CASE_CONV_R(GLFW_KEY_KP_1, Key::NUMPAD1);
        CASE_CONV_R(GLFW_KEY_KP_2, Key::NUMPAD2);
        CASE_CONV_R(GLFW_KEY_KP_3, Key::NUMPAD3);
        CASE_CONV_R(GLFW_KEY_KP_4, Key::NUMPAD4);
        CASE_CONV_R(GLFW_KEY_KP_5, Key::NUMPAD5);
        CASE_CONV_R(GLFW_KEY_KP_6, Key::NUMPAD6);
        CASE_CONV_R(GLFW_KEY_KP_7, Key::NUMPAD7);
        CASE_CONV_R(GLFW_KEY_KP_8, Key::NUMPAD8);
        CASE_CONV_R(GLFW_KEY_KP_9, Key::NUMPAD9);
        CASE_CONV_R(GLFW_KEY_KP_DECIMAL, Key::NUMPAD_DECIMAL);
        CASE_CONV_R(GLFW_KEY_KP_DIVIDE, Key::NUMPAD_DIVIDE);
        CASE_CONV_R(GLFW_KEY_KP_MULTIPLY, Key::NUMPAD_MULTIPLY);
        CASE_CONV_R(GLFW_KEY_KP_SUBTRACT, Key::NUMPAD_SUBSTRACT);
        CASE_CONV_R(GLFW_KEY_KP_ADD, Key::NUMPAD_ADD);
        CASE_CONV_R(GLFW_KEY_KP_ENTER, Key::NUMPAD_ENTER);
        CASE_CONV_R(GLFW_KEY_KP_EQUAL, Key::NUMPAD_EQUAL);
        CASE_CONV_R(GLFW_KEY_LEFT_SHIFT, Key::LEFT_SHIFT);
        CASE_CONV_R(GLFW_KEY_LEFT_CONTROL, Key::LEFT_CONTROL);
        CASE_CONV_R(GLFW_KEY_LEFT_ALT, Key::LEFT_ALT);
        CASE_CONV_R(GLFW_KEY_LEFT_SUPER, Key::LEFT_SUPER);
        CASE_CONV_R(GLFW_KEY_RIGHT_SHIFT, Key::RIGHT_SHIFT);
        CASE_CONV_R(GLFW_KEY_RIGHT_CONTROL, Key::RIGHT_CONTROL);
        CASE_CONV_R(GLFW_KEY_RIGHT_ALT, Key::RIGHT_ALT);
        CASE_CONV_R(GLFW_KEY_RIGHT_SUPER, Key::RIGHT_SUPER);
        CASE_CONV_R(GLFW_KEY_MENU, Key::MENU);
        default: 
            return GLFW_KEY_UNKNOWN;
    }
}

Key IncrementKey(Key k) {
    return static_cast<Key>(static_cast<int>(k) + 1);
}

MouseButton IncrementButton(MouseButton b) {
    return static_cast<MouseButton>(static_cast<int>(b) + 1);
}

std::array<bool, kKeyCount> okami::KeyboardStateFromGLFW(GLFWwindow* window) {
    std::array<bool, kKeyCount> arr;
    std::fill(arr.begin(), arr.end(), false);

    for (Key k = Key::SPACE; k < Key::COUNT; k = IncrementKey(k)) {
        auto state = glfwGetKey(window, ConvertGLFWFromKey(k));
        if (state == GLFW_PRESS) {
            arr[static_cast<int>(k)] = true;
        }
    }

    return arr;
}

std::array<bool, kMouseButtonCount> okami::MouseStateFromGLFW(GLFWwindow* window) {
    std::array<bool, kMouseButtonCount> arr;
    std::fill(arr.begin(), arr.end(), false);

    for (MouseButton button = MouseButton::LEFT; button < MouseButton::COUNT; button = IncrementButton(button)) {
        auto state = glfwGetMouseButton(window, ConvertGLFWFromMouseButton(button));
        if (state == GLFW_PRESS) {
            arr[static_cast<int>(button)] = true;
        }
    }

    return arr;
}

KeyAction okami::ConvertKeyActionFromGLFW(int action) {
    switch (action) {
        CASE_CONV_F(GLFW_PRESS, KeyAction::PRESS);
        CASE_CONV_F(GLFW_RELEASE, KeyAction::RELEASE);
        CASE_CONV_F(GLFW_REPEAT, KeyAction::REPEAT);
        default:
            return KeyAction::NONE;
    }
}
KeyModifiers okami::ConvertKeyModifiersFromGLFW(int mods) {
    KeyModifiers result;
    if (mods & GLFW_MOD_SHIFT) {
        result.value |= KeyModifiers::SHIFT;
    }
    if (mods & GLFW_MOD_CONTROL) {
        result.value |= KeyModifiers::CONTROL;
    }
    if (mods & GLFW_MOD_ALT) {
        result.value |= KeyModifiers::ALT;
    }
    if (mods & GLFW_MOD_SUPER) {
        result.value |= KeyModifiers::SUPER;
    }
    if (mods & GLFW_MOD_CAPS_LOCK) {
        result.value |= KeyModifiers::CAPS_LOCK;
    }
    if (mods & GLFW_MOD_NUM_LOCK) {
        result.value |= KeyModifiers::NUM_LOCK;
    }
    return result;
}

Error okami::GLFWKeyHandler(StaticVector<KeyEvent, kKeyboardMaxEvents>& buffer,
    int key, int scancode, int action, int mods) {
    KeyEvent evnt;
    evnt.key = ConvertKeyFromGLFW(key);
    evnt.action = ConvertKeyActionFromGLFW(action);
    evnt.mods = ConvertKeyModifiersFromGLFW(mods);
    evnt.scancode = scancode;

    if (buffer.emplace(evnt)) {
        return {};
    } else {
        return OKAMI_ERR_MAKE(RuntimeError{"Key event buffer is full!"});
    }
}

Error okami::GLFWMouseHandler(StaticVector<MouseEvent, kMouseMaxEvents>& buffer,
    int button, int action, int mods) {
    MouseEvent evnt;
    evnt.button = ConvertMouseButtonFromGLFW(button);
    evnt.action = ConvertKeyActionFromGLFW(action);
    evnt.mods = ConvertKeyModifiersFromGLFW(mods);

    if (buffer.emplace(evnt)) {
        return {};
    } else {
        return OKAMI_ERR_MAKE(RuntimeError{"Mouse event buffer is full!"});
    }
}