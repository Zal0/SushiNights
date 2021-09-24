#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Scroll.h"
#include "Trig.h"
#include "ZGBMain.h"
#include<string.h>

extern UINT8 next_oam_idx;
extern UINT8* oam;
UINT8 hook_rope[12];

typedef struct {
	UINT8 dist;
	INT8 ang;
	UINT8 hooked;
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
	data->ang = player_ptr->mirror == NO_MIRROR ? -32 : 128 + 32;
	data->hooked = 0;
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
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	
	if(!data->hooked) {
		data->dist += 4;
		radius.w = (INT8)SIN(data->dist);

		if((INT16)radius.w >= 0) {
			tmp_x.w = COS(data->ang) * radius.w;
			tmp_y.w = SIN(data->ang) * radius.w;

			if(TranslateSprite(THIS, player_ptr->x + (player_ptr->coll_w >> 1) + (INT8)tmp_x.h - THIS->x, player_ptr->y + (INT8)tmp_y.h - THIS-> y)) {
				HookPlayer(THIS->x, THIS->y, data->ang, radius.l);
				data->hooked = 1;
			}
		} else {
			SpriteManagerRemove(THIS_IDX);
		}
	}

	DrawRope();
}

void DESTROY() {
	hook_ptr = 0;
}