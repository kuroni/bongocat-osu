#include "header.hpp"
#include "SFML/Graphics/Texture.hpp"








namespace keyboard
{
	int maxtime(sf::Time* time, int count);


	Json::Value stopemoticon_value;
	bool is_letterbox, is_left_handed,is_emoticonkeep;
	sf::Sprite bg, left_up,right_up;

	int lefthand_state = -1;
	int righthand_state=-1;
	int face_state = -1;
	int sound_state=-1;
	int sound_play_state[MAX_KEYS_NUM]={0};
	int sound_play_state_last[MAX_KEYS_NUM] = {0};


	sf::Clock key_clock;
	sf::Time init_time;




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

	sf::Time timer_hand_left[MAX_KEYS_NUM];
	sf::Time timer_hand_right[MAX_KEYS_NUM];
	int hand_left_play_state[MAX_KEYS_NUM] = { 0 };
	int hand_left_play_state_last[MAX_KEYS_NUM] = { 0 };
	int hand_right_play_state[MAX_KEYS_NUM] = { 0 };
	int hand_right_play_state_last[MAX_KEYS_NUM] = { 0 };




	bool init()
	{

		/*	atex.create(1, 1, sf::Color(254, 196, 120));
			atextex.setRepeated(true);
			atextex.loadFromImage(atex);
			armTex.texture = &atextex;
			*/

		key_clock.restart();//初始化时间
		init_time = timer_hand_left[0] = key_clock.getElapsedTime();





		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			keybord[i] = data::cfg["keyboard"]["keyboard"][i].asInt();
			keybord_press[i].setTexture(data::load_texture2("img/keyboard/keyboard/" + std::to_string(i) + ".png"));
			lefthand[i] = data::cfg["keyboard"]["lefthand"][i].asInt();
			lefthand_press[i].setTexture(data::load_texture2("img/keyboard/lefthand/" + std::to_string(i) + ".png"));
			righthand[i] = data::cfg["keyboard"]["righthand"][i].asInt();
			righthand_press[i].setTexture(data::load_texture2("img/keyboard/righthand/" + std::to_string(i) + ".png"));

			face[i] = data::cfg["keyboard"]["face"][i].asInt();
			face_press[i].setTexture(data::load_texture2("img/keyboard/face/" + std::to_string(i) + ".png"));



			sounds[i]= data::cfg["keyboard"]["sounds"][i].asInt();



			if (!soundBuffer[i].loadFromFile(("img/keyboard/sound/" + std::to_string(i) + ".wav")))
				if (!soundBuffer[i].loadFromFile(("img/keyboard/sound/" + std::to_string(i) + ".ogg")))
					if (!soundBuffer[i].loadFromFile(("img/keyboard/sound/" + std::to_string(i) + ".flac")))
						;
			
			
			sound[i].setBuffer(soundBuffer[i]);

			timer_hand_left[i] = timer_hand_right[i] = timer_hand_left[0];

		}




		is_emoticonkeep = data::cfg["decoration"]["emoticonKeep"].asBool();
		stopemoticon_value = data::cfg["decoration"]["emoticonClear"];


		// importing sprites
		left_up.setTexture(data::load_texture("img/keyboard/lefthand/leftup.png"));
		right_up.setTexture(data::load_texture("img/keyboard/righthand/rightup.png"));


		bg.setTexture(data::load_texture("img/keyboard/bg.png"));

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
			if (GetKeyState(lefthand[i]) & 0x8000) {
				lefthand_state = i;
				hand_left_play_state[lefthand_state] = 1;
				if (hand_left_play_state[lefthand_state] == 1 && hand_left_play_state_last[lefthand_state] == 0)
					timer_hand_left[i] = key_clock.getElapsedTime();
			}
			else
				timer_hand_left[i] = init_time;
			hand_left_play_state_last[i] = hand_left_play_state[i];
			hand_left_play_state[i] = 0;


			if (GetKeyState(righthand[i]) & 0x8000) {
				righthand_state = i;
				hand_right_play_state[righthand_state] = 1;
				if (hand_right_play_state[righthand_state] == 1 && hand_right_play_state_last[righthand_state] == 0)
					timer_hand_right[i] = key_clock.getElapsedTime();
			}
			else
				timer_hand_right[i] = init_time;
			hand_right_play_state_last[i] = hand_right_play_state[i];
			hand_right_play_state[i] = 0;



			if (GetKeyState(face[i]) & 0x8000) 
				face_state = i;
			sound_play_state_last[i] = sound_play_state[i];
			sound_play_state[i] = 0;
			
		}

		sound_state = -1;


		if (lefthand_state != -1) {
			window.draw(lefthand_press[maxtime(timer_hand_left, MAX_KEYS_NUM)]);
		}
		else
			window.draw(left_up);
		if (righthand_state != -1) {
			window.draw(righthand_press[maxtime(timer_hand_right, MAX_KEYS_NUM)]);
		}
		else
			window.draw(right_up);


		if (is_emoticonkeep) {
			if (face_state != -1) 
				window.draw(face_press[face_state]);
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








	int maxtime(sf::Time* time, int count)
	{
		int state = 0;
		sf::Time max_time = time[0];

		for (int i = 0; i < count; i++)
			if (max_time < time[i]) {
				max_time = time[i];
				state = i;
			}
		return state;
	}





}; // namespace keyboard






