#include "header.hpp"
#include "SFML/Graphics/Texture.hpp"



/*
#define p_left_x 391.
#define p_left_y 219.
#define p_right_x 469.
#define p_right_y 213.
#define p_left_vx -1.
#define p_left_vy 21.
#define p_right_vx -17.
#define p_right_vy 80.
#define hand_v 30.
#define d_arm1 10//分割数
#define d_arm2 10
#define hand_size 20.
#define recposp1x 343//定位矩形
#define recposp1y 241
#define recposp2x 422
#define recposp2y 255
#define recposp3x 334
#define recposp3y 289

#define screenw 1920.
#define screenh 1080.


double p_mouse[2] = { 0 };
double midv[6] = { 0 };//贝塞尔曲线中间点参数集合
double Bx[41] = { 0 };
double By[41] = { 0 };

void gethandpoint(double *);
void getarm();




*/




namespace morekeys
{
	Json::Value mouse_leftkey_value, mouse_rightkey_value, stopemoticon_value;

	int osu_x, osu_y, osu_h, osu_v;
	int offset_x, offset_y;
	int horizontal, vertical;
	double scale;
	bool is_mouse, is_letterbox, is_left_handed, is_emoticonkeep;
	sf::Sprite bg, device, mouse_left, mouse_right,up;

	int key_state = -1;
	int mouse_state = 0;
	int hand_state = 0;
	int face_state = -1;


	int keybord[MAX_KEYS_NUM];
	sf::Sprite keybord_press[MAX_KEYS_NUM];
	int hand[MAX_KEYS_NUM];
	sf::Sprite hand_press[MAX_KEYS_NUM];
	int face[MAX_KEYS_NUM];
	sf::Sprite face_press[MAX_KEYS_NUM];








	sf::Texture atextex;









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






	bool init()
	{

		/*	atex.create(1, 1, sf::Color(254, 196, 120));
			atextex.setRepeated(true);
			atextex.loadFromImage(atex);
			armTex.texture = &atextex;
			*/



		red_value = data::cfg["addition"]["armLineColor"][0].asInt();
		green_value = data::cfg["addition"]["armLineColor"][1].asInt();
		blue_value = data::cfg["addition"]["armLineColor"][2].asInt();



		if (!atextex.loadFromFile("img/morekeys/arm.png"))
		{
			data::error_msg("File not successfully loaded", "Error loading files");
		}




		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			keybord[i]=data::cfg["morekeys"]["keybord"][i].asInt();
			keybord_press[i].setTexture(data::load_texture2("img/morekeys/keybord/" + std::to_string(i) + ".png"));
			hand[i]= data::cfg["morekeys"]["hand"][i].asInt();
			hand_press[i].setTexture(data::load_texture2("img/morekeys/hand/" + std::to_string(i) + ".png"));
			face[i] = data::cfg["morekeys"]["face"][i].asInt();
			face_press[i].setTexture(data::load_texture2("img/morekeys/face/" + std::to_string(i) + ".png"));
		}



			// getting configs
		is_mouse = data::cfg["morekeys"]["mouse"].asBool();
		is_emoticonkeep = data::cfg["addition"]["emoticonKeep"].asBool();
		stopemoticon_value = data::cfg["addition"]["emoticonClear"];


		bool chk[256];
		std::fill(chk, chk + 256, false);
		mouse_leftkey_value = data::cfg["mouse"]["mouse_left"];
		for (Json::Value& v : mouse_leftkey_value)
			chk[v.asInt()] = true;
		mouse_rightkey_value = data::cfg["mouse"]["mouse_right"];
		for (Json::Value& v : mouse_rightkey_value)
			chk[v.asInt()] = true;


		is_letterbox = data::cfg["resolution"]["letterboxing"].asBool();
		osu_x = data::cfg["resolution"]["width"].asInt();
		osu_y = data::cfg["resolution"]["height"].asInt();
		osu_h = data::cfg["resolution"]["horizontalPosition"].asInt();
		osu_v = data::cfg["resolution"]["verticalPosition"].asInt();
		is_left_handed = data::cfg["decoration"]["leftHanded"].asBool();

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

		// importing sprites
		up.setTexture(data::load_texture("img/morekeys/up.png"));

		if (is_mouse)
		{
			bg.setTexture(data::load_texture("img/morekeys/mousebg.png"));
			device.setTexture(data::load_texture("img/morekeys/mouse.png"), true);
			mouse_left.setTexture(data::load_texture("img/morekeys/mouse_left.png"), true);
			mouse_right.setTexture(data::load_texture("img/morekeys/mouse_right.png"), true);
		}
		else
		{
			bg.setTexture(data::load_texture("img/morekeys/tabletbg.png"));
			device.setTexture(data::load_texture("img/morekeys/tablet.png"), true);
			mouse_left.setTexture(data::load_texture("img/morekeys/tablet_left.png"), true);
			mouse_right.setTexture(data::load_texture("img/morekeys/tablet_right.png"), true);
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

		return true;
	}

	void draw()
	{
		// getting device resolution
		double letter_x, letter_y, s_height, s_width;

		HWND handle = GetForegroundWindow();
		if (handle)
		{
			TCHAR w_title[256];
			GetWindowText(handle, w_title, GetWindowTextLength(handle));
			std::string title = w_title;
			if (title.find("osu!") == 0)
			{
				RECT oblong;
				GetWindowRect(handle, &oblong);
				s_height = osu_y * 0.8;
				s_width = s_height * 4 / 3;
				if (!is_letterbox)
				{
					letter_x = oblong.left + ((oblong.right - oblong.left) - s_width) / 2;
					letter_y = oblong.top + osu_y * 0.117;
				}
				else
				{
					double l = (horizontal - osu_x) * (osu_h + 100) / 200.0;
					double r = l + osu_x;
					letter_x = l + ((r - l) - s_width) / 2;
					letter_y = (vertical - osu_y) * (osu_v + 100) / 200.0 + osu_y * 0.117;
				}
			}
			else
			{
				s_width = horizontal;
				s_height = vertical;
				letter_x = 0;
				letter_y = 0;
			}
		}
		else
		{
			s_width = horizontal;
			s_height = vertical;
			letter_x = 0;
			letter_y = 0;
		}
		double x, y;
		POINT point;
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
	/*	sf::VertexArray fill(sf::TriangleStrip, 26);
		for (int i = 0; i < 26; i += 2)
		{
			fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
			fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
		}
		*/




		sf::ConvexShape fill(26); 
		for (int i = 0; i < 26; i++)
		{
			fill.setPoint(i, sf::Vector2f(pss2[i*2], pss2[i*2+1]));
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







		key_state = -1;
		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			if (GetKeyState(keybord[i]) & 0x8000)
			{
				window.draw(keybord_press[i]);
			}
		}

		hand_state=-1;
		for (int i = 0; i < MAX_KEYS_NUM; i++)
		{
			if (GetKeyState(hand[i]) & 0x8000)
				hand_state = i;
			if (GetKeyState(face[i]) & 0x8000)
				face_state = i;
		}
		if (hand_state != -1) {
			window.draw(hand_press[hand_state]);
		}
		else
			window.draw(up);

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
}; // namespace morekeys






