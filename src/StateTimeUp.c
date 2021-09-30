#include "Banks/SetAutoBank.h"
#include "Scroll.h"
#include "ZGBMain.h"
#include "Keys.h"
#include "Music.h"

IMPORT_MAP(gameover);
DECLARE_MUSIC(gameover);

void START() {
	InitScroll(BANK(gameover), &gameover, 0, 0);
	PlayMusic(gameover, 1);

}

void UPDATE() {
	if(KEY_TICKED(J_START)) {
		SetState(StateGame);
	}
}