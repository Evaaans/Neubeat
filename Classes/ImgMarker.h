#pragma once

#include "cocos2d.h"
#include "BaseMarker.h"

USING_NS_CC;

class ImgMarker : public BaseMarker
{
public:
	virtual void playNoteMarker(int index);
	virtual void playJudgeMarker(int index, int status);
	virtual void init(PlayMusicLayer *PML);

	ImgMarker();
	~ImgMarker();

private:
	cocos2d::Animation* m_normal;
	cocos2d::Animation* m_perfect;
	cocos2d::Animation* m_great;
	cocos2d::Animation* m_good;
	cocos2d::Animation* m_bad;
	void addAnimation(int frameCount, const char *fnameFormat, Animation *pa);

	cocos2d::Sprite* m_markerBlock[16];
	cocos2d::Sprite* getMarkerBlock(int index);
};