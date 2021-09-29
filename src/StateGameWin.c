#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Print.h"

IMPORT_TILES(font);
IMPORT_MAP(victory);
extern UINT8 anim_idle_menu[];

extern UINT8 anim_frog[];
extern UINT8 anim_stag[];
extern UINT8 anim_sushi[];

void START() {
	InitScroll(BANK(victory), &victory, 0, 0);		
	Sprite* sprite = SpriteManagerAdd(SpritePlayerMenu, 64, 64);
	SetSpriteAnim(sprite, anim_idle_menu, 6u);



	sprite = SpriteManagerAdd(SpritePlayerMenu, 60, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 80, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	sprite = SpriteManagerAdd(SpritePlayerMenu, 100, 12);
	SetSpriteAnim(sprite, anim_sushi, 6u);
	 
	sprite = SpriteManagerAdd(SpritePlayerMenu, 2	, 42);
	SetSpriteAnim(sprite, anim_frog, 6u);

	sprite = SpriteManagerAdd(SpritePlayerMenu, 140, 42);
	SetSpriteAnim(sprite, anim_stag, 6u);	
	
	INIT_FONT(font, PRINT_BKG);
	PRINT(6, 13, " YOU WIN");
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateSplash);
	}
}