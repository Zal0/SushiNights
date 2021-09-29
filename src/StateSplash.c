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

void START() {
	InitScroll(BANK(mainmenu), &mainmenu, 0, 0);
	SpriteManagerAdd(SpritePlayerMenu, 122,56);

	INIT_FONT(font, PRINT_BKG);
	PRINT(0, 13, " PRESS START ");	
	PRINT(1, 16, "MONIS GAMES");
	PRINT(2, 17, "GBCOMPO21");
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		current_level = 0;
		SetState(StateGame);
		//SetState(StateGameWin);
	}
}