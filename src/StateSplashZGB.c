#include "Banks/SetAutoBank.h"
#include "main.h"
#include "Scroll.h"
#include "Keys.h"
#include "ZGBMain.h"

IMPORT_MAP(splash_zgb);

static UINT8 n_frames;

void START() {
	n_frames = 0;
	InitScroll(BANK(splash_zgb), &splash_zgb, 0, 0);
}

void UPDATE() {
	n_frames ++;

	if(n_frames == 60 || KEY_TICKED(J_START)) {
		SetState(StateSplash);
	}
}