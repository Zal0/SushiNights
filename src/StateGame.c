#include "Banks/SetAutoBank.h"

#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"
#include "Print.h"
#include <string.h>
#include "BankManager.h"
#include "MapInfo.h"
#include "Music.h"
#include "Keys.h"

IMPORT_MAP(map);
IMPORT_MAP(level01);
IMPORT_MAP(level02);
IMPORT_MAP(level03);
IMPORT_MAP(level04);
IMPORT_MAP(level05);
IMPORT_MAP(maikel1);
IMPORT_MAP(maikel2);

IMPORT_TILES(blackfont);
IMPORT_TILES(font);
IMPORT_MAP(window);
DECLARE_MUSIC(level1);

#define BANKED_MAP(MAP, SECONDS) {BANK(MAP), &MAP, SECONDS}
#define LEVELS_END {0, 0}

struct MapInfoBanked {
	UINT8 bank;
	struct MapInfo* map;
	UINT16 seconds;
};

const struct MapInfoBanked levels[] = {
	BANKED_MAP(level01, 200),
	BANKED_MAP(level02, 200),
	BANKED_MAP(level03, 200),
	BANKED_MAP(level04, 200),
	BANKED_MAP(level05, 200),
	BANKED_MAP(maikel1, 200),
	BANKED_MAP(maikel2, 200),

	LEVELS_END
};

#define INITIAL_ROPE_LENGTH 64

UINT8 current_level = 7;

UINT8 coll_tiles[] = {1, 2,4,5,6,7,13,15,50,51,52,53, 0};
UINT8 highscore[] = { 0,0,0,0,0,0,0,0 };

UINT8 rope_length;
UINT8 sushi_collected;
UINT8 num_clients;
UINT8 clients_collected;
UINT16 countdown;
UINT8 ticks;
UINT8 level_complete;

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

const UINT8 CHECKED_TILE = 75;
const UINT8 UNCHECKED_TILE = 74;
const UINT8 SUSHI_TILE= 86;
const UINT8 NOSUSHI_TILE = 87;
void RefreshSushies() BANKED {
	UINT8 i;

	if (sushi_collected)
	{
		set_win_tiles(6, 0, 1, 1, &SUSHI_TILE);
	}
	else
	{
		set_win_tiles(6, 0, 1, 1, &NOSUSHI_TILE);
	}

	for(i = 0; i != clients_collected; ++i) {
		set_win_tiles(19 - i, 0, 1, 1, &CHECKED_TILE);
	}
	for(; i != num_clients; ++i) {
		set_win_tiles(19 - i, 0, 1, 1, &UNCHECKED_TILE);
	}
}

void START() {
	UINT8 start_x, start_y;
	const struct MapInfoBanked* level = &levels[current_level];
	highscore[current_level] = 0;

	rope_length = INITIAL_ROPE_LENGTH;
	sushi_collected = 0;
	memset(collectables_taken, 0, sizeof(collectables_taken));
	scroll_top_movement_limit = 72;
	num_clients = 0;
	clients_collected = 0;
	countdown = level->seconds + 1;
	ticks = 59; //next frame will update the value
	level_complete = 0;

	LocateStuff(level->bank, level->map, &start_x, &start_y);
	scroll_target = SpriteManagerAdd(SpritePlayer, start_x << 3, (start_y - 1) << 3);
	InitScroll(level->bank, level->map, coll_tiles, 0);

	InitRope();

	INIT_FONT(font, PRINT_WIN);
	WX_REG = 7;
	WY_REG = 136;
	scroll_h_border = 1 << 3;
	InitWindow(0, 0, BANK(window), &window);
	RefreshSushies();
	PRINT(19 - num_clients - 6, 0, "CLIENTS");
	SHOW_WIN;
	//INIT_CONSOLE(font, 3, 2);

	NR52_REG = 0x80;
	NR51_REG = 0xFF;
	NR50_REG = 0x77;
	PlayMusic(level1, 1);
}

void UPDATE() {
	if(!level_complete) {
		ticks ++;
		if(ticks == 60) {
			ticks = 0;
			countdown --;
			PRINT_POS(2, 0);
			Printf("%d", countdown);
			if(countdown == 0) {
				//Time up!
				SetState(StateTimeUp);
				HIDE_WIN;
			}
		}
	} else {
		if(KEY_TICKED(J_START)) {
			current_level ++;
			if(levels[current_level].map == 0)
				SetState(StateGameWin);
			else
				SetState(StateGame);
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
	RefreshSushies();
}

void DoAnimLevelEnd();
void CheckLevelComplete() BANKED {
	if(clients_collected == num_clients) {
		highscore[current_level] = countdown;
		DoAnimLevelEnd();
	}
}

extern Sprite* player_ptr;
void ShowVictoryAnim() BANKED;
void DoAnimLevelEnd() {
	UINT8 top_bar_start    = (((player_ptr->y                     ) >> 3) - 1) & 0x1F;
	UINT8 bottom_bar_start = (((player_ptr->y + player_ptr->coll_h) >> 3) + 1) & 0x1F;

	UINT8 n_bars = 15;

	UINT8 x = scroll_x >> 3;
	UINT8 black_tile = 0;

	UINT8 i;
	Sprite* spr;
	HIDE_WIN;
	SPRITEMANAGER_ITERATE(i, spr) {
		if(spr != player_ptr)
			SpriteManagerRemove(i);
	}
	
	for(i = n_bars + 1; i != 0; --i) {
		for(int j = 0; j < 21; ++j) {
			set_bkg_tiles((x + j) & 0x1F, (top_bar_start - i) & 0x1F, 1, 1, &black_tile);
			set_bkg_tiles((x + j) & 0x1F, (bottom_bar_start + i) & 0x1F, 1, 1, &black_tile);
		}
		delay(20);
	}
	ShowVictoryAnim();
	INIT_FONT(blackfont, PRINT_WIN);
	print_target = PRINT_BKG;
	PRINT_POS(x + 2 , (top_bar_start - 3) & 0x1F);

	Printf(" LVL SCORE %d00  ", highscore[current_level]);

	PRINT(x + 5, (top_bar_start - 1) & 0x1F, "GOOD JOB!");	
	level_complete = 1;
}
