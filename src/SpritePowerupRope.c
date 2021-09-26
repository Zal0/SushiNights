#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"

extern Sprite* player_ptr;
UINT8 IsCollected(Sprite* collectable) BANKED;
void TakeCollectable(Sprite* powerup) BANKED;
extern UINT8 rope_length;

void START() {
	if(IsCollected(THIS) != 255) {
		SpriteManagerRemove(THIS_IDX);
	}
}

void UPDATE() {
	if(CheckCollision(THIS, player_ptr)) {
		rope_length += 10;
		TakeCollectable(THIS);
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY() {
}