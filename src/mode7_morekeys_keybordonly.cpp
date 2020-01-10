#include "header.hpp"
#include "SFML/Graphics/Texture.hpp"







namespace morekeys_keybordonly
{
	Json::Value stopemoticon_value;
	bool is_letterbox, is_left_handed,is_emoticonkeep;
	sf::Sprite bg, left_up,right_up;

	int lefthand_state = -1;
	int righthand_state=-1;
	int face_state = -1;



	int keybord[MAX_KEYS_NUM];
	sf::Sprite keybord_press[MAX_KEYS_NUM];
	int lefthand[MAX_KEYS_NUM];
	int righthand[MAX_KEYS_NUM];
	sf::Sprite lefthand_press[MAX_KEYS_NUM];
	sf::Sprite righthand_press[MAX_KEYS_NUM];
	int face[MAX_KEYS_NUM];
	sf::Sprite face_press[MAX_KEYS_NUM];









	bool init()
	{

		/*	atex.create(1, 1, sf::Color(254, 196, 120));
			atextex.setRepeated(true);
			atextex.loadFromImage(atex);
			armTex.texture = &atextex;
			*/



		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			keybord[i] = data::cfg["morekeys_keybordonly"]["keybord"][i].asInt();
			keybord_press[i].setTexture(data::load_texture2("img/keybordonly/keybord/" + std::to_string(i) + ".png"));
			lefthand[i] = data::cfg["morekeys_keybordonly"]["lefthand"][i].asInt();
			lefthand_press[i].setTexture(data::load_texture2("img/keybordonly/lefthand/" + std::to_string(i) + ".png"));
			righthand[i] = data::cfg["morekeys_keybordonly"]["righthand"][i].asInt();
			righthand_press[i].setTexture(data::load_texture2("img/keybordonly/righthand/" + std::to_string(i) + ".png"));

			face[i] = data::cfg["morekeys_keybordonly"]["face"][i].asInt();
			face_press[i].setTexture(data::load_texture2("img/keybordonly/face/" + std::to_string(i) + ".png"));
		}




		is_emoticonkeep = data::cfg["addition"]["emoticonKeep"].asBool();
		stopemoticon_value = data::cfg["addition"]["emoticonClear"];


		// importing sprites
		left_up.setTexture(data::load_texture("img/keybordonly/lefthand/leftup.png"));
		right_up.setTexture(data::load_texture("img/keybordonly/righthand/rightup.png"));


		bg.setTexture(data::load_texture("img/keybordonly/bg.png"));

		// getting resolution
		RECT desktop;
		const HWND h_desktop = GetDesktopWindow();
		GetWindowRect(h_desktop, &desktop);

		return true;
	}

	void draw()
	{
		window.draw(bg);
		



		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			if (GetKeyState(keybord[i]) & 0x8000)
			{
				window.draw(keybord_press[i]);
			}
		}

		lefthand_state = -1;
		righthand_state = -1;
		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			if (GetKeyState(lefthand[i]) & 0x8000)
				lefthand_state = i;
			if (GetKeyState(righthand[i]) & 0x8000)
				righthand_state = i;
			if (GetKeyState(face[i]) & 0x8000)
				face_state = i;
		}
		if (lefthand_state != -1) {
			window.draw(lefthand_press[lefthand_state]);
		}
		else
			window.draw(left_up);
		if (righthand_state != -1) {
			window.draw(righthand_press[righthand_state]);
		}
		else
			window.draw(right_up);
		if (is_emoticonkeep) {
			if (face_state != -1) {
				window.draw(face_press[face_state]);
			}
			for (Json::Value& v : stopemoticon_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					face_state = -1;
					break;
				}

			}
		else {
			if (face_state != -1) {
				window.draw(face_press[face_state]);
			}
			face_state = -1;
		}



	}
}; // namespace morekeys_keybordonly






