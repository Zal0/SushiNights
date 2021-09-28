#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "SpriteManager.h"

IMPORT_MAP(mainmenu);


extern UINT8 current_level;

void START() {
	InitScroll(BANK(mainmenu), &mainmenu, 0, 0);
	SpriteManagerAdd(SpritePlayerMenu, 122,56);
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		current_level = 0;

		SetState(StateGame);
	}
}