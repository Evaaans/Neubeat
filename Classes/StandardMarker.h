#pragma once

#include "cocos2d.h"
#include "BaseMarker.h"

class StandardMarker : public BaseMarker
{
public:
	virtual void playNoteMarker(int index);
	virtual void playJudgeMarker(int index, int status);
	virtual void init(PlayMusicLayer *PML);

private:
	cocos2d::Sprite *m_markerBlock[16];
	cocos2d::Sprite *m_black[16];
	cocos2d::Sprite *m_white[16];
	cocos2d::Sprite* getMarkerBlock(int index);
	cocos2d::Sprite* getBlack(int index);
	cocos2d::Sprite* getWhite(int index);
};