#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include <time.h>
#include <windows.h>
#include <math.h>

#include <SFML/Graphics.hpp>
#include <json/value.h>
#include <json/reader.h>

std::tuple<double, double> bezier(double ratio, std::vector<double> &points, int length) {
	double fact[22] = {0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0};
	int nn = (length / 2) - 1;
	double xx = 0;
	double yy = 0;

	for (int point = 0; point <= nn; point++) {
		double tmp = fact[nn] / (fact[point] * fact[nn - point]) * pow(ratio, point) * pow(1 - ratio, nn - point);
		xx += points[2 * point] * tmp;
		yy += points[2 * point + 1] * tmp;
	}

	return std::make_tuple(xx / 1000, yy / 1000);
}

void GetDesktopResolution(int &horizontal, int &vertical) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void loadTexture(sf::Texture &tex, std::string path) {
	if (!tex.loadFromFile(path)) {
		// error here
		// exit(1);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	sf::RenderWindow window(sf::VideoMode(612, 352), "\"ESC\" for Mouse |  Set Keys/Letterboxing etc. in \"config.json\"", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(240);

	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	// loading configs

	std::ifstream cfgFile("config.json", std::ifstream::binary);
	std::string cfgString((std::istreambuf_iterator<char>(cfgFile)), std::istreambuf_iterator<char>());
	Json::Reader cfgReader;
	Json::Value cfg;
	if (!cfgReader.parse(cfgString, cfg)) {
		std::cout << "Error reading the config\n";
		return 0;
	}
	cfgFile.close();

	bool isMouse = cfg["osu"]["mouse"].asBool();

	// here leftKeyValue and rightKeyValue holds value for the virtual key-codes
	// http://nehe.gamedevice.net/article/msdn_virtualkey_codes/15009/ for reference
	int leftKeyValue = cfg["osu"]["key1"].asInt();
	int rightKeyValue = cfg["osu"]["key2"].asInt();

	bool isLetterbox = cfg["resolution"]["letterboxing"].asBool();
	double osuX = cfg["resolution"]["width"].asInt();
	double osuY = cfg["resolution"]["height"].asInt();
	double osuH = cfg["resolution"]["horizontalPosition"].asInt();
	double osuV = cfg["resolution"]["verticalPosition"].asInt();

	int redValue = cfg["decoration"]["red"].asInt();
	int greenValue = cfg["decoration"]["green"].asInt();
	int blueValue = cfg["decoration"]["blue"].asInt();
	bool isLeftHanded = cfg["decoration"]["leftHanded"].asBool();
	double mouseDX = cfg["decoration"]["mouseXOffset"].asInt();
	double mouseDY = cfg["decoration"]["mouseYOffset"].asInt();
	double mouseScale = cfg["decoration"]["mouseScalar"].asInt();
	double tabletDX = cfg["decoration"]["tabletXOffset"].asInt();
	double tabletDY = cfg["decoration"]["tabletYOffset"].asInt();
	double tabletScale = cfg["decoration"]["tabletScalar"].asInt();

	bool isSwitch = false;
	int keyState = 0;
	bool leftKeyState = false;
	bool rightKeyState = false;
	double timerLeftKey = -1;
	double timerRightKey = -1;

	// loading textures

	sf::Texture bgTex;
	if (isMouse) {
		loadTexture(bgTex, "img/osu/mousebg.png");
	}
	else {
	double timerLeftKey = -1;
	double timerRightKey = -1;
		loadTexture(bgTex, "img/osu/tabletbg.png");
	}
	sf::Sprite bg(bgTex);

	sf::Texture upTex;
	loadTexture(upTex, "img/osu/up.png");
	sf::Sprite up(upTex);

	sf::Texture leftTex;
	loadTexture(leftTex, "img/osu/left.png");
	sf::Sprite left(leftTex);

	sf::Texture rightTex;
	loadTexture(rightTex, "img/osu/right.png");
	sf::Sprite right(rightTex);

	sf::Texture deviceTex;
	if (isMouse) {
		loadTexture(deviceTex, "img/osu/mouse.png");
	}
	else {
		loadTexture(deviceTex, "img/osu/tablet.png");
	}
	sf::Sprite device(deviceTex);

	if (isMouse) {
		device.setScale(mouseScale, mouseScale);
	}
	else {
		device.setScale(tabletScale, tabletScale);
	}

	while (window.isOpen())	{
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		double letterX;
		double letterY;
		double sWidth;
		double sHeight;

		bool isBongo;

		// getting resolution

		HWND handle = GetForegroundWindow();
		if (handle) {
			RECT oblong;
			TCHAR WTitle[256];

			GetWindowRect(handle, &oblong);
			GetWindowText(handle, WTitle, GetWindowTextLength(handle));
			std::string test = WTitle; //and convert to string.
			std::size_t index = test.find("osu!");
			if (index < 300) {
				if (!isLetterbox) {
					sHeight = (oblong.bottom - oblong.top) * 0.8;
					sWidth = sHeight * 4 / 3;
					letterX = oblong.left + ((oblong.right - oblong.left) - sWidth) / 2;
					letterY = oblong.top + sHeight / 0.8 * 0.117;
				}
				else {
					sHeight = osuY * 0.8;
					sWidth = sHeight * 4 / 3;
					double lll = (horizontal - osuX) * (osuH + 100) / 200; ////109
					double rrr = lll + osuX;								  ////1389
					letterX = lll + ((rrr - lll) - sWidth) / 2;
					letterY = (vertical - osuY) * (osuV + 100) / 200 + sHeight / 0.8 * 0.117;
				}
			}
			else {
				sWidth = horizontal;
				sHeight = vertical;
				letterX = 0;
				letterY = 0;
			}
			index = test.find("Set Keys/Letterboxing et");
			if (index < 300) {
				isBongo = true;
			}
			else {
				isBongo = false;
			}

			//std::cout << oblong.left;
		}
		else {
			isBongo = false;
			sWidth = horizontal;
			sHeight = vertical;
			letterX = 0;
			letterY = 0;
		}

		double x;
		double y;
		POINT point;
		if (GetCursorPos(&point)) {
			double fx = ((double)point.x - letterX) / sWidth;
			if (isLeftHanded) {
				fx = 1 - fx;
			}
			double fy = ((double)point.y - letterY) / sHeight;
			fx = std::min(fx, 1.0);
			fx = std::max(fx, 0.0);
			fy = std::min(fy, 1.0);
			fy = std::max(fy, 0.0);
			x = -97 * fx + 44 * fy + 184;
			y = -76 * fx - 40 * fy + 324;
		}

		window.clear(sf::Color(redValue, greenValue, blueValue));
		window.draw(bg);

		// ESCAPE for switching device
		// A few suggestion here, instead of switching device, we can implement so that ESCAPE means reloading the config.json file
		// Which is better for later on since I'm planning to support other game modes if you don't bother?
		if ((GetKeyState(VK_ESCAPE) & 0x8000) && isBongo) {
			if (!isSwitch) {
				isMouse = !isMouse;
				if (isMouse) {
					loadTexture(bgTex, "img/osu/mousebg.png");
					loadTexture(deviceTex, "img/osu/mouse.png");
					device.setScale(mouseScale, mouseScale);
				}
				else {
					loadTexture(bgTex, "img/osu/tabletbg.png");
					loadTexture(deviceTex, "img/osu/tablet.png");
					device.setScale(tabletScale, tabletScale);
				}
			}
			isSwitch = true;
		}
		else {
			isSwitch = false;
		}
	
		// drawing keypresses
		if (GetKeyState(leftKeyValue) & 0x8000) {
			if (!leftKeyState) {
				keyState = 1;
				leftKeyState = true;
			}
		}
		else {
			leftKeyState = false;
		}

		if (GetKeyState(rightKeyValue) & 0x8000) {
			if (!rightKeyState) {
				keyState = 2;
				rightKeyState = true;
			}
		}
		else {
			rightKeyState = false;
		}
		if (!leftKeyState && !rightKeyState) {
			keyState = 0;
			window.draw(up);
		}
		if (keyState == 1) {
			if (clock() - 31 > timerRightKey) {
				window.draw(left);
				timerLeftKey = clock();
			}
			else {
				window.draw(up);
			}
		}
		else if (keyState == 2) {
			if (clock() - 31 > timerLeftKey) {
				window.draw(right);
				timerRightKey = clock();
			}
			else {
				window.draw(up);
			}
		}

		// initializing pss and pss2
		int oof = 6;
		std::vector<double> pss = {211.0, 159.0};
		double dist = hypot(211 - x, 159 - y);
		double centreleft0 = 211 - 0.7237 * dist / 2;
		double centreleft1 = 159 + 0.69 * dist / 2;
		for (double i = 1; i < oof; i++) {
			double p0;
			double p1;
			std::vector<double> bez = {211, 159, centreleft0, centreleft1, x, y};
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
		for (double i = 1; i < oof; i++) {
			double p0;
			double p1;
			std::vector<double> bez = {x, y, x + s, y + t, a + s2, b + t2, a, b};
			std::tie(p0, p1) = bezier(i / oof, bez, 8);
			pss.push_back(p0);
			pss.push_back(p1);
		}
		pss.push_back(a);
		pss.push_back(b);
		for (double i = oof - 1; i > 0; i--) {
			double p0;
			double p1;
			std::vector<double> bez = {1.0 * a1, 1.0 * a2, centreright0, centreright1, a, b};
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

		std::vector<double> pss2 = {pss[0] + dx, pss[1] + dy};
		for (double i = 1; i < iter; i++) {
			double p0;
			double p1;
			std::tie(p0, p1) = bezier(i / iter, pss, 38);
			pss2.push_back(p0 + dx);
			pss2.push_back(p1 + dy);
		}
		pss2.push_back(pss[36] + dx);
		pss2.push_back(pss[37] + dy);

		if (isMouse) {
			device.setPosition(mpos0 + dx + mouseDX, mpos1 + dy + mouseDY);
		}
		else {
			device.setPosition(mpos0 + dx + tabletDX, mpos1 + dy + tabletDY);
		}

		// drawing mouse
		if (isMouse) {
			window.draw(device);
		}

		// drawing arms
		sf::VertexArray fill(sf::TriangleStrip, 26);
		for (int i = 0; i < 26; i += 2) {
			fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
			fill[i].color = sf::Color(redValue, greenValue, blueValue);
			fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
			fill[i + 1].color = sf::Color(redValue, greenValue, blueValue);
		}
		window.draw(fill);

		// drawing circ
		int shad = 77;
		sf::VertexArray edge(sf::TriangleStrip, 52);
		double width = 7;
		sf::CircleShape circ(width / 2);
		circ.setFillColor(sf::Color(0, 0, 0, shad));
		circ.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
		window.draw(circ);
		for (int i = 0; i < 50; i += 2) {
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
		edge[50].color = sf::Color(0, 0, 0, shad);
		edge[51].color = sf::Color(0, 0, 0, shad);
		window.draw(edge);
		circ.setRadius(width / 2);
		circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
		window.draw(circ);

		// drawing circ2
		sf::VertexArray edge2(sf::TriangleStrip, 52);
		width = 6;
		sf::CircleShape circ2(width / 2);
		circ2.setFillColor(sf::Color::Black);
		circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
		window.draw(circ2);
		for (int i = 0; i < 50; i += 2)	{
			vec0 = pss2[i] - pss2[i + 2];
			vec1 = pss2[i + 1] - pss2[i + 3];
			double dist = hypot(vec0, vec1);
			edge2[i].position = sf::Vector2f(pss2[i] + vec1 / dist * width / 2, pss2[i + 1] - vec0 / dist * width / 2);
			edge2[i + 1].position = sf::Vector2f(pss2[i] - vec1 / dist * width / 2, pss2[i + 1] + vec0 / dist * width / 2);
			edge2[i].color = sf::Color::Black;
			edge2[i + 1].color = sf::Color::Black;
			width -= 0.08;
		}
		vec0 = pss2[50] - pss2[48];
		vec1 = pss2[51] - pss2[49];
		dist = hypot(vec0, vec1);
		edge2[51].position = sf::Vector2f(pss2[50] + vec1 / dist * width / 2, pss2[51] - vec0 / dist * width / 2);
		edge2[50].position = sf::Vector2f(pss2[50] - vec1 / dist * width / 2, pss2[51] + vec0 / dist * width / 2);
		edge2[50].color = sf::Color::Black;
		edge2[51].color = sf::Color::Black;
		window.draw(edge2);
		circ2.setRadius(width / 2);
		circ2.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
		window.draw(circ2);

		// drawing tablet
		if (!isMouse) {
			window.draw(device);
		}

		//	window.draw(message);

		window.display();
	}

	return 0;
}