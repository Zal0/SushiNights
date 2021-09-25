#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Scroll.h"
#include "Trig.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include<string.h>
#include "Keys.h"

extern UINT8 next_oam_idx;
extern UINT8* oam;
UINT8 hook_rope[12];
extern UINT8 rope_length;

typedef struct {
	UINT8 dist;
	INT8 ang;
	UINT8 hooked;
	UINT8 max_length;
} CUSTOM_DATA;

Sprite* hook_ptr = 0;
extern Sprite* player_ptr;
void HookPlayer(UINT16 x, UINT16 y, INT8 ang, UINT8 radius) BANKED;

void InitRope() BANKED {
	UINT8 i = 0;
	SpriteManagerLoad(SpriteHook);
	while(i != 12) {
		hook_rope[i ++] = 50;
		hook_rope[i ++] = 50 + i;
		hook_rope[i ++] = spriteIdxs[SpriteHook] + 2;
		hook_rope[i ++] = 0;
	}
}

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	data->dist = 0;
	if(KEY_PRESSED(J_UP)){
		data->ang = -64;
	} else {
		data->ang = player_ptr->mirror == NO_MIRROR ? -32 : 128 + 32;
	}
	data->hooked = 0;
	data->max_length = rope_length;

	hook_ptr = THIS;
}

void DrawRope() {
	UINT8 start_x = player_ptr->x - scroll_x + 8 + 4;
	UINT8 start_y = player_ptr->y - scroll_y + 16;
	INT8 step_x = (THIS->x - player_ptr->x - 4) >> 2;
	INT8 step_y = (THIS->y - player_ptr->y) >> 2;

	UINT8 i;
	INT8 x_inc = step_x;
	INT8 y_inc = step_y;

	for(i = 0; i != 12; i += 4, x_inc += step_x, y_inc += step_y) {
		hook_rope[i    ] = start_y + y_inc;
		hook_rope[i + 1] = start_x + x_inc;
	}

	memcpy(oam + (next_oam_idx << 2), hook_rope, sizeof(hook_rope));
	next_oam_idx += sizeof(hook_rope) >> 2;
}

void UPDATE() {
	fixed radius;
	fixed tmp_x;
	fixed tmp_y;
	UINT8 coll_tile;
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	
	if(!data->hooked) {
		data->dist += 4;
		radius.w = ((SIN(data->dist) * data->max_length) >> 6);

		if((INT16)radius.w >= 0) {
			tmp_x.w = COS(data->ang) * radius.w;
			tmp_y.w = SIN(data->ang) * radius.w;

			THIS->x = player_ptr->x + (player_ptr->coll_w >> 1) + (INT8)tmp_x.h;
			THIS->y = player_ptr->y + (INT8)tmp_y.h;
			if(data->dist < 64) { //ignore collision when going back
				coll_tile = GetScrollTile(THIS->x >> 3, THIS->y >> 3);
				if(coll_tile) {
					if(coll_tile == 1 || coll_tile == 3) {
						HookPlayer(THIS->x, THIS->y, data->ang, radius.l);
						data->hooked = 1;
					} else {
						if(data->dist < 64) {
							data->dist = 128 - data->dist;
						}
					}
				}
			}
		} else {
			SpriteManagerRemove(THIS_IDX);
		}
	}

	DrawRope();
}

void RetireHook(Sprite* hook, INT8 ang, INT8 radius) BANKED {
	CUSTOM_DATA* data = (CUSTOM_DATA*)hook->custom_data;
	data->hooked = 0;
	data->ang = ang > 0 ? 128 - (ang - 64) : (-64 - ang);
	data->max_length = radius;
	if(data->dist < 64) {
		data->dist = 128 - data->dist;
	}
}

void DESTROY() {
	hook_ptr = 0;
}