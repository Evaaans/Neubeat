#pragma once

//////////////////////////////////////////////////////////////////////////
// Global constants
//////////////////////////////////////////////////////////////////////////
#define OPTION_CLAP_ON			1

#define COLOR_COMBO_NORMAL		Color3B(0xFF, 0xFF, 0xFF)
#define COLOR_COMBO_MISS		Color3B(0x30, 0x00, 0x00)

#define DIFFICULTY_BASIC		1
#define DIFFICULTY_ADVANCED		2
#define DIFFICULTY_EXTREME		3

//////////////////////////////////////////////////////////////////////////
// Music bar constants
//////////////////////////////////////////////////////////////////////////
#define MAX_MUSIC_BARS			120
#define MAX_MUSIC_BAR_HEIGHT	8

#define MB_POS_HEIGHT			494

#define MB_TOTAL_STATUS			4
#define MB_STATUS_READY			0
#define MB_STATUS_PERFECT		1
#define MB_STATUS_GREAT			2
#define MB_STATUS_MISS			3

#define MB_CURSOR_OFFSET		2

//////////////////////////////////////////////////////////////////////////
// Show block constants
//////////////////////////////////////////////////////////////////////////
#define NOTE_PREVIEW_TIME_MS	500

#define COLOR_ORIGINAL			Color3B(0xFF, 0xFF, 0xFF)
#define COLOR_BLOCK_DOWN		Color3B(0x66, 0xCC, 0xFF)
#define COLOR_JUDGE_PERFECT		Color3B(0xFF, 0xCF, 0)
#define COLOR_JUDGE_GREAT		Color3B(0xCF, 0x5F, 0)
#define COLOR_JUDGE_GOOD		Color3B(0, 0x5F, 0xCF)
#define COLOR_JUDGE_BAD			Color3B(0x5F, 0, 0)

//////////////////////////////////////////////////////////////////////////
// Judge constants
//////////////////////////////////////////////////////////////////////////
#define JUDGE_RESULT_PERFECT	1
#define JUDGE_RESULT_GREAT		2
#define JUDGE_RESULT_GOOD		3
#define JUDGE_RESULT_BAD		4
#define JUDGE_RESULT_NONE		0xFF

#define JUDGE_PERFECT_TIME_MS	42 							 // PERFECT = 34 + 17 + 34 = 5 frames
#define JUDGE_GREAT_TIME_MS		JUDGE_PERFECT_TIME_MS * 2	 // GREAT = 68 + 17 + 68 = 9 frames
#define JUDGE_GOOD_TIME_MS		JUDGE_GREAT_TIME_MS * 2	     // GOOD = 136 + 17 + 136 = 17 frames
