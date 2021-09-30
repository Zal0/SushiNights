#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Sounds.h"
#include "ZGBMain.h"


extern Sprite* player_ptr;
extern UINT8 sushi_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* powerup) BANKED;

typedef struct {
	UINT16 start_y;
	UINT8 frame;
} CUSTOM_DATA;

void START() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	if(IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	} else {
		data->start_y = THIS->y;
		data->frame = 0;
	}
  SetFrame(THIS, (((THIS->x >> 3) & 0x2) == 0) ? 0 : 1);
}

void UPDATE() {
	CUSTOM_DATA* data = (CUSTOM_DATA*)THIS->custom_data;
	data->frame ++;
	THIS->y = data->start_y + (SIN(data->frame + (UINT8)THIS->x) >> 5);

	if(!sushi_collected) {
		if(CheckCollision(THIS, player_ptr)) {
			sushi_collected = 1;
			TakeCollectable(THIS);
			SpriteManagerAdd(SpriteBubble, THIS->x , THIS->y );
			PlayFx(FX_PICKUP);
			SpriteManagerRemove(THIS_IDX);
		}
	}
}

void DESTROY() {
}