#include "Banks/SetAutoBank.h"
#include "Trig.h"
#include "SpriteManager.h"


const UINT8 anim_idle_menu[] = { 2, 0,1};

void START() {
	SetSpriteAnim(THIS, anim_idle_menu, 6u);
	THIS->mirror =V_MIRROR;

}

void UPDATE() 
{
	
}

void DESTROY() {
}