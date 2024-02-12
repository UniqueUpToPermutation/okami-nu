#pragma once

#include <cstdint>
#include <functional>

namespace okami {
    enum class KeyState {
        PRESS,
        RELEASE
    };

    enum class MouseButton {
        LEFT,
        RIGHT,
        MIDDLE,
        BUTTON4,
        BUTTON5,
        BUTTON6,
        BUTTON7,
        BUTTON8,
        COUNT
    };
    constexpr size_t kMouseButtonCount = static_cast<size_t>(MouseButton::COUNT);

    enum class Key {
        UNKNOWN,
        SPACE,
        APOSTROPHE,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        _0,
        _1,
        _2,
        _3,
        _4,
        _5,
        _6,
        _7,
        _8,
        _9,
        SEMICOLON,
        EQUAL,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K, 
        L,
        M, 
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        LEFT_BRACKET,
        BACKSLASH,
        RIGHT_BRACKET,
        GRAVE_ACCENT,
        WORLD1,
        WORLD2,
        ESCAPE,
        ENTER,
        TAB,
        BACKSPACE,
        INSERT,
        DELETE,
        RIGHT,
        LEFT,
        DOWN,
        UP,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        CAPS_LOCK,
        SCROLL_LOCK,
        NUM_LOCK,
        PRINT_SCREEN,
        PAUSE,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        NUMPAD0,
        NUMPAD1,
        NUMPAD2,
        NUMPAD3,
        NUMPAD4,
        NUMPAD5,
        NUMPAD6,
        NUMPAD7,
        NUMPAD8,
        NUMPAD9,
        NUMPAD_DECIMAL,
        NUMPAD_DIVIDE,
        NUMPAD_MULTIPLY,
        NUMPAD_SUBSTRACT,
        NUMPAD_ADD,
        NUMPAD_ENTER,
        NUMPAD_EQUAL,
        LEFT_SHIFT,
        LEFT_CONTROL,
        LEFT_ALT,
        LEFT_SUPER,
        RIGHT_SHIFT,
        RIGHT_CONTROL,
        RIGHT_ALT,
        RIGHT_SUPER,
        MENU,
        COUNT
    };
    constexpr size_t kKeyCount = static_cast<size_t>(Key::COUNT);

    enum class KeyModifier {
        SHIFT,
        CONTROL,
        ALT,
        SUPER,
        CAPS_LOCK,
        NUM_LOCK,
        COUNT
    };
    constexpr size_t kKeyModifierCount = static_cast<size_t>(KeyModifier::COUNT);

    struct KeyModifiers {
        uint32_t value = 0;

        static constexpr uint32_t SHIFT = (1u << static_cast<uint32_t>(KeyModifier::SHIFT));
        static constexpr uint32_t CONTROL = (1u << static_cast<uint32_t>(KeyModifier::CONTROL));
        static constexpr uint32_t ALT = (1u << static_cast<uint32_t>(KeyModifier::ALT));
        static constexpr uint32_t SUPER = (1u << static_cast<uint32_t>(KeyModifier::SUPER));
        static constexpr uint32_t CAPS_LOCK = (1u << static_cast<uint32_t>(KeyModifier::CAPS_LOCK));
        static constexpr uint32_t NUM_LOCK = (1u << static_cast<uint32_t>(KeyModifier::NUM_LOCK));

        bool Has(KeyModifier mod) {
            return value & (1u << static_cast<uint32_t>(mod));
        }
    };

    enum class KeyAction {
        PRESS,
        REPEAT,
        RELEASE,
        NONE,
        COUNT
    };
    constexpr size_t kKeyAction = static_cast<size_t>(KeyAction::COUNT);

    struct KeyEvent {
        int scancode;
        Key key;
        KeyAction action;
        KeyModifiers mods;
    };

    struct MouseEvent {
        MouseButton button;
        KeyAction action;
        KeyModifiers mods;
    };

    template <typename T, size_t kMaxCount>
    struct StaticVector {
        std::array<T, kMaxCount> arr;
        int count = 0;

        inline auto begin() { return arr.begin(); }
        inline auto end() { return arr.begin() + count; }
        inline auto begin() const { return arr.begin(); }
        inline auto end() const { return arr.begin() + count; }
        inline bool clear() { count = 0; }
        bool emplace(T obj) {
            if (count >= kMaxCount) {
                return false;
            } else {
                arr[count++] = std::move(obj); 
                return true;
            }
        }
        bool emplace(T&& obj) { 
            if (count >= kMaxCount) {
                return false;
            } else {
                arr[count++] = std::move(obj); 
            }
        }
    };

    template <typename EventT, typename KeyT, size_t kInputCount, size_t kMaxEvents>
    class InputState {
    private:
        using state_t = uint32_t;

        std::array<bool, kInputCount> _states;
        StaticVector<EventT, kMaxEvents> _events;

    public:
        inline InputState() {
            std::fill(_states.begin(), _states.end(), false);
        }
        inline InputState(std::array<bool, kInputCount> states,
            StaticVector<EventT, kMaxEvents> events = {}) : _states(states), _events(events) {}

        void SetState(std::array<bool, kInputCount> states) {
            _states = states;
        }
        void ClearEvents() {
            _events.clear();
        }
        bool AddEvent(KeyEvent event) {
            _events.emplace(event);
        }
        bool IsPressed(KeyT key) const {
            return _states[static_cast<int>(key)];
        }
        bool IsReleased(KeyT key) const {
            return _states[static_cast<int>(key)];
        }
        inline StaticVector<EventT, kMaxEvents> const& Events() const {
            return _events;
        }
    };

    constexpr size_t kKeyboardMaxEvents = 32;
    constexpr size_t kMouseMaxEvents = 8;

    using KeyboardState = InputState<KeyEvent, Key, kKeyCount, kKeyboardMaxEvents>;

    class MouseState : public InputState<MouseEvent, MouseButton, kMouseButtonCount, kMouseMaxEvents> {
    private:
        int _x;
        int _y;
    
    public:
        inline MouseState() : InputState() {}
        inline MouseState(std::array<bool, kMouseButtonCount> states,
            int x, int y,
            StaticVector<MouseEvent, kMouseMaxEvents> events = {}) : 
                InputState(states, events), _x(x), _y(y) {}

        int X() const { return _x; }
        int Y() const { return _y; }
    };
}