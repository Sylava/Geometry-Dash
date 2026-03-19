#include "keylist.h"


Keylist::Keylist() {

	key_list[sf::Keyboard::Key::Space] = "Space";
	key_list[sf::Keyboard::Key::A] = "A";
	key_list[sf::Keyboard::Key::B] = "B";
	key_list[sf::Keyboard::Key::C] = "C";
	key_list[sf::Keyboard::Key::D] = "D";
	key_list[sf::Keyboard::Key::E] = "E";
	key_list[sf::Keyboard::Key::F] = "F";
	key_list[sf::Keyboard::Key::G] = "G";
	key_list[sf::Keyboard::Key::H] = "H";
	key_list[sf::Keyboard::Key::I] = "I";
	key_list[sf::Keyboard::Key::J] = "J";
	key_list[sf::Keyboard::Key::K] = "K";
	key_list[sf::Keyboard::Key::L] = "L";
	key_list[sf::Keyboard::Key::M] = "M";
	key_list[sf::Keyboard::Key::N] = "N";
	key_list[sf::Keyboard::Key::O] = "O";
	key_list[sf::Keyboard::Key::P] = "P";
	key_list[sf::Keyboard::Key::Q] = "Q";
	key_list[sf::Keyboard::Key::R] = "R";
	key_list[sf::Keyboard::Key::S] = "S";
	key_list[sf::Keyboard::Key::T] = "T";
	key_list[sf::Keyboard::Key::U] = "U";
	key_list[sf::Keyboard::Key::V] = "V";
	key_list[sf::Keyboard::Key::W] = "W";
	key_list[sf::Keyboard::Key::X] = "X";
	key_list[sf::Keyboard::Key::Y] = "Y";
	key_list[sf::Keyboard::Key::Z] = "Z";
	key_list[sf::Keyboard::Key::Up] = "up arrow";
	key_list[sf::Keyboard::Key::Down] = "down arrow";
	key_list[sf::Keyboard::Key::Left] = "left arrow";
	key_list[sf::Keyboard::Key::Right] = "right arrow";
	key_list[sf::Keyboard::Key::Num0] = "0";
	key_list[sf::Keyboard::Key::Num1] = "1";
	key_list[sf::Keyboard::Key::Num2] = "2";
	key_list[sf::Keyboard::Key::Num3] = "3";
	key_list[sf::Keyboard::Key::Num4] = "4";
	key_list[sf::Keyboard::Key::Num5] = "5";
	key_list[sf::Keyboard::Key::Num6] = "6";
	key_list[sf::Keyboard::Key::Num7] = "7";
	key_list[sf::Keyboard::Key::Num8] = "8";
	key_list[sf::Keyboard::Key::Num9] = "9";
	key_list[sf::Keyboard::Key::Backspace] = "Backspace";
	key_list[sf::Keyboard::Key::LShift] = "Left Shift";
	key_list[sf::Keyboard::Key::RShift] = "Right Shift";
	key_list[sf::Keyboard::Key::LControl] = "Left Ctrl";
	key_list[sf::Keyboard::Key::RControl] = "Right Ctrl";
	key_list[sf::Keyboard::Key::Apostrophe] = "'";
	key_list[sf::Keyboard::Key::Backslash] = "\\";
	key_list[sf::Keyboard::Key::Add] = "+";
	key_list[sf::Keyboard::Key::Comma] = ",";
	key_list[sf::Keyboard::Key::Divide] = "Divide Sign";
	key_list[sf::Keyboard::Key::Enter] = "Enter";
	key_list[sf::Keyboard::Key::Equal] = "=";
	key_list[sf::Keyboard::Key::Escape] = "Escape";
	key_list[sf::Keyboard::Key::Hyphen] = "-";
	key_list[sf::Keyboard::Key::LBracket] = "(";
	key_list[sf::Keyboard::Key::Multiply] = "*";
	key_list[sf::Keyboard::Key::Numpad0] = "num 0";
	key_list[sf::Keyboard::Key::Numpad1] = "num 1";
	key_list[sf::Keyboard::Key::Numpad2] = "num 2";
	key_list[sf::Keyboard::Key::Numpad3] = "num 3";
	key_list[sf::Keyboard::Key::Numpad4] = "num 4";
	key_list[sf::Keyboard::Key::Numpad5] = "num 5";
	key_list[sf::Keyboard::Key::Numpad6] = "num 6";
	key_list[sf::Keyboard::Key::Numpad7] = "num 7";
	key_list[sf::Keyboard::Key::Numpad8] = "num 8";
	key_list[sf::Keyboard::Key::Numpad9] = "num 9";
	key_list[sf::Keyboard::Key::Period] = ".";
	key_list[sf::Keyboard::Key::LAlt] = "Left alt";
	key_list[sf::Keyboard::Key::RAlt] = "Right alt";
	key_list[sf::Keyboard::Key::RBracket] = ")";
	key_list[sf::Keyboard::Key::Semicolon] = ";";
	key_list[sf::Keyboard::Key::Slash] = "/";
	key_list[sf::Keyboard::Key::Subtract] = "-";
	key_list[sf::Keyboard::Key::Tab] = "Tab";
}

bool Keylist::inlist(sf::Keyboard::Key test_key) {
	return key_list.contains(test_key);
}

std::string Keylist::printkey(sf::Keyboard::Key key) {
	if (inlist(key)) {
		return key_list[key];
	}
	else {
		return "Key invalid";
	}

}