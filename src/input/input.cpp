#include "input.h"

#include <iostream>
#include <optional>
#include <map>
#include <GLFW/glfw3.h>

namespace Input {
	Input::Input(GLFWwindow& glfw_window) : glfw_window(&glfw_window)
	{
	}
	bool Input::is_key_pressed(int key)
	{
		int result = glfwGetKey(glfw_window, key);
		return (result == GLFW_PRESS);
	}
	std::optional<int> key_str_to_key_glfw(std::string key) {
		static const std::map<std::string, int> map = {
			{" "	, GLFW_KEY_SPACE        },
			{"'"	, GLFW_KEY_APOSTROPHE	},
			{","	, GLFW_KEY_COMMA		},
			{"-"	, GLFW_KEY_MINUS		},
			{"."	, GLFW_KEY_PERIOD		},
			{"/"	, GLFW_KEY_SLASH		},
			{"0"	, GLFW_KEY_0			},
			{"1"	, GLFW_KEY_1			},
			{"2"	, GLFW_KEY_2			},
			{"3"	, GLFW_KEY_3			},
			{"4"	, GLFW_KEY_4			},
			{"5"	, GLFW_KEY_5			},
			{"6"	, GLFW_KEY_6			},
			{"7"	, GLFW_KEY_7			},
			{"8"	, GLFW_KEY_8			},
			{"9"	, GLFW_KEY_9			},
			{";"	, GLFW_KEY_SEMICOLON	},
			{"="	, GLFW_KEY_EQUAL		},
			{"a"	, GLFW_KEY_A			},
			{"b"	, GLFW_KEY_B			},
			{"c"	, GLFW_KEY_C			},
			{"d"	, GLFW_KEY_D			},
			{"e"	, GLFW_KEY_E			},
			{"f"	, GLFW_KEY_F			},
			{"g"	, GLFW_KEY_G			},
			{"h"	, GLFW_KEY_H			},
			{"i"	, GLFW_KEY_I			},
			{"j"	, GLFW_KEY_J			},
			{"k"	, GLFW_KEY_K			},
			{"l"	, GLFW_KEY_L			},
			{"m"	, GLFW_KEY_M			},
			{"n"	, GLFW_KEY_N			},
			{"o"	, GLFW_KEY_O			},
			{"p"	, GLFW_KEY_P			},
			{"q"	, GLFW_KEY_Q			},
			{"r"	, GLFW_KEY_R			},
			{"s"	, GLFW_KEY_S			},
			{"t"	, GLFW_KEY_T			},
			{"u"	, GLFW_KEY_U			},
			{"v"	, GLFW_KEY_V			},
			{"w"	, GLFW_KEY_W			},
			{"x"	, GLFW_KEY_X			},
			{"y"	, GLFW_KEY_Y			},
			{"z"	, GLFW_KEY_Z			},
			{"("	, GLFW_KEY_LEFT_BRACKET	},
			{"\\"	, GLFW_KEY_BACKSLASH	},
			{")"	, GLFW_KEY_RIGHT_BRACKET},
			{"`"	, GLFW_KEY_GRAVE_ACCENT	},
			{""	, GLFW_KEY_WORLD_1		},
			{""	, GLFW_KEY_WORLD_2		},
			{"Esc"	, GLFW_KEY_ESCAPE		},
			{"Enter"	, GLFW_KEY_ENTER		},
			{"Tab"	, GLFW_KEY_TAB			},
			{"Backspace"	, GLFW_KEY_BACKSPACE	},
			{"Insert"	, GLFW_KEY_INSERT		},
			{"Delete"	, GLFW_KEY_DELETE		},
			{"Right"	, GLFW_KEY_RIGHT		},
			{"Left"	, GLFW_KEY_LEFT			},
			{"Down"	, GLFW_KEY_DOWN			},
			{"Up"	, GLFW_KEY_UP			},
			{"PgUp"	, GLFW_KEY_PAGE_UP		},
			{"PgDown"	, GLFW_KEY_PAGE_DOWN	},
			{"Home"	, GLFW_KEY_HOME			},
			{"End"	, GLFW_KEY_END			},
			{"CapsLock"	, GLFW_KEY_CAPS_LOCK	},
			{"ScrollLock"	, GLFW_KEY_SCROLL_LOCK	},
			{"NumLock"	, GLFW_KEY_NUM_LOCK		},
			{"PrtScr"	, GLFW_KEY_PRINT_SCREEN	},
			{"Pause"	, GLFW_KEY_PAUSE		},
			{"F1"	, GLFW_KEY_F1			},
			{"F2"	, GLFW_KEY_F2			},
			{"F3"	, GLFW_KEY_F3			},
			{"F4"	, GLFW_KEY_F4			},
			{"F5"	, GLFW_KEY_F5			},
			{"F6"	, GLFW_KEY_F6			},
			{"F7"	, GLFW_KEY_F7			},
			{"F8"	, GLFW_KEY_F8			},
			{"F9"	, GLFW_KEY_F9			},
			{"F10"	, GLFW_KEY_F10			},
			{"F11"	, GLFW_KEY_F11			},
			{"F12"	, GLFW_KEY_F12			},
			{"F13"	, GLFW_KEY_F13			},
			{"F14"	, GLFW_KEY_F14			},
			{"F15"	, GLFW_KEY_F15			},
			{"F16"	, GLFW_KEY_F16			},
			{"F17"	, GLFW_KEY_F17			},
			{"F18"	, GLFW_KEY_F18			},
			{"F19"	, GLFW_KEY_F19			},
			{"F20"	, GLFW_KEY_F20			},
			{"F21"	, GLFW_KEY_F21			},
			{"F22"	, GLFW_KEY_F22			},
			{"F23"	, GLFW_KEY_F23			},
			{"F24"	, GLFW_KEY_F24			},
			{"F25"	, GLFW_KEY_F25			},
			{"Kp0"	, GLFW_KEY_KP_0			},
			{"Kp1"	, GLFW_KEY_KP_1			},
			{"Kp2"	, GLFW_KEY_KP_2			},
			{""	, GLFW_KEY_KP_3			},
			{""	, GLFW_KEY_KP_4			},
			{""	, GLFW_KEY_KP_5			},
			{""	, GLFW_KEY_KP_6			},
			{""	, GLFW_KEY_KP_7			},
			{""	, GLFW_KEY_KP_8			},
			{""	, GLFW_KEY_KP_9			},
			{"Decimal"	, GLFW_KEY_KP_DECIMAL	},
			{"Divide"	, GLFW_KEY_KP_DIVIDE	},
			{"*"	, GLFW_KEY_KP_MULTIPLY	},
			{"-"	, GLFW_KEY_KP_SUBTRACT	},
			{"+"	, GLFW_KEY_KP_ADD		},
			{"KpEnter"	, GLFW_KEY_KP_ENTER		},
			{"="	, GLFW_KEY_KP_EQUAL		},
			{"LeftShift"	, GLFW_KEY_LEFT_SHIFT	},
			{"LeftCtrl"	, GLFW_KEY_LEFT_CONTROL	},
			{"LeftALt"	, GLFW_KEY_LEFT_ALT		},
			{"LeftSuper"	, GLFW_KEY_LEFT_SUPER	},
			{"RightShift"	, GLFW_KEY_RIGHT_SHIFT	},
			{"RightCtrl"	, GLFW_KEY_RIGHT_CONTROL},
			{"RightAlt"	, GLFW_KEY_RIGHT_ALT	},
			{"RightSuper"	, GLFW_KEY_RIGHT_SUPER	},
			{"Menu"	, GLFW_KEY_MENU         }
		};
		if (map.contains(key)) {
			return map.at(key);
		}
		return std::nullopt;
	}
	bool Input::is_key_pressed_str(std::string key)
	{
		auto glfw_key = key_str_to_key_glfw(key);
		if (!glfw_key) {
			return false;
		}
		bool is_pressed = is_key_pressed(*glfw_key);
		return is_pressed;
	}
	void Input::enable_cursor()
	{
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	void Input::disable_cursor()
	{
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}