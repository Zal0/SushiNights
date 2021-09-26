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

#define MAX_POWERUPS 10
UINT16 powerups_taken[MAX_POWERUPS + 1];

void InitRope() BANKED;

void START() {
	rope_length = 64;
	memset(powerups_taken, 0, sizeof(powerups_taken));
	scroll_top_movement_limit = 72;

	scroll_target = SpriteManagerAdd(SpritePlayer, 50, 50);
	InitScroll(BANK(map), &map, coll_tiles, 0);

	InitRope();

	INIT_CONSOLE(font, 3, 2);
}

void UPDATE() {
}

UINT8 CheckPowerUp(UINT16 id) BANKED {
	UINT8 i;
	for(i = 1; i != powerups_taken[0] + 1; ++i) {
		if(powerups_taken[i] == id)
			return i;
	}
	return 255;
}

void TakePowerUp(Sprite* powerup) BANKED {
	powerups_taken[++ powerups_taken[0]] = powerup->unique_id;
}