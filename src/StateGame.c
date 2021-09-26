#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include <string.h>

IMPORT_MAP(map);
IMPORT_TILES(font);

#define BANKED_MAP(MAP) {BANK(MAP), &MAP}
#define LEVELS_END {0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(map),

	LEVELS_END
};

UINT8 current_level = 0;

UINT8 coll_tiles[] = {1, 2,4,5,6,7,13,15,50,51,52,53, 0};

UINT8 rope_length;
UINT8 sushi_collected;

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void InitRope() BANKED;

void START() {
	UINT16 start_x, start_y;
	const struct MapInfoBanked* level = &levels[current_level];
	UINT8 level_w, level_h;

	rope_length = 64;
	sushi_collected = 0;
	memset(collectables_taken, 0, sizeof(collectables_taken));
	scroll_top_movement_limit = 72;

	GetMapSize(level->bank, level->map, &level_w, &level_h);
	ScrollFindTile(level->bank, level->map, 255, 0, 0, level_w, level_h, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1)<< 3);
	InitScroll(BANK(map), &map, coll_tiles, 0);

	InitRope();

	//INIT_CONSOLE(font, 3, 2);
}

void UPDATE() {
}

UINT8 IsCollected(Sprite* collectable) BANKED {
	UINT8 i;
	for(i = 1; i != collectables_taken[0] + 1; ++i) {
		if(collectables_taken[i] == collectable->unique_id)
			return i;
	}
	return 255;
}

void TakeCollectable(Sprite* collectable) BANKED {
	collectables_taken[++ collectables_taken[0]] = collectable->unique_id;
}