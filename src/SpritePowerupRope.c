#include "Banks/SetAutoBank.h"
#include "SpriteManager.h"

extern Sprite* player_ptr;
UINT8 CheckPowerUp(UINT16 id) BANKED;
void TakePowerUp(Sprite* powerup) BANKED;
extern UINT8 rope_length;

void START() {
	if(CheckPowerUp(THIS->unique_id) != 255) {
		SpriteManagerRemove(THIS_IDX);
	}
}

void UPDATE() {
	if(CheckCollision(THIS, player_ptr)) {
		rope_length += 10;
		TakePowerUp(THIS);
		SpriteManagerRemove(THIS_IDX);
	}
}

void DESTROY() {
}