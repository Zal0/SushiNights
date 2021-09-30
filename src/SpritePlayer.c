#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Keys.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include "Print.h"
#include "Sounds.h"

Sprite* player_ptr;
extern Sprite* hook_ptr;
extern UINT8 rope_length;

void RetireHook(Sprite* hook, INT8 ang, INT8 radius) BANKED;

//Walk
UINT16 safe_jump_x;
UINT16 safe_jump_y;

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
	STATE_FLYING,
	STATE_FALL_RESPAWN,
	STATE_DELIVERING_SUSHI,
	STATE_VICTORY,
} PLAYER_STATE;
PLAYER_STATE player_state = STATE_WALKING;

const UINT8 anim_idle[] = {3, 0,1,2};
const UINT8 anim_walk[] = {4, 3, 4,5,6};
const UINT8 anim_jump[] = {1, 7};
const UINT8 anim_hooked[] = { 3, 7,8,10 };
const UINT8 anim_transform[] = { 5, 11,12,13,12,13, 13, 13, 13}; //Change to victory loop once THIS->anim_frame ==4
const UINT8 anim_victory[] = { 2, 14,15 };
const UINT8 anim_respawn[] = {1, 16};
const UINT8 anim_happy[] = { 2, 17,18 };
const UINT8 ANIMATION_SPEED = 16; 

void SetPlayerState(PLAYER_STATE state) {
	player_state = state;

	switch(player_state) {
		case STATE_WALKING:
			bounce_on_coll = 0;
			break;

		case STATE_HOOKED:
			SetSpriteAnim(player_ptr, anim_hooked, ANIMATION_SPEED);
			break;

		case STATE_FLYING:
			speed_y = 0;
			decimal_y.w = 0;
			check_key_released_on_jump = 0;
			bounce_on_coll = 1;
			SetSpriteAnim(player_ptr, anim_jump, ANIMATION_SPEED);
			break;

		case STATE_FALL_RESPAWN:
			speed_x = 0;
			speed_y = 0;
			decimal_x.w = 0;
			decimal_y.w = 0;
			SetSpriteAnim(player_ptr, anim_respawn, ANIMATION_SPEED);
			break;

		case STATE_DELIVERING_SUSHI:
			SetSpriteAnim(player_ptr, anim_happy, ANIMATION_SPEED);
			break;

		case STATE_VICTORY:
			SetSpriteAnim(player_ptr, anim_transform, ANIMATION_SPEED);
			break;
	}
}

void HookPlayer(UINT16 x, UINT16 y, INT8 ang, UINT8 radius) BANKED {
	hook_x = x;
	hook_y = y;
	hook_radius.w = radius;
	hook_ang.h = ang > 0 ? 128 - (ang - 64) : (-64 - ang);
	hook_speed = 0;

	SpriteManagerAdd(SpriteBubble, x-6, y-5);


	SetPlayerState(STATE_HOOKED);
}

extern UINT8 clients_collected;
void CheckLevelComplete() BANKED;
void RefreshSushies() BANKED;
UINT8 deliver_countdown;
PLAYER_STATE cached_state;
UINT8* cached_anim;
void DeliverSushi(Sprite* client) BANKED {
	clients_collected ++;
	RefreshSushies();

	deliver_countdown = 60;
	
	if(hook_ptr && player_state != STATE_HOOKED) {
		//RetireHook(hook_ptr, hook_ang.h, hook_radius.w >> 1);
		SpriteManagerRemoveSprite(hook_ptr);
	}
	cached_state = player_state;
	cached_anim = player_ptr->anim_data;
	SetPlayerState(STATE_DELIVERING_SUSHI);
}

void ShowVictoryAnim() BANKED {
	SetPlayerState(STATE_VICTORY);
}

void START() {
	THIS->lim_x = 1000;
	THIS->lim_y = 1000;

	player_ptr = THIS;
	speed_x = 0;
	speed_y = 0;
	decimal_x.w = 0;
	decimal_y.w = 0;

	safe_jump_x = THIS->x;
	safe_jump_y = THIS->y;

	SetPlayerState(STATE_WALKING);
}


#define MAX_X_SPEED 256
#define X_SPEED_INCREMENT 40
#define X_SPEED_INCREMENT_OPPOSITE 20
#define DRAG (speed_x >> 4)

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
		if(bounce_on_coll)
			PlayFx(FX_HIT_WALL);
	}
	
	decimal_x.h = 0;

	if(speed_x) {
		THIS->mirror = speed_x > 0 ? NO_MIRROR : V_MIRROR;
	}
}

#define JUMP_SPEED 870

void UpdateWalk() {
	safe_jump_x = THIS->x;
	safe_jump_y = THIS->y;

	HorizontalMove();

	SetSpriteAnim(THIS, speed_x ? anim_walk : anim_idle, ANIMATION_SPEED);
	
	if(KEY_TICKED(J_A)){
		SetPlayerState(STATE_FLYING);
		PlayFx(FX_JUMP);
		speed_y = -JUMP_SPEED;
		check_key_released_on_jump = 1;
		bounce_on_coll = 0;
	} 

	//Check falling
	if(TranslateSprite(THIS, 0, 1) == 0) {
		SetPlayerState(STATE_FLYING);
		bounce_on_coll = 0;
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
			rad_incr = -3;
			hook_radius.w -= 3;
		}
	} else if(KEY_PRESSED(J_DOWN)){
		hook_radius.w += 3;
		if(hook_radius.w > (rope_length << 1))
			hook_radius.w = (rope_length << 1);
		rad_incr = 3;
	}

	//swing
	if((ang > 250 || ang < 6) && hook_speed > -HOOK_SWING_SPEED && hook_speed < HOOK_SPEED) {
		if(KEY_PRESSED(J_LEFT))
			hook_speed = -600;
		if(KEY_PRESSED(J_RIGHT))
			hook_speed = 600;
	} else {
		if(ang < 127 && hook_speed < 0 && KEY_PRESSED(J_LEFT))
			hook_speed -= HOOK_SWING_SPEED;
		if(ang > 128 && hook_speed > 0 && KEY_PRESSED(J_RIGHT))
			hook_speed += HOOK_SWING_SPEED;
	}
	
	//drag
	hook_speed -= HOOK_DRAG;

	hook_speed += (hook_ang.h > 128) ? HOOK_SPEED : -HOOK_SPEED;
	if(hook_speed > MAX_HOOK_SPEED) hook_speed = MAX_HOOK_SPEED;
	if(hook_speed < -MAX_HOOK_SPEED) hook_speed = -MAX_HOOK_SPEED;

	cached_ang = hook_ang;
	hook_ang.w += hook_speed;

	tmp_x.w = SIN(hook_ang.h) * hook_radius.w; //using SIN instead of cos because I am rotating the axis so 0 points down, 64 points right and -64 points left
	tmp_y.w = COS(hook_ang.h) * hook_radius.w;

	new_x = hook_x + (INT8)tmp_x.h - (THIS->coll_w >> 1);
	new_y = hook_y + (INT8)tmp_y.h;

	if(hook_speed > 256) THIS->mirror = NO_MIRROR;
	else if(hook_speed < -256) THIS->mirror = V_MIRROR;

	if(KEY_TICKED(J_B) || KEY_TICKED(J_A)) {
			SetPlayerState(STATE_FLYING);
			speed_x = (new_x - THIS->x) << 8;
			speed_y = ((new_y - THIS->y) << 8) << 1; //Multiplying by 2 gives the effect of the character jumping
			if(speed_y < -JUMP_SPEED)
				speed_y = -JUMP_SPEED;
		
			RetireHook(hook_ptr, hook_ang.h, hook_radius.w >> 1);
	} else {
		if(TranslateSprite(THIS, new_x - THIS->x, new_y - THIS->y) != 0) {
			hook_speed = -hook_speed; //Bounce
			hook_radius.w -= rad_incr; //Cancel radius increment
			hook_ang = cached_ang; //Canel ang
			PlayFx(FX_HIT_WALL);
		}
	}
}

#define MAX_Y_SPEED 900
#define GRAVITY 28

void UpdateFlying() {
	if(speed_y < MAX_Y_SPEED) {
		speed_y += GRAVITY;
	}

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

	if((INT16)THIS->y > (INT16)scroll_h) {
		SetPlayerState(STATE_FALL_RESPAWN);
	}
}

void UpdateFallRespawn() {
	if(THIS->x > safe_jump_x)
		THIS->x --;
	else if(THIS->x < safe_jump_x)
		THIS->x ++;

	if(THIS->y > safe_jump_y)
		THIS->y --;
	else if(THIS->y < safe_jump_y)
		THIS->y ++;

	if(THIS->x == safe_jump_x && THIS->y == safe_jump_y)
		SetPlayerState(STATE_WALKING);
}

void UpdateDeliveringSushi() {
	deliver_countdown --;
	if(deliver_countdown == 0) {
		CheckLevelComplete();
		
		if(player_state == STATE_DELIVERING_SUSHI) {
			player_state = cached_state;
			SetSpriteAnim(player_ptr, cached_anim, ANIMATION_SPEED);
		}
	}
}

void UpdateVictory() {
	if(THIS->anim_frame == 4) {
		SetSpriteAnim(THIS, anim_victory, 6u);
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
		case STATE_FLYING:
			UpdateFlying();
			break;
		case STATE_FALL_RESPAWN:
			UpdateFallRespawn();
			break;
		case STATE_DELIVERING_SUSHI:
			UpdateDeliveringSushi();
			break;
		case STATE_VICTORY:
			UpdateVictory();
			break;
	}

	if(KEY_TICKED(J_B) && player_state != STATE_FALL_RESPAWN && player_state != STATE_DELIVERING_SUSHI && !hook_ptr) {
		SpriteManagerAdd(SpriteHook, THIS->x, THIS->y);
	}
}

void DESTROY() {
}