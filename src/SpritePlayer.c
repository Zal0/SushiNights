#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"
#include "Keys.h"

fixed radius;
UINT8 ang = 0;
UINT8 frame = 0;
fixed max_ang;

void UPDATE();
void START() {
	radius.w = 80;
	max_ang.w = 0;//200 << 8;
	UPDATE();
}

void UPDATE() {
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

void DESTROY() {
}