#include "MemoManager.h"
#include "ConfigManager.h"
#include "SimpleAudioEngine.h"
#include "Constants.h"
#include "stdlib.h"

#include <iostream>
#include <string>

// #define DEBUG

using namespace std;

MemoManager* MemoManager::instance = NULL;

//////////////////////////////////////////////////////////////////////////
// Constructor & Destructor
//////////////////////////////////////////////////////////////////////////
THit::THit()
{
	BPM = 100;
	OFFSET = 0;
	touchTime = 0;
	musicBarIndex = 0;
	displayed = false;
	isLastHitOfBar = false;
	memset(blockHited, 0, sizeof(blockHited));
	memset(block, 0, sizeof(block));
}

MemoManager::MemoManager()
{
	initMemo();
}

MemoManager::~MemoManager()
{
}

//////////////////////////////////////////////////////////////////////////
// Get singleton
//////////////////////////////////////////////////////////////////////////
MemoManager *MemoManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new MemoManager();
	}

	return instance;
}

void MemoManager::freeInstance()
{
	if (instance != NULL)
	{
		delete instance;
	}
}

void MemoManager::initMemo()
{
	pMemoBlock = -1;
	// pMusicBar = 0;

	m_BPM = 100; // DEFAULT BPM - Must be change in memo, at least on the beginning!
	m_OFFSET = 0;
	m_platformOFFSET = 0;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	m_platformOFFSET = ConfigManager::getInstance()->getPlatformOffset();
#endif

	m_songLength = 0;
	m_playPosition = 0;
	m_totalCombos = 0;
	m_currentScore = 0;
	m_currentBPM = 0;
	m_currentBonusTimes = 0;

	pHitIndex = 0;
	pAutoHitIndex = 0;
	// For judge hit
	pJudgeHitIndexFrom = 0;
	pJudgeHitIndexTo = -1;

	blockLines.clear();
	beatLines.clear();
	hitList.clear();
}

//////////////////////////////////////////////////////////////////////////
// Return next hit by hitNum
//////////////////////////////////////////////////////////////////////////
THit MemoManager::getNextHit(const char hitNum, const double touchTimeMS, int BPM)
{
	// Get block detail
	vector<string> block;
	THit newHit;

	while (1)
	{
		// Get current 4*4 block
		block.clear();
		for (int i = 4 * pMemoBlock; i < 4 * (pMemoBlock + 1); i++)
		{
			block.push_back(blockLines[i]);
		}

		// Get the match
		bool match = false;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (block[i][j] == hitNum)
				{
					m_totalCombos++;
					match = true;
					newHit.block[i][j] = true;
					block[i][j] = 'x';
				}

		if (match) break;
		pMemoBlock++;
	}

	newHit.touchTime = touchTimeMS;
	newHit.BPM = BPM;

	return newHit;
}

//////////////////////////////////////////////////////////////////////////
// Check if a hit note
//////////////////////////////////////////////////////////////////////////
bool isHitNoteChar(char hitNum)
{
	return ((hitNum >= '1' && hitNum <= '9') || (hitNum >= 'a' && hitNum <= 'w')); // Note that 'x' is empty position
}

//////////////////////////////////////////////////////////////////////////
// Get duration between a hit to next
//////////////////////////////////////////////////////////////////////////
double getTimePerUnitMS(int BPM, int beatCount)
{
	double timePerBeatMS = 60.0 * 1000 / BPM;
	return timePerBeatMS / beatCount;
}

//////////////////////////////////////////////////////////////////////////
// Split a string to vector
//////////////////////////////////////////////////////////////////////////
void split(std::string &s, std::string &delim, std::vector<std::string> *ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);

	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}

	if (index - last > 0)
	{
		ret->push_back(s.substr(last, index - last));
	}
} 

//////////////////////////////////////////////////////////////////////////
// Get info
//////////////////////////////////////////////////////////////////////////
string getSongInfoField(vector<string> &info, string field)
{
	for (unsigned int i = 0; i < info.size(); i++)
		if (info[i] == "[" + field + "]")
		{
			while (info[i + 1] == "") i++;
			return info[i + 1];
		}
	
	return "";
}

//////////////////////////////////////////////////////////////////////////
// Read memo text file
//////////////////////////////////////////////////////////////////////////
void MemoManager::readMemo(const char *songName, int difficulty)
{
	initMemo();
	
	// Read song info
	vector<string> info;
	char infoFilename[80];
	sprintf(infoFilename, "songs/%s_info.txt", songName);
	string buf = cocos2d::FileUtils::getInstance()->getStringFromFile(infoFilename);
	string delim = "\r\n";
	split(buf, delim, &info);

	m_songName = getSongInfoField(info, "NAME");
	m_songArtist = getSongInfoField(info, "ARTIST");
	m_songDifficulty = difficulty;
	if (m_songDifficulty == DIFFICULTY_BASIC) m_songLevel = atoi(getSongInfoField(info, "BASIC").c_str());
	else if (m_songDifficulty == DIFFICULTY_ADVANCED) m_songLevel = atoi(getSongInfoField(info, "ADVANCED").c_str());
	else m_songLevel = atoi(getSongInfoField(info, "EXTREME").c_str());

	const int memoType = 2;
	if (memoType == 1) // NORMAL MEMO TYPE (unicode)
	{
		// TODO to be continued...
	}
	else if (memoType == 2) // IBOOGIE MEMO TYPE
	{
		vector<char> validCharList;

		string validCharSet = cocos2d::FileUtils::getInstance()->getStringFromFile("valid_char.txt");

		// Init filter
		for (unsigned int i = 0; i < validCharSet.length(); i++)
		{
			char ch = validCharSet[i];
			if (ch != '\n' && ch != '\r')
			{
				validCharList.push_back(ch);
			}
		}

		// Read memo text to [blockStringList] & [beatStringList]
		vector<string> lines;
		char memoFilename[80];
		if (difficulty == DIFFICULTY_BASIC) sprintf(memoFilename, "songs/%s_bsc.txt", songName);
		if (difficulty == DIFFICULTY_ADVANCED) sprintf(memoFilename, "songs/%s_adv.txt", songName);
		if (difficulty == DIFFICULTY_EXTREME) sprintf(memoFilename, "songs/%s_ext.txt", songName);
		string line = cocos2d::FileUtils::getInstance()->getStringFromFile(memoFilename);
		string delim = "\r\n";
		split(line, delim, &lines);
		
		for (unsigned int i = 0; i < lines.size(); i++)
		{
			line = lines[i];

			// Valid character filter
			string tmp = "";
			for (size_t i = 0; i < line.size(); i++)
			{
				for (size_t j = 0; j < validCharList.size(); j++)
					if (line[i] == validCharList[j])
					{
						tmp += line[i];
						break;
					}
			}

			// Skip empty line
			if (tmp == "") continue;

			// Split line to block & beat
			string block = tmp.substr(0, 4);
			string beat = tmp.substr(4);
			if (block != "") blockLines.push_back(block);
			if (beat != "") beatLines.push_back(beat);
		}

		// Get hitList by [blockStringList] & [beatStringList]
		pMemoBlock = -1;
		double timeMS = 0;
		for (size_t beatIndex = 0; beatIndex < beatLines.size(); beatIndex++)
		{
			const string beat = beatLines[beatIndex];
			int p = 1;
			int beatSize = 0;

			// Calculate the beat size = 4 / 6 / 8
			while (beat[p] != '|')
			{
				char hitNum = beat[p];

				// Skip (BPM) label
				if (hitNum == '(')
				{
					while (beat[p] != ')') p++;
					hitNum = beat[++p];
				}

				// Skip [OFFSET] label
				if (hitNum == '[')
				{
					while (beat[p] != ']') p++;
					hitNum = beat[++p];
				}

				// Count <XX> half note
				if (hitNum == '<')
				{
					while (beat[p] != '>') p++;
				}
				beatSize++;
				p++;
			}

			// Get each hit
			p = 1;
			while (beat[p] != '|')
			{
				char hitNum = beat[p];

				// Get the (BPM) change if exist
				if (hitNum == '(')
				{
					int start = p + 1;
					int end = beat.find(')', start);
					int len = end - start;
					string num = beat.substr(p + 1, len);
					m_BPM = atoi(num.c_str());
					p = end;
					hitNum = beat[++p];
				}

				// Get the [OFFSET]
				if (hitNum == '[')
				{
					int start = p + 1;
					int end = beat.find(']', start);
					int len = end - start;
					string num = beat.substr(p + 1, len);
					if (beatIndex == 0) // Set offset on first beat only
					{
						m_OFFSET = atoi(num.c_str());
						m_OFFSET += m_platformOFFSET;
					}	
					p = end;
					hitNum = beat[++p];
				}

				// Add a new hit
				if (isHitNoteChar(hitNum))
				{
					// Goto next block
					if (hitNum == '1') pMemoBlock++;
					THit newHit = getNextHit(hitNum, timeMS, m_BPM);
					hitList.push_back(newHit);
				}

				// Add <XX> half note
				if (hitNum == '<')
				{
					hitNum = beat[++p];
					if (isHitNoteChar(hitNum))
					{
						hitList.push_back(getNextHit(hitNum, timeMS, m_BPM));
					}
					hitNum = beat[++p];
					if (isHitNoteChar(hitNum))
					{
						hitList.push_back(getNextHit(hitNum, timeMS + getTimePerUnitMS(m_BPM, beatSize) / 2, m_BPM));
					}
					p++;
				}

				timeMS += getTimePerUnitMS(m_BPM, beatSize);
				p++;
			}
		}

		// Initialize music bar
		memset(musicBarHeight, 0, sizeof(musicBarHeight));
		memset(musicBarStatPerfects, 0, sizeof(musicBarStatPerfects));
		memset(musicBarStatCombos, 0, sizeof(musicBarStatCombos));

		m_songLength = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicLength();
		for (unsigned int i = 0; i < hitList.size(); i++)
		{
			int barLength = m_songLength / MAX_MUSIC_BARS;
			int barIndex = (hitList[i].touchTime + m_OFFSET - m_platformOFFSET) / barLength;
			hitList[i].musicBarIndex = barIndex;

			if (i == hitList.size() - 1)
			{
				hitList[i].isLastHitOfBar = true;
			}
			else
			{
				int nextBarIndex = (hitList[i + 1].touchTime + m_OFFSET - m_platformOFFSET) / barLength;
				if (barIndex != nextBarIndex)
				{
					hitList[i].isLastHitOfBar = true;
				}
			}

			int hits = 0;
			for (int x = 0; x < 4; x++)
				for (int y = 0; y < 4; y++)
					if (hitList[i].block[x][y]) hits++;

			musicBarHeight[barIndex] += hits;
		}
	}

	// Init score related members
	m_comboScore = 900000.0 / getTotalCombo();
}

int MemoManager::getMusicBarHeight(int index)
{
	return musicBarHeight[index];
}

bool getMissCount(THit hit)
{
	for (int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			if (hit.block[i][j] && !hit.blockHited[i][j])
				return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Play the memo
//////////////////////////////////////////////////////////////////////////
void MemoManager::playMemo(int autoPlayOn, int clapOn, int currentBgmTime, PlayMusicLayer* PML)
{
	// Clap & auto play
	while (pAutoHitIndex < (int)hitList.size()
		&& hitList[pAutoHitIndex].touchTime + m_OFFSET <= currentBgmTime)
	{
		// Change BPM if it is changed
		if (m_currentBPM != hitList[pAutoHitIndex].BPM)
		{
			m_currentBPM = hitList[pAutoHitIndex].BPM;
			if (m_currentBonusTimes > 0)
			{
				PML->updateJumpingBackgroundBPM();
				PML->updateJumpingDoorBPM();
			}
		}

		for (int r = 0; r < 4; r++)
		{
			for (int c = 0; c < 4; c++)
			{
				if (hitList[pAutoHitIndex].block[r][c]) 
				{	
					// Auto play
					if (autoPlayOn)
					{
						PML->autoPlayTouch(r, c);
					}
				}
			}
		}

		// Play clap sound if its option on
		if (clapOn)
		{
			PML->playClapSound();
		}

		pAutoHitIndex++;
	}

	// Display note & play hit sound
	while (pHitIndex < (int)hitList.size()
		&& hitList[pHitIndex].touchTime + m_OFFSET - NOTE_PREVIEW_TIME_MS <= currentBgmTime 
		&& hitList[pHitIndex].displayed == false)
	{
		for (int r = 0; r < 4; r++)
			for (int c = 0; c < 4; c++)
			{
				if (hitList[pHitIndex].block[r][c]) 
				{
					PML->playBlockNote(r * 4 + c);
				}
			}

		// Add this note to judge queue
		pJudgeHitIndexTo = pHitIndex;
		hitList[pHitIndex].displayed = true;
		pHitIndex++;
	}

	// Update judge range if a hit passed away
	while (pJudgeHitIndexFrom <= pJudgeHitIndexTo
		&& hitList[pJudgeHitIndexFrom].touchTime + m_OFFSET + NOTE_PREVIEW_TIME_MS <= currentBgmTime)
	{
		// Update music bar if the passed hit is the last hit of a bar
		if (hitList[pJudgeHitIndexFrom].isLastHitOfBar)
		{
			int pMusicBar = hitList[pJudgeHitIndexFrom].musicBarIndex;

			if (musicBarHeight[pMusicBar] == musicBarStatPerfects[pMusicBar]) PML->updateMusicBar(pMusicBar, MB_STATUS_PERFECT);
			else if (musicBarHeight[pMusicBar] == musicBarStatCombos[pMusicBar]) PML->updateMusicBar(pMusicBar, MB_STATUS_GREAT);
			else /* if BAD or MISS exist */ PML->updateMusicBar(pMusicBar, MB_STATUS_MISS);

			/*
			CCLOG("bar[%d] height = %d, perfects = %d, combos = %d", 
				pMusicBar, musicBarHeight[pMusicBar], musicBarStatPerfects[pMusicBar], musicBarStatCombos[pMusicBar]);
			*/
		}

		int missCount = getMissCount(hitList[pJudgeHitIndexFrom]);
		if (missCount > 0)
		{
			// Update bonus & door
			for (int i = 0; i <= missCount; i++) updateBonusTimes(JUDGE_RESULT_BAD); // same to MISS
			PML->updateDoorLayer();

			// Clear combo
			PML->updateComboLabel(0);
		}
		pJudgeHitIndexFrom++;
	}

}

int MemoManager::judgeResult(int deltaTime)
{
	// Special judge for perfect (5 frames about 42ms)
	// if (deltaTime > 16.6f * -3 && deltaTime < 16.6f * 2) return JUDGE_RESULT_PERFECT;

	if (deltaTime < 0) deltaTime = -deltaTime;
	if (deltaTime < JUDGE_PERFECT_TIME_MS) return JUDGE_RESULT_PERFECT;
	if (deltaTime < JUDGE_GREAT_TIME_MS) return JUDGE_RESULT_GREAT;
	if (deltaTime < JUDGE_GOOD_TIME_MS) return JUDGE_RESULT_GOOD;
	return JUDGE_RESULT_BAD;
}

double getScoreByJudgeResult(int judgeRes, double perfectScore)
{
	if (judgeRes == JUDGE_RESULT_BAD) return perfectScore * 0.1;
	if (judgeRes == JUDGE_RESULT_GOOD) return perfectScore * 0.4;
	if (judgeRes == JUDGE_RESULT_GREAT) return perfectScore * 0.7;
	
	return perfectScore * 1.0;
}

void MemoManager::updateBonusTimes(int judgeRes)
{
	if (judgeRes == JUDGE_RESULT_PERFECT || judgeRes == JUDGE_RESULT_GREAT)
	{
		double times = m_currentBonusTimes + 2.0 / m_totalCombos;
		m_currentBonusTimes = (times > 1 ? 1 : times);
		return;
	}

	if (judgeRes == JUDGE_RESULT_GOOD)
	{
		double times = m_currentBonusTimes + 1.0 / m_totalCombos;
		m_currentBonusTimes = (times > 1 ? 1 : times);
		return;
	}

	// BAD or MISS
	double times = m_currentBonusTimes - 8.0 / m_totalCombos;
	m_currentBonusTimes = (times < 0 ? 0 : times);
}

int MemoManager::judgeTouch(int r, int c, int userTouchTime, int lastFrameTime, int &deltaTime, int &correctTouchTime)
{
	for (int i = pJudgeHitIndexFrom; i <= pJudgeHitIndexTo; i++)
	{
		if (hitList[i].block[r][c] && !hitList[i].blockHited[r][c])
		{
			hitList[i].blockHited[r][c] = true;

			correctTouchTime = hitList[i].touchTime + m_OFFSET;

			int frameTime = 17;
			if (ConfigManager::getInstance()->isAutoPlay()) 
			{
				frameTime = lastFrameTime;
				userTouchTime -= frameTime;
			}

			int userTouchPeriodBegin = userTouchTime;
			int userTouchPeriodEnd = userTouchTime + frameTime;

			if (userTouchPeriodBegin <= correctTouchTime && correctTouchTime <= userTouchPeriodEnd)
			{
				deltaTime = 0;
			}
			else
			{
				deltaTime = min(abs(userTouchPeriodBegin - correctTouchTime), abs(userTouchPeriodEnd - correctTouchTime));
			}

			//if (ConfigManager::getInstance()->isAutoPlay()) deltaTime = 0;
			
			int res = judgeResult(deltaTime);
			m_currentScore += getScoreByJudgeResult(res, m_comboScore);

			// Error fix :)
			if (m_currentScore > 899990) m_currentScore = 900000;

			// Update music bar statistics
			int barIndex = hitList[i].musicBarIndex;
			if (res == JUDGE_RESULT_PERFECT)
			{
				musicBarStatCombos[barIndex]++;
				musicBarStatPerfects[barIndex]++;
			}
			else if (res != JUDGE_RESULT_BAD)
			{
				musicBarStatCombos[barIndex]++;
			}

			return res;
		}
	}

	return JUDGE_RESULT_NONE;
}

int MemoManager::getSongLength()
{
	return m_songLength;
}

int MemoManager::getPlayPosition()
{
	return m_playPosition;
}

void MemoManager::setPlayPosition(int pos)
{
	m_playPosition = pos;
}

int MemoManager::getTotalCombo()
{
	return m_totalCombos;
}

int MemoManager::getCurrentScore()
{
	return (int)m_currentScore;
}

double MemoManager::getCurrentBonusTimes()
{
	return m_currentBonusTimes;
}

int MemoManager::getCurrentBPM()
{
	return m_currentBPM;
}

string MemoManager::getSongName()
{
	return m_songName;
}

string MemoManager::getSongArtist()
{
	return m_songArtist;
}

int MemoManager::getSongDifficulty()
{
	return m_songDifficulty;
}

int MemoManager::getSongLevel()
{
	return m_songLevel;
}

int MemoManager::getPlatformOffset()
{
	return m_platformOFFSET;
}