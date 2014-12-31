#pragma once

#include <queue>
#include <vector>
#include <string>

#include "PlayMusicScene.h"

using namespace std;

class THit
{
public:
	int BPM;
	int OFFSET;
	int musicBarIndex;
	double touchTime;
	bool displayed;
	bool isLastHitOfBar;
	bool blockHited[4][4];
	bool block[4][4];

	// Constructor
	THit();
};

class MemoManager
{
public:
	static MemoManager *getInstance();
	static void freeInstance();

	void readMemo(const char *FILENAME, int difficulty);
	void playMemo(int, int, int, PlayMusicLayer*);
	int judgeTouch(int r, int c, int userTouchTime, int lastFrameTime, int &deltaTime, int &correctTouchTime);
	int getMusicBarHeight(int index);
	int getTotalCombo();
	int getCurrentScore();
	int getCurrentBPM();
	int getSongLength();
	int getPlayPosition();
	void setPlayPosition(int length);

	double getCurrentBonusTimes();
	void updateBonusTimes(int judgeRes);

	int getSongLevel();
	int getSongDifficulty();
	string getSongArtist();
	string getSongName();

	int getPlatformOffset();

private:
	static MemoManager *instance;

	// Song info
	int m_BPM;
	int m_OFFSET;
	int m_platformOFFSET;
	int m_songLength;
	int m_playPosition;
	int m_totalCombos;
	int m_currentBPM;
	double m_currentScore;
	double m_currentBonusTimes;
	double m_comboScore;
	string m_songName;
	string m_songArtist;
	int m_songLevel;
	int m_songDifficulty;	

	// For memo reading
	int pMemoBlock; 
	vector<string> blockLines;
	vector<string> beatLines;
	void initMemo();
	THit getNextHit(const char, const double, const int);

	// For music bar color updating
	// int pMusicBar;
	int musicBarHeight[MAX_MUSIC_BARS];
	int musicBarStatPerfects[MAX_MUSIC_BARS];
	int musicBarStatCombos[MAX_MUSIC_BARS];

	// For memo playing & display note
	int pHitIndex;
	int pAutoHitIndex;
	vector<THit> hitList;

	// For hit judging
	int pJudgeHitIndexFrom;
	int pJudgeHitIndexTo;
	int judgeResult(int deltaTime);

	MemoManager();
	~MemoManager();
};