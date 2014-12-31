#include "PlayMusicScene.h"
#include "MemoManager.h"
#include "StandardMarker.h"
#include "ImgMarker.h"
#include "SimpleAudioEngine.h"
#include "ConfigManager.h"

#include <string>

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
// Debug timer
//////////////////////////////////////////////////////////////////////////
#define TIMER_START timer(1)
#define TIMER_END timer(2)
#define TIMER_DURATION timer(3)
#define TIMER_OUTPUT CCLOG("duration = %d ms", timer(3))

int timer(int cmd)
{
	static int begin = 0, end = 0;
	static struct timeval tv;
	static int status = 0;

	if (cmd == 1) 
	{   
		while (status);
		gettimeofday(&tv, NULL);
		begin = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		status = 1;
	}
	else if (cmd == 2) 
	{
		gettimeofday(&tv, NULL);     
		end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		status = 0;
	}

	return end - begin;
}

//////////////////////////////////////////////////////////////////////////
// Scene creator
//////////////////////////////////////////////////////////////////////////
Scene* PlayMusicScene::createScene()
{
	auto scene = Scene::create();
	auto layer = PlayMusicLayer::create();
	scene->addChild(layer);
	return scene;
}

//////////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////////
PlayMusicLayer::PlayMusicLayer()
{
	for (int statusIndex = 0; statusIndex < MB_TOTAL_STATUS; statusIndex++)
		for (int height = 1; height <= 8; height++)
		{
			m_musicBarCache[statusIndex][height] = nullptr;
		}

	m_markerBlocks = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// Destructor to release memory
//////////////////////////////////////////////////////////////////////////
PlayMusicLayer::~PlayMusicLayer()
{
	// Release music bar frame cache
	for (int statusIndex = 0; statusIndex < MB_TOTAL_STATUS; statusIndex++)
		for (int height = 1; height <= 8; height++)
			if (m_musicBarCache[statusIndex][height] != nullptr)
			{
				m_musicBarCache[statusIndex][height]->release();
			}

	// Don't forget to release marker memory
	if (m_markerBlocks != nullptr)
	{
		delete m_markerBlocks;
	}
}

void PlayMusicLayer::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

//////////////////////////////////////////////////////////////////////////
// Initialize instance & members
//////////////////////////////////////////////////////////////////////////
bool PlayMusicLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
	}

	// Add external resources
	auto searchPaths = FileUtils::getInstance()->getSearchPaths();
	searchPaths.push_back("/mnt/sdcard/Android/Data/com.evaaans.jubeatist/");
	reverse(searchPaths.begin(), searchPaths.end());
	FileUtils::getInstance()->setSearchPaths(searchPaths);

	string song_name = ConfigManager::getInstance()->getSongName();

	// Get memo & bgm filename
	const string mp3Filename = "songs/" + song_name + ".mp3";

	// Preload sound
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("hit.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(mp3Filename.c_str(), false);
	TIMER_START;
	// CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();

	// read memo
	MemoManager::getInstance()->readMemo(song_name.c_str(), ConfigManager::getInstance()->getDifficulty());

	initTitles(song_name);
	initTouchAreaBlocks();
	initMusicBarCache();
	initMusicBars();
	
	//CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	
	// Touch listener registration
	auto dispatcher = Director::getInstance()->getEventDispatcher();
	auto listener = EventListenerTouchAllAtOnce::create();
	listener->onTouchesBegan = CC_CALLBACK_2(PlayMusicLayer::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(PlayMusicLayer::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(PlayMusicLayer::onTouchesEnded, this);
	listener->onTouchesCancelled = CC_CALLBACK_2(PlayMusicLayer::onTouchesCancelled, this);
	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    this->schedule( schedule_selector(PlayMusicLayer::updateGame) );  
		
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Init music bar frame cache pool
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::initMusicBarCache()
{
	for (int statusIndex = 0; statusIndex < MB_TOTAL_STATUS; statusIndex++)
	{
		char fname[10];
		sprintf(fname, "bar%d.png", statusIndex + 1);

		for (int height = 1; height <= 8; height++)
		{
			m_musicBarCache[statusIndex][height] = SpriteFrame::create(fname, Rect(0, 0, 3, height * 3));
			m_musicBarCache[statusIndex][height]->retain();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Init music bar
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::initMusicBars()
{
	Size winSize = Director::getInstance()->getWinSize();

	for (int i = 0; i < MAX_MUSIC_BARS; i++)
	{
		int height = min(MAX_MUSIC_BAR_HEIGHT, MemoManager::getInstance()->getMusicBarHeight(i));
		if (height == 0) continue;

		m_musicBar[i] = Sprite::createWithSpriteFrame(m_musicBarCache[MB_STATUS_READY][height]);
		m_musicBar[i]->setAnchorPoint(Point(0, 0));
		m_musicBar[i]->setPosition((winSize.width / 8) + 5 + i * 3, MB_POS_HEIGHT);
		this->addChild(m_musicBar[i], 1);
	}

	m_musicBarCursor = Sprite::create("bar_stick.png");
	m_musicBarCursor->setAnchorPoint(Point(0, 0));
	m_musicBarCursor->setPosition(winSize.width / 8 + 5, MB_POS_HEIGHT - 5);
	this->addChild(m_musicBarCursor, 2);
}

//////////////////////////////////////////////////////////////////////////
// Update music bar
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::updateMusicBar(int index, int status)
{
	int height = min(MAX_MUSIC_BAR_HEIGHT, MemoManager::getInstance()->getMusicBarHeight(index));
	if (height == 0 || index < 0 || index > MAX_MUSIC_BARS) return;
	m_musicBar[index]->setSpriteFrame(m_musicBarCache[status][height]);
}

void PlayMusicLayer::updateMusicBarCursorPosition(int bgmTime)
{
	bgmTime -= MemoManager::getInstance()->getPlatformOffset();
	Size winSize = Director::getInstance()->getWinSize();
	int pos = ((double)bgmTime / MemoManager::getInstance()->getSongLength()) * 360;
	m_musicBarCursor->setPosition((winSize.width / 8) + pos + MB_CURSOR_OFFSET , MB_POS_HEIGHT - 5);
}

//////////////////////////////////////////////////////////////////////////
// Init sprites on title area
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::initTitles(string songEngName)
{
	Size winSize = Director::getInstance()->getWinSize();
	Point origin = Director::getInstance()->getVisibleOrigin();

	m_titleBackground = Sprite::create("title_back.png");
	m_titleBackground->setAnchorPoint(Point(0, 1));
	m_titleBackground->setPosition(Point(0, winSize.height));
	this->addChild(m_titleBackground, 0);

	m_label = LabelTTF::create("[Debug messages]", "fonts/Saucer.ttf", 21);
	m_label->setAnchorPoint(Point(0, 0));
	//m_label->setPosition(Point(origin.x + 15, origin.y + winSize.height - m_label->getContentSize().height - 15));
	m_label->setPosition(Point(15, 560));
	//m_label->setVisible(false);
	this->addChild(m_label, 1);

	m_songNameLabel = LabelTTF::create(MemoManager::getInstance()->getSongName(), "Microsoft YaHei", 20);
	m_songNameLabel->setAnchorPoint(Point(0, 1));
	if (m_songNameLabel->getContentSize().width > 200)
	{
		m_songNameLabel->setScaleX(200.0f / m_songNameLabel->getContentSize().width);
	}
	m_songNameLabel->setPosition(Point(270, 640 - 6));
	this->addChild(m_songNameLabel, 1);

	m_songArtistLabel = LabelTTF::create(MemoManager::getInstance()->getSongArtist(), "Microsoft YaHei", 13);
	m_songArtistLabel->setAnchorPoint(Point(0, 1));
	m_songArtistLabel->setPosition(Point(270, 640 - 31));
	this->addChild(m_songArtistLabel, 1);

	char artworkFilename[80];
	sprintf(artworkFilename, "songs/%s.png", songEngName.c_str());
	m_songArtworkLabel = Sprite::create(artworkFilename);
	m_songArtworkLabel->setAnchorPoint(Point(0, 1));
	m_songArtworkLabel->setScale(40.0f / m_songArtworkLabel->getContentSize().width);
	m_songArtworkLabel->setPosition(Point(218, 640 - 9));
	this->addChild(m_songArtworkLabel, 1);

	char lvstr[10];
	int lv = MemoManager::getInstance()->getSongLevel();
	if (lv == 10) lv = 0;
	sprintf(lvstr, "%d", lv);
	m_levelLabel = LabelBMFont::create(lvstr, "fonts/Level.fnt");
	m_levelLabel->setScale(0.8f);
	m_levelLabel->setAnchorPoint(Point(0, 1));
	m_levelLabel->setPosition(Point(179, 640 - 10));
	this->addChild(m_levelLabel, 1);

	char diffstr[20];
	int diff = MemoManager::getInstance()->getSongDifficulty();
	if (diff == DIFFICULTY_BASIC) sprintf(diffstr, "diff_b.png");
	if (diff == DIFFICULTY_ADVANCED) sprintf(diffstr, "diff_a.png");
	if (diff == DIFFICULTY_EXTREME) sprintf(diffstr, "diff_e.png");
	m_difficultyLabel = Sprite::create(diffstr);
	m_difficultyLabel->setAnchorPoint(Point(0, 1));
	m_difficultyLabel->setPosition(Point(13, 640 - 30));
	this->addChild(m_difficultyLabel, 1);
	
	m_currentCombo = 0;
	m_comboLabel = LabelBMFont::create("0", "fonts/Combo.fnt");
	m_comboLabel->setColor(COLOR_COMBO_NORMAL);
	m_comboLabel->setAnchorPoint(Point(0.5f, 0.5f));
	m_comboLabel->setVisible(false);
	m_comboLabel->setPosition(Point(winSize.width / 2 - 4, (winSize.height * 0.75) / 2));
	m_comboLabel->setScale(1.0f);
	this->addChild(m_comboLabel, 0);

	m_comboText = Sprite::create("combo_label.png");
	m_comboText->setPosition(Point(winSize.width / 2 + 60, (winSize.height * 0.75) / 2 - 65));
	m_comboText->setVisible(false);
	this->addChild(m_comboText, 0);

	m_scoreLabel = LabelBMFont::create("0", "fonts/Score.fnt");
	m_scoreLabel->setScale(0.70f);
	m_scoreLabel->setAnchorPoint(Point(1.0f, 0));
	m_scoreLabel->setPosition(Point(winSize.width - 5, 518));
	this->addChild(m_scoreLabel, 1);

	string playerId = "7*PRINCE";
	m_playerIdLabel = LabelBMFont::create(playerId, "fonts/PlayerId.fnt");
	m_playerIdLabel->setAnchorPoint(Point(0, 0));
	//m_playerIdLabel->setColor(Color3B(0xDF, 0xDF, 0xDF));
	m_playerIdLabel->setPosition(Point(15, 525));
	this->addChild(m_playerIdLabel, 1);
}

//////////////////////////////////////////////////////////////////////////
// Init marker block area
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::initTouchAreaBlocks()
{
	memset(m_blockTouchDown, 0, sizeof(m_blockTouchDown));

	// Choose standard marker
	//m_markerBlocks = new StandardMarker();
	m_markerBlocks = new ImgMarker();
	m_markerBlocks->init(this);

	if (ConfigManager::getInstance()->isHighQuality()) m_markerBackground = Sprite::create("block_bg_1.png");
	else m_markerBackground = Sprite::create("block_bg_2.png");
	m_markerBackground->setScale(480.0 / m_markerBackground->getContentSize().height);
	m_markerBackground->setPosition(Point(240, 240));
	this->addChild(m_markerBackground, -3);

	if (ConfigManager::getInstance()->isHighQuality())
	{
		m_bottomDoorLayer = Layer::create();
		m_topDoorLayer = Layer::create();
		m_bottomDoorLayer->setPosition(Point(0, 0));
		m_topDoorLayer->setPosition(Point(0, 0));
		this->addChild(m_bottomDoorLayer, -2);
		this->addChild(m_topDoorLayer, -1);

		m_bottomDoor = Sprite::create("door.png");
		m_bottomDoor->setPosition(Point(240, 240));
		m_bottomDoorLayer->addChild(m_bottomDoor);

		m_topDoor = Sprite::create("door.png");
		m_topDoor->setPosition(Point(240, 240));
		m_topDoor->setScale(-1.0f, -1.0f);
		m_topDoorLayer->addChild(m_topDoor);

		isBackgroundJumping = false;
		isDoorJumping = false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Display note marker
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::playBlockNote(int index)
{
	m_markerBlocks->playNoteMarker(index);
}

//////////////////////////////////////////////////////////////////////////
// Callback for clap sound
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::playClapSoundCallback()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
#endif
}

void PlayMusicLayer::playClapSound()
{
	/*
	float preTouchTime = NOTE_PREVIEW_TIME_MS / 1000.0;

	auto DT = DelayTime::create(preTouchTime);
	auto playClap = CallFuncN::create( CC_CALLBACK_0(PlayMusicLayer::playClapSoundCallback, this) );

	this->runAction(Sequence::create(DT, playClap, NULL));
	*/
	playClapSoundCallback();
}

//////////////////////////////////////////////////////////////////////////
// Auto play entry
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::autoPlayTouch(int r, int c)
{
	/*
	float preTouchTime = NOTE_PREVIEW_TIME_MS / 1000.0;

	auto DT = DelayTime::create(preTouchTime);
	auto TOUCH = CallFuncN::create( CC_CALLBACK_0(PlayMusicLayer::judgeTouch, this, r, c) );

	TIMER_START;
	this->runAction(Sequence::create(DT, TOUCH, NULL));
	*/
	judgeTouch(r, c);
}

//////////////////////////////////////////////////////////////////////////
// Update song play position for memo
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::updateSongPlayPosition()
{
	/*
	// System time
	static int begin = 0, end = 0;
	static struct timeval tv;
	if (begin == 0)
	{	
		gettimeofday(&tv, NULL);
		begin = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		begin -= 600;
	}

	gettimeofday(&tv, NULL);     
	end = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	int bgmPosition = end - begin;
	*/
	int bgmPosition = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicPosition();
	
	TIMER_END;
	m_lastFrameDuration = TIMER_DURATION;
	TIMER_START;
	MemoManager::getInstance()->setPlayPosition(bgmPosition);
}

//////////////////////////////////////////////////////////////////////////
// Main update entry for each frame
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::updateGame(float dt)
{
	updateSongPlayPosition();
	int bgmTime = MemoManager::getInstance()->getPlayPosition();

	MemoManager::getInstance()->playMemo(ConfigManager::getInstance()->isAutoPlay(), OPTION_CLAP_ON, bgmTime, this);
	updateMusicBarCursorPosition(bgmTime);

	/*
	char buf[20];
	sprintf(buf, "%d", bgmTime);
	m_label->setString(buf);
	*/
}

//////////////////////////////////////////////////////////////////////////
// Touch related callbacks
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
	/*
	char buf[90];
	sprintf(buf, "onTouchesBegan size = %d", touches.size());
	m_label->setString(buf);
	*/

	for (unsigned int touchIndex = 0; touchIndex < touches.size(); touchIndex++)
	{
		// Choose one of the touches to work with
		Touch* touch = touches[touchIndex];
		Point location = touch->getLocation();

		int r = 3 - (int)location.y / 120;
		int c = (int)location.x / 120;

		// Judge a touch if its position in a valid block area
		if (r >= 0 && r <= 3 && c >= 0 && c <= 3)
		{
			int blockIndex = r * 4 + c;

			// Skip the judge if the block was touching down already
			if (!m_blockTouchDown[blockIndex])
			{
				m_blockTouchDown[blockIndex] = true;
				m_markerBlocks->blockTouchDown(blockIndex);
				judgeTouch(r, c);
			}
		}
	}
}

void PlayMusicLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
	/*
	char buf[90];
	sprintf(buf, "onTouchesEnded size = %d", touches.size());
	m_label->setString(buf);
	*/

	for (unsigned int touchIndex = 0; touchIndex < touches.size(); touchIndex++)
	{
		// Choose one of the touches to work with
		Touch* touch = touches[touchIndex];
		Point location = touch->getLocation();

		int r = 3 - (int)location.y / 120;
		int c = (int)location.x / 120;

		// Turn off the block frame highlight for the block which lost touching
		if (r >= 0 && r <= 3 && c >= 0 && c <= 3)
		{
			int blockIndex = r * 4 + c;
			m_blockTouchDown[blockIndex] = false;
			m_markerBlocks->blockTouchUp(blockIndex);
		}
	}
}

void PlayMusicLayer::onTouchesCancelled(const std::vector<Touch*>& touches, Event* event)
{
	// Turn off all block frame highlight and release all touches for unexpected interrupt
	for (int i = 0; i < 16; i++)
	{
		m_blockTouchDown[i] = false;
		m_markerBlocks->blockTouchUp(i);
	}
}

void PlayMusicLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
	bool currentBlockTouchDown[16] = { false };

	for (unsigned int touchIndex = 0; touchIndex < touches.size(); touchIndex++)
	{
		// Choose one of the touches to work with
		Touch* touch = touches[touchIndex];
		Point location = touch->getLocation();

		int r = 3 - (int)location.y / 120;
		int c = (int)location.x / 120;

		// Judge a touch if its position in a valid block area
		if (r >= 0 && r <= 3 && c >= 0 && c <= 3)
		{
			int blockIndex = r * 4 + c;

			// Skip the judge if its block was touching down already
			if (!m_blockTouchDown[blockIndex])
			{
				m_blockTouchDown[blockIndex] = true;
				m_markerBlocks->blockTouchDown(blockIndex);
				judgeTouch(r, c);
			}

			currentBlockTouchDown[blockIndex] = true;
		}
	}

	// Turn off block frame highlight for blocks which lost touching after move
	for (int i = 0; i < 16; i++)
	{
		if (!currentBlockTouchDown[i])
		{
			m_blockTouchDown[i] = false;
			m_markerBlocks->blockTouchUp(i);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Update score by rolling animation
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::rollScoreAnimationCallback(LabelBMFont* label, int score, bool isLastStep)
{
	if (isLastStep)
	{
		char buf[10];
		sprintf(buf, "%d", score);
		label->setString(buf);
	}
	else
	{
		int currentScore = atoi(label->getString().c_str());

		char buf[10];
		sprintf(buf, "%d", currentScore + score);
		label->setString(buf);
	}
}

void PlayMusicLayer::rollScoreLabel(LabelBMFont* label, int toScore)
{
	label->stopAllActions();
	int currentScore = atoi(label->getString().c_str());
	int deltaScore = toScore - currentScore;

	if (ConfigManager::getInstance()->isHighQuality())
	{
		auto DT = DelayTime::create(0.015f);
		auto CF = CallFuncN::create( CC_CALLBACK_0(PlayMusicLayer::rollScoreAnimationCallback, this, label, deltaScore / 10, false) );
		auto STEP = Sequence::create(DT, CF, NULL);
		auto REP = Repeat::create(STEP, 9);
		auto CFFINAL = CallFuncN::create( CC_CALLBACK_0(PlayMusicLayer::rollScoreAnimationCallback, this, label, toScore, true) );

		label->runAction(Sequence::create(REP, CFFINAL, NULL));
	}
	else
	{
		auto CFFINAL = CallFuncN::create( CC_CALLBACK_0(PlayMusicLayer::rollScoreAnimationCallback, this, label, toScore, true) );

		label->runAction(Sequence::create(CFFINAL, NULL));
	}
}

//////////////////////////////////////////////////////////////////////////
// Judge touch by current music play position
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::judgeTouch(int r, int c)
{
	//int userTouchTime = CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicPosition();
	int userTouchTime = MemoManager::getInstance()->getPlayPosition();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!ConfigManager::getInstance()->isAutoPlay()) 
		userTouchTime -= ConfigManager::getInstance()->getScreenDelay();
#endif

	int songLength = MemoManager::getInstance()->getSongLength();
	if (userTouchTime >= songLength) return;

	int deltaTime = 0;
	static int deltaTimeTotal = 0;
	static int frameTimeTotal = 0;
	static int judgeCount = 0;

	int correctTime = 0;
	int status = MemoManager::getInstance()->judgeTouch(r, c, userTouchTime, m_lastFrameDuration, deltaTime, correctTime);
	if (status != JUDGE_RESULT_NONE)
	{
		// Update bonus & door
		MemoManager::getInstance()->updateBonusTimes(status);
		updateDoorLayer();

		// Update score
		rollScoreLabel(m_scoreLabel, MemoManager::getInstance()->getCurrentScore());
		if (MemoManager::getInstance()->getCurrentScore() >= 700000)
		{
			m_scoreLabel->setFntFile("fonts/ScoreHighlight.fnt");
		}

		// Debug statistics
		deltaTimeTotal += deltaTime;
		int frameTime = m_lastFrameDuration;
		frameTimeTotal += frameTime;

		static int maxDelta = -1000;
		static int minDelta = 1000;
		bool needupdate = false;
		if (deltaTime > maxDelta) needupdate = true;
		maxDelta = max(deltaTime, maxDelta);
		minDelta = min(deltaTime, minDelta);

		static int maxFD = -1000;
		static int minFD = 1000;
		maxFD = max(frameTime, maxFD);
		minFD = min(frameTime, minFD);
		
		judgeCount++;
		int deltaTimeAvg = deltaTimeTotal / judgeCount;
		int frameTimeAvg = frameTimeTotal / judgeCount;

		//if (needupdate)
		{
			char buf[100];
			//sprintf(buf, "D = %d, F = %d, U = %d, C = %d", 
			//	deltaTime, frameTime, userTouchTime, correctTime);
			sprintf(buf, "D = %d, AVG = %d (%d, %d) F = %d, FAVG = %d (%d, %d)", 
				deltaTime, deltaTimeAvg, minDelta, maxDelta, frameTime, frameTimeAvg, minFD, maxFD);
			m_label->setString(buf);
		}
	}

	int blockIndex = r * 4 + c;

	if (status == JUDGE_RESULT_NONE) return;

	if (status == JUDGE_RESULT_PERFECT) 
	{
		m_currentCombo++;
	}
	else if (status == JUDGE_RESULT_GREAT) 
	{
		m_currentCombo++;
	}
	else if (status == JUDGE_RESULT_GOOD)
	{
		m_currentCombo++;
	}
	else /* if (status == JUDGE_RESULT_BAD) */
	{
		m_currentCombo = 0; // Miss combo with a BAD
	}

	updateComboLabel(m_currentCombo);
	m_markerBlocks->playJudgeMarker(blockIndex, status);
}

//////////////////////////////////////////////////////////////////////////
// Update combo display
//////////////////////////////////////////////////////////////////////////
void PlayMusicLayer::updateComboLabel(int combo)
{
	Size winSize = Director::getInstance()->getWinSize();

	if (combo == 0)
	{
		m_currentCombo = 0;

		m_comboText->setVisible(false);
		if (m_comboLabel->getString() != "0")
		{
			m_comboLabel->setString("0");
			m_comboLabel->setPositionY((winSize.height * 0.75) / 2);
			m_comboLabel->setColor(COLOR_COMBO_MISS);

			auto FO = FadeOut::create(0.2f);
			m_comboLabel->stopAllActions();
			m_comboLabel->setPositionY((winSize.height * 0.75) / 2);
			m_comboLabel->runAction(FO);
		}
	}
	else
	{
		char buf[10];
		sprintf(buf, "%d", combo);
		m_comboLabel->setString(buf);
		m_comboLabel->setColor(COLOR_COMBO_NORMAL);
		m_comboLabel->setOpacity(GLubyte(0xFF));

		if (combo >= 5)
		{
			m_comboLabel->setVisible(true);
			m_comboText->setVisible(true);
		}
		else 
		{
			m_comboLabel->setVisible(false);
			m_comboText->setVisible(false);
		}

		if (ConfigManager::getInstance()->isHighQuality())
		{
			// Run jump effect for combo
			auto JUMP = MoveBy::create(0.04f, Point(0, 7));
			m_comboLabel->stopAllActions();
			m_comboLabel->setPositionY((winSize.height * 0.75) / 2);
			m_comboLabel->runAction(Sequence::create(JUMP, JUMP->reverse(), NULL));
		}
	}
}

void PlayMusicLayer::updateJumpingBackgroundBPM()
{
	if (ConfigManager::getInstance()->isHighQuality() == 0) return;

	/*
	// If door fully closed, update is not necessary 
	double bonusTimes = MemoManager::getInstance()->getCurrentBonusTimes();
	if (bonusTimes <= 0) return;
	*/

	static int BPM = 0;
	// If BPM changed or the door is closed but it should be open now
	if (MemoManager::getInstance()->getCurrentBPM() != BPM || isBackgroundJumping == false)
	{
		BPM = MemoManager::getInstance()->getCurrentBPM();
		float jumpTime = 60.0f / BPM;
		static const float backgroundOriginalScale = 480.0 / m_markerBackground->getContentSize().height;
		static const float scale = 1.06f;

		// Background 
		auto ZOOMIN = ScaleTo::create(jumpTime * 0.35f, backgroundOriginalScale * scale);
		auto ZOOMOUT = ScaleTo::create(jumpTime * 0.65f, backgroundOriginalScale * 1.0f);

		auto JUMP = Sequence::create(ZOOMOUT, ZOOMIN, NULL);
		auto REP = RepeatForever::create(JUMP);

		m_markerBackground->stopAllActions();
		m_markerBackground->runAction(REP);

		isBackgroundJumping = true;
	}
}

void PlayMusicLayer::updateJumpingDoorBPM()
{
	if (ConfigManager::getInstance()->isHighQuality() == 0) return;
	/*
	// If door fully closed, update is not necessary 
	double bonusTimes = MemoManager::getInstance()->getCurrentBonusTimes();
	if (bonusTimes <= 0) return;
	*/

	static int BPM = 0;
	// If BPM changed or the door is closed but it should be open now
	if (MemoManager::getInstance()->getCurrentBPM() != BPM || isDoorJumping == false)
	{
		BPM = MemoManager::getInstance()->getCurrentBPM();
		float jumpTime = 60.0f / BPM;
		static const int dist = 3;

		// Door
		auto TUP = MoveTo::create(jumpTime * 0.35f, Point(240, 240 + dist));
		auto TDOWN = MoveTo::create(jumpTime * 0.65f, Point(240, 240 - dist));

		auto BDOWN = MoveTo::create(jumpTime * 0.35f, Point(240, 240 - dist));
		auto BUP = MoveTo::create(jumpTime * 0.65f, Point(240, 240 + dist));

		auto TOPREP = RepeatForever::create(Sequence::create(TDOWN, TUP, NULL));
		auto BOTTOMREP = RepeatForever::create(Sequence::create(BUP, BDOWN, NULL));

		m_topDoor->stopAllActions();
		m_bottomDoor->stopAllActions();

		m_topDoor->runAction(TOPREP);
		m_bottomDoor->runAction(BOTTOMREP);

		isDoorJumping = true;
	}
}

void PlayMusicLayer::updateDoorLayer()
{
	if (ConfigManager::getInstance()->isHighQuality() == 0) return;

	double bonusTime = MemoManager::getInstance()->getCurrentBonusTimes();

	if (bonusTime <= 0)
	{
		closeDoorLayer();

		if (isDoorJumping)
		{
			// Reset door
			m_bottomDoor->stopAllActions();
			m_topDoor->stopAllActions();
			m_bottomDoor->runAction(MoveTo::create(1.0f, Point(240, 240)));
			m_topDoor->runAction(MoveTo::create(1.0f, Point(240, 240)));
			isDoorJumping = false;

			// Reset background
			static const float backgroundOriginalScale = 480.0 / m_markerBackground->getContentSize().height;
			m_markerBackground->stopAllActions();
			m_markerBackground->runAction(ScaleTo::create(1.0f, backgroundOriginalScale * 1.0f));
			isBackgroundJumping = false;
		}
	}
	else
	{
		const float time = 0.2f;
		const int dist = 60;
		m_topDoorLayer->stopAllActions();
		m_bottomDoorLayer->stopAllActions();
		m_topDoorLayer->runAction(MoveTo::create(time, Point(0, 0 + (240 + dist) * bonusTime)));
		m_bottomDoorLayer->runAction(MoveTo::create(time, Point(0, 0 - (240 + dist) * bonusTime)));

		updateJumpingBackgroundBPM();
		updateJumpingDoorBPM();
	}
}

void PlayMusicLayer::closeDoorLayer()
{
	const float time = 1.5f;

	auto GOUP = MoveTo::create(time, Point(0, 0));
	auto GODOWN = MoveTo::create(time, Point(0, 0));

	m_topDoorLayer->stopAllActions();
	m_bottomDoorLayer->stopAllActions();
	m_topDoorLayer->runAction(GOUP);
	m_bottomDoorLayer->runAction(GODOWN);
}