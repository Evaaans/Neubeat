#pragma once

#include <string>

using namespace std;

class ConfigManager
{
public:
	static ConfigManager *getInstance();
	static void freeInstance();

	string getSongName();
	int getDifficulty();
	int getPlatformOffset();
	int getScreenDelay();
	int isAutoPlay();
	int isHighQuality();

private:
	static ConfigManager *instance;
	ConfigManager();

	string songName;
	int songDifficulty;
	int platformOffset;
	int screenDelay;
	int autoPlay;
	int highQuality;
};