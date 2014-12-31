#include "StandardMarker.h"
#include "Constants.h"

USING_NS_CC;

void StandardMarker::init(PlayMusicLayer* PML)
{
	// Don't forget init super first
	BaseMarker::init(PML);

	Size winSize = Director::getInstance()->getWinSize();
	int blockLen = winSize.width / 4;

	// Add 16 block sprites
	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			auto sp = Sprite::create("marker.png");
			sp->setPosition(Point(blockLen * c + blockLen / 2, blockLen * (3 - r) + blockLen / 2));
			sp->setVisible(false);
			m_markerBlock[r * 4 + c] = sp;
			PML->addChild(sp, 0);
		}
	}

	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			auto sp = Sprite::create("black.png");
			sp->setPosition(Point(blockLen * c + blockLen / 2, blockLen * (3 - r) + blockLen / 2));
			sp->setVisible(false);
			m_black[r * 4 + c] = sp;
			PML->addChild(sp, 1);
		}
	}

	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			auto sp = Sprite::create("white.png");
			sp->setPosition(Point(blockLen * c + blockLen / 2, blockLen * (3 - r) + blockLen / 2));
			sp->setVisible(false);
			m_white[r * 4 + c] = sp;
			PML->addChild(sp, 2);
		}
	}
}

void StandardMarker::playNoteMarker(int index)
{
	float preTouchTime = NOTE_PREVIEW_TIME_MS / 1000.0;
	float preFadeInTime = 0.0f;

	auto ST = ScaleTo::create(preTouchTime * 0.95f, 0.0f);
	auto PREFI = FadeIn::create(preFadeInTime);
	auto DT = DelayTime::create(preTouchTime - preFadeInTime);
	auto FO = FadeOut::create(0.2f);

	auto ptr = getBlack(index);
	ptr->setScale(1.0f);
	getMarkerBlock(index)->setColor(COLOR_ORIGINAL);
	getMarkerBlock(index)->setVisible(true);

	getBlack(index)->runAction(Sequence::create(ST, NULL));
	getBlack(index)->setVisible(true);
	getMarkerBlock(index)->runAction(Sequence::create(PREFI, DT, FO, NULL));
}

void StandardMarker::playJudgeMarker(int index, int status)
{
	if (status == JUDGE_RESULT_PERFECT) 
	{
		getWhite(index)->setColor(COLOR_JUDGE_PERFECT);
	}
	else if (status == JUDGE_RESULT_GREAT) 
	{
		getWhite(index)->setColor(COLOR_JUDGE_GREAT);
	}
	else if (status == JUDGE_RESULT_GOOD)
	{
		getWhite(index)->setColor(COLOR_JUDGE_GOOD);
	}
	else
	{
		getWhite(index)->setColor(COLOR_JUDGE_BAD);
	}

	auto DT = DelayTime::create(0.15f);
	auto FO = FadeOut::create(0.25f);
	auto SEQ = Sequence::create(DT, FO, NULL);

	getMarkerBlock(index)->setVisible(false);
	getBlack(index)->setVisible(false);
	getMarkerBlock(index)->stopAllActions();
	getBlack(index)->stopAllActions();

	getWhite(index)->setOpacity((GLubyte)0xFF);
	getWhite(index)->runAction(SEQ);
	getWhite(index)->setVisible(true);
}

cocos2d::Sprite* StandardMarker::getMarkerBlock(int index)
{
	return m_markerBlock[index];
}

cocos2d::Sprite* StandardMarker::getBlack(int index)
{
	return m_black[index];
}

cocos2d::Sprite* StandardMarker::getWhite(int index)
{
	return m_white[index];
}