#pragma once

#include "PlayMusicScene.h"

class BaseMarker
{
public:
	virtual void playNoteMarker(int index) = 0;
	virtual void playJudgeMarker(int index, int status) = 0;
	virtual void init(PlayMusicLayer *PML) = 0;

	virtual void blockTouchDown(int index);
	virtual void blockTouchUp(int index);
	
	BaseMarker();
	~BaseMarker();

protected:
	cocos2d::Sprite* getEmptyBlock(int index);

private:
	cocos2d::SpriteFrame *m_emptyBlockFrameDown;
	cocos2d::SpriteFrame *m_emptyBlockFrameUp;
	cocos2d::Sprite *m_emptyBlock[16];
};