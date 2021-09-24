#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"

Sprite* player_ptr;

//Walk
fixed decimal_y;
fixed accel_y;
UINT8 falling;
UINT8 jump_done;
extern Sprite* hook_ptr;

//Hooked
fixed hook_radius;
//UINT8 frame = 0;
//fixed max_ang;
UINT16 hook_x, hook_y;
fixed hook_ang;
INT16 hook_speed;

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

	switch(player_state) {
		case STATE_HOOKED:
			SetSpriteAnim(player_ptr, anim_hooked, 6);
			break;
	}
}

void HookPlayer(UINT16 x, UINT16 y, INT8 ang, UINT8 radius) BANKED {
	hook_x = x;
	hook_y = y;
	hook_radius.w = radius;
	hook_ang.h = ang > 0 ? 128 - (ang - 64) : (-64 - ang);
	hook_speed = 0;

	SetPlayerState(STATE_HOOKED);
}

void UPDATE();
void START() {
	player_ptr = THIS;

	SetPlayerState(STATE_WALKING);
	hook_radius.w = 80;
	hook_ang.w = 0;
	hook_speed = 0;
	hook_x = 68;
	hook_y = 68;

	decimal_y.w = 0;
	accel_y.w = 0;
	falling = 1;
	jump_done = 0;
	UPDATE();
}

void Jump() {
	if(!falling && !jump_done) {
		accel_y.w = (UINT16)-900;
		falling = 1;
		jump_done = 1;
	}
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
		Jump();
	} else {
		jump_done = 0;
		if((INT16)accel_y.w < 0) {
			accel_y.w = 0;
		}
	}

	if(!hook_ptr && KEY_PRESSED(J_B)) {
		SpriteManagerAdd(SpriteHook, THIS->x, THIS->y);
	}

	accel_y.w += 30;
	decimal_y.w += accel_y.w;
	if(decimal_y.h) {
		if(TranslateSprite(THIS, 0, decimal_y.h) != 0) {
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
	INT16 new_x, new_y;
	UINT8 ang = hook_ang.h;
	INT8 rad_incr = 0;
	fixed cached_ang;
	
	if(KEY_PRESSED(J_UP)){
		if(hook_radius.w > 16) {
			rad_incr = -1;
			hook_radius.w -= 1;
		}
	} else if(KEY_PRESSED(J_DOWN)){
		hook_radius.w += 1;
		rad_incr = 1;
	}

	//swing
	if(ang < 127 && hook_speed < 0 && KEY_PRESSED(J_LEFT))
		hook_speed -= 20;
	if(ang > 128 && hook_speed > 0 && KEY_PRESSED(J_RIGHT))
		hook_speed += 20;

	//drag
	hook_speed -= hook_speed >> 7;

	hook_speed += (hook_ang.h > 128) ? 20 : -20;
#define MAX_SPEED 1000
	if(hook_speed > MAX_SPEED) hook_speed = MAX_SPEED;
	if(hook_speed < -MAX_SPEED) hook_speed = -MAX_SPEED;

	cached_ang = hook_ang;
	hook_ang.w += hook_speed ;

	tmp_x.w = SIN(hook_ang.h) * hook_radius.w;
	tmp_y.w = COS(hook_ang.h) * hook_radius.w;

	new_x = hook_x + (INT8)tmp_x.h - (THIS->coll_w >> 1);
	new_y = hook_y + (INT8)tmp_y.h;
	if(TranslateSprite(THIS, new_x - THIS->x, new_y - THIS->y) != 0) {
		hook_speed = -hook_speed;
		hook_radius.w -= rad_incr; //Cancel radius increment
		hook_ang = cached_ang;
	}

	if(KEY_PRESSED(J_A)) {
		SetPlayerState(STATE_WALKING);
		jump_done = 0;
		falling = 0;
		Jump();
		SpriteManagerRemoveSprite(hook_ptr);
	}
}

void UPDATE() {
	switch(player_state) {
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