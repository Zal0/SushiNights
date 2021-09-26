#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"

IMPORT_MAP(gamewin);

void START() {
	InitScroll(BANK(gamewin), &gamewin, 0, 0);
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateSplash);
	}
}