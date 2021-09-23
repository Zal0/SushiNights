#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Scroll.h"
#include "Trig.h"

typedef struct {
	UINT8 dist;
	INT8 ang;
	UINT8 hooked;
} CUSTOM_DATA;

Sprite* hook_ptr = 0;
extern Sprite* player_ptr;
void HookPlayer(UINT16 x, UINT16 y, INT8 ang, UINT8 radius) BANKED;

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	data->dist = 0;
	data->ang = player_ptr->mirror == NO_MIRROR ? -32 : 128 + 32;
	data->hooked = 0;
	hook_ptr = THIS;
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

			if(TranslateSprite(THIS, player_ptr->x + (INT8)tmp_x.h - THIS->x, player_ptr->y + (INT8)tmp_y.h - THIS-> y)) {
				HookPlayer(THIS->x, THIS->y, data->ang, radius.l);
				data->hooked = 1;
			}
		} else {
			SpriteManagerRemove(THIS_IDX);
		}
	}
}

void DESTROY() {
	hook_ptr = 0;
}