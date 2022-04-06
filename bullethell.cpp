#include "class.h"
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include <sstream>

//左上角座標90,30
//右下角座標1290,1060
//1200*1030

Game game;

void Fps()
{
	while (game.isRunning())
	{
		int fps = game.game_frames;
		this_thread::sleep_for(1000ms);
		game.printFPS(game.game_frames - fps);
	}
}

int main()
{
	game.start_up();
	thread FPS(Fps);
	while (game.isRunning())
	{
		if (!game.isLoading())
		{
			game.process_input();
			if (!game.isPaused())
			{
				game.stage1();
				game.update();
			}
		}
		game.render();
	}
	FPS.join();
}