#include "Banks/SetAutoBank.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "Keys.h"

IMPORT_MAP(timeup);

void START() {
	InitScroll(BANK(timeup), &timeup, 0, 0);
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
	}
}