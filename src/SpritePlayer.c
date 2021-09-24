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
INT16 speed_y;
UINT8 jump_done;
extern Sprite* hook_ptr;

//Hooked
fixed hook_radius;
UINT16 hook_x, hook_y;
fixed hook_ang;
INT16 hook_speed;

//Flying
fixed decimal_x;
INT16 speed_x;
UINT8 check_key_released_on_jump;
UINT8 bounce_on_coll;

typedef enum {
	STATE_WALKING,
	STATE_HOOKED,
	STATE_FLYING
} PLAYER_STATE;
PLAYER_STATE player_state = STATE_WALKING;

const UINT8 anim_idle[] = {3, 0,1,2};
const UINT8 anim_walk[] = {4, 3, 4,5,6};
const UINT8 anim_jump[] = {1, 6};
const UINT8 anim_hooked[] = {1, 6};
const UINT8 ANIMATION_SPEED = 16; 

void SetPlayerState(PLAYER_STATE state) {
	player_state = state;

	switch(player_state) {
		case STATE_WALKING:
			jump_done = KEY_PRESSED(J_A);
			break;

		case STATE_HOOKED:
			speed_x = 0;
			speed_y = 0;
			SetSpriteAnim(player_ptr, anim_hooked, ANIMATION_SPEED);
			break;

		case STATE_FLYING:
			speed_x = 0;
			speed_y = 0;
			decimal_x.w = 0;
			decimal_y.w = 0;
			check_key_released_on_jump = 0;
			bounce_on_coll = 1;
			SetSpriteAnim(player_ptr, anim_jump, ANIMATION_SPEED);
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
	speed_y = 0;
	jump_done = 0;
	
	decimal_x.w = 0;
	speed_x = 0;
	UPDATE();
}

void UpdateWalk() {
	INT8 desp_x = 0;
	if(KEY_PRESSED(J_LEFT)){
		desp_x = -1;
		TranslateSprite(THIS, -1, 0);
		THIS->mirror = V_MIRROR;
		SetSpriteAnim(THIS, anim_walk, ANIMATION_SPEED);
	} else if(KEY_PRESSED(J_RIGHT)){
		desp_x = 1;
		TranslateSprite(THIS, 1, 0);
		THIS->mirror = NO_MIRROR;
		SetSpriteAnim(THIS, anim_walk, ANIMATION_SPEED);
	} else {
		SetSpriteAnim(THIS, anim_idle, ANIMATION_SPEED);
	}

	if(KEY_PRESSED(J_A)){
		if(!jump_done) {
			SetPlayerState(STATE_FLYING);
			speed_x = desp_x << 8;
			speed_y = -900;
			check_key_released_on_jump = 1;
			bounce_on_coll = 0;
		}
	} else {
		jump_done = 0;
	}

	if(TranslateSprite(THIS, 0, 1) == 0) {
		SetPlayerState(STATE_FLYING);
		bounce_on_coll = 0;
		speed_x = desp_x << 8;
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

	if(KEY_PRESSED(J_A)) {
		SetPlayerState(STATE_FLYING);
		speed_x = (new_x - THIS->x) << 8;
		speed_y = ((new_y - THIS->y) << 8) << 1;
		//speed_y = -900;
		
		SpriteManagerRemoveSprite(hook_ptr);
	} else {
		if(TranslateSprite(THIS, new_x - THIS->x, new_y - THIS->y) != 0) {
			hook_speed = -hook_speed; //Bounce
			hook_radius.w -= rad_incr; //Cancel radius increment
			hook_ang = cached_ang; //Canel ang
		}
	}
}

void UpdateFlying() {
	if(speed_y < 900)
		speed_y += 30;

	if(KEY_PRESSED(J_LEFT)) {
		if(speed_x > -256) {
			speed_x -= THIS->mirror == NO_MIRROR ? 20 : 40;
		}
	} else if(KEY_PRESSED(J_RIGHT)) {
		if(speed_x < 256) {
			speed_x += THIS->mirror == NO_MIRROR ? 40 : 20;
		}
	} else {
		 speed_x -= speed_x >> 4;
	}
	if(check_key_released_on_jump){
		if(!KEY_PRESSED(J_A) && speed_y < 0) {
			speed_y = 0;
		}
	}

	decimal_x.w += speed_x;
	decimal_y.w += speed_y;

	if(TranslateSprite(THIS, decimal_x.h, 0) != 0) {
		if(bounce_on_coll) {
			speed_x = -speed_x;
		}
	}
	
	if(TranslateSprite(THIS, 0, decimal_y.h) != 0) {
		if((INT8)decimal_y.h > 0) {
			SetPlayerState(STATE_WALKING);
		}
		speed_y = 0;
	}
	decimal_x.h = 0;
	decimal_y.h = 0;
}

void UPDATE() {
	switch(player_state) {
		case STATE_WALKING:
			UpdateWalk();
			break;
		case STATE_HOOKED:
			UpdateHooked();
			break;
		case STATE_FLYING:
			UpdateFlying();
			break;
	}

	if(!hook_ptr && KEY_PRESSED(J_B)) {
		SpriteManagerAdd(SpriteHook, THIS->x, THIS->y);
	}
}

void DESTROY() {
}