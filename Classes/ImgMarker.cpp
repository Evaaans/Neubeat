#include "ImgMarker.h"
#include "Constants.h"

USING_NS_CC;

ImgMarker::ImgMarker()
{
	m_normal = nullptr;
	m_perfect = nullptr;
	m_great = nullptr;
	m_good = nullptr;
	m_bad = nullptr;
}

ImgMarker::~ImgMarker()
{
	if (m_normal != nullptr) m_normal->release();
	if (m_perfect != nullptr) m_perfect->release();
	if (m_great != nullptr) m_great->release();
	if (m_good != nullptr) m_good->release();
	if (m_bad != nullptr) m_bad->release();
}

void ImgMarker::addAnimation(int frameCount, const char *fnameFormat, Animation *pa)
{
	for(int i = 0; i < frameCount; ++ i)
	{
		char fName[50];
		sprintf(fName, fnameFormat, i);
		
		char path[50];
		sprintf(path, "mkr/1/%s", fName);

		pa->addSpriteFrameWithFile(path);
	}
}

void ImgMarker::init(PlayMusicLayer *PML)
{
	// Don't forget init super first
	BaseMarker::init(PML);

	Size winSize = Director::getInstance()->getWinSize();
	int blockLen = winSize.width / 4;

	float frameDuration = 1.0f / 30.0f;

	// Normal and miss animation
	m_normal = Animation::create();
	m_normal->setDelayPerUnit(frameDuration);
	addAnimation(14, "1_normal_%d.png", m_normal);
	addAnimation(3, "1_normalpassed_%d.png", m_normal);
	addAnimation(5, "1_miss_%d.png", m_normal);
	addAnimation(1, "1_empty.png", m_normal);
	m_normal->retain();
	
	// Perfect
	m_perfect = Animation::create();
	m_perfect->setDelayPerUnit(frameDuration);
	addAnimation(15, "1_perfect_%d.png", m_perfect);
	addAnimation(1, "1_empty.png", m_perfect);
	m_perfect->retain();

	// Great
	m_great = Animation::create();
	m_great->setDelayPerUnit(frameDuration);
	addAnimation(15, "1_great_%d.png", m_great);
	addAnimation(1, "1_empty.png", m_great);
	m_great->retain();

	// Good
	m_good = Animation::create();
	m_good->setDelayPerUnit(frameDuration);
	addAnimation(14, "1_good_%d.png", m_good);
	addAnimation(1, "1_empty.png", m_good);
	m_good->retain();

	// Bad
	m_bad = Animation::create();
	m_bad->setDelayPerUnit(frameDuration);
	addAnimation(13, "1_bad_%d.png", m_bad);
	addAnimation(1, "1_empty.png", m_bad);
	m_bad->retain();

	// Marker sprites for animation
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			auto sp = Sprite::create();
			sp->setPosition(Point(blockLen * c + blockLen / 2, blockLen * (3 - r) + blockLen / 2));
			sp->setScale(0.67f);
			m_markerBlock[r * 4 + c] = sp;
			PML->addChild(sp, 0);
		}
	}
}

void ImgMarker::playNoteMarker(int index)
{
	auto pAnimate = Animate::create(m_normal);
	getMarkerBlock(index)->stopAllActions();
	getMarkerBlock(index)->runAction(pAnimate);
}

void ImgMarker::playJudgeMarker(int index, int status)
{	
	getMarkerBlock(index)->stopAllActions();
	if (status == JUDGE_RESULT_PERFECT) 
	{
		auto pAnimate = Animate::create(m_perfect);
		getMarkerBlock(index)->runAction(pAnimate);
	}
	else if (status == JUDGE_RESULT_GREAT) 
	{
		auto pAnimate = Animate::create(m_great);
		getMarkerBlock(index)->runAction(pAnimate);
	}
	else if (status == JUDGE_RESULT_GOOD)
	{
		auto pAnimate = Animate::create(m_good);
		getMarkerBlock(index)->runAction(pAnimate);
	}
	else
	{
		auto pAnimate = Animate::create(m_bad);
		getMarkerBlock(index)->runAction(pAnimate);
	}
}

cocos2d::Sprite* ImgMarker::getMarkerBlock(int index)
{
	return m_markerBlock[index];
}
