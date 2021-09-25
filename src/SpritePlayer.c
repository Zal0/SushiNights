#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"

Sprite* player_ptr;
extern Sprite* hook_ptr;
extern UINT8 rope_length;

void RetireHook(Sprite* hook, INT8 ang) BANKED;

//Walk
UINT8 jump_done;

//Hooked
fixed hook_radius;
UINT16 hook_x, hook_y;
fixed hook_ang;
INT16 hook_speed;

//Flying
INT16 speed_x;
INT16 speed_y;
fixed decimal_x;
fixed decimal_y;
UINT8 check_key_released_on_jump; //when J_A is no longer pressed, and speed going up reset speed to stop jump
UINT8 bounce_on_coll; //Bouncing after making a jump doesn't look good

typedef enum {
	STATE_WALKING,
	STATE_HOOKED,
	STATE_FLYING
} PLAYER_STATE;
PLAYER_STATE player_state = STATE_WALKING;

const UINT8 anim_idle[] = {1, 0};
const UINT8 anim_walk[] = {2, 0, 1};
const UINT8 anim_jump[] = {1, 1};
const UINT8 anim_hooked[] = {1, 2};
 
void SetPlayerState(PLAYER_STATE state) {
	player_state = state;

	switch(player_state) {
		case STATE_WALKING:
			jump_done = KEY_PRESSED(J_A);
			bounce_on_coll = 0;
			break;

		case STATE_HOOKED:
			SetSpriteAnim(player_ptr, anim_hooked, 6);
			break;

		case STATE_FLYING:
			//speed_x = 0;
			speed_y = 0;
			//decimal_x.w = 0;
			decimal_y.w = 0;
			check_key_released_on_jump = 0;
			bounce_on_coll = 1;
			SetSpriteAnim(player_ptr, anim_jump, 6);
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

void START() {
	player_ptr = THIS;
	SetPlayerState(STATE_WALKING);
}


#define MAX_X_SPEED 256
#define X_SPEED_INCREMENT 40
#define X_SPEED_INCREMENT_OPPOSITE 20
#define DRAG (speed_x >> 3)

void HorizontalMove() {
	if(KEY_PRESSED(J_LEFT)) {
		if(speed_x > -MAX_X_SPEED) {
			speed_x -= THIS->mirror == NO_MIRROR ? X_SPEED_INCREMENT_OPPOSITE : X_SPEED_INCREMENT;
		}
	} else if(KEY_PRESSED(J_RIGHT)) {
		if(speed_x < MAX_X_SPEED) {
			speed_x += THIS->mirror == NO_MIRROR ? X_SPEED_INCREMENT : X_SPEED_INCREMENT_OPPOSITE;
		}
	} else if(DRAG == 0) {
		speed_x = 0;
	}
	
	//Drag
	speed_x -= DRAG;

	decimal_x.w += speed_x;

	if(TranslateSprite(THIS, decimal_x.h, 0) != 0) {
		speed_x = bounce_on_coll ? -speed_x : 0;
	}
	
	decimal_x.h = 0;
}

#define JUMP_SPEED 900

void UpdateWalk() {
	HorizontalMove();

	if(speed_x) {
		THIS->mirror = speed_x > 0 ? NO_MIRROR : V_MIRROR;
	}
	SetSpriteAnim(THIS, speed_x ? anim_walk : anim_idle, 16);
	
	if(KEY_PRESSED(J_A)){
		if(!jump_done) {
			SetPlayerState(STATE_FLYING);
			//speed_x = desp_x << 8;
			speed_y = -JUMP_SPEED;
			check_key_released_on_jump = 1;
			bounce_on_coll = 0;
		}
	} else {
		jump_done = 0;
	}

	//Check falling
	if(TranslateSprite(THIS, 0, 1) == 0) {
		SetPlayerState(STATE_FLYING);
		bounce_on_coll = 0;
		//speed_x = desp_x << 8;
	}
}


#define HOOK_SWING_SPEED 20
#define HOOK_SPEED 20
#define MAX_HOOK_SPEED 1000
#define HOOK_DRAG (hook_speed >> 7)

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
		if(hook_radius.w > (rope_length << 1))
			hook_radius.w = (rope_length << 1);
		rad_incr = 1;
	}

	//swing
	if(ang < 127 && hook_speed < 0 && KEY_PRESSED(J_LEFT))
		hook_speed -= HOOK_SWING_SPEED;
	if(ang > 128 && hook_speed > 0 && KEY_PRESSED(J_RIGHT))
		hook_speed += HOOK_SWING_SPEED;

	//drag
	hook_speed -= HOOK_DRAG;

	hook_speed += (hook_ang.h > 128) ? HOOK_SPEED : -HOOK_SPEED;
	if(hook_speed > MAX_HOOK_SPEED) hook_speed = MAX_HOOK_SPEED;
	if(hook_speed < -MAX_HOOK_SPEED) hook_speed = -MAX_HOOK_SPEED;

	cached_ang = hook_ang;
	hook_ang.w += hook_speed ;

	tmp_x.w = SIN(hook_ang.h) * hook_radius.w; //using SIN instead of cos because I am rotating the axis so 0 points down, 64 points right and -64 points left
	tmp_y.w = COS(hook_ang.h) * hook_radius.w;

	new_x = hook_x + (INT8)tmp_x.h - (THIS->coll_w >> 1);
	new_y = hook_y + (INT8)tmp_y.h;

	if(KEY_PRESSED(J_A)) {
		SetPlayerState(STATE_FLYING);
		speed_x = (new_x - THIS->x) << 8;
		speed_y = ((new_y - THIS->y) << 8) << 1; //Multiplying by 2 gives the effect of the character jumping
		if(speed_y < -JUMP_SPEED)
			speed_y = -JUMP_SPEED;
		
		//SpriteManagerRemoveSprite(hook_ptr);
		//hook_ang.h = ang > 0 ? 128 - (ang - 64) : (-64 - ang);
		RetireHook(hook_ptr, hook_ang.h);
	} else {
		if(TranslateSprite(THIS, new_x - THIS->x, new_y - THIS->y) != 0) {
			hook_speed = -hook_speed; //Bounce
			hook_radius.w -= rad_incr; //Cancel radius increment
			hook_ang = cached_ang; //Canel ang
		}
	}
}

#define MAX_Y_SPEED 900
#define GRAVITY 30

void UpdateFlying() {
	if(speed_y < MAX_Y_SPEED)
		speed_y += GRAVITY;

	HorizontalMove();

	if(check_key_released_on_jump){
		if(!KEY_PRESSED(J_A) && speed_y < 0) {
			speed_y = 0;
		}
	}

	decimal_y.w += speed_y;
	
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