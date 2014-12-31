#include "BaseMarker.h"

USING_NS_CC;

BaseMarker::BaseMarker()
{
	m_emptyBlockFrameUp = nullptr;
	m_emptyBlockFrameDown = nullptr;
}

BaseMarker::~BaseMarker()
{
	if (m_emptyBlockFrameUp != nullptr) m_emptyBlockFrameUp->release();
	if (m_emptyBlockFrameDown != nullptr) m_emptyBlockFrameDown->release();
}

void BaseMarker::init(PlayMusicLayer* PML)
{
	Size winSize = Director::getInstance()->getWinSize();
	int blockLen = winSize.width / 4;

	m_emptyBlockFrameUp = SpriteFrame::create("block.png", Rect(0, 0, 120, 120));
	m_emptyBlockFrameDown = SpriteFrame::create("block_touched.png", Rect(0, 0, 120, 120));
	m_emptyBlockFrameUp->retain();
	m_emptyBlockFrameDown->retain();

	// Add 16 empty block frames
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			auto sp = Sprite::createWithSpriteFrame(m_emptyBlockFrameUp);
			sp->setPosition(Point(blockLen * c + blockLen / 2, blockLen * (3 - r) + blockLen / 2));
			sp->setVisible(true);
			m_emptyBlock[r * 4 + c] = sp;
			PML->addChild(sp, 3);
		}
	}
}

cocos2d::Sprite* BaseMarker::getEmptyBlock(int index)
{
	return m_emptyBlock[index];
}

void BaseMarker::blockTouchDown(int index)
{
	getEmptyBlock(index)->setSpriteFrame(m_emptyBlockFrameDown);
}

void BaseMarker::blockTouchUp(int index)
{
	getEmptyBlock(index)->setSpriteFrame(m_emptyBlockFrameUp);
}