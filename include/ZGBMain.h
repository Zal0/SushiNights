#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES \
_STATE(StateSplashZGB)\
_STATE(StateSplash)\
_STATE(StateGame)\
_STATE(StateGameWin)\
_STATE(StateTimeUp)\
STATE_DEF_END

#define SPRITES \
_SPRITE_DMG(SpritePlayer, player)\
_SPRITE_DMG(SpriteHook, hook)\
_SPRITE_DMG(SpritePowerupRope, powerup_rope)\
_SPRITE_DMG(SpriteClient, client)\
_SPRITE_DMG(SpriteSushi, sushi)\
_SPRITE_DMG(SpritePlayerMenu, playermenu)\
_SPRITE_DMG(SpriteBubble, bubble)\
SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif