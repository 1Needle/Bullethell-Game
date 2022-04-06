#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;
using namespace sf;

class Game {
public:
	int times = 0;
	long long int game_frames = 0;
	void printFPS(int);
	void start_up();
	void process_input();
	void render();
	void update();
	bool isRunning();
	bool isPaused();
	bool isMenuOpen();
	bool isLoading();
	void stage1();
};

class character {
public:
	Vector2f coor;
	Vector2f destination = Vector2f(0, 0);
	bool destination_switch = 0;
	double slowdown_distance = 0;
	int phase = 0;
	double facing;
	double velocity;
	FloatRect hitbox;
	double hp;
	double hp_red = 0;
	short drop_type;

	character(Vector2f coor, double facing, double velocity, FloatRect hitbox, double hp, short drop_type) :
		coor(coor), facing(facing), velocity(velocity), hitbox(hitbox), hp(hp), drop_type(drop_type) {};
	
	void update();
	bool out_of_screen();
	bool collision(character);
	double face(Vector2f);
};

class bullets {
public:
	Vector2f coor;
	double facing, velocity, orientation, damage;
	short type;
	FloatRect hitbox;
	int frames = 0, phase = 0, target = -1, stop = 0;
	bool grazed = 0, start_setting = 0, erased = 0;

	bullets(Vector2f coor, double facing, double velocity, int type, FloatRect hitbox, double damage) :
		coor(coor), facing(facing), velocity(velocity), type(type), hitbox(hitbox), damage(damage), orientation(-90 + facing) {};
	bullets(Vector2f coor, double facing, double velocity, int type, FloatRect hitbox, double damage , double orientation) :
		coor(coor), facing(facing), velocity(velocity), type(type), hitbox(hitbox), damage(damage), orientation (orientation) {};

	void update();
	bool out_of_screen();
	bool collision(character);
	double face(Vector2f);
};

class animation {
public:
	short frames_remain;
	double transparency = 255;
	short type;
	double facing;
	double velocity;
	double scale = 1;
	short frames_total = frames_remain;
	double transparency_reduction = 255 / frames_total;

	animation(Vector2f coor, short frames_remain, double facing, short type, double velocity) :
		coor(coor), frames_remain(frames_remain), facing(facing), type(type), velocity(velocity) {};
	animation(Vector2f coor, short frames_remain, short type, double scale) :
		coor(coor), frames_remain(frames_remain), facing(0), type(type), velocity(0), scale(scale) {};
	Vector2f coor;
	
	void update();
	static void enemy_died_a(character);
	static void reimu_died_a();
	static void bullet_hit_a(Vector2f);
};

class item {
public:
	item(Vector2f coor, short type) :
		coor(coor), type(type), facing(-90), destination(Vector2f(0, 0)), track(0) {};
	item(Vector2f coor, short type, bool track) :
		coor(coor), type(type), facing(-90), destination(Vector2f(0, 0)), track(track) {};
	item(Vector2f coor, short type, Vector2f destination) :
		coor(coor), type(type), facing(200), destination(destination), track(0) {};
	Vector2f coor;
	Vector2f destination;
	short type;
	double facing;
	double velocity = 5;
	bool track;
	bool erased = 0;

	void update();
	bool pick_up(Vector2f);
	bool out_of_screen();
};

class float_text
{
public:
	float_text(Vector2f coor, short type) :
		coor(coor), type(type) {};
	Vector2f coor;
	short type;
	double velocity = 3;
	int frames = 0;
	bool erased = 0;

	void update();
};