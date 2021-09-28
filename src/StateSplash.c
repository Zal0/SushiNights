#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Print.h"

IMPORT_MAP(mainmenu);
IMPORT_TILES(font);

extern UINT8 current_level;
extern UINT8 anim_idle_menu[];

void START() {
	InitScroll(BANK(mainmenu), &mainmenu, 0, 0);
	Sprite* sprite = SpriteManagerAdd(SpritePlayerMenu, 122,56);
	SetSpriteAnim(sprite, anim_idle_menu, 6u);
	sprite->mirror = V_MIRROR;
	INIT_FONT(font, PRINT_BKG);
	PRINT(0, 13, " PRESS START ");	
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		current_level = 0;
		SetState(StateGame);
		//SetState(StateGameWin); FOR TESTING
	}
}