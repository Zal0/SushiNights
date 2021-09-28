#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include <string.h>
#include "BankManager.h"
#include "MapInfo.h"

IMPORT_MAP(map);
IMPORT_MAP(level01);
IMPORT_MAP(level02);
IMPORT_MAP(level03);
IMPORT_MAP(level06);
IMPORT_TILES(font);

#define BANKED_MAP(MAP, SECONDS) {BANK(MAP), &MAP, SECONDS}
#define LEVELS_END {0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 seconds;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(level01, 30),
	BANKED_MAP(level02, 30),
	BANKED_MAP(level03, 60),
	BANKED_MAP(level06, 30),

	LEVELS_END
};

#define INITIAL_ROPE_LENGTH 64

UINT8 current_level = 0;

UINT8 coll_tiles[] = {1, 2,4,5,6,7,13,15,50,51,52,53, 0};

UINT8 rope_length;
UINT8 sushi_collected;
UINT8 num_clients;
UINT8 clients_collected;
UINT16 countdown;
UINT8 ticks;

#define MAX_COLLECTABLES 10
UINT16 collectables_taken[MAX_COLLECTABLES + 1];

void InitRope() BANKED;

void LocateStuff(UINT8 map_bank, struct MapInfo* map, UINT8* start_x, UINT8* start_y) __nonbanked{
	UINT8 x, y, tile;
	UINT8* data;
	PUSH_BANK(map_bank);
	data = map->data;
	for(y = 0; y < map->height; ++ y) {
		for(x = 0; x < map->width; ++ x) {
			tile = *(data ++);
			if(tile == 252) {//client
				num_clients ++;
			}	else if(tile == 255) {
				*start_x = x;
				*start_y = y;
			}
		}
	}
	POP_BANK;
}

void START() {
	UINT8 start_x, start_y;
	const struct MapInfoBanked* level = &levels[current_level];

	rope_length = INITIAL_ROPE_LENGTH;
	sushi_collected = 0;
	memset(collectables_taken, 0, sizeof(collectables_taken));
	scroll_top_movement_limit = 72;
	num_clients = 0;
	clients_collected = 0;
	countdown = level->seconds + 1;
	ticks = 59; //next frame will update the value

	LocateStuff(level->bank, level->map, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1) << 3);
	InitScroll(level->bank, level->map, coll_tiles, 0);

	InitRope();

	INIT_FONT(font, PRINT_WIN);
	WX_REG = 7;
	WY_REG = 128;
	scroll_h_border = 2 << 3;
	SHOW_WIN;
	//INIT_CONSOLE(font, 3, 2);
}

void UPDATE() {
	ticks ++;
	if(ticks == 60) {
		ticks = 0;
		countdown --;
		PRINT_POS(2, 0);
		Printf("%d  ", countdown);
		if(countdown == 0) {
			//Time up!
			SetState(StateTimeUp);
			HIDE_WIN;
		}
	}
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

void CheckLevelComplete() BANKED {
	if(clients_collected == num_clients) {
		current_level ++;
		if(levels[current_level].map == 0)
			SetState(StateGameWin);
		else
			SetState(StateGame);
		HIDE_WIN;
	}
}