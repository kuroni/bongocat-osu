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
	int sound_state=-1;
	int sound_play_state[MAX_KEYS_NUM]={0};
	int sound_play_state_last[MAX_KEYS_NUM] = {0};







	int keybord[MAX_KEYS_NUM];
	sf::Sprite keybord_press[MAX_KEYS_NUM];
	int lefthand[MAX_KEYS_NUM];
	int righthand[MAX_KEYS_NUM];
	sf::Sprite lefthand_press[MAX_KEYS_NUM];
	sf::Sprite righthand_press[MAX_KEYS_NUM];
	int face[MAX_KEYS_NUM];
	sf::Sprite face_press[MAX_KEYS_NUM];
	int sounds[MAX_KEYS_NUM];


	sf::SoundBuffer soundBuffer[MAX_KEYS_NUM];
	sf::Sound sound[MAX_KEYS_NUM];






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



			sounds[i]= data::cfg["morekeys_keybordonly"]["sounds"][i].asInt();



			if (!soundBuffer[i].loadFromFile(("img/keybordonly/sound/" + std::to_string(i) + ".wav")))
				if (!soundBuffer[i].loadFromFile(("img/keybordonly/sound/" + std::to_string(i) + ".ogg")))
					if (!soundBuffer[i].loadFromFile(("img/keybordonly/sound/" + std::to_string(i) + ".flac")))
						;
			
			
			sound[i].setBuffer(soundBuffer[i]);


		}




		is_emoticonkeep = data::cfg["decoration"]["emoticonKeep"].asBool();
		stopemoticon_value = data::cfg["decoration"]["emoticonClear"];


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
			if (GetKeyState(sounds[i]) & 0x8000) {
				sound_state = i;
				sound_play_state[sound_state] = 1;
				if (sound_play_state[sound_state] == 1 && sound_play_state_last[sound_state] == 0) {
					sound[sound_state].play();
				}
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
			sound_play_state_last[i] = sound_play_state[i];
			sound_play_state[i] = 0;
			
		}

		sound_state = -1;


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
					/*sound_state = 1;
					if (sound_state == 1 && sound_state_last == 0)
						sound.play();
						*/

					

					face_state = -1;
					break;
				}

			/*sound_state_last = sound_state;
			sound_state = 0;*/


			}
		else {
			if (face_state != -1) {
				window.draw(face_press[face_state]);
			}
			face_state = -1;
		}





	}
}; // namespace morekeys_keybordonly






