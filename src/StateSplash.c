#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"

IMPORT_MAP(splash);

extern UINT8 current_level;

void START() {
	InitScroll(BANK(splash), &splash, 0, 0);
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		current_level = 0;

		SetState(StateGame);
	}
}