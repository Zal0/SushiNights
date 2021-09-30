#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"
#include "Sounds.h"

extern Sprite* player_ptr;
extern UINT8 sushi_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* powerup) BANKED;
void DeliverSushi(Sprite* client) BANKED;

static UINT8 anim_idle[] = {2, 0, 1};
static UINT8 anim_idle_alt[] = {2, 4, 5};
static UINT8 anim_happy[] = {2, 2, 3};
static UINT8 anim_happy_alt[] = {2, 6, 7};

#define ANIM(A) (((THIS->x >> 3) & 0x2) == 0) ? A : A ## _alt

void START() {
	if(IsCollected(THIS) == 255) {
		SetSpriteAnim(THIS, ANIM(anim_idle), 4);
	} else {
		SetSpriteAnim(THIS, ANIM(anim_happy), 4);
	}
}

void UPDATE() {
	if(sushi_collected && (THIS->anim_data == anim_idle || THIS->anim_data == anim_idle_alt)) {
		if(CheckCollision(THIS, player_ptr)) {
			SetSpriteAnim(THIS, ANIM(anim_happy), 4);
			sushi_collected = 0;
			TakeCollectable(THIS);
			PlayFx(FX_PICKUP);
			
			DeliverSushi(THIS);
		}
	}
}

void DESTROY() {
}