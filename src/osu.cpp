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
namespace osu
{
Json::Value left_key_value, right_key_value, smoke_key_value, wave_key_value, key3_value, key4_value;
int osu_x, osu_y, osu_h, osu_v;
int offset_x, offset_y;
int horizontal, vertical;
double scale;
bool is_mouse, is_letterbox, is_left_handed;
sf::Sprite bg, up, left, right, device, smoke, wave,key3,key4;

int key_state = 0;
bool left_key_state = false;
bool right_key_state = false;
bool wave_key_state = false;

bool key3_state = false;
bool key4_state = false;


double timer_left_key = -1;
double timer_right_key = -1;
double timer_wave_key = -1;

double timeer_key3_key = -1;
double timeer_key4_key = -1;


sf::Image atex;



sf::RenderStates armTex;
sf::Texture atextex;






std::tuple<double, double> bezier(double ratio, std::vector<double> &points, int length)
{
    double fact[22] = {0.001, 0.001, 0.002, 0.006, 0.024, 0.12, 0.72, 5.04, 40.32, 362.88, 3628.8, 39916.8, 479001.6, 6227020.8, 87178291.2, 1307674368.0, 20922789888.0, 355687428096.0, 6402373705728.0, 121645100408832.0, 2432902008176640.0, 51090942171709440.0};
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










bool init()
{

/*	atex.create(1, 1, sf::Color(254, 196, 120));
	atextex.setRepeated(true);
	atextex.loadFromImage(atex);
	armTex.texture = &atextex;
	*/








    // getting configs
    is_mouse = data::cfg["osu"]["mouse"].asBool();

    bool chk[256];
    std::fill(chk, chk + 256, false);


    left_key_value = data::cfg["osu"]["key1"];
    for (Json::Value &v : left_key_value)
        chk[v.asInt()] = true;
    right_key_value = data::cfg["osu"]["key2"];
    for (Json::Value &v : right_key_value)
        if (chk[v.asInt()])
        {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }
    wave_key_value = data::cfg["osu"]["wave"];
    for (Json::Value &v : wave_key_value)
        if (chk[v.asInt()])
        {
            data::error_msg("Overlapping osu! keybinds", "Error reading configs");
            return false;
        }





	key3_value = data::cfg["osu"]["key3"];
	for (Json::Value& v : key3_value)
		if (chk[v.asInt()])
		{
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}
	key4_value = data::cfg["osu"]["key4"];
	for (Json::Value& v : key4_value)
		if (chk[v.asInt()])
		{
			data::error_msg("Overlapping osu! keybinds", "Error reading configs");
			return false;
		}




    smoke_key_value = data::cfg["osu"]["smoke"];

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
    up.setTexture(data::load_texture("img/osu/up.png"));
    left.setTexture(data::load_texture("img/osu/left.png"));
    right.setTexture(data::load_texture("img/osu/right.png"));
    wave.setTexture(data::load_texture("img/osu/wave.png"));



	key3.setTexture(data::load_texture("img/osu/key3.png"));
	key4.setTexture(data::load_texture("img/osu/key4.png"));



    if (is_mouse)
    {
        bg.setTexture(data::load_texture("img/osu/mousebg.png"));
        device.setTexture(data::load_texture("img/osu/mouse.png"), true);
    }
    else
    {
        bg.setTexture(data::load_texture("img/osu/tabletbg.png"));
        device.setTexture(data::load_texture("img/osu/tablet.png"), true);
    }
    smoke.setTexture(data::load_texture("img/osu/smoke.png"));
    device.setScale(scale, scale);

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
    std::vector<double> pss = {211.0, 159.0};
    double dist = hypot(211 - x, 159 - y);
    double centreleft0 = 211 - 0.7237 * dist / 2;
    double centreleft1 = 159 + 0.69 * dist / 2;
    for (double i = 1; i < oof; i++)
    {
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
    for (double i = 1; i < oof; i++)
    {
        double p0;
        double p1;
        std::vector<double> bez = {x, y, x + s, y + t, a + s2, b + t2, a, b};
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

    // drawing mouse
    if (is_mouse)
        window.draw(device);

    // drawing arms
    sf::VertexArray fill(sf::TriangleStrip, 26);
    for (int i = 0; i < 26; i += 2)
    {
        fill[i].position = sf::Vector2f(pss2[i], pss2[i + 1]);
        fill[i + 1].position = sf::Vector2f(pss2[52 - i - 2], pss2[52 - i - 1]);
    }





    window.draw(fill,armTex);

    // drawing first arm arc
    int shad = 77;
    sf::VertexArray edge(sf::TriangleStrip, 52);
    double width = 7;
    sf::CircleShape circ(width / 2);
    circ.setFillColor(sf::Color(0, 0, 0, shad));
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
    edge[50].color = sf::Color(0, 0, 0, shad);
    edge[51].color = sf::Color(0, 0, 0, shad);
    window.draw(edge);
    circ.setRadius(width / 2);
    circ.setPosition(pss2[50] - width / 2, pss2[51] - width / 2);
    window.draw(circ);

    // drawing second arm arc
    sf::VertexArray edge2(sf::TriangleStrip, 52);
    width = 6;
    sf::CircleShape circ2(width / 2);
    circ2.setFillColor(sf::Color::Black);
    circ2.setPosition(pss2[0] - width / 2, pss2[1] - width / 2);
    window.draw(circ2);
    for (int i = 0; i < 50; i += 2)
    {
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
    





    // drawing smoke
    for (Json::Value &v : smoke_key_value)
        if (GetKeyState(v.asInt()) & 0x8000)
        {
            window.draw(smoke);
            break;
        }







    // drawing keypresses
    bool left_key = false;
    for (Json::Value &v : left_key_value)
        if (GetKeyState(v.asInt()) & 0x8000)
        {
            left_key = true;
            break;
        }
    if (left_key)
    {
        if (!left_key_state)
        {
            key_state = 1;
            left_key_state = true;
        }
    }
    else
        left_key_state = false;

    bool right_key = false;
    for (Json::Value &v : right_key_value)
        if (GetKeyState(v.asInt()) & 0x8000)
        {
            right_key = true;
            break;
        }
    if (right_key)
    {
        if (!right_key_state)
        {
            key_state = 2;
            right_key_state = true;
        }
    }
    else
        right_key_state = false;
    
    bool wave_key = false;
    for (Json::Value &v : wave_key_value)
        if (GetKeyState(v.asInt()) & 0x8000)
        {
            wave_key = true;
            break;
        }
    if (wave_key)
    {
        if (!wave_key_state)
        {
            key_state = 3;
            wave_key_state = true;
        }
    }
    else
        wave_key_state = false;



	bool key3_key = false;
	for (Json::Value& v : key3_value)
		if (GetKeyState(v.asInt()) & 0x8000)
		{
			key3_key = true;
			break;
		}
	if (key3_key)
	{
		if (!key3_state)
		{
			key_state = 4;
			key3_state = true;
		}
	}
	else
		key3_state = false;


	bool key4_key = false;
	for (Json::Value& v : key4_value)
		if (GetKeyState(v.asInt()) & 0x8000)
		{
			key4_key = true;
			break;
		}
	if (key4_key)
	{
		if (!key4_state)
		{
			key_state = 5;
			key4_state = true;
		}
	}
	else
		key4_state = false;







    if (!left_key_state && !right_key_state && !wave_key_state && !key3_state && !key4_state)
    {
        key_state = 0;
        window.draw(up);
    }
    if (key_state == 1)
    {
        if ((clock() - max(max(timer_right_key, timer_wave_key), max(timeer_key3_key, timeer_key4_key))) / CLOCKS_PER_SEC > 0.031)
        {
            if (!is_left_handed)
                window.draw(left);
            else
                window.draw(right);
            timer_left_key = clock();
        }
        else
            window.draw(up);
    }





	///////////
    else if (key_state == 2)
    {
        if ((clock() - max(max(timer_left_key, timer_wave_key),max(timeer_key3_key, timeer_key4_key))) / CLOCKS_PER_SEC > 0.031)
        {
            if (!is_left_handed)
                window.draw(right);
            else
                window.draw(left);
            timer_right_key = clock();
        }
        else
            window.draw(up);
    }
    else if (key_state == 3)
    {
        if ((clock() - max(max(timer_left_key, timer_right_key), max(timeer_key3_key, timeer_key4_key))) / CLOCKS_PER_SEC > 0.031)
        {
            window.draw(wave);
            timer_wave_key = clock();
        }
        else
            window.draw(up);
    }
	else if (key_state == 4)
	{
		if ((clock() - max(max(timer_left_key, timer_wave_key), max(timer_right_key, timeer_key4_key))) / CLOCKS_PER_SEC > 0.031)
		{
			window.draw(key3);
			timeer_key3_key = clock();
		}
		else
			window.draw(up);
	}
	else if (key_state == 5)
	{
		if ((clock() - max(max(timer_left_key, timer_wave_key), max(timeer_key3_key, timer_right_key))) / CLOCKS_PER_SEC > 0.031)
		{
			window.draw(key4);
			timeer_key4_key = clock();
		}
		else
			window.draw(up);
	}





	//my test point


	/*
	recposp1x+(recposp2x-recposp1x)*point.x/screenw;//pv1x
	recposp1y + (recposp2y - recposp1y)* point.x / screenw;//pv1y
	recposp1x + (recposp3x - recposp1x)* point.y / screenh;//pv2x
	recposp1y + (recposp3y - recposp1y)* point.y / screenh;//pv2y
	*/


	/*p_mouse[0] = recposp1x + (recposp2x - recposp1x) * point.x / screenw +(recposp3x - recposp1x) * point.y / screenh;
	p_mouse[1]= recposp1y + (recposp2y - recposp1y) * point.x / screenw +(recposp3y - recposp1y) * point.y / screenh;
	gethandpoint(p_mouse);
	getarm();





	sf::ConvexShape convex;
	convex.setPointCount(d_arm1 + d_arm2+1);
	for (int i = 0; i <= d_arm1 + d_arm2; i ++)
	{
		convex.setPoint(i, sf::Vector2f(Bx[i], By[i]));
	}
	sf::ConvexShape convex2;
	convex2.setPointCount(2*(d_arm1 + d_arm2)+2);
	for (int i = 0; i <= d_arm1 + d_arm2; i++)
	{
		convex2.setPoint(i, sf::Vector2f(Bx[i], By[i]));
	}
	for (int i = 0; i <= d_arm1 + d_arm2; i++)
	{
		convex2.setPoint(d_arm1 + d_arm2+i+1, sf::Vector2f(Bx[d_arm1 + d_arm2-i], By[d_arm1 + d_arm2-i]
		));
	}
	convex2.setOutlineThickness(6.f);
	convex2.setOutlineColor(sf::Color(0, 0, 0));
	/*for (int i = 0; i < d_arm1+d_arm2; i+=2)
	{
		myfill[i].position = sf::Vector2f(Bx[i/2], By[i/2]);
		myfill[i+1].position = sf::Vector2f(Bx[d_arm1 + d_arm2-i/2], By[d_arm1 + d_arm2 - i/2]);
	}*/

	/*sf::VertexArray myline(sf::TriangleStrip, d_arm1 + d_arm2);
	for (int i = 0; i <= d_arm1 + d_arm2; i++)
	{
		myline[i] = sf::Vertex(sf::Vector2f(Bx[i], By[i]));
		line[i].setOutlineThickness(10.f);
		line.setOutlineColor(sf::Color(250, 150, 100));
	}
	*/

	/**
	window.draw(convex2);
	convex.setFillColor(sf::Color(254, 196, 120));
	window.draw(convex);


	*/
    // drawing tablet
    if (!is_mouse)
        window.draw(device);
}
}; // namespace osu


/*
void getarm(void)
{
	double a, b, c,t;
	for (int i = 0; i < d_arm1; i++)
	{
		t = i / (double)d_arm1;
		c=b = a = 1 - t;
		a = a * a * a;
		b = b * b * t * 3;
		c = 3 * t * t * c;
		t = t * t * t;
		Bx[i] = a * p_left_x + b * (p_left_x + p_left_vx) + c * midv[2] + t * midv[0];
		By[i] = a * p_left_y + b * (p_left_y + p_left_vy) + c * midv[3] + t * midv[1];
	}
	for (int i = 0; i <= d_arm2; i++)
	{
		t = i / (double)d_arm2;
		c = b = a = 1 - t;
		a = a * a * a;
		b = b * b * t * 3;
		c = 3 * t * t * c;
		t = t * t * t;
		Bx[i+d_arm1] = a * midv[0] + b * midv[4] + c * (p_right_x+ p_right_vx) + t * p_right_x;
		By[i+d_arm1] = a * midv[1] + b * midv[5] + c * (p_right_y + p_right_vy) + t * p_right_y;
	}
}


void gethandpoint(double* x)
{
	double pmidx = (double)0.5 * p_left_x + (double)0.5 * p_right_x;
	double pmidy = (double)0.5 * p_left_y + (double)0.5 * p_right_y;
	double dist = hypot(abs(pmidx-x[0]), abs(pmidy-x[1]));
	double point0x = (x[0]- pmidx) * hand_size / dist+x[0];
	double point0y = ( x[1]-pmidy) * hand_size / dist+x[1];
	double point1x = hand_v * (pmidy - x[1]) / dist + point0x;
	double point1y = hand_v * (x[0] - pmidx) / dist + point0y;
	double point2x = hand_v * (x[1] - pmidy) / dist + point0x;
	double point2y = hand_v * (pmidx - x[0]) / dist + point0y;
	midv[0] = point0x;
	midv[1] = point0y;
	midv[2] = point1x;
	midv[3] = point1y;
	midv[4] = point2x;
	midv[5] = point2y;
}


*/







