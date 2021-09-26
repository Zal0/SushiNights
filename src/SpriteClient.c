#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"

extern Sprite* player_ptr;
extern UINT8 sushi_collected;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* powerup) BANKED;

static UINT8 anim_idle[] = {2, 0, 1};
static UINT8 anim_happy[] = {2, 2, 3};

void START() {
	if(IsCollected(THIS) == 255) {
		SetSpriteAnim(THIS, anim_idle, 4);
	} else {
		SetSpriteAnim(THIS, anim_happy, 4);
	}
}

void UPDATE() {
	if(THIS->anim_data == anim_idle && sushi_collected) {
		if(CheckCollision(THIS, player_ptr)) {
			SetSpriteAnim(THIS, anim_happy, 4);
			sushi_collected = 0;
			TakeCollectable(THIS);
		}
	}
}

void DESTROY() {

}