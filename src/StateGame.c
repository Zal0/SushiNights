#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"

IMPORT_MAP(map);
IMPORT_TILES(font);

UINT8 coll_tiles[] = {1, 0};

void InitRope() BANKED;

void START() {
	scroll_target = SpriteManagerAdd(SpritePlayer, 50, 50);
	InitScroll(BANK(map), &map, coll_tiles, 0);

	InitRope();

	INIT_CONSOLE(font, 3, 2);
}

void UPDATE() {
}
