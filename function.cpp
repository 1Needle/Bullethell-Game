#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <math.h>
#include <thread>
#include "class.h"
#include <iostream>
#include <sstream>
using namespace sf;
using namespace std;

#define PI 3.1415927

FloatRect enemy_rect(-42, -42, 84, 84),
enemy_bullet1_rect(-6, -6, 12, 12),
enemy_bullet2_rect(-1, -5, 8, 10),
enemy_bullet3_rect(-6.6, -2.2, 13.2, 13.2),
reimu_rect(-1, -1, 2, 2),
reimu_bullets_rect(-14.25, -18, 30, 37.5),
reimu_bomb_rect(-40, -40, 80, 80),
reimu_subbullets_rect(-30, -30, 60, 60);
vector<bullets> bullet_v;
character reimu(Vector2f(690, 960), 270, 8, reimu_rect, 3, 0);
vector<character> enemy;
vector<animation> animation_v;
vector<float_text> float_text_v;
vector<item> item_v;
Text fps_t,
player_t,
score_t,
bomb_t,
power_t,
float_text_t[24],
graze_t,
test_t,
main_volume_t,
SE_volume_t,
BGM_volume_t;
RectangleShape rect_shape;
bool pause = 1, stop = 0, gameover = 0, invincible = 0, bombing = 0, return_to_menu_warning = 0, quit_game_warning = 0, setting_menu = 0,
save_warning, isMenu = 1;
int sec = 0, reimu_died_frames = 0, bomb_frames = 0, invincible_frames = 0, graze = 0;
short shoot_cd = 0, posture = 0, power = 0, bomb = 3, graze_gap = 0, pause_selection_v = 0, pause_selection_h = 0,
pause_selection_v_limit = 2, pause_selection_h_limit = 0;
long long int score = 0, frames = 0;
RenderTexture draw_t;
Texture background_playfield_t,
background_frame_t,
background_spellcard_t,
bullets_t,
characters_t,
animation_t,
item_t,
hp_bar_t,
pause_black_t,
slide_bar_t,
boss_target_t,
menu_t,
loading_t;
Sprite background_playfield_s,
background_frame_s,
background_spellcard_s,
bullet_s[26],
character_s[2][3],
animation_s[10],
item_s[6],
hp_bar_white,
hp_bar_red,
pause_black_s,
slide_bar_bar_s,
slide_bar_slider_s,
boss_target_s,
menu_s,
loading_s;
Font font1, font2;
Vector2i mouse_coor;
RenderWindow window(VideoMode(1920, 1080), "Bullethell Game", Style::Fullscreen);
stringstream ss;
SoundBuffer sb[10];
Sound se[10];
Music bgm[3];

double angle = 0;
int color = 0;
int bullet_count = 0;
int round_count = 0;
int move_count = 0;
int change_pattern = 0;
int last_score = 0;
int last_power = 0;
int last_graze = 0;
double  main_volume = 100, BGM_volume = 100, SE_volume = 100,
last_main_volume = 0, last_SE_volume = 0, last_BGM_volume = 0;
double loading_frames = 0;

double face(Vector2f coor1, Vector2f coor2)
{
	return atan2f(coor2.y - coor1.y, coor2.x - coor1.x) * (180 / PI);
}
void draw_window(float width, float height)
{
	rect_shape.setSize(Vector2f(width, height));
	rect_shape.setFillColor(Color::White);
	rect_shape.setOrigin(width / 2, height / 2);
	rect_shape.setPosition(Vector2f(690 + isMenu * 270, 545));
	window.draw(rect_shape);
	rect_shape.setSize(Vector2f(width - 20, height - 20));
	rect_shape.setFillColor(Color::Black);
	rect_shape.setOrigin(width / 2 - 10, height / 2 - 10);
	rect_shape.setPosition(690 + isMenu * 270, 545);
	window.draw(rect_shape);
}
void draw_focus(int num)
{
	animation_s[9].setScale(float_text_t[num].getLocalBounds().width / 30 + 2, float_text_t[num].getLocalBounds().height / 30 + 1);
	animation_s[9].setPosition(float_text_t[num].getPosition().x, float_text_t[num].getPosition().y + 10);
	window.draw(animation_s[9]);
}
void reset()
{
	bgm[0].stop();
	float_text_t[7].setCharacterSize(60);
	float_text_t[7].setOrigin(float_text_t[7].getLocalBounds().width / 2, float_text_t[7].getLocalBounds().height / 2);
	float_text_t[7].setPosition(1190, 625);
	float_text_t[9].setCharacterSize(60);
	float_text_t[9].setOrigin(float_text_t[9].getLocalBounds().width / 2, float_text_t[9].getLocalBounds().height / 2);
	float_text_t[9].setPosition(1300, 710);
	for (int i = 11; i < 23; i++)
		float_text_t[i].setPosition(float_text_t[i].getPosition().x + 270, float_text_t[i].getPosition().y);
	main_volume_t.setPosition(830, 400);
	SE_volume_t.setPosition(830, 500);
	BGM_volume_t.setPosition(830, 600);
	while (!enemy.empty())
		enemy.pop_back();
	while (!bullet_v.empty())
		bullet_v.pop_back();
	while (!animation_v.empty())
		animation_v.pop_back();
	while (!float_text_v.empty())
		float_text_v.pop_back();
	while (!item_v.empty())
		item_v.pop_back();
	frames = 0;
	graze = 0;
	score = 0;
	power = 0;
	reimu.hp = 3;
	bomb = 3;
}
void loading_screen()
{
	if (loading_frames == 120)
		loading_s.setColor(Color(255, 255, 255, 255));
	loading_frames--;
	if (loading_frames <= 40)
		loading_s.setColor(Color(255, 255, 255, loading_frames / 40 * 255));
	window.draw(loading_s);
}

void Game::start_up()
{
	window.setFramerateLimit(60);
	fstream settings;
	settings.open("settings.txt", ios::in);
	string buffer;
	settings >> buffer >> buffer >> main_volume
		>>buffer>>buffer>>SE_volume
		>>buffer>>buffer>>BGM_volume;
	settings.close();
	for (auto& se : se)
		se.setVolume(SE_volume * (main_volume / 100));
	for (auto& bgm : bgm)
		bgm.setVolume(BGM_volume * (main_volume / 100));
	srand(time(NULL));
	{
		draw_t.create(16, 9);
		rect_shape.setSize(Vector2f(16, 9));
		rect_shape.setFillColor(Color::Black);
		draw_t.draw(rect_shape);
		draw_t.display();
	}
	background_frame_t.loadFromFile("resources/images/background_frame.png");
	background_playfield_t.loadFromFile("resources/images/background_playfield.png");
	background_spellcard_t.loadFromFile("resources/images/background_spellcard.png");
	bullets_t.loadFromFile("resources/images/bullets.png");
	characters_t.loadFromFile("resources/images/characters.png");
	animation_t.loadFromFile("resources/images/animation.png");
	item_t.loadFromFile("resources/images/items.png");
	hp_bar_t.loadFromFile("resources/images/hp_bar.png");
	slide_bar_t.loadFromFile("resources/images/slide_bar.png");
	boss_target_t.loadFromFile("resources/images/boss_target.png");
	menu_t.loadFromFile("resources/images/menu.png");
	loading_t.loadFromFile("resources/images/loading.png");
	background_frame_s.setTexture(background_frame_t);
	background_playfield_s.setTexture(background_playfield_t);
	background_spellcard_s.setTexture(background_spellcard_t);
	pause_black_t = draw_t.getTexture();
	pause_black_s.setTexture(pause_black_t);
	pause_black_s.setScale(120, 120);
	pause_black_s.setColor(Color(255, 255, 255, 120));
	background_playfield_s.setPosition(90, 30);
	background_spellcard_s.setPosition(90, 30);
	for (int i = 0; i < 3; i++)
	{
		character_s[0][i].setTexture(characters_t);
		character_s[0][i].setTextureRect(IntRect(i * 28, 0, 28, 31));
		character_s[0][i].setOrigin(14, 18);
		character_s[0][i].setScale(3, 3);
	}
	for (int i = 0; i < 3; i++)
	{
		character_s[1][i].setTexture(characters_t);
		character_s[1][i].setTextureRect(IntRect(i * 28, 31, 28, 28));
		character_s[1][i].setScale(3, 3);
		character_s[1][i].setOrigin(14, 14);
	}
	for (int i = 0; i < 26; i++)
		bullet_s[i].setTexture(bullets_t);
	bullet_s[0].setTextureRect(IntRect(0, 0, 20, 25));
	bullet_s[0].setScale(1.5, 1.5);
	for (int i = 0; i < 7; i++)
	{
		bullet_s[i + 1].setTextureRect(IntRect((i + 1) * 20, 0, 20, 20));
		bullet_s[i + 1].setScale(1.5, 1.5);
	}
	bullet_s[8].setTextureRect(IntRect(160, 0, 30, 30));
	bullet_s[8].setScale(2, 2);
	for (int i = 9; i < 12; i++)
	{
		bullet_s[i].setTextureRect(IntRect(100 * (i - 9), 400, 100, 100));
	}
	for (int i = 12; i < 19; i++)
	{
		bullet_s[i].setTextureRect(IntRect((i - 12) * 10, 25, 10, 15));
		bullet_s[i].setScale(2, 2.5);
	}
	for (int i = 19; i < 26; i++)
	{
		bullet_s[i].setTextureRect(IntRect((i - 19) * 10, 40, 10, 12));
		bullet_s[i].setScale(2.2, 2.2);
	}
	for (int i = 0; i < 26; i++)
	{
		bullet_s[i].setOrigin(bullet_s[i].getLocalBounds().width / 2, bullet_s[i].getLocalBounds().height / 2);
	}
	for (int i = 0; i < 10; i++)
	{
		animation_s[i].setTexture(animation_t);
	}
	animation_s[0].setTextureRect(IntRect(0, 0, 20, 30));
	animation_s[0].setScale(2, 2);
	for (int i = 1; i < 4; i++)
	{
		animation_s[i].setTextureRect(IntRect(20 + (i - 1) * 13, 0, 13, 13));
		animation_s[i].setScale(1.5, 1.5);
	}
	for (int i = 4; i < 8; i++)
	{
		animation_s[i].setTextureRect(IntRect(300 * (i - 4), 200, 300, 300));
	}
	animation_s[8].setTextureRect(IntRect(59, 0, 8, 8));
	animation_s[8].setScale(1.5, 1.5);
	animation_s[9].setTextureRect(IntRect(0, 30, 30, 30));
	for (int i = 0; i < 10; i++)
	{
		animation_s[i].setOrigin(animation_s[i].getLocalBounds().width / 2, animation_s[i].getLocalBounds().height / 2);
	}
	for (int i = 0; i < 6; i++)
	{
		item_s[i].setTexture(item_t);
	}
	for (int i = 0; i < 3; i++)
	{
		item_s[i].setTexture(item_t);
		item_s[i].setTextureRect(IntRect(i * 30, 0, 30, 30));
		item_s[i].setOrigin(15, 15);
	}
	for (int i = 4; i < 6; i++)
	{
		item_s[i].setTexture(item_t);
		item_s[i].setTextureRect(IntRect((i - 4) * 30, 30, 30, 30));
		item_s[i].setOrigin(15, 15);
	}
	item_s[2].setTextureRect(IntRect(0, 0, 30, 30));
	item_s[2].setScale(1.3, 1.3);
	item_s[3].setTextureRect(IntRect(60, 0, 30, 30));
	item_s[3].setScale(0.9, 0.9);
	item_s[4].setScale(1.3, 1.3);
	item_s[5].setScale(1.3, 1.3);
	hp_bar_red.setTexture(hp_bar_t);
	hp_bar_red.setTextureRect(IntRect(0, 0, 30, 5));
	hp_bar_red.setScale(5, 2);
	hp_bar_red.setPosition(90, 10);
	hp_bar_white.setTexture(hp_bar_t);
	hp_bar_white.setScale(35, 2);
	hp_bar_white.setTextureRect(IntRect(0, 5, 30, 5));
	hp_bar_white.setPosition(240, 10);
	slide_bar_bar_s.setTexture(slide_bar_t);
	slide_bar_bar_s.setTextureRect(IntRect(0, 10, 200, 60));
	slide_bar_bar_s.setScale(2, 1);
	slide_bar_bar_s.setOrigin(slide_bar_bar_s.getLocalBounds().width / 2, slide_bar_bar_s.getLocalBounds().height / 2);
	slide_bar_slider_s.setTexture(slide_bar_t);
	slide_bar_slider_s.setTextureRect(IntRect(200, 0, 10, 80));
	slide_bar_slider_s.setScale(2, 1);
	slide_bar_slider_s.setOrigin(slide_bar_slider_s.getLocalBounds().width / 2, slide_bar_slider_s.getLocalBounds().height / 2);
	boss_target_s.setTexture(boss_target_t);
	boss_target_s.setOrigin(boss_target_s.getLocalBounds().width / 2, boss_target_s.getLocalBounds().height / 2);
	menu_s.setTexture(menu_t);
	loading_s.setTexture(loading_t);
	font1.loadFromFile("resources/OzCaramel.ttf");
	font2.loadFromFile("resources/I.Ngaan.ttf");
	score_t.setFont(font2);
	score_t.setString("Score : 0");
	score_t.setPosition(1375, 140);
	player_t.setFont(font2);
	player_t.setPosition(1375, 200);
	player_t.setString("Player :");
	bomb_t.setFont(font2);
	bomb_t.setPosition(1375, 240);
	bomb_t.setString("Bomb :");
	fps_t.setFont(font1);
	fps_t.setPosition(1841, 1041);
	power_t.setFont(font2);
	power_t.setPosition(1375, 280);
	power_t.setString("Power : 0");
	float_text_t[0].setString("100");
	float_text_t[1].setString("10000");
	float_text_t[1].setFillColor(Color::Yellow);
	float_text_t[2].setString("5000");
	float_text_t[3].setString("10");
	float_text_t[4].setString(L"POWER UP！");
	float_text_t[4].setCharacterSize(40);
	float_text_t[5].setString(L"暫    停");
	float_text_t[5].setPosition(690, 250);
	float_text_t[5].setCharacterSize(80);
	float_text_t[6].setString(L"返回遊戲");
	float_text_t[6].setPosition(690, 500);
	float_text_t[7].setString(L"設定");
	float_text_t[7].setPosition(1190, 625);
	float_text_t[8].setString(L"返回主畫面");
	float_text_t[8].setPosition(690, 640);
	float_text_t[9].setString(L"退出遊戲");
	float_text_t[9].setPosition(1300, 710);
	float_text_t[10].setString(L"確定要返回主畫面嗎\n目前為止的進度將會消失");
	float_text_t[10].setPosition(690, 490);
	float_text_t[11].setString(L"確定要退出遊戲嗎\n目前為止的進度將會消失");
	float_text_t[11].setPosition(960, 490);
	float_text_t[12].setString(L"是");
	float_text_t[12].setPosition(885, 590);
	float_text_t[13].setString(L"否");
	float_text_t[13].setPosition(1035, 590);
	float_text_t[14].setString(L"設    定");
	float_text_t[14].setCharacterSize(80);
	float_text_t[14].setPosition(960, 250);
	float_text_t[15].setString(L"主音量");
	float_text_t[15].setPosition(670, 400);
	float_text_t[16].setString(L"音效");
	float_text_t[16].setPosition(670, 500);
	float_text_t[17].setString(L"背景音樂");
	float_text_t[17].setPosition(670, 600);
	float_text_t[18].setString(L"確定");
	float_text_t[18].setPosition(860, 800);
	float_text_t[19].setString(L"取消");
	float_text_t[19].setPosition(1060, 800);
	float_text_t[20].setString(L"您剛才所更動的項目尚未儲存\n是否要儲存?");
	float_text_t[20].setPosition(960, 490);
	float_text_t[21].setString(L"儲存變更");
	float_text_t[21].setPosition(850, 590);
	float_text_t[22].setString(L"放棄變更");
	float_text_t[22].setPosition(1070, 590);
	float_text_t[23].setString(L"開始遊戲");
	float_text_t[23].setPosition(1170, 540);
	graze_t.setFont(font2);
	graze_t.setString("Graze : 0");
	graze_t.setPosition(1375, 320);
	test_t.setFont(font2);
	test_t.setPosition(1375, 450);
	main_volume_t.setFont(font2);
	main_volume_t.setCharacterSize(40);
	main_volume_t.setPosition(830, 400);
	main_volume_t.setString("100");
	main_volume_t.setOrigin(main_volume_t.getLocalBounds().width / 2, main_volume_t.getLocalBounds().height / 2);
	ss << main_volume;
	main_volume_t.setString(ss.str());
	ss.clear();
	ss.str("");
	SE_volume_t.setFont(font2);
	SE_volume_t.setCharacterSize(40);
	SE_volume_t.setPosition(830, 500);
	SE_volume_t.setString("100");
	SE_volume_t.setOrigin(SE_volume_t.getLocalBounds().width / 2, SE_volume_t.getLocalBounds().height / 2);
	ss << SE_volume;
	SE_volume_t.setString(ss.str());
	ss.clear();
	ss.str("");
	BGM_volume_t.setFont(font2);
	BGM_volume_t.setCharacterSize(40);
	BGM_volume_t.setPosition(830, 600);
	BGM_volume_t.setString("100");
	BGM_volume_t.setOrigin(BGM_volume_t.getLocalBounds().width / 2, BGM_volume_t.getLocalBounds().height / 2);
	ss << BGM_volume;
	BGM_volume_t.setString(ss.str());
	ss.clear();
	ss.str("");
	for (int i = 0; i < 4; i++)
		float_text_t[i].setCharacterSize(15);
	for (int i = 6; i < 24; i++)
	{
		if (i == 14)
			continue;
		float_text_t[i].setCharacterSize(40);
	}
	float_text_t[7].setCharacterSize(60);
	float_text_t[9].setCharacterSize(60);
	float_text_t[23].setCharacterSize(60);
	for (int i = 0; i < 24; i++)
	{
		float_text_t[i].setFont(font2);
		float_text_t[i].setOrigin(float_text_t[i].getLocalBounds().width / 2, float_text_t[i].getLocalBounds().height / 2);
	}
	sb[0].loadFromFile("resources/sounds/select.wav");
	sb[1].loadFromFile("resources/sounds/selection_change.wav");
	sb[2].loadFromFile("resources/sounds/fire.wav");
	sb[3].loadFromFile("resources/sounds/power_up.wav");
	sb[4].loadFromFile("resources/sounds/reimu_died.wav");
	sb[5].loadFromFile("resources/sounds/enemy_died.wav");
	sb[6].loadFromFile("resources/sounds/bomb1.wav");
	sb[7].loadFromFile("resources/sounds/bomb2.wav");
	sb[8].loadFromFile("resources/sounds/pick_up.wav");
	sb[9].loadFromFile("resources/sounds/graze.wav");
	for (int i = 0; i < 10; i++)
		se[i].setBuffer(sb[i]);
	bgm[0].openFromFile("resources/sounds/stage1.ogg");
	bgm[1].openFromFile("resources/sounds/bossfight.ogg");
	bgm[2].openFromFile("resources/sounds/menu.ogg");
	for (auto& bgm : bgm)
		bgm.setLoop(true);
	bgm[2].play();
}
void Game::process_input()
{
	Event e;
	if (!pause)
		if (reimu_died_frames <= 180)
		{
			if (Keyboard::isKeyPressed(Keyboard::Up) && reimu.coor.y >= 30 + 54)
			{
				reimu.coor.y -= reimu.velocity;
			}
			if (Keyboard::isKeyPressed(Keyboard::Down) && reimu.coor.y <= 1060 - 36)
			{
				reimu.coor.y += reimu.velocity;
			}
			if (Keyboard::isKeyPressed(Keyboard::Left) && reimu.coor.x >= 90 + 42)
			{
				reimu.coor.x -= reimu.velocity;
			}
			if (Keyboard::isKeyPressed(Keyboard::Right) && reimu.coor.x <= 1290 - 39)
			{
				reimu.coor.x += reimu.velocity;
			}
			if (Keyboard::isKeyPressed(Keyboard::Z) && shoot_cd == 0)
			{
				if (power == 100)
				{
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -75, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -105, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -30, 15, 8, reimu_subbullets_rect, 1));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -150, 15, 8, reimu_subbullets_rect, 1));
				}
				else if (power < 100 && power >= 66)
				{
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -105, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -75, 15, 0, reimu_bullets_rect, 2));
				}
				else if (power < 66 && power >= 33)
				{
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x - 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x + 20, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 2));
				}
				else if (power < 33)
				{
					bullet_v.push_back(bullets(Vector2f(reimu.coor.x, reimu.coor.y - 75), -90, 15, 0, reimu_bullets_rect, 3));
				}
				se[2].play();
				shoot_cd = 8;
			}
			if (Keyboard::isKeyPressed(Keyboard::X) && bomb != 0 && bombing == 0)
			{
				invincible = 1;
				invincible_frames = 180;
				bombing = 1;
				bomb--;
			}
		}
	if (window.pollEvent(e))
	{
		if (!pause)
		{
			switch (e.type)
			{
			case Event::KeyPressed:
				switch (e.key.code)
				{
				case Keyboard::Escape:
					pause = 1;
					pause_selection_v = 0;
					pause_selection_v_limit = 3;
					se[0].play();
					break;
				case Keyboard::LShift:
					reimu.velocity = 4;
					break;
				default:
					break;
				}
				break;
			case Event::KeyReleased:
				if (e.key.code == Keyboard::LShift)
					reimu.velocity = 10;
				break;
			case Event::MouseButtonPressed:
				mouse_coor = Mouse::getPosition();
				std::cout << "(" << mouse_coor.x << "," << mouse_coor.y << ")" << endl;
				break;
			case Event::Closed:
				window.close();
				break;
			}
			
		}
		else
		{
			switch (e.type)
			{
			case Event::MouseButtonPressed:
				mouse_coor = Mouse::getPosition();
				std::cout << "(" << mouse_coor.x << "," << mouse_coor.y << ")" << endl;
				break;
			case Event::KeyPressed:
				switch (e.key.code)
				{
				case Keyboard::Up:
					if (--pause_selection_v < 0)
						pause_selection_v = pause_selection_v_limit;
					se[1].play();
					break;
				case Keyboard::Down:
					if (++pause_selection_v > pause_selection_v_limit)
						pause_selection_v = 0;
					se[1].play();
					break;
				case Keyboard::Left:
					if (setting_menu && pause_selection_v != 3 && !save_warning)
					{
						switch (pause_selection_v)
						{
						case 0:
							if (main_volume > 0)
							{
								main_volume--;
								for (auto& se : se)
									se.setVolume(SE_volume * (main_volume / 100));
								for (auto& bgm : bgm)
									bgm.setVolume(BGM_volume * (main_volume / 100));
								ss << main_volume;
								main_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						case 1:
							if (SE_volume > 0)
							{
								SE_volume--;
								for (auto& se : se)
									se.setVolume(SE_volume * (main_volume / 100));
								ss << SE_volume;
								SE_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						case 2:
							if (BGM_volume > 0)
							{
								BGM_volume--;
								for (auto& bgm : bgm)
									bgm.setVolume(BGM_volume * (main_volume / 100));
								ss << BGM_volume;
								BGM_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						default:
							break;
						}
					}
					else if (--pause_selection_h < 0)
						pause_selection_h = pause_selection_h_limit;
					se[1].play();
					break;
				case Keyboard::Right:
					if (setting_menu && pause_selection_v != 3 && !save_warning)
					{
						switch (pause_selection_v)
						{
						case 0:
							if (main_volume < 100)
							{
								main_volume++;
								for (auto& se : se)
									se.setVolume(SE_volume * (main_volume / 100));
								for (auto& bgm : bgm)
									bgm.setVolume(BGM_volume * (main_volume / 100));
								ss << main_volume;
								main_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						case 1:
							if (SE_volume < 100)
							{
								SE_volume++;
								for (auto& se : se)
									se.setVolume(SE_volume * (main_volume / 100));
								ss << SE_volume;
								SE_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						case 2:
							if (BGM_volume < 100)
							{
								BGM_volume++;
								for (auto& bgm : bgm)
									bgm.setVolume(BGM_volume * (main_volume / 100));
								ss << BGM_volume;
								BGM_volume_t.setString(ss.str());
								ss.str("");
								ss.clear();
							}
							break;
						default:
							break;
						}
					}
					else if (++pause_selection_h > pause_selection_h_limit)
						pause_selection_h = 0;
					se[1].play();
					break;
				case Keyboard::Escape:
					if (return_to_menu_warning)
					{
						pause_selection_v = 2;
						pause_selection_v_limit = 3;
						return_to_menu_warning = 0;
					}
					else if (quit_game_warning)
					{
						pause_selection_v = 3;
						pause_selection_v_limit = 3;
						quit_game_warning = 0;
					}
					else if (setting_menu)
					{
						if (last_main_volume != main_volume || last_SE_volume != SE_volume || last_BGM_volume != BGM_volume)
						{
							save_warning = 1;
							pause_selection_h = 0;
							pause_selection_h_limit = 1;
						}
						else
						{
							pause_selection_v = 1;
							pause_selection_v_limit = 3 - isMenu;
							setting_menu = 0;
						}
					}
					else if (isMenu)
					{
						quit_game_warning = 1;
						pause_selection_h = 1;
						pause_selection_h_limit = 1;
					}
					else
					{
						pause = 0;
					}
					se[0].play();
					this_thread::sleep_for(5ms);
					break;
				case Keyboard::Z:
					if (return_to_menu_warning)
					{
						if (pause_selection_h == 1)
						{
							pause_selection_v = 2;
							pause_selection_v_limit = 3;
							return_to_menu_warning = 0;
						}
						else
						{
							loading_frames = 120;
							isMenu = 1;
							bgm[2].play();
							return_to_menu_warning = 0;
							pause_selection_v_limit = 2;
							pause_selection_v = 0;
							reset();
						}
					}
					else if (quit_game_warning)
					{
						if (pause_selection_h == 1)
						{
							pause_selection_v = 3;
							pause_selection_v_limit = 3;
							quit_game_warning = 0;
						}
						else
						{
							window.close();
						}
					}
					else if (setting_menu)
					{
						if (save_warning)
						{
							if (pause_selection_h == 1)
							{
								main_volume = last_main_volume;
								SE_volume = last_SE_volume;
								BGM_volume = last_BGM_volume;
								ss << main_volume;
								main_volume_t.setString(ss.str());
								ss.clear();
								ss.str("");
								ss << SE_volume;
								SE_volume_t.setString(ss.str());
								ss.clear();
								ss.str("");
								ss << BGM_volume;
								BGM_volume_t.setString(ss.str());
								ss.clear();
								ss.str("");
							}
							pause_selection_h = 0;
							pause_selection_h_limit = 0;
							pause_selection_v = 0;
							pause_selection_v_limit = 3 - isMenu;
							save_warning = 0;
							setting_menu = 0;
							fstream f;
							f.open("settings.txt", ios::out);
							f << "main_volume = " << main_volume << endl
								<< "SE_volume = " << SE_volume << endl
								<< "BGM_volume = " << BGM_volume << endl;
							f.close();
						}
						else if (pause_selection_v == 3)
						{
							if (pause_selection_h == 1)
							{
								if (last_main_volume != main_volume || last_SE_volume != SE_volume || last_BGM_volume != BGM_volume)
								{
									save_warning = 1;
									pause_selection_h = 0;
									pause_selection_h_limit = 1;
								}
							}
							if(!save_warning)
							{
								pause_selection_v = 1;
								pause_selection_v_limit = 3 - isMenu;
								setting_menu = 0;
								fstream f;
								f.open("settings.txt", ios::out);
								f << "main_volume = " << main_volume << endl
									<< "SE_volume = " << SE_volume << endl
									<< "BGM_volume = " << BGM_volume << endl;
								f.close();
							}
						}
					}
					else if (isMenu)
					{
						switch (pause_selection_v)
						{
						case 0:
							loading_frames = 120;
							isMenu = 0;
							pause = 0;
							bgm[2].stop();
							float_text_t[7].setCharacterSize(40);
							float_text_t[7].setOrigin(float_text_t[7].getLocalBounds().width / 2, float_text_t[7].getLocalBounds().height / 2);
							float_text_t[7].setPosition(690, 570);
							float_text_t[9].setCharacterSize(40);
							float_text_t[9].setOrigin(float_text_t[9].getLocalBounds().width / 2, float_text_t[9].getLocalBounds().height / 2);
							float_text_t[9].setPosition(690, 710);
							for (int i = 11; i < 23; i++)
								float_text_t[i].setPosition(float_text_t[i].getPosition().x - 270, float_text_t[i].getPosition().y);
							main_volume_t.setPosition(560, 400);
							SE_volume_t.setPosition(560, 500);
							BGM_volume_t.setPosition(560, 600);
							break;
						case 1:
							setting_menu = 1;
							pause_selection_h = 0;
							pause_selection_v = 0;
							pause_selection_h_limit = 1;
							pause_selection_v_limit = 3;
							last_main_volume = main_volume;
							last_SE_volume = SE_volume;
							last_BGM_volume = BGM_volume;
							break;
						case 2:
							quit_game_warning = 1;
							pause_selection_h = 1;
							pause_selection_h_limit = 1;
							break;
						}
					}
					else
					{
						switch (pause_selection_v)
						{
						case 0:
							pause = 0;
							break;
						case 1:
							setting_menu = 1;
							pause_selection_h = 0;
							pause_selection_v = 0;
							pause_selection_h_limit = 1;
							pause_selection_v_limit = 3;
							last_main_volume = main_volume;
							last_SE_volume = SE_volume;
							last_BGM_volume = BGM_volume;
							break;
						case 2:
							return_to_menu_warning = 1;
							pause_selection_h = 1;
							pause_selection_h_limit = 1;
							break;
						case 3:
							quit_game_warning = 1;
							pause_selection_h = 1;
							pause_selection_h_limit = 1;
							break;
						}
						this_thread::sleep_for(5ms);
					}
					se[0].play();
					break;
				}
				break;
			case Event::Closed:
				window.close();
				break;
			default:
				break;
			}
		}
	}
}
void Game::update() 
{
	for (int i = 0; i < enemy.size(); i++)
	{
		if (enemy[0].phase % 2 == 0)
		{
			hp_bar_white.setScale(enemy[0].hp / 800 * 35, 2);
		}
		else
		{
			hp_bar_red.setScale(enemy[0].hp / 500 * 5, 2);
		}
		enemy[i].update();
		if (enemy[i].hp <= 0)
		{
			item_v.push_back(item(enemy[i].coor, 0));
			animation::enemy_died_a(enemy[i]);
			se[5].play();
			enemy.erase(enemy.begin() + i--);
			score += 10000;
			continue;
		}
		if (enemy[i].out_of_screen())
		{
			enemy.erase(enemy.begin() + i--);
			continue;
		}
		if (enemy[i].collision(reimu))
		{
			if (invincible == 0)
			{
				invincible = 1;
				invincible_frames = 300;
				reimu_died_frames = 300;
				reimu.hp--;
			}
		}
	}
	for (int i = 0; i < animation_v.size(); i++)
	{
		animation_v[i].update();
		if (animation_v[i].frames_remain <= 0)
		{
			animation_v.erase(animation_v.begin() + i--);
			continue;
		}
	}
	for (int i = 0; i < bullet_v.size(); i++)
	{
		bullet_v[i].update();
		if (bullet_v[i].out_of_screen() || bullet_v[i].erased == 1)
		{
			if (bullet_v[i].type >= 9 && bullet_v[i].type <= 11)
			{
				animation_v.push_back(animation(bullet_v[i].coor, 120, bullet_v[i].type - 4, 0.1));
				se[7].play();
			}
			bullet_v.erase(bullet_v.begin() + i--);
		}
	}
	if (graze_gap)
	{
		graze_gap--;
	}
	for (int i = 0; i < item_v.size(); i++)
	{
		item_v[i].update();
		if (item_v[i].out_of_screen() || item_v[i].erased == 1)
		{
			item_v.erase(item_v.begin() + i--);
			continue;
		}
	}
	for (int i = 0; i < float_text_v.size(); i++)
	{
		float_text_v[i].update();
		if (float_text_v[i].erased == 1)
		{
			float_text_v.erase(float_text_v.begin() + i--);
		}
	}
	if (reimu.coor.y <= 250)
	{
		for (int i = 0; i < item_v.size(); i++)
		{
			item_v[i].track = 1;
		}
	}
	if (bombing)
	{
		if (bomb_frames == 0)
		{
			bullet_v.push_back(bullets(reimu.coor, -90, 10, 9, reimu_bomb_rect, 10));
			se[6].play();
		}
		else if (bomb_frames == 20)
		{
			bullet_v.push_back(bullets(reimu.coor, -90, 10, 10, reimu_bomb_rect, 10));
			se[6].play();
		}
		else if (bomb_frames == 40)
		{
			bullet_v.push_back(bullets(reimu.coor, -90, 10, 11, reimu_bomb_rect, 10));
			se[6].play();
		}
		bomb_frames++;
		if (bomb_frames == 181)
		{
			bombing = 0;
			bomb_frames = 0;
		}
	}
	if (invincible == 1)
	{
		if (reimu_died_frames > 0)
		{
			reimu_died_frames--;
		}
		if (reimu_died_frames == 180 && !bombing)
		{
			if (reimu.hp == 0)
				gameover = 1;
			else
				reimu.coor = Vector2f(690, 960);
		}
		if (invincible_frames == 0)
		{
			for (int i = 0; i < 3; i++)
				character_s[0][i].setColor(Color(255, 255, 255, 255));
			invincible = 0;
		}
		invincible_frames--;
	}
	frames++;
	this->game_frames++;
	if (last_score != score)
	{
		last_score = score;
		ss << score;
		score_t.setString("Score : " + ss.str());
		ss.clear();
		ss.str("");
	}
	if (last_power != power)
	{
		if ((last_power < 33 && power >= 33) || (last_power < 66 && power >= 66) || (last_power < 100 && power == 100))
		{
			se[3].play();
			float_text_v.push_back(float_text(Vector2f(-200, 400), 4));
		}
		if (power >= 100)
		{
			last_power = 100;
			power = 100;
			power_t.setString("Power : MAX");
		}
		else
		{
			last_power = power;
			ss << power;
			power_t.setString("Power : " + ss.str());
			ss.str("");
			ss.clear();
		}
	}
	if (last_graze != graze)
	{
		last_graze = graze;
		ss << graze;
		graze_t.setString("Graze : " + ss.str());
		ss.clear();
		ss.str("");
	}
	if (frames % 60 == 0)
		sec++;
	if (frames % 10 == 0)
	{
		if (posture == 2)
			posture = 1;
		else
			posture++;
	}
	if (shoot_cd != 0)
		shoot_cd--;
}
void Game::render()
{
	window.clear();
	if (isMenu)
	{
		window.draw(menu_s);
		if(!setting_menu)
			switch (pause_selection_v)
		{
		case 0:
			draw_focus(23);
			break;
		case 1:
			draw_focus(7);
			break;
		case 2:
			draw_focus(9);
			break;
		}
		window.draw(float_text_t[7]);
		window.draw(float_text_t[9]);
		window.draw(float_text_t[23]);
	}
	else
	{
		window.draw(background_playfield_s);
		if (invincible == 0)
		{
			character_s[0][posture].setPosition(reimu.coor);
			window.draw(character_s[0][posture]);
		}
		else if (reimu_died_frames <= 180 && invincible_frames % 20 >= 5)
		{
			character_s[0][posture].setColor(Color(255, 255, 255, 255));
			character_s[0][posture].setPosition(reimu.coor);
			window.draw(character_s[0][posture]);
		}
		else if (reimu_died_frames <= 180 && invincible_frames % 20 < 5)
		{
			character_s[0][posture].setColor(Color(255, 255, 255, 50));
			character_s[0][posture].setPosition(reimu.coor);
			window.draw(character_s[0][posture]);
		}
		for (auto& e : enemy)
		{
			character_s[1][posture].setPosition(e.coor);
			window.draw(character_s[1][posture]);
		}
		for (auto& a : animation_v)
		{
			animation_s[a.type].setColor(Color(255, 255, 255, a.transparency));
			animation_s[a.type].setPosition(a.coor);
			window.draw(animation_s[a.type]);
		}
		for (auto& b : bullet_v)
		{
			bullet_s[b.type].setPosition(b.coor);
			bullet_s[b.type].setRotation(b.orientation);
			window.draw(bullet_s[b.type]);
		}
		for (auto& it : item_v)
		{
			item_s[it.type].setPosition(it.coor);
			window.draw(item_s[it.type]);
		}
		for (auto& f : float_text_v)
		{
			float_text_t[f.type].setPosition(f.coor);
			window.draw(float_text_t[f.type]);
		}
		window.draw(background_frame_s);
		for (int i = 0; i < reimu.hp; i++)
		{
			item_s[4].setPosition(Vector2f(1535 + i * 40, 220));
			window.draw(item_s[4]);
		}
		for (int i = 0; i < bomb; i++)
		{
			item_s[5].setPosition(Vector2f(1515 + i * 40, 260));
			window.draw(item_s[5]);
		}
		if (enemy.size() == 1)
		{
			boss_target_s.setPosition(enemy[0].coor.x, 1070);
		}
		window.draw(boss_target_s);
		window.draw(player_t);
		window.draw(fps_t);
		window.draw(bomb_t);
		window.draw(score_t);
		window.draw(power_t);
		window.draw(graze_t);
		if (enemy.size() == 1)
		{
			if (enemy[0].phase % 2 == 0)
			{
				window.draw(hp_bar_white);
			}
			window.draw(hp_bar_red);
		}
		if (pause)
		{
			window.draw(pause_black_s);
			draw_focus(6 + pause_selection_v);
			for (int i = 5; i < 10; i++)
				window.draw(float_text_t[i]);
		}
	}
	if (return_to_menu_warning || quit_game_warning)
	{
		draw_window(600, 400);
		draw_focus(12 + pause_selection_h);
		if (return_to_menu_warning)
			window.draw(float_text_t[10]);
		else
			window.draw(float_text_t[11]);
		window.draw(float_text_t[12]);
		window.draw(float_text_t[13]);
	}
	if (setting_menu)
	{
		draw_window(800, 700);
		if (!save_warning)
		{
			if (pause_selection_v == 3)
				draw_focus(18 + pause_selection_h);
			else
				draw_focus(15 + pause_selection_v);
		}
		for (int i = 14; i < 20; i++)
		{
			window.draw(float_text_t[i]);
		}
		slide_bar_bar_s.setPosition(800 + isMenu * 270, 410);
		window.draw(slide_bar_bar_s);
		slide_bar_bar_s.setPosition(800 + isMenu * 270, 510);
		window.draw(slide_bar_bar_s);
		slide_bar_bar_s.setPosition(800 + isMenu * 270, 610);
		window.draw(slide_bar_bar_s);
		slide_bar_slider_s.setPosition(600 + isMenu * 270 + main_volume * 4, 410);
		window.draw(slide_bar_slider_s);
		slide_bar_slider_s.setPosition(600 + isMenu * 270 + SE_volume * 4, 510);
		window.draw(slide_bar_slider_s);
		slide_bar_slider_s.setPosition(600 + isMenu * 270 + BGM_volume * 4, 610);
		window.draw(slide_bar_slider_s);
		window.draw(main_volume_t);
		window.draw(SE_volume_t);
		window.draw(BGM_volume_t);
		if (save_warning)
		{
			draw_window(600, 400);
			draw_focus(21 + pause_selection_h);
			for (int i = 0; i < 3; i++)
				window.draw(float_text_t[20 + i]);
		}
	}
	if (loading_frames > 0)
		loading_screen();
	window.display();
}
void Game::printFPS(int fps)
{
	ss << fps;
	fps_t.setString("FPS: " + ss.str());
	ss.clear();
	ss.str("");
}
bool Game::isRunning()
{
	if (window.isOpen())
		return true;
	return false;
}
bool Game::isPaused()
{
	if (pause == 1)
		return 1;
	return 0;
}
bool Game::isMenuOpen()
{
	if (isMenu)
		return 1;
	return 0;
}
bool Game::isLoading()
{
	if (loading_frames == 0)
		return 0;
	return 1;
}
void Game::stage1()
{
	if (pause)
	{
		return;
	}
	if (frames == 60)
	{
		enemy.push_back(character(Vector2f(690, -50), 90, 3, enemy_rect, 800, 1));
		power = 100;
		enemy[0].hp_red = 700;
		enemy[0].destination = Vector2f(690, 250);
		enemy[0].velocity = 3;
		bgm[0].play();
	}
	if (enemy.size() == 1)
	{
		if (enemy[0].phase == 0)
		{
			if (enemy[0].hp > 400)
			{
				if (frames % 2 == 0)
				{
					double x = rand() % 180;
					bullet_v.push_back(bullets(enemy[0].coor, x, 6, 6, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, x + 45, 6, 6, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, x - 45, 6, 6, enemy_bullet1_rect, 1));
				}
			}
			else if (change_pattern == 0)
			{
				angle = -15;
				for (int i = 0; i < bullet_v.size(); i++)
				{
					if (bullet_v[i].type >= 1 && bullet_v[i].type <= 7)
					{
						item_v.push_back(item(bullet_v[i].coor, 3, 1));
						bullet_v.erase(bullet_v.begin() + i--);
					}
				}
				change_pattern++;
			}
			else if (enemy[0].hp > 10)
			{
				if (frames % 50 == 0)
				{
					bullet_v.push_back(bullets(enemy[0].coor, angle + 90, 4.5, 1, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, angle + 70, 4.5, 1, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, angle + 50, 4, 1, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, angle + 110, 4.5, 1, enemy_bullet1_rect, 1));
					bullet_v.push_back(bullets(enemy[0].coor, angle + 130, 4, 1, enemy_bullet1_rect, 1));
					if (angle == 15)
						angle = -15;
					else
						angle += 5;
					int j = bullet_v.size();
					for (int i = 0; i < j; i++)
					{
						if (bullet_v[i].type == 1)
						{
							bullet_v.push_back(bullets(bullet_v[i].coor, bullet_v[i].facing + 20, 3.5, 4, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(bullet_v[i].coor, bullet_v[i].facing - 20, 3.5, 4, enemy_bullet1_rect, 1));
						}
					}
				}
			}
			else if (change_pattern == 1)
			{
				for (int i = 0; i < bullet_v.size(); i++)
				{
					if (bullet_v[i].type >= 1 && bullet_v[i].type <= 7)
					{
						item_v.push_back(item(bullet_v[i].coor, 3, 1));
						bullet_v.erase(bullet_v.begin() + i--);
					}
				}
				change_pattern = 0;
				enemy[0].hp = enemy[0].hp_red;
				enemy[0].phase++;
			}
		}
		if (enemy[0].phase == 1)
		{
			if (enemy[0].hp > 10)
			{
				if (frames % 60 == 0)
				{
					double bullet_change = 0;
					if (frames % 120 == 0)
						bullet_change = 1;
					for (int i = 0; i < 30; i++)
					{
						bullet_v.push_back(bullets(enemy[0].coor, (double)i * 12 - 180 + bullet_change * 6, 2 + bullet_change, 6, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, (double)i * 12 - 180 + 6 + bullet_change * 6, 3 - bullet_change, 7, enemy_bullet1_rect, 1));
					}
					if (frames % 120 == 0)
					{
						for (int i = 0; i < 5; i++)
						{
							bullet_v.push_back(bullets(Vector2f(150, 50), face(Vector2f(150, 50), reimu.coor) - 20 + (double)i * 10, 3, 3, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(Vector2f(1230, 50), face(Vector2f(1230, 50), reimu.coor) - 20 + (double)i * 10, 3, 3, enemy_bullet1_rect, 1));
						}
					}
					if (frames % 60 == 0)
					{
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor), 3, 2, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + 5, 2.7, 2, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - 5, 2.7, 2, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor), 2.4, 2, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + 10, 2.4, 2, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - 10, 2.4, 2, enemy_bullet1_rect, 1));
					}
				}
			}
			else if (change_pattern == 0)
			{
				for (int i = 0; i < bullet_v.size(); i++)
				{
					if (bullet_v[i].type >= 1 && bullet_v[i].type <= 7)
					{
						item_v.push_back(item(bullet_v[i].coor, 3, 1));
						bullet_v.erase(bullet_v.begin() + i--);
					}
				}
				change_pattern++;
			}
		}
	}
	if (enemy.size() == 2)
	{
		if (enemy[0].phase == 0)
		{
			if (enemy[0].hp > 400)
			{
				if (frames % 120 == 0)
				{
					int random = rand();
					for (int i = 0; i < 8; i++)
					{
						bullet_v.push_back(bullets(enemy[0].coor, 90 + i * 45 + random, 3.6, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 86 + i * 45 + random, 3.3, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 83 + i * 45 + random, 3, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 81 + i * 45 + random, 2.7, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 80 + i * 45 + random, 2.4, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 94 + i * 45 + random, 3.3, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 97 + i * 45 + random, 3, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 99 + i * 45 + random, 2.7, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 100 + i * 45 + random, 2.4, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 86 + i * 45 + random, 1.5, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 83 + i * 45 + random, 1.8, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 81 + i * 45 + random, 2.1, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 94 + i * 45 + random, 1.5, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 97 + i * 45 + random, 1.8, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 99 + i * 45 + random, 2.1, 13, enemy_bullet2_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, 90 + i * 45 + random, 1.2, 13, enemy_bullet2_rect, 1));
					}
					for (int i = 0; i < 36; i++)
					{
						bullet_v.push_back(bullets(enemy[0].coor, i * 10 + random, 3.6, 25, enemy_bullet3_rect, 1, 180));
						bullet_v.push_back(bullets(enemy[0].coor, i * 10 + random, 2.4, 25, enemy_bullet3_rect, 1, 180));
						bullet_v.push_back(bullets(enemy[0].coor, i * 10 + random, 1.2, 25, enemy_bullet3_rect, 1, 180));
					}
					if (round_count == 0)
						round_count++;
					else
						round_count = 0;
				}
				if (frames % 5 == 0)
				{
					for (int i = 0; i < 4; i++)
					{
						bullet_v.push_back(bullets(enemy[0].coor, i * 90 + angle, 3, 4, enemy_bullet1_rect, 1));
						bullet_v.push_back(bullets(enemy[0].coor, i * 90 + angle, 2.5, 4, enemy_bullet1_rect, 1));
					}
					angle += 5.5;
					if (angle >= 360)
						angle = 0;
				}
			}
		}
		if (enemy[0].phase == 1)
		{
			if (enemy[0].hp > 400)
			{
				if (enemy[0].velocity == 0)
				{
					if (bullet_count % 4 == 0)
					{
						if (move_count == 0 || move_count == 2)
						{
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 4, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 4, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 3, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 3, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 2, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 2, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 1, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 1, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
						}
						else if (move_count == 3)
						{
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 4, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 3, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 2, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) - ((double)bullet_count / 4 - 3) * 15, 1, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
						}
						else if (move_count == 1)
						{
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 4, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 3, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 2, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
							bullet_v.push_back(bullets(enemy[0].coor, enemy[0].face(reimu.coor) + ((double)bullet_count / 4 - 3) * 15, 1, bullet_count / 4 + 1, enemy_bullet1_rect, 1));
						}
					}
					bullet_count++;
					if (bullet_count == 25)
					{
						if (move_count == 0)
						{
							enemy[0].destination = Vector2f(390, 160);
							enemy[0].velocity = 3;
							move_count++;
						}
						else if (move_count == 1)
						{
							enemy[0].destination = Vector2f(690, 100);
							enemy[0].velocity = 3;
							move_count++;
						}
						else if (move_count == 2)
						{
							enemy[0].destination = Vector2f(990, 160);
							enemy[0].velocity = 3;
							move_count++;
						}
						else if (move_count == 3)
						{
							enemy[0].destination = Vector2f(690, 100);
							enemy[0].velocity = 3;
							move_count = 0;
						}
						bullet_count = 0;
					}
				}
			}
			else if (change_pattern == 0)
			{
				for (int i = 0; i < bullet_v.size(); i++)
				{
					if (bullet_v[i].type >= 1 && bullet_v[i].type <= 7)
					{
						item_v.push_back(item(bullet_v[i].coor, 3, 1));
						bullet_v.erase(bullet_v.begin() + i--);
					}
				}
				this_thread::sleep_for(1s);
				bullet_count = 0;
				change_pattern++;
			}
			else if (enemy[0].hp > 10)
			{
				if (bullet_count < 72 && round_count < 5)
				{
					for (int i = 0; i < 2; i++)
					{
						if (i == 1)
							bullet_v.push_back(bullets(bullet_v.back()));
						else
							bullet_v.push_back(bullets(enemy[0].coor, (double)bullet_count * 5 + (double)round_count * 3, 45 + (double)round_count * 30, 12 + color, enemy_bullet2_rect, 1, 90));
						bullet_v.back().facing += (double)i * 5;
						bullet_v.back().orientation += (double)i * 5;
						bullet_v.back().update();
						bullet_v.back().stop = 180 + bullet_count - bullet_count / 2;
						bullet_v.back().velocity = 7;
						bullet_count++;
					}
				}
				else if (round_count < 5)
				{
					bullet_count = 0;
					round_count++;
				}
				else
				{
					this_thread::sleep_for(0.2s);
					round_count = 0;
					if (color == 6)
						color = 0;
					else
						color++;
				}
			}
		}
	}
}

void bullets::update()
{
	if (this->stop != 0)
	{
		stop--;
		return;
	}
	this->coor.x += cos(this->facing * PI / 180) * this->velocity;
	this->coor.y += sin(this->facing * PI / 180) * this->velocity;
	if (this->facing > 180)
		facing -= 360;
	if (this->facing <= -180)
		facing += 360;
	if (this->type >= 12 && this->type <= 25 && this->start_setting == 0)
	{
		this->start_setting = 1;
		this->orientation = this->facing + 90;
	}
	if (this->type >= 8 && this->type <= 11)
	{
		if (this->type >= 9 && this->type <= 11)
		{
			for (int i = 0; i < bullet_v.size(); i++)
			{
				if (bullet_v[i].type == 0 || (bullet_v[i].type >= 8 && bullet_v[i].type <= 11))
					continue;
				FloatRect rect1 = bullet_s[this->type].getGlobalBounds();
				FloatRect rect2(bullet_v[i].coor.x + bullet_v[i].hitbox.left, bullet_v[i].coor.y + bullet_v[i].hitbox.top, bullet_v[i].hitbox.width, bullet_v[i].hitbox.height);
				if (rect1.intersects(rect2))
				{
					item_v.push_back(item(bullet_v[i].coor, 3, 1));
					bullet_v.erase(bullet_v.begin() + i--);
				}
			}
		}
		this->orientation += 7;
		if (this->orientation >= 360)
			this->orientation -= 360;
		int minimum_distance = 9000;
		this->target = -1;
		for (int i = 0; i < enemy.size(); i++)
		{
			int distance = sqrt(pow(enemy[i].coor.x - this->coor.x, 2) + pow(enemy[i].coor.y - this->coor.y, 2));
			if (this->coor.y < enemy[i].coor.y)
				continue;
			if (minimum_distance > distance)
			{
				minimum_distance = distance;
				this->target = i;
			}
		}
		if (this->target != -1)
		{
			if (this->type == 8)
			{
				int angle = this->facing - this->face(enemy[this->target].coor);
				if (angle <= -3 && angle >= -180)
				{
					this->facing += 3;
				}
				else if (angle > -3 && angle < 3)
				{
					this->facing = this->face(enemy[this->target].coor);
				}
				else
				{
					this->facing -= 3;
				}
			}
			else
			{
				this->facing = this->face(enemy[this->target].coor);
			}
		}
	}
	if (this->type == 0 || (this->type >= 8 && this->type <= 11))
	{
		for (int i = 0;i < enemy.size();i++)
			if (this->collision(enemy[i]))
			{
				enemy[i].hp -= this->damage;
				if (this->type >= 9 && this->type <= 11)
				{
					animation_v.push_back(animation(this->coor, 120, this->type - 4, 0.1));
					se[7].play();
				}
				else
				{
					animation::bullet_hit_a(this->coor);
				}
				this->erased = 1;
				score += 50;
				break;
			}
	}
	else
	{
		if (this->collision(reimu))
		{
			if (invincible == 0)
			{
				animation::reimu_died_a();
				se[4].play();
				for (int j = 0; j < 10; j++)
				{
					item_v.push_back(item(reimu.coor, 0, Vector2f(rand() % 400 + 490, rand() % 400 + 50)));
				}
				power *= 0.6;
				invincible = 1;
				invincible_frames = 300;
				reimu_died_frames = 300;
				reimu.hp--;
			}
			if (invincible == 1 && reimu_died_frames <= 180)
			{
				this->erased = 1;
			}
		}
		if (sqrt(pow(this->coor.x - reimu.coor.x, 2) + pow(this->coor.y - reimu.coor.y, 2)) < 100 && this->grazed == 0)
		{
			this->grazed = 1;
			se[9].play();
			graze++;
			score += 5;
			if (graze_gap == 0)
			{
				animation_v.push_back(animation(reimu.coor, 20, double(rand() % 360 - 180), 8, 3));
				graze_gap = 12;
			}
		}
	}
}
bool bullets::out_of_screen()
{
	FloatRect screen(90, 30, 1200, 1030);
	FloatRect rect(this->coor.x + this->hitbox.left, this->coor.y + this->hitbox.top, this->hitbox.width, this->hitbox.height);
	if (!rect.intersects(screen))
		return 1;
	else
		return 0;
}
bool bullets::collision(character ch)
{
	FloatRect rect1(ch.coor.x + ch.hitbox.left, ch.coor.y + ch.hitbox.top, ch.hitbox.width, ch.hitbox.height),
		rect2(this->coor.x + this->hitbox.left, this->coor.y + this->hitbox.top, this->hitbox.width, this->hitbox.height);
	if (rect1.intersects(rect2))
		return 1;
	else
		return 0;
}
double bullets::face(Vector2f coor)
{
	return atan2f(coor.y - this->coor.y, coor.x - this->coor.x) * (180/PI);
}

void character::update()
{
	if (this->destination != Vector2f(0, 0) && this->destination_switch == 0)
	{
		this->facing = this->face(this->destination);
		this->destination_switch = 1;
		double sum = 0;
		double speed_count = this->velocity;
		while (speed_count > 0)
		{
			sum += speed_count;
			speed_count -= 0.1;
		}
		if (sum > sqrt(pow(this->coor.x - this->destination.x, 2) + pow(this->coor.y - this->destination.y, 2)))
		{
			cout << "out of distance" << endl;
		}
		this->slowdown_distance = sum;
	}
	if (this->destination_switch == 1 && this->slowdown_distance >= sqrt(pow(this->coor.x - this->destination.x, 2) + pow(this->coor.y - this->destination.y, 2)))
	{
		this->velocity -= 0.1;
		if (this->velocity <= 0)
		{
			this->coor = this->destination;
			this->velocity = 0;
			this->destination = Vector2f(0, 0);
			this->destination_switch = 0;
		}
	}
	this->coor.x += cos(this->facing * PI / 180) * this->velocity;
	this->coor.y += sin(this->facing * PI / 180) * this->velocity;
	if (this->facing > 180)
		facing -= 360;
	if (this->facing <= -180)
		facing += 360;
}
bool character::out_of_screen()
{
	FloatRect screen(90, -500, 1200, 1530);
	FloatRect rect(this->coor.x + this->hitbox.left, this->coor.y + this->hitbox.top, this->hitbox.width, this->hitbox.height);
	if (!rect.intersects(screen))
		return 1;
	else
		return 0;
}
bool character::collision(character ch)
{
	FloatRect rect1(ch.coor.x + ch.hitbox.left, ch.coor.y + ch.hitbox.top, ch.hitbox.width, ch.hitbox.height),
		rect2(this->coor.x + this->hitbox.left, this->coor.y + this->hitbox.top, this->hitbox.width, this->hitbox.height);
	if (rect1.intersects(rect2))
		return 1;
	else
		return 0;
}
double character::face(Vector2f coor)
{
	return atan2f(coor.y - this->coor.y, coor.x - this->coor.x) * (180 / PI);
}

void animation::update()
{
	this->coor.x += cos(this->facing * PI / 180) * this->velocity;
	this->coor.y += sin(this->facing * PI / 180) * this->velocity;
	this->frames_remain--;
	this->transparency -= transparency_reduction;
	if (this->facing > 180)
		facing -= 360;
	if (this->facing <= -180)
		facing += 360;
	if (this->type >= 4 && this->type <= 7)
	{
		this->scale += 0.1;
		animation_s[this->type].setScale(this->scale, this->scale);
		FloatRect rect1 = animation_s[this->type].getGlobalBounds();
		int j = 0;
		for (int i = 0; i < bullet_v.size(); i++)
		{
			if (bullet_v[i].type != 0 && !(bullet_v[i].type >= 8 && bullet_v[i].type <= 11))
			{
				FloatRect rect2(bullet_v[i].coor.x + bullet_v[i].hitbox.left, bullet_v[i].coor.y + bullet_v[i].hitbox.top, bullet_v[i].hitbox.width, bullet_v[i].hitbox.height);
				if (rect1.intersects(rect2))
				{
					if (this->type >= 5 && this->type <= 7)
						item_v.push_back(item(bullet_v[i].coor, 3, 1));
					bullet_v.erase(bullet_v.begin() + i--);
					continue;
				}
			}
		}
		if (this->type >= 5 && this->type <= 7)
		{
			for (int i = 0; i < enemy.size(); i++)
			{
				FloatRect rect2(enemy[i].coor.x + enemy[i].hitbox.left, enemy[i].coor.y + enemy[i].hitbox.top, enemy[i].hitbox.width, enemy[i].hitbox.height);
				if (rect1.intersects(rect2))
					enemy[j].hp -= 0.1;
			}
		}
	}
}
void animation::enemy_died_a(character c)
{
	Vector2f &coor = c.coor;
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
	animation_v.push_back(animation(coor, rand() & 360, rand() & 360, 8, rand() % 2 + 1));
}
void animation::reimu_died_a()
{
	for (int j = 0; j < 12; j++)
		animation_v.push_back(animation(reimu.coor, 90, (double)j * 30, j % 3 + 1, 3));
	animation_v.push_back(animation(reimu.coor, 120, 4, 0.1));
}
void animation::bullet_hit_a(Vector2f coor)
{
	animation_v.push_back(animation(Vector2f(coor.x - 16.5, coor.y - 20), 30, -90, 0, 2));
	animation_v.push_back(animation(coor, 45, double(rand() % 360) - 180, 8, 3));
	animation_v.push_back(animation(coor, 45, double(rand() % 360) - 180, 8, 3));
}

void item::update()
{
	if (this->facing == 200)
	{
		this->velocity = 10;
		facing = atan2f(this->destination.y - this->coor.y, this->destination.x - this->coor.x) * (180 / PI);
	}
	else if (this->destination != Vector2f(0, 0))
	{
		bool arrive = 0;
		if (this->facing >= 0 && this->facing < 90)
		{
			if (this->destination.x <= this->coor.x && this->destination.y <= this->coor.y)
				arrive = 1;
		}
		else if (this->facing >= 90 && this->facing <= 180)
		{
			if (this->destination.x >= this->coor.x && this->destination.y <= this->coor.y)
				arrive = 1;
		}
		else if (this->facing < 0 && this->facing >= -90)
		{
			if (this->destination.x <= this->coor.x && this->destination.y >= this->coor.y)
				arrive = 1;
		}
		else if (this->facing < -90 && this->facing >= -180)
		{
			if (this->destination.x >= this->coor.x && this->destination.y >= this->coor.y)
				arrive = 1;
		}
		if (arrive)
		{
			this->destination = Vector2f(0, 0);
			this->velocity = 5;
			this->facing = 270;
		}
	}
	else if (this->track == 1)
	{
		this->velocity = 12;
		this->facing = atan2f(reimu.coor.y - this->coor.y, reimu.coor.x - this->coor.x) * (180 / PI);
	}
	else
	{
		if (this->velocity > -3)
			this->velocity -= 0.05;
	}
	if (reimu_died_frames <= 180)
	{
		if (this->pick_up(reimu.coor))
		{
			se[8].play();
			switch (this->type)
			{
			case 0:
				if (power >= 100)
				{
					float_text_v.push_back(float_text(this->coor, 1));
					score += 10000;
				}
				else
				{
					float_text_v.push_back(float_text(this->coor, 0));
					power++;
					score += 100;
				}
				break;
			case 1:
				float_text_v.push_back(float_text(this->coor, 2));
				score += 5000;
				break;
			case 2:
				if (power >= 100)
				{
					float_text_v.push_back(float_text(this->coor, 1));
					score += 10000;
				}
				else
				{
					float_text_v.push_back(float_text(this->coor, 0));
					power += 10;
					score += 100;
				}
				break;
			case 3:
				float_text_v.push_back(float_text(this->coor, 3));
				score += 10;
				break;
			}
			this->erased = 1;
		}
	}
	this->coor.x += cos(this->facing * PI / 180) * this->velocity;
	this->coor.y += sin(this->facing * PI / 180) * this->velocity;
	if (this->facing > 180)
		facing -= 360;
	if (this->facing <= -180)
		facing += 360;
}
bool item::pick_up(Vector2f coor)
{
	FloatRect rect1(-42 + coor.x, -54 + coor.y, 84, 93);
	FloatRect rect2(-15 + this->coor.x, -15 + this->coor.y, 30, 30);
	if (rect1.intersects(rect2))
		return 1;
	return 0;
}
bool item::out_of_screen()
{
	FloatRect screen(90, -500, 1200, 1560);
	int a = 15;
	if (this->type == 2)
		a += 15 * 1.3;
	FloatRect rect(this->coor.x - a, this->coor.y - a, 2 * a, 2 * a);
	if (!rect.intersects(screen))
		return 1;
	else
		return 0;
}

void float_text::update()
{
	if (this->type == 4)
	{
		this->velocity = 15;
		if (this->coor.x < 690)
		{
			this->coor.x += this->velocity;
		} 
		else if (this->coor.x >= 690)
		{
			this->frames += 1;
			if (this->frames >= 60)
			{
				this->coor.x += this->velocity;
			}
		}
		if (this->coor.x > 1490)
		{
			this->erased = 1;
		}
	}
	else
	{
		if (this->velocity > 0)
		{
			this->coor.y -= this->velocity;
		}
		this->velocity -= 0.1;
	}
	if (this->velocity < -2)
	{
		this->erased = 1;
	}
}