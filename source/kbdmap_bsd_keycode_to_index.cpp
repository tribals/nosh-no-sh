/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "kbdmap.h"
#include "kbdmap_utils.h"

/// The atkbd device keycode maps to a row+column in the current keyboard map, which contains an action for that row+column.
uint16_t
bsd_keycode_to_keymap_index (
	const uint16_t k
) {
	switch (k) {
		default:	break;
		case 0x01:	return KBDMAP_INDEX_ESC;
		case 0x02:	return KBDMAP_INDEX_1;
		case 0x03:	return KBDMAP_INDEX_2;
		case 0x04:	return KBDMAP_INDEX_3;
		case 0x05:	return KBDMAP_INDEX_4;
		case 0x06:	return KBDMAP_INDEX_5;
		case 0x07:	return KBDMAP_INDEX_6;
		case 0x08:	return KBDMAP_INDEX_7;
		case 0x09:	return KBDMAP_INDEX_8;
		case 0x0A:	return KBDMAP_INDEX_9;
		case 0x0B:	return KBDMAP_INDEX_0;
		case 0x0C:	return KBDMAP_INDEX_E11;		// minus
		case 0x0D:	return KBDMAP_INDEX_E12;		// equals
		case 0x0E:	return KBDMAP_INDEX_BACKSPACE;
		case 0x0F:	return KBDMAP_INDEX_TAB;
		case 0x10:	return KBDMAP_INDEX_Q;
		case 0x11:	return KBDMAP_INDEX_W;
		case 0x12:	return KBDMAP_INDEX_E;
		case 0x13:	return KBDMAP_INDEX_R;
		case 0x14:	return KBDMAP_INDEX_T;
		case 0x15:	return KBDMAP_INDEX_Y;
		case 0x16:	return KBDMAP_INDEX_U;
		case 0x17:	return KBDMAP_INDEX_I;
		case 0x18:	return KBDMAP_INDEX_O;
		case 0x19:	return KBDMAP_INDEX_P;
		case 0x1A:	return KBDMAP_INDEX_D11;		// left brace
		case 0x1B:	return KBDMAP_INDEX_D12;		// right brace
		case 0xE01C:	return KBDMAP_INDEX_KP_ENTER;
		case 0x1C:	return KBDMAP_INDEX_RETURN;
		case 0xE01D:	return KBDMAP_INDEX_CONTROL2;
		case 0x1D:	return KBDMAP_INDEX_CONTROL1;
		case 0x1E:	return KBDMAP_INDEX_A;
		case 0x1F:	return KBDMAP_INDEX_S;
		case 0x20:	return KBDMAP_INDEX_D;
		case 0x21:	return KBDMAP_INDEX_F;
		case 0x22:	return KBDMAP_INDEX_G;
		case 0x23:	return KBDMAP_INDEX_H;
		case 0x24:	return KBDMAP_INDEX_J;
		case 0x25:	return KBDMAP_INDEX_K;
		case 0x26:	return KBDMAP_INDEX_L;
		case 0x27:	return KBDMAP_INDEX_C10;		// semicolon
		case 0x28:	return KBDMAP_INDEX_C11;		// apostrophe
		case 0x29:	return KBDMAP_INDEX_E00;		// grave
		case 0x2A:	return KBDMAP_INDEX_SHIFT1;
		case 0x2B:	return KBDMAP_INDEX_C12;		// Europe1
		case 0x2C:	return KBDMAP_INDEX_Z;
		case 0x2D:	return KBDMAP_INDEX_X;
		case 0x2E:	return KBDMAP_INDEX_C;
		case 0x2F:	return KBDMAP_INDEX_V;
		case 0x30:	return KBDMAP_INDEX_B;
		case 0x31:	return KBDMAP_INDEX_N;
		case 0x32:	return KBDMAP_INDEX_M;
		case 0x33:	return KBDMAP_INDEX_B08;		// comma
		case 0x34:	return KBDMAP_INDEX_B09;		// dot
		case 0xE035:	return KBDMAP_INDEX_KP_SLASH;
		case 0x35:	return KBDMAP_INDEX_B10;		// slash1
		case 0x36:	return KBDMAP_INDEX_SHIFT2;
		case 0xE037:	return KBDMAP_INDEX_PRINT_SCREEN;	// PC/AT Print Screen (was Shift-Numpad-Asterisk on the PC/XT)
		case 0x37:	return KBDMAP_INDEX_KP_ASTERISK;
		case 0xE038:	return KBDMAP_INDEX_OPTION;
		case 0x38:	return KBDMAP_INDEX_ALT;
		case 0x39:	return KBDMAP_INDEX_SPACE;
		case 0x3A:	return KBDMAP_INDEX_CAPSLOCK;
		case 0x3B:	return KBDMAP_INDEX_F1;
		case 0x3C:	return KBDMAP_INDEX_F2;
		case 0x3D:	return KBDMAP_INDEX_F3;
		case 0x3E:	return KBDMAP_INDEX_F4;
		case 0x3F:	return KBDMAP_INDEX_F5;
		case 0x40:	return KBDMAP_INDEX_F6;
		case 0x41:	return KBDMAP_INDEX_F7;
		case 0x42:	return KBDMAP_INDEX_F8;
		case 0x43:	return KBDMAP_INDEX_F9;
		case 0x44:	return KBDMAP_INDEX_F10;
		case 0x45:	return KBDMAP_INDEX_NUMLOCK;
		case 0xE046:	return KBDMAP_INDEX_PAUSE;		// PC/AT Pause
		case 0x46:	return KBDMAP_INDEX_SCROLLLOCK;
		case 0xE047:	return KBDMAP_INDEX_HOME;		// PC/AT editing keypad
		case 0x47:	return KBDMAP_INDEX_KP_7;
		case 0xE048:	return KBDMAP_INDEX_UP_ARROW;		// PC/AT cursor keypad
		case 0x48:	return KBDMAP_INDEX_KP_8;
		case 0xE049:	return KBDMAP_INDEX_PAGE_UP;		// PC/AT editing keypad
		case 0x49:	return KBDMAP_INDEX_KP_9;
		case 0x4A:	return KBDMAP_INDEX_KP_MINUS;
		case 0xE04B:	return KBDMAP_INDEX_LEFT_ARROW;		// PC/AT cursor keypad
		case 0x4B:	return KBDMAP_INDEX_KP_4;
		case 0x4C:	return KBDMAP_INDEX_KP_5;
		case 0xE04D:	return KBDMAP_INDEX_RIGHT_ARROW;	// PC/AT cursor keypad
		case 0x4D:	return KBDMAP_INDEX_KP_6;
		case 0x4E:	return KBDMAP_INDEX_KP_PLUS;
		case 0xE04F:	return KBDMAP_INDEX_END;		// PC/AT editing keypad
		case 0x4F:	return KBDMAP_INDEX_KP_1;
		case 0xE050:	return KBDMAP_INDEX_DOWN_ARROW;		// PC/AT cursor keypad
		case 0x50:	return KBDMAP_INDEX_KP_2;
		case 0xE051:	return KBDMAP_INDEX_PAGE_DOWN;		// PC/AT editing keypad
		case 0x51:	return KBDMAP_INDEX_KP_3;
		case 0xE052:	return KBDMAP_INDEX_INSERT;		// PC/AT editing keypad
		case 0x52:	return KBDMAP_INDEX_KP_0;
		case 0xE053:	return KBDMAP_INDEX_DELETE;		// PC/AT editing keypad
		case 0x53:	return KBDMAP_INDEX_KP_DECIMAL;
		case 0x54:	return KBDMAP_INDEX_ATTENTION;		// Alt-PrtScn
		case 0x56:	return KBDMAP_INDEX_B00;		// europe2
		case 0x57:	return KBDMAP_INDEX_F11;
		case 0x58:	return KBDMAP_INDEX_F12;
		// FreeBSD's atkbdc driver sometimes translates these from the 0xE0nn codes.
		case 0x59:	return KBDMAP_INDEX_KP_ENTER;
		case 0x5A:	return KBDMAP_INDEX_CONTROL2;
		case 0x5B:	return KBDMAP_INDEX_KP_SLASH;
		case 0x5C:	return KBDMAP_INDEX_PRINT_SCREEN;	// PC/AT Print Screen (was Shift-Numpad-Asterisk on the PC/XT)
		case 0x5D:	return KBDMAP_INDEX_OPTION;
		case 0x5E:	return KBDMAP_INDEX_HOME;
		case 0x5F:	return KBDMAP_INDEX_UP_ARROW;
		case 0x60:	return KBDMAP_INDEX_PAGE_UP;
		case 0x61:	return KBDMAP_INDEX_LEFT_ARROW;
		case 0x62:	return KBDMAP_INDEX_RIGHT_ARROW;
		case 0x63:	return KBDMAP_INDEX_END;
		case 0x64:	return KBDMAP_INDEX_DOWN_ARROW;
		case 0x65:	return KBDMAP_INDEX_PAGE_DOWN;
		case 0x66:	return KBDMAP_INDEX_INSERT;
		case 0x67:	return KBDMAP_INDEX_DELETE;
		case 0x68:	return KBDMAP_INDEX_PAUSE;		// PC/AT Pause (was Ctrl-NumLock on the PC/XT)
		case 0xE05B:	// Microsoft keyboards de facto standard
		case 0x69:	return KBDMAP_INDEX_SUPER1;
		case 0xE05C:	// Microsoft keyboards de facto standard
		case 0x6A:	return KBDMAP_INDEX_SUPER2;
		case 0xE05D:	// Microsoft keyboards de facto standard
		case 0x6B:	return KBDMAP_INDEX_POPUP_MENU;
		case 0x6C:	return KBDMAP_INDEX_STOP_OR_BREAK;	// Ctrl-Pause/Ctrl-ScrollLock
		case 0xE05E:	// Microsoft keyboards de facto standard
		case 0x6D:	return KBDMAP_INDEX_POWER;
		case 0xE05F:	// Microsoft keyboards de facto standard
		case 0x6E:	return KBDMAP_INDEX_SLEEP;
		case 0xE063:	// Microsoft keyboards de facto standard
		case 0x6F:	return KBDMAP_INDEX_WAKE;
		case 0xE11D:	return KBDMAP_INDEX_CONTROL3;		// FakcCtrl used by PC/AT for Ctrl+Break
		// FreeBSD's atkbdc driver does not generate codes beyond this point.
		// SCO Unix went up to 0x7F in its keyboard(7) doco, but this conflicts with real FreeBSD keymaps.
		case 0x70:	return KBDMAP_INDEX_KATAKANA_HIRAGANA;	// Intl2
		case 0x73:	return KBDMAP_INDEX_B11;		// slash2, Intl1
		case 0x77:	return KBDMAP_INDEX_HIRAGANA;		// Lang4
		case 0x78:	return KBDMAP_INDEX_KATAKANA;		// Lang3
		case 0x79:	return KBDMAP_INDEX_HENKAN;		// Intl4
		case 0x7B:	return KBDMAP_INDEX_MUHENKAN;		// Intl5
		case 0x7D:	return KBDMAP_INDEX_E13;		// 109: Yen, Intl3
		case 0x7E:	return KBDMAP_INDEX_KP_THOUSANDS;	// 107: KP .
		// de facto standard media keys (Logitech/Microsoft)
		case 0xE010:	return KBDMAP_INDEX_PREVIOUS_TRACK;
		case 0xE019:	return KBDMAP_INDEX_NEXT_TRACK;
		case 0xE020:	return KBDMAP_INDEX_MUTE;
		case 0xE022:	return KBDMAP_INDEX_PLAY_PAUSE;
		case 0xE024:	return KBDMAP_INDEX_STOP_PLAYING;
		case 0xE02E:	return KBDMAP_INDEX_VOLUME_DOWN;
		case 0xE030:	return KBDMAP_INDEX_VOLUME_UP;
		case 0xE06D:	return KBDMAP_INDEX_MEDIA_PLAYER;
		// de facto standard Internet/Office keys (Logitech/Microsoft)
		case 0xE021:	return KBDMAP_INDEX_CALCULATOR;
		case 0xE032:	return KBDMAP_INDEX_WWW_BROWSER;
		case 0xE03B:	return KBDMAP_INDEX_FN_F1;		// untested with a real keyboard
		case 0xE03C:	return KBDMAP_INDEX_FN_F2;		// untested with a real keyboard
		case 0xE03D:	return KBDMAP_INDEX_FN_F3;		// untested with a real keyboard
		case 0xE03E:	return KBDMAP_INDEX_FN_F4;		// untested with a real keyboard
		case 0xE03F:	return KBDMAP_INDEX_FN_F5;		// untested with a real keyboard
		case 0xE040:	return KBDMAP_INDEX_FN_F6;		// untested with a real keyboard
		case 0xE041:	return KBDMAP_INDEX_FN_F7;		// untested with a real keyboard
		case 0xE042:	return KBDMAP_INDEX_FN_F8;		// untested with a real keyboard
		case 0xE043:	return KBDMAP_INDEX_FN_F9;		// untested with a real keyboard
		case 0xE044:	return KBDMAP_INDEX_FN_F10;		// untested with a real keyboard
		case 0xE057:	return KBDMAP_INDEX_FN_F11;		// untested with a real keyboard
		case 0xE058:	return KBDMAP_INDEX_FN_F12;		// untested with a real keyboard
		case 0xE065:	return KBDMAP_INDEX_SEARCH;
		case 0xE066:	return KBDMAP_INDEX_BOOKMARKS;
		case 0xE067:	return KBDMAP_INDEX_REFRESH;
		case 0xE068:	return KBDMAP_INDEX_STOP_LOADING;
		case 0xE069:	return KBDMAP_INDEX_APP_FORWARD;
		case 0xE06A:	return KBDMAP_INDEX_APP_BACK;
		case 0xE06B:	return KBDMAP_INDEX_MY_COMPUTER;
		case 0xE06C:	return KBDMAP_INDEX_MAIL;
		// keys from old "Office" keyboards that I have not been able to test
		case 0xE005:	return KBDMAP_INDEX_FILE_MANAGER;	// untested with a real keyboard
		case 0xE007:	return KBDMAP_INDEX_REDO;		// untested with a real keyboard
		case 0xE008:	return KBDMAP_INDEX_UNDO;		// untested with a real keyboard
		case 0xE009:	return KBDMAP_INDEX_APP_LEFT;		// untested with a real keyboard
		case 0xE00A:	return KBDMAP_INDEX_PASTE;		// untested with a real keyboard
		case 0xE011:	return KBDMAP_INDEX_INSTANT_MESSAGING;	// untested with a real keyboard
		case 0xE013:	return KBDMAP_INDEX_WORDPROCESSOR;	// untested with a real keyboard
		case 0xE014:	return KBDMAP_INDEX_SPREADSHEET;	// untested with a real keyboard
		case 0xE015:	return KBDMAP_INDEX_CALENDAR;		// untested with a real keyboard
		case 0xE016:	return KBDMAP_INDEX_LOGOFF;		// untested with a real keyboard
		case 0xE017:	return KBDMAP_INDEX_CUT;		// untested with a real keyboard
		case 0xE018:	return KBDMAP_INDEX_COPY;		// untested with a real keyboard
		case 0xE01E:	return KBDMAP_INDEX_APP_RIGHT;		// untested with a real keyboard
		case 0xE023:	return KBDMAP_INDEX_SPELL;		// untested with a real keyboard
		case 0xE064:	return KBDMAP_INDEX_MY_PICTURES;	// untested with a real keyboard
		// keys from old "access" keyboards that I have not been able to test
		case 0xE012:	break;	// CD forward/find/webcam
		case 0xE025:	break;	// Internet/Logitech
		case 0xE026:	break;	// Internet shopping/add favourite
		case 0xE02F:	break;	// eject
		case 0xE078:	break;	// record
		case 0xE07A:	break;	// WWW
	}
	return 0xFFFF;
}
