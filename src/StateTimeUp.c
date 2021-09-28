#include "Banks/SetAutoBank.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "Keys.h"

IMPORT_MAP(gameover);

void START() {
	InitScroll(BANK(gameover), &gameover, 0, 0);
}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
	}
}