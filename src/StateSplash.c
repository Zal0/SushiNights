#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"
#include "Print.h"
#include "Music.h"


IMPORT_MAP(mainmenu);
IMPORT_TILES(blackfont);
DECLARE_MUSIC(start);

extern UINT8 current_level;

void PlayerMenuSetAnim(Sprite* sprite, UINT8 idx) BANKED;

void START() {
	InitScroll(BANK(mainmenu), &mainmenu, 0, 0);
	Sprite* sprite = SpriteManagerAdd(SpritePlayerMenu, 122,56);
	PlayerMenuSetAnim(sprite, 0);
	sprite->mirror = V_MIRROR;
	INIT_FONT(blackfont, PRINT_BKG);
	PRINT(0, 13, " PRESS START ");	
	PRINT(1, 16, "MONIS GAMES");
	PRINT(2, 17, "GBCOMPO21");
	PlayMusic(start, 1);

}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		current_level = 0;
		SetState(StateGame);
		//SetState(StateGameWin); FOR TESTING
	}
}