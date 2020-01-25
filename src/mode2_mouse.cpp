#include "header.hpp"

#include "SFML/Graphics/Texture.hpp"


namespace  mouse
{
	Json::Value mouse_leftkey_value,  mouse_rightkey_value,stopemoticon_value;
	int offset_x, offset_y;
	int horizontal, vertical;
	double scale;
	bool is_emoticonkeep, is_mouse, is_mouse_force_move, is_left_handed, is_workarea;
	int mouse_state=0;
	sf::Sprite bg,device,mouse_left,mouse_right;

	int sound_state = -1;
	int sound_play_state[MAX_KEYS_NUM] = { 0 };
	int sound_play_state_last[MAX_KEYS_NUM] = { 0 };
	sf::SoundBuffer soundBuffer[MAX_KEYS_NUM];
	sf::Sound sound[MAX_KEYS_NUM];
	int sounds[MAX_KEYS_NUM];


	int face[MAX_KEYS_NUM];
	sf::Sprite face_press[MAX_KEYS_NUM];
	int key_state = -1;


	sf::Texture atextex;


	double letter_x, letter_y, s_height, s_width;

	DWORD WINAPI ThreadProc1(LPVOID lpParam);

	std::tuple<double, double> bezier(double ratio, std::vector<double>& points, int length)
	{
		double fact[22] = { 0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0 };
		int nn = (length / 2) - 1;
		double xx = 0;
		double yy = 0;

		for (int point = 0; point <= nn; point++)
		{
			double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
			xx += points[2 * point] * tmp;
			yy += points[2 * point + 1] * tmp;
		}

		return std::make_tuple(xx / 1000, yy / 1000);
	}



	//arm lines color
	int red_value;
	int green_value;
	int blue_value;

	int top_left;
	int right_bottom;

	POINT point, point_now, point_last, point_lastlast;




	bool init()
	{
		// getting configs


		red_value = data::cfg["decoration"]["armLineColor"][0].asInt();
		green_value = data::cfg["decoration"]["armLineColor"][1].asInt();
		blue_value = data::cfg["decoration"]["armLineColor"][2].asInt();


		if (!atextex.loadFromFile("img/mouse/arm.png"))
		{
			data::error_msg("File not successfully loaded", "Error loading files");
		}

		is_mouse = data::cfg["mouse"]["mouse"].asBool();
		is_emoticonkeep = data::cfg["decoration"]["emoticonKeep"].asBool();
		is_workarea = data::cfg["workarea"]["workarea"].asBool();
		is_mouse_force_move = data::cfg["decoration"]["mouse_force_move"].asBool();
		stopemoticon_value = data::cfg["decoration"]["emoticonClear"];


		if (is_mouse)
		{
			offset_x = (data::cfg["decoration"]["offsetX"])[0].asInt();
			offset_y = (data::cfg["decoration"]["offsetY"])[0].asInt();
			scale = (data::cfg["decoration"]["scalar"])[0].asDouble();
		}
		else
		{
			offset_x = (data::cfg["decoration"]["offsetX"])[1].asInt();
			offset_y = (data::cfg["decoration"]["offsetY"])[1].asInt();
			scale = (data::cfg["decoration"]["scalar"])[1].asDouble();
		}



		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			face[i] = data::cfg["mouse"]["face"][i].asInt();
			face_press[i].setTexture(data::load_texture2("img/mouse/face/" + std::to_string(i) + ".png"));


			sounds[i] = data::cfg["mouse"]["sounds"][i].asInt();
			if (!soundBuffer[i].loadFromFile(("img/mouse/sound/" + std::to_string(i) + ".wav")))
				if (!soundBuffer[i].loadFromFile(("img/mouse/sound/" + std::to_string(i) + ".ogg")))
					if (!soundBuffer[i].loadFromFile(("img/mouse/sound/" + std::to_string(i) + ".flac")))
						;


			sound[i].setBuffer(soundBuffer[i]);

		}



		bool chk[256];
		std::fill(chk, chk + 256, false);
		mouse_leftkey_value = data::cfg["mouse"]["mouse_left"];
		for (Json::Value& v : mouse_leftkey_value)
			chk[v.asInt()] = true;
		mouse_rightkey_value = data::cfg["mouse"]["mouse_right"];
		for (Json::Value& v : mouse_rightkey_value)
			chk[v.asInt()] = true;

	
		// importing sprites



			if (is_mouse)
			{
				bg.setTexture(data::load_texture("img/mouse/mousebg.png"));
				device.setTexture(data::load_texture("img/mouse/mouse.png"), true);
				mouse_left.setTexture(data::load_texture("img/mouse/mouse_left.png"), true);
				mouse_right.setTexture(data::load_texture("img/mouse/mouse_right.png"), true);
			}
			else
			{
				bg.setTexture(data::load_texture("img/mouse/tabletbg.png"));
				device.setTexture(data::load_texture("img/mouse/tablet.png"), true);
				mouse_left.setTexture(data::load_texture("img/mouse/tablet_left.png"), true);
				mouse_right.setTexture(data::load_texture("img/mouse/tablet_right.png"), true);
			}


		device.setScale(scale, scale);
		mouse_left.setScale(scale, scale);
		mouse_right.setScale(scale, scale);
		// getting resolution
		RECT desktop;
		const HWND h_desktop = GetDesktopWindow();
		GetWindowRect(h_desktop, &desktop);
		horizontal = desktop.right;
		vertical = desktop.bottom;



		if (is_workarea) {
			letter_x = data::cfg["workarea"]["top_left"][0].asInt();
			letter_y = data::cfg["workarea"]["top_left"][1].asInt();
			s_width = data::cfg["workarea"]["right_bottom"][0].asInt() - letter_x;
			s_height = data::cfg["workarea"]["right_bottom"][1].asInt() - letter_y;

		}
		else
		{
			s_width = horizontal;
			s_height = vertical;
			letter_x = 0;
			letter_y = 0;
		}

		if (GetCursorPos(&point_now))
			point.x = point_now.x;
		point.y = point_now.y;
		point_last.x = point_now.x;
		point_last.y = point_now.y;
		point_lastlast.x = point_now.x;
		point_lastlast.y = point_now.y;

		if (is_mouse_force_move)
		{
			HANDLE thread_handle;

			DWORD thread_ID;

			thread_handle = CreateThread(

				NULL,

				0,

				(LPTHREAD_START_ROUTINE)ThreadProc1,

				NULL,

				0,

				&thread_ID

			);

			if (!thread_handle)
				data::error_msg("duoxiancheng sile", "Error loading files");
		}
		return true;
	}

	void draw()
	{
		double x, y;
		if (is_mouse_force_move) {
			double fx = ((double)point.x - letter_x) / s_width;
			if (is_left_handed)
				fx = 1 - fx;
			double fy = ((double)point.y - letter_y) / s_height;
			fx = min(fx, 1.0);
			fx = max(fx, 0.0);
			fy = min(fy, 1.0);
			fy = max(fy, 0.0);
			x = -97 * fx + 44 * fy + 184;
			y = -76 * fx - 40 * fy + 324;
		}
		else
			if (GetCursorPos(&point))
			{
				double fx = ((double)point.x - letter_x) / s_width;
				if (is_left_handed)
					fx = 1 - fx;
				double fy = ((double)point.y - letter_y) / s_height;
				fx = min(fx, 1.0);
				fx = max(fx, 0.0);
				fy = min(fy, 1.0);
				fy = max(fy, 0.0);
				x = -97 * fx + 44 * fy + 184;
				y = -76 * fx - 40 * fy + 324;
			}
			else { x = y = 0; }
		window.draw(bg);

		// initializing pss and pss2 (kuvster's magic)
		int oof = 6;
		std::vector<double> pss = { 211.0, 159.0 };
		double dist = hypot(211 - x, 159 - y);
		double centreleft0 = 211 - 0.7237 * dist / 2;
		double centreleft1 = 159 + 0.69 * dist / 2;
		for (double i = 1; i < oof; i++)
		{
			double p0;
			double p1;
			std::vector<double> bez = { 211, 159, centreleft0, centreleft1, x, y };
			std::tie(p0, p1) = bezier(i / oof, bez, 6);
			pss.push_back(p0);
			pss.push_back(p1);
		}
		pss.push_back(x);
		pss.push_back(y);
		double a = y - centreleft1;
		double b = centreleft0 - x;
		double le = hypot(a, b);
		a = x + a / le * 60;
		b = y + b / le * 60;
		int a1 = 258;
		int a2 = 228;
		dist = hypot(a1 - a, a2 - b);
		double centreright0 = a1 - 0.6 * dist / 2;
		double centreright1 = a2 + 0.8 * dist / 2;
		int push = 20;
		double s = x - centreleft0;
		double t = y - centreleft1;
		le = hypot(s, t);
		s *= push / le;
		t *= push / le;
		double s2 = a - centreright0;
		double t2 = b - centreright1;
		le = hypot(s2, t2);
		s2 *= push / le;
		t2 *= push / le;
		for (double i = 1; i < oof; i++)
		{
			double p0;
			double p1;
			std::vector<double> bez = { x, y, x + s, y + t, a + s2, b + t2, a, b };
			std::tie(p0, p1) = bezier(i / oof, bez, 8);
			pss.push_back(p0);
			pss.push_back(p1);
		}
		pss.push_back(a);
		pss.push_back(b);
		for (double i = oof - 1; i > 0; i--)
		{
			double p0;
			double p1;
			std::vector<double> bez = { 1.0 * a1, 1.0 * a2, centreright0, centreright1, a, b };
			std::tie(p0, p1) = bezier(i / oof, bez, 6);
			pss.push_back(p0);
			pss.push_back(p1);
		}
		pss.push_back(a1);
		pss.push_back(a2);
		double mpos0 = (a + x) / 2 - 52 - 15;
		double mpos1 = (b + y) / 2 - 34 + 5;
		double dx = -38;
		double dy = -50;

		const int iter = 25;

		std::vector<double> pss2 = { pss[0] + dx, pss[1] + dy };
		for (double i = 1; i < iter; i++)
		{
			double p0;
			double p1;
			std::tie(p0, p1) = bezier(i / iter, pss, 38);
			pss2.push_back(p0 + dx);
			pss2.push_back(p1 + dy);
		}
		pss2.push_back(pss[36] + dx);
		pss2.push_back(pss[37] + dy);

		device.setPosition(mpos0 + dx + offset_x, mpos1 + dy + offset_y);
		mouse_left.setPosition(mpos0 + dx + offset_x, mpos1 + dy + offset_y);
		mouse_right.setPosition(mpos0 + dx + offset_x, mpos1 + dy + offset_y);

		// drawing mouse

		if (is_mouse) {
			window.draw(device);
			for (Json::Value& v : mouse_leftkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					window.draw(mouse_left);
					break;
				}
			for (Json::Value& v : mouse_rightkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					window.draw(mouse_right);
					break;
				}
		}




		// drawing arms
		sf::ConvexShape fill(26);
		for (int i = 0; i < 26; i++)
		{
			fill.setPoint(i, sf::Vector2f(pss2[i * 2], pss2[i * 2 + 1]));
		}
		fill.setTexture(&atextex);
		window.draw(fill);

		// drawing first arm arc
		int shad = 77;
		sf::VertexArray edge(sf::TriangleStrip, 52);
		double width = 7;
		sf::CircleShape circ(width / 2);
		circ.setFillColor(sf::Color(red_value, green_value, blue_value, shad));
		circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);




		window.draw(circ);
		for (int i = 0; i < 50; i += 2)
		{
			double vec0 = pss2[i] - pss2[i + 2];
			double vec1 = pss2[i + 1] - pss2[i + 3];
			double dist = hypot(vec0, vec1);
			edge[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
			edge[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
			edge[i].color = sf::Color(0, 0, 0, shad);
			edge[i + 1].color = sf::Color(0, 0, 0, shad);
			width -= 0.08;
		}
		double vec0 = pss2[50] - pss2[48];
		double vec1 = pss2[51] - pss2[49];
		dist = hypot(vec0, vec1);
		edge[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
		edge[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
		edge[50].color = sf::Color(red_value, green_value, blue_value, shad);
		edge[51].color = sf::Color(red_value, green_value, blue_value, shad);
		window.draw(edge);
		circ.setRadius(width / 2);
		circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
		window.draw(circ);

		// drawing second arm arc
		sf::VertexArray edge2(sf::TriangleStrip, 52);
		width = 6;
		sf::CircleShape circ2(width / 2);
		circ2.setFillColor(sf::Color(red_value, green_value, blue_value));
		circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
		window.draw(circ2);
		for (int i = 0; i < 50; i += 2)
		{
			vec0 = pss2[i] - pss2[i + 2];
			vec1 = pss2[i + 1] - pss2[i + 3];
			double dist = hypot(vec0, vec1);
			edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
			edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
			edge2[i].color = sf::Color(red_value, green_value, blue_value);
			edge2[i + 1].color = sf::Color(red_value, green_value, blue_value);
			width -= 0.08;
		}
		vec0 = pss2[50] - pss2[48];
		vec1 = pss2[51] - pss2[49];
		dist = hypot(vec0, vec1);
		edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
		edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
		edge2[50].color = sf::Color(red_value, green_value, blue_value);
		edge2[51].color = sf::Color(red_value, green_value, blue_value);
		window.draw(edge2);
		circ2.setRadius(width / 2);
		circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
		window.draw(circ2);




		// drawing emoticon








		if (is_emoticonkeep) {
			if (key_state != -1) {
				window.draw(face_press[key_state]);
			}
			for (int i = 0; i < MAX_KEYS_NUM; i++)
			{
				if (GetKeyState(face[i]) & 0x8000)
					key_state = i;
			}

			for (Json::Value& v : stopemoticon_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					key_state = -1;
					break;
				}
		
		}
		else {
			for (int i = 0; i < MAX_KEYS_NUM; i++)
			{
				if (GetKeyState(face[i]) & 0x8000)
					key_state = i;
			}
			if (key_state != -1) {
				window.draw(face_press[key_state]);
			}
			key_state = -1;
		}


		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			if (GetKeyState(sounds[i]) & 0x8000) {
				sound_state = i;
				sound_play_state[sound_state] = 1;
				if (sound_play_state[sound_state] == 1 && sound_play_state_last[sound_state] == 0) {
					sound[sound_state].play();
				}
			}
			sound_play_state_last[i] = sound_play_state[i];
			sound_play_state[i] = 0;

		}
		sound_state = -1;



		if (!is_mouse) {

			for (Json::Value& v : mouse_leftkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					mouse_state = 1;
					break;
				}
			for (Json::Value& v : mouse_rightkey_value)
				if (GetKeyState(v.asInt()) & 0x8000)
				{
					mouse_state = 2;
					break;
				}
			switch (mouse_state)
			{
			case 1:
				window.draw(mouse_left);
				break;
			case 2:
				window.draw(mouse_right);
				break;
			default:
				window.draw(device);
				break;
			}
			mouse_state = 0;
		}


	}



	DWORD WINAPI ThreadProc1(LPVOID lpParam) {



		while (1) {
			if (GetCursorPos(&point_now))
				if (((point_now.x - point_last.x) * (point_last.x - point_lastlast.x) < 0) || ((point_now.y - point_last.y) * (point_last.y - point_lastlast.y) < 0))
				{
					point_last.x = point_now.x;
					point_last.y = point_now.y;
				}

			point.x += point_now.x - point_last.x;
			point.y += point_now.y - point_last.y;
			point_lastlast.x = point_last.x;
			point_lastlast.y = point_last.y;
			point_last.x = point_now.x;
			point_last.y = point_now.y;
			if (point.x < letter_x)point.x = letter_x;
			if (point.x > (s_width + letter_x))point.x = s_width + letter_x;
			if (point.y < letter_y)point.y = letter_y;
			if (point.y > (s_width + letter_x))point.y = s_height + letter_y;

			Sleep(2);
		}

	}
}; // namespace mouse
