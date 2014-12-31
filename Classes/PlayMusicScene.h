#ifndef __PLAYMUSIC_SCENE_H__
#define __PLAYMUSIC_SCENE_H__

#include "cocos2d.h"
#include "Constants.h"

#include <string>

using namespace std;

class BaseMarker;

class PlayMusicScene
{
public:
	static cocos2d::Scene* createScene();
};

class PlayMusicLayer : public cocos2d::Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(PlayMusicLayer);

	PlayMusicLayer();
	~PlayMusicLayer();

	void updateGame(float dt);

	void updateMusicBar(int index, int status);
	void updateComboLabel(int combo);
	void updateJumpingBackgroundBPM();
	void updateJumpingDoorBPM();
	void updateDoorLayer();

	void playBlockNote(int index);
	void playClapSound();
	void autoPlayTouch(int r, int c);

private:
	// For title
	cocos2d::Sprite *m_titleBackground;
	cocos2d::LabelTTF *m_label;
	cocos2d::LabelTTF *m_songNameLabel;
	cocos2d::LabelTTF *m_songArtistLabel;
	cocos2d::Sprite *m_difficultyLabel;
	cocos2d::Sprite *m_songArtworkLabel;
	cocos2d::LabelBMFont *m_levelLabel;
	cocos2d::LabelBMFont *m_scoreLabel;
	cocos2d::LabelBMFont *m_playerIdLabel;
	void initTitles(string songName);
	void rollScoreLabel(cocos2d::LabelBMFont* label, int toScore);
	void rollScoreAnimationCallback(cocos2d::LabelBMFont* label, int score, bool isLastStep);

	// For combo
	cocos2d::LabelBMFont *m_comboLabel;
	cocos2d::Sprite *m_comboText;
	int m_currentCombo;

	// For marker & note display
	BaseMarker *m_markerBlocks;
	void initTouchAreaBlocks();
	void playClapSoundCallback();

	// For touch screen
	bool m_blockTouchDown[16];
	void judgeTouch(int r, int c);
	void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	void onTouchesCancelled(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

	// For music bar
	cocos2d::SpriteFrame *m_musicBarCache[MB_TOTAL_STATUS + 1][MAX_MUSIC_BAR_HEIGHT + 1];
	cocos2d::Sprite *m_musicBar[MAX_MUSIC_BARS + 1];
	cocos2d::Sprite *m_musicBarCursor;
	void initMusicBars();
	void initMusicBarCache();
	void updateMusicBarCursorPosition(int bgmTime);

	// For jumping background & door
	cocos2d::Sprite *m_markerBackground;
	cocos2d::Sprite *m_topDoor;
	cocos2d::Sprite *m_bottomDoor;
	cocos2d::Layer *m_topDoorLayer;
	cocos2d::Layer *m_bottomDoorLayer;
	void closeDoorLayer();

	bool isBackgroundJumping;
	bool isDoorJumping;
	int m_lastFrameDuration;

	void updateSongPlayPosition();
};

#endif // __PLAYMUSIC_SCENE_H__
