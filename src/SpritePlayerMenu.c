#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"

const UINT8 anim_idle_menu[] = { 2, 0,1};
const UINT8 anim_frog[] = { 2, 2, 3 };
const UINT8 anim_stag[] = { 2, 4, 5 };
const UINT8 anim_sushi[] = { 1, 6 };

void PlayerMenuSetAnim(Sprite* sprite, UINT8 idx) BANKED {
	switch(idx) {
		case 0: SetSpriteAnim(sprite, anim_idle_menu, 6u); break;
		case 1: SetSpriteAnim(sprite, anim_frog, 6u); break;
		case 2: SetSpriteAnim(sprite, anim_stag, 6u); break;
		case 3: SetSpriteAnim(sprite, anim_sushi, 6u); break;
	}
}

void START() {
}

void UPDATE() {
}

void DESTROY() {
}