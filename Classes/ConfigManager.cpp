#include "ConfigManager.h"
#include "cocos2d.h"

#include <vector>

USING_NS_CC;

ConfigManager* ConfigManager::instance = NULL;

//////////////////////////////////////////////////////////////////////////
// Split a string to vector
//////////////////////////////////////////////////////////////////////////
extern void split(std::string &s, std::string &delim, std::vector<std::string> *ret);

//////////////////////////////////////////////////////////////////////////
// Get info
//////////////////////////////////////////////////////////////////////////
string getField(vector<string> &info, string field)
{
	for (unsigned int i = 0; i < info.size(); i++)
		if (info[i] == "[" + field + "]")
		{
			while (info[i + 1] == "") i++;
			return info[i + 1];
		}

		return "";
}

ConfigManager::ConfigManager()
{
	songName = "";
	songDifficulty = 3;
	platformOffset = 0;
	screenDelay = 0;
	autoPlay = 0;
	highQuality = 0;

	// Read config info
	char infoFilename[80];
	sprintf(infoFilename, "config.dat");

	string buf = cocos2d::FileUtils::getInstance()->getStringFromFile(infoFilename);
	string delim = "\r\n";
	vector<string> info;
	split(buf, delim, &info);

	songName = getField(info, "SONG");
	songDifficulty = atoi(getField(info, "DIFFICULTY").c_str());
	platformOffset = atoi(getField(info, "PLATFORM_OFFSET").c_str());
	screenDelay = atoi(getField(info, "SCREEN_DELAY").c_str());
	autoPlay = atoi(getField(info, "AUTO_PLAY").c_str());
	highQuality = atoi(getField(info, "HIGH_QUALITY").c_str());
}

ConfigManager* ConfigManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new ConfigManager();
	}

	return instance;
}

void ConfigManager::freeInstance()
{
	if (instance != NULL)
	{
		delete instance;
	}
}

string ConfigManager::getSongName()
{
	return songName;
}

int ConfigManager::getDifficulty()
{
	return songDifficulty;
}

int ConfigManager::getPlatformOffset()
{
	return platformOffset;
}

int ConfigManager::getScreenDelay()
{
	return screenDelay;
}

int ConfigManager::isAutoPlay()
{
	return autoPlay;
}

int ConfigManager::isHighQuality()
{
	return highQuality;
}
