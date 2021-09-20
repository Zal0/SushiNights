#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Keys.h"

//Walk
fixed decimal_y;
fixed accel_y;
UINT8 falling;
UINT8 jump_done;

//Hooked
fixed radius;
UINT8 ang = 0;
UINT8 frame = 0;
fixed max_ang;

typedef enum {
	STATE_WALKING,
	STATE_HOOKED
} PLAYER_STATE;
PLAYER_STATE player_state = STATE_WALKING;

const UINT8 anim_idle[] = {1, 0};
const UINT8 anim_walk[] = {2, 0, 1};
const UINT8 anim_hooked[] = {1, 2};

void SetPlayerState(PLAYER_STATE state) {
	player_state = state;
}

void UPDATE();
void START() {
	SetPlayerState(STATE_WALKING);
	radius.w = 80;
	max_ang.w = 0;//200 << 8;

	decimal_y.w = 0;
	accel_y.w = 0;
	falling = 1;
	jump_done = 0;
	UPDATE();
}

void UpdateWalk() {
	if(KEY_PRESSED(J_LEFT)){
		TranslateSprite(THIS, -1, 0);
		THIS->mirror = V_MIRROR;
		SetSpriteAnim(THIS, anim_walk, 20);
	} else if(KEY_PRESSED(J_RIGHT)){
		TranslateSprite(THIS, 1, 0);
		THIS->mirror = NO_MIRROR;
		SetSpriteAnim(THIS, anim_walk, 20);
	} else {
		SetSpriteAnim(THIS, anim_idle, 6);
	}

	if(KEY_PRESSED(J_A)){
		if(!falling && !jump_done) {
			accel_y.w = -900;
			falling = 1;
			jump_done = 1;
		}
	} else {
		jump_done = 0;
		if((INT16)accel_y.w < 0)
		{
			accel_y.w = 0;
		}
	}

	accel_y.w += 30;
	decimal_y.w += accel_y.w;
	if(decimal_y.h)
	{
		if(TranslateSprite(THIS, 0, decimal_y.h) != 0)
		{
			if((INT8)decimal_y.h > 0) {
				accel_y.w = 0;
				falling = 0;
			}
		}
		decimal_y.h = 0;
	}
}

void UpdateHooked() {
	fixed tmp_x, tmp_y;

	if(KEY_PRESSED(J_UP)){
		if(radius.w > 16) {
			radius.w -= 1;
		}
	} else if(KEY_PRESSED(J_DOWN)){
		radius.w += 1;
	}

	tmp_x.w = SIN(ang) * radius.w;
	tmp_y.w = COS(ang) * radius.w;

	THIS->x = 68 + (INT8)tmp_x.h;
	THIS->y = 68 + (INT8)tmp_y.h;

	if(max_ang.w)
	{
		if(KEY_PRESSED(J_LEFT)){
			if((INT8)tmp_x.h < 0){
				max_ang.w += 100;
			}
		} else if(KEY_PRESSED(J_RIGHT)){
			if((INT8)tmp_x.h > 0) {
				max_ang.w += 100;
			}
		} else {
			if(max_ang.w > 300)
				max_ang.w -= 300;
			else
				max_ang.w = 0;
		}
	} else {
		if(KEY_PRESSED(J_LEFT)){
			frame = 64;
			max_ang.w = 5000;
		} else if(KEY_PRESSED(J_RIGHT)){
			frame = -64;
			max_ang.w = 5000;
		}
	}

	frame += 3;
	ang = (COS(frame) * (max_ang.h)) >> 8;
}

void UPDATE() {
	switch(player_state)
	{
		case STATE_WALKING:
			UpdateWalk();
			break;
		case STATE_HOOKED:
			UpdateHooked();
			break;
	}
}

void DESTROY() {
}