#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Print.h"

IMPORT_TILES(font);
IMPORT_MAP(victory);
const UINT8 anim_frog[] = { 2, 2, 3 };
const UINT8 anim_stag[] = { 2, 4, 5 };
const UINT8 anim_sushi[] = { 1, 6 };

void START() {
	InitScroll(BANK(victory), &victory, 0, 0);		
	Sprite* sprite = SpriteManagerAdd(SpritePlayerMenu, 64, 64);
	
	sprite = SpriteManagerAdd(SpritePlayerMenu, 40, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 60, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 80, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	 
	sprite = SpriteManagerAdd(SpritePlayerMenu, 8, 56);
	SetSpriteAnim(sprite, anim_frog, 6u);

	sprite = SpriteManagerAdd(SpritePlayerMenu, 122, 56);
	SetSpriteAnim(sprite, anim_stag, 6u);	
	
	INIT_FONT(font, PRINT_BKG);
	PRINT(6, 13, " YOU WIN");
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateSplash);
	}
}