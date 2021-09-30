#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Print.h"

IMPORT_TILES(blackfont);
IMPORT_MAP(victory);
extern UINT8 anim_idle_menu[];

extern UINT8 highscore[];
extern UINT8 current_level;

void PlayerMenuSetAnim(Sprite* sprite, UINT8 idx) BANKED;
UINT8 totalScore = 0;

void START() {
	InitScroll(BANK(victory), &victory, 0, 0);		
	Sprite* sprite = SpriteManagerAdd(SpritePlayerMenu, 64, 64);
	PlayerMenuSetAnim(sprite, 0);

	sprite = SpriteManagerAdd(SpritePlayerMenu, 60, 12);
	PlayerMenuSetAnim(sprite, 3);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 80, 12);
	PlayerMenuSetAnim(sprite, 3);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 100, 12);
	PlayerMenuSetAnim(sprite, 3);
	 
	sprite = SpriteManagerAdd(SpritePlayerMenu, 2	, 42);
	PlayerMenuSetAnim(sprite, 1);

	sprite = SpriteManagerAdd(SpritePlayerMenu, 140, 42);
	PlayerMenuSetAnim(sprite, 2);
	
	INIT_FONT(blackfont, PRINT_BKG);
	PRINT(6, 13, " YOU WIN");
	UINT8 i;
	for ( i = 0; i < current_level; i++)
	{
		totalScore += highscore[i];
	}
	
	PRINT_POS(3, 4);
	Printf("SCORE:  %d00 ", totalScore);	
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateSplash);
	}
}