/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_KBDMAP_H)
#define INCLUDE_KBDMAP_H

#include <stdint.h>

enum { KBDMAP_ROWS = 0x13, KBDMAP_COLS = 0x10 };

/// The entries in a keyboard map file are 10 bigendian 32-bit integers that unpack to this.
struct kbdmap_entry {
	uint32_t cmd,p[16];
};

typedef kbdmap_entry KeyboardMap[KBDMAP_ROWS][KBDMAP_COLS];

/// \brief keyboard map action types
enum {
	KBDMAP_ACTION_MASK	= 0xFF000000,
	KBDMAP_ACTION_UCS3	= 0x01000000,
	// Don't clash with 0x11000000 being accelerator UCS3 in input messages.
	KBDMAP_ACTION_SYSTEM	= 0x02000000,
	KBDMAP_ACTION_MODIFIER	= 0x03000000,
	// Don't clash with 0x04000000 being X mouse positions in input messages.
	// Don't clash with 0x05000000 being Y mouse positions in input messages.
	// Don't clash with 0x06000000 being mouse wheel in input messages.
	// Don't clash with 0x07000000 being mouse buttons in input messages.
	// Don't clash with 0x09000000 being pasted UCS3 in input messages.
	KBDMAP_ACTION_SCREEN	= 0x0A000000,
	KBDMAP_ACTION_CONSUMER	= 0x0C000000,
	KBDMAP_ACTION_EXTENDED	= 0x0E000000,
	KBDMAP_ACTION_EXTENDED1	= 0x1E000000,
	KBDMAP_ACTION_FUNCTION	= 0x0F000000,
	KBDMAP_ACTION_FUNCTION1	= 0x1F000000,
};

/// \brief bitflag numbering for modifier keys
enum {
	KBDMAP_MODIFIER_1ST_LEVEL2	=  0U,
	KBDMAP_MODIFIER_1ST_LEVEL3	=  1U,
	KBDMAP_MODIFIER_1ST_GROUP2	=  2U,
	KBDMAP_MODIFIER_1ST_CONTROL	=  3U,
	KBDMAP_MODIFIER_1ST_SUPER	=  4U,
	KBDMAP_MODIFIER_1ST_ALT		=  5U,
	KBDMAP_MODIFIER_1ST_META	=  6U,
	// 7U: Reserved for future expansion.
	KBDMAP_MODIFIER_2ND_LEVEL2	=  8U,
	KBDMAP_MODIFIER_2ND_LEVEL3	=  9U,	// Some FreeBSD keyboard maps pretend that there are two distinct level 3 shift keys.
	// 10U: No-one has a 2nd group 2 shift key, but reserve this bit for one.
	KBDMAP_MODIFIER_2ND_CONTROL	= 11U,
	KBDMAP_MODIFIER_2ND_SUPER	= 12U,
	// 13U No-one has a 2nd Alt key, but reserve this bit for one.
	KBDMAP_MODIFIER_2ND_META	= 14U,
	// 15U: Reserved for future expansion.
	// 16U: No-one has a 3rd level 2 shift key, but reserve this bit for one.
	// 17U: No-one has a 3rd level 3 shift key, but reserve this bit for one.
	// 18U: No-one has a 3rd group 2 shift key, but reserve this bit for one.
	KBDMAP_MODIFIER_3RD_CONTROL	= 19U,	// PC AT and PS/2 keyboards pretend that they have three control keys.
	// 20U: Reserved for future expansion.
	// 21U: Reserved for future expansion.
	// 22U: Reserved for future expansion.
	// 23U: Reserved for future expansion.
	KBDMAP_MODIFIER_CAPS		= 24U,
	KBDMAP_MODIFIER_NUM		= 25U,
	KBDMAP_MODIFIER_SCROLL		= 26U,
	KBDMAP_MODIFIER_LEVEL2		= 27U,	// A modifier that gets locked on, so distinct from the momentary modifiers.
	KBDMAP_MODIFIER_LEVEL3		= 28U,	// A modifier that gets locked on, so distinct from the momentary modifiers.
	KBDMAP_TOTAL_MODIFIERS
};

/// \brief subcommand types for modifier actions
enum {
	KBDMAP_MODIFIER_CMD_MOMENTARY	= 0x01,	///< a momentary contact version of the modifier
	KBDMAP_MODIFIER_CMD_LATCH	= 0x02,	///< a latching version of the modifier
	KBDMAP_MODIFIER_CMD_LOCK	= 0x03,	///< a locking version of the modifier
};

/// \brief keyboard map indices
enum {
	// ISO 9995 "E" row
	KBDMAP_INDEX_ESC		= 0x0000,	// traditional position
	KBDMAP_INDEX_1			= 0x0001,
	KBDMAP_INDEX_2			= 0x0002,
	KBDMAP_INDEX_3			= 0x0003,
	KBDMAP_INDEX_4			= 0x0004,
	KBDMAP_INDEX_5			= 0x0005,
	KBDMAP_INDEX_6			= 0x0006,
	KBDMAP_INDEX_7			= 0x0007,
	KBDMAP_INDEX_8			= 0x0008,
	KBDMAP_INDEX_9			= 0x0009,
	KBDMAP_INDEX_0			= 0x000A,
	KBDMAP_INDEX_E11		= 0x000B,	// minus
	KBDMAP_INDEX_E12		= 0x000C,	// equals
	KBDMAP_INDEX_E13		= 0x000D,	// 109: Yen
	KBDMAP_INDEX_BACKSPACE		= 0x000F,

	// ISO 9995 "D" row
	KBDMAP_INDEX_TAB		= 0x0100,
	KBDMAP_INDEX_Q			= 0x0101,
	KBDMAP_INDEX_W			= 0x0102,
	KBDMAP_INDEX_E			= 0x0103,
	KBDMAP_INDEX_R			= 0x0104,
	KBDMAP_INDEX_T			= 0x0105,
	KBDMAP_INDEX_Y			= 0x0106,
	KBDMAP_INDEX_U			= 0x0107,
	KBDMAP_INDEX_I			= 0x0108,
	KBDMAP_INDEX_O			= 0x0109,
	KBDMAP_INDEX_P			= 0x010A,
	KBDMAP_INDEX_D11		= 0x010B,	// left brace
	KBDMAP_INDEX_D12		= 0x010C,	// right brace
	KBDMAP_INDEX_RETURN		= 0x010F,

	// ISO 9995 "C" row
	KBDMAP_INDEX_A			= 0x0201,
	KBDMAP_INDEX_S			= 0x0202,
	KBDMAP_INDEX_D			= 0x0203,
	KBDMAP_INDEX_F			= 0x0204,
	KBDMAP_INDEX_G			= 0x0205,
	KBDMAP_INDEX_H			= 0x0206,
	KBDMAP_INDEX_J			= 0x0207,
	KBDMAP_INDEX_K			= 0x0208,
	KBDMAP_INDEX_L			= 0x0209,
	KBDMAP_INDEX_C10		= 0x020A,	// semicolon; Brazilian C
	KBDMAP_INDEX_C11		= 0x020B,	// apostrophe
	KBDMAP_INDEX_E00		= 0x020C,	// grave; 105: `¬| 109: Kanji/Zenkaku/Hankaku
	KBDMAP_INDEX_C12		= 0x020D,	// Europe1; 104: \| (at D13), 105+107: #~

	// ISO 9995 "B" row
	KBDMAP_INDEX_B00		= 0x0301,	// Europe2; 105: \|, 107: \|
	KBDMAP_INDEX_Z			= 0x0302,
	KBDMAP_INDEX_X			= 0x0303,
	KBDMAP_INDEX_C			= 0x0304,
	KBDMAP_INDEX_V			= 0x0305,
	KBDMAP_INDEX_B			= 0x0306,
	KBDMAP_INDEX_N			= 0x0307,
	KBDMAP_INDEX_M			= 0x0308,
	KBDMAP_INDEX_B08		= 0x0309,
	KBDMAP_INDEX_B09		= 0x030A,
	KBDMAP_INDEX_B10		= 0x030B,	// slash1; 107: ;:, 104+106+109: /?
	KBDMAP_INDEX_B11		= 0x030C,	// slash2; 107: /?, 109: "_ | \ Z"

	// Modifier row
	KBDMAP_INDEX_SHIFT1		= 0x0400,
	KBDMAP_INDEX_SHIFT2		= 0x0401,
	KBDMAP_INDEX_OPTION		= 0x0402,
	KBDMAP_INDEX_CONTROL1		= 0x0403,
	KBDMAP_INDEX_CONTROL2		= 0x0404,
	KBDMAP_INDEX_CONTROL3		= 0x0405,
	KBDMAP_INDEX_SUPER1		= 0x0406,
	KBDMAP_INDEX_SUPER2		= 0x0407,
	KBDMAP_INDEX_ALT		= 0x0408,
	KBDMAP_INDEX_CAPSLOCK		= 0x040C,
	KBDMAP_INDEX_SCROLLLOCK		= 0x040D,
	KBDMAP_INDEX_NUMLOCK		= 0x040E,

	// ISO 9995 "A" row
	KBDMAP_INDEX_KATAKANA_HIRAGANA	= 0x0501,
	KBDMAP_INDEX_ZENKAKU_HANKAKU	= 0x0502,	// not a PC keyboard key
	KBDMAP_INDEX_HIRAGANA		= 0x0503,	// not a PC keyboard key
	KBDMAP_INDEX_KATAKANA		= 0x0504,	// not a PC keyboard key
	KBDMAP_INDEX_HENKAN		= 0x0505,	// xfer	on PC-98
	KBDMAP_INDEX_MUHENKAN		= 0x0506,	// nfer	on PC-98
	KBDMAP_INDEX_HAN_YEONG		= 0x0508,
	KBDMAP_INDEX_HANJA		= 0x0509,
	KBDMAP_INDEX_ALTERNATE_ERASE	= 0x050D,
	KBDMAP_INDEX_COMPOSE		= 0x050E,
	KBDMAP_INDEX_SPACE		= 0x050F,

	// Cursor/editing keypad
	KBDMAP_INDEX_HOME		= 0x0600,
	KBDMAP_INDEX_UP_ARROW		= 0x0601,
	KBDMAP_INDEX_PAGE_UP		= 0x0602,
	KBDMAP_INDEX_LEFT_ARROW		= 0x0603,
	KBDMAP_INDEX_RIGHT_ARROW	= 0x0604,
	KBDMAP_INDEX_END		= 0x0605,
	KBDMAP_INDEX_DOWN_ARROW		= 0x0606,
	KBDMAP_INDEX_PAGE_DOWN		= 0x0607,
	KBDMAP_INDEX_INSERT		= 0x0608,
	KBDMAP_INDEX_DELETE		= 0x0609,
	KBDMAP_INDEX_CUT		= 0x060A,	// Sun+Microsoft "Office" keyboards
	KBDMAP_INDEX_COPY		= 0x060B,	// Sun+Microsoft "Office" keyboards
	KBDMAP_INDEX_PASTE		= 0x060C,	// Sun+Microsoft "Office" keyboards
	KBDMAP_INDEX_FIND		= 0x060D,	// Sun keyboard
	KBDMAP_INDEX_UNDO		= 0x060E,	// Sun+Microsoft "Office" keyboards
	KBDMAP_INDEX_REDO		= 0x060F,	// Microsoft "Office" keyboard

	// Calculator keypad part 1
	KBDMAP_INDEX_KP_ASTERISK	= 0x0700,
	KBDMAP_INDEX_KP_7		= 0x0701,
	KBDMAP_INDEX_KP_8		= 0x0702,
	KBDMAP_INDEX_KP_9		= 0x0703,
	KBDMAP_INDEX_KP_MINUS		= 0x0704,
	KBDMAP_INDEX_KP_4		= 0x0705,
	KBDMAP_INDEX_KP_5		= 0x0706,
	KBDMAP_INDEX_KP_6		= 0x0707,
	KBDMAP_INDEX_KP_PLUS		= 0x0708,
	KBDMAP_INDEX_KP_1		= 0x0709,
	KBDMAP_INDEX_KP_2		= 0x070A,
	KBDMAP_INDEX_KP_3		= 0x070B,
	KBDMAP_INDEX_KP_0		= 0x070C,
	KBDMAP_INDEX_KP_DECIMAL		= 0x070D,
	KBDMAP_INDEX_KP_ENTER		= 0x070E,
	KBDMAP_INDEX_KP_SLASH		= 0x070F,

	// Calculator keypad part 2
	KBDMAP_INDEX_KP_THOUSANDS	= 0x0800,
	KBDMAP_INDEX_KP_JPCOMMA		= 0x0801,
	KBDMAP_INDEX_KP_EQUALS		= 0x0802,
	KBDMAP_INDEX_KP_AS400_EQUALS	= 0x0803,
	KBDMAP_INDEX_KP_SIGN		= 0x0804,
	KBDMAP_INDEX_KP_LBRACKET	= 0x0805,
	KBDMAP_INDEX_KP_RBRACKET	= 0x0806,
	KBDMAP_INDEX_KP_LBRACE		= 0x0807,
	KBDMAP_INDEX_KP_RBRACE		= 0x0808,

	// Function keypad part 1
	KBDMAP_INDEX_F0			= 0x0900,	// for consistency
	KBDMAP_INDEX_F1			= 0x0901,
	KBDMAP_INDEX_F2			= 0x0902,
	KBDMAP_INDEX_F3			= 0x0903,
	KBDMAP_INDEX_F4			= 0x0904,
	KBDMAP_INDEX_F5			= 0x0905,
	KBDMAP_INDEX_F6			= 0x0906,
	KBDMAP_INDEX_F7			= 0x0907,
	KBDMAP_INDEX_F8			= 0x0908,
	KBDMAP_INDEX_F9			= 0x0909,
	KBDMAP_INDEX_F10		= 0x090A,
	KBDMAP_INDEX_F11		= 0x090B,
	KBDMAP_INDEX_F12		= 0x090C,
	KBDMAP_INDEX_F13		= 0x090D,
	KBDMAP_INDEX_F14		= 0x090E,
	KBDMAP_INDEX_F15		= 0x090F,

	// Function keypad part 2
	KBDMAP_INDEX_F16		= 0x0A00,
	KBDMAP_INDEX_F17		= 0x0A01,
	KBDMAP_INDEX_F18		= 0x0A02,
	KBDMAP_INDEX_F19		= 0x0A03,
	KBDMAP_INDEX_F20		= 0x0A04,
	KBDMAP_INDEX_F21		= 0x0A05,
	KBDMAP_INDEX_F22		= 0x0A06,
	KBDMAP_INDEX_F23		= 0x0A07,
	KBDMAP_INDEX_F24		= 0x0A08,

	// Function keypad part 3
	// The USB keyboard page and the IBM 1397000 have 24 function keys.
	// There is room for expansion here up to 47.

	// Function keypad part 4
	// Various PS/2 "Office" keyboards have a Fn-Lock mode where the function keys send different scancodes.
	KBDMAP_INDEX_FN_F1		= 0x0C01,
	KBDMAP_INDEX_FN_F2		= 0x0C02,
	KBDMAP_INDEX_FN_F3		= 0x0C03,
	KBDMAP_INDEX_FN_F4		= 0x0C04,
	KBDMAP_INDEX_FN_F5		= 0x0C05,
	KBDMAP_INDEX_FN_F6		= 0x0C06,
	KBDMAP_INDEX_FN_F7		= 0x0C07,
	KBDMAP_INDEX_FN_F8		= 0x0C08,
	KBDMAP_INDEX_FN_F9		= 0x0C09,
	KBDMAP_INDEX_FN_F10		= 0x0C0A,
	KBDMAP_INDEX_FN_F11		= 0x0C0B,
	KBDMAP_INDEX_FN_F12		= 0x0C0C,

	// System commands keypad
	KBDMAP_INDEX_TASK_MANAGER	= 0x0D00,
	KBDMAP_INDEX_POWER		= 0x0D01,
	KBDMAP_INDEX_SLEEP		= 0x0D02,
	KBDMAP_INDEX_WAKE		= 0x0D03,
	KBDMAP_INDEX_DEBUG		= 0x0D04,
	KBDMAP_INDEX_LOCK		= 0x0D05,
	KBDMAP_INDEX_LOGOFF		= 0x0D06,
//	KBDMAP_INDEX_LOGON		= 0x0D07,
//	KBDMAP_INDEX_HIBERNATE		= 0x0D08,
	KBDMAP_INDEX_NEXT_TASK		= 0x0D09,
	KBDMAP_INDEX_PREVIOUS_TASK	= 0x0D0A,
	KBDMAP_INDEX_SELECT_TASK	= 0x0D0B,
	KBDMAP_INDEX_HALT_TASK		= 0x0D0C,

	// Application shortcut keypad part 1
	KBDMAP_INDEX_CALCULATOR		= 0x0E00,
	KBDMAP_INDEX_FILE_MANAGER	= 0x0E01,
	KBDMAP_INDEX_WWW_BROWSER	= 0x0E02,
	KBDMAP_INDEX_HOME_PAGE		= 0x0E03,
	KBDMAP_INDEX_MAIL		= 0x0E04,
	KBDMAP_INDEX_MY_COMPUTER	= 0x0E05,
	KBDMAP_INDEX_CLI		= 0x0E06,
	KBDMAP_INDEX_WORDPROCESSOR	= 0x0E07,
	KBDMAP_INDEX_SPREADSHEET	= 0x0E08,
//	KBDMAP_INDEX_GRAPHICS_EDITOR	= 0x0E09,
	KBDMAP_INDEX_INSTANT_MESSAGING	= 0x0E0A,
	KBDMAP_INDEX_MEDIA_PLAYER	= 0x0E0B,
	KBDMAP_INDEX_MY_PICTURES	= 0x0E0C,
//	KBDMAP_INDEX_TEXT_EDITOR	= 0x0E0D,
	KBDMAP_INDEX_SPELL		= 0x0E0E,
	KBDMAP_INDEX_CALENDAR		= 0x0E0F,

	// Application shortcut keypad part 2
	// Reserved for future expansion.

	// Application Commands keypad part 1
	KBDMAP_INDEX_POPUP_MENU		= 0x1000,	// Windows keyboard
	KBDMAP_INDEX_STOP_PLAYING	= 0x1001,
	KBDMAP_INDEX_NEXT_TRACK		= 0x1002,
	KBDMAP_INDEX_PREVIOUS_TRACK	= 0x1003,
	KBDMAP_INDEX_PLAY_PAUSE		= 0x1004,
	KBDMAP_INDEX_MUTE		= 0x1005,
	KBDMAP_INDEX_VOLUME_UP		= 0x1006,
	KBDMAP_INDEX_VOLUME_DOWN	= 0x1007,
	KBDMAP_INDEX_REWIND		= 0x1008,
	KBDMAP_INDEX_FAST_FORWARD	= 0x1009,
	KBDMAP_INDEX_EJECT		= 0x100A,
	KBDMAP_INDEX_RECORD		= 0x100B,
	KBDMAP_INDEX_APP_BACK		= 0x100C,
	KBDMAP_INDEX_APP_FORWARD	= 0x100D,
	KBDMAP_INDEX_APP_LEFT		= 0x100E,
	KBDMAP_INDEX_APP_RIGHT		= 0x100F,

	// Application Commands keypad part 2
	KBDMAP_INDEX_HELP		= 0x1100,	// Sun keyboard
	KBDMAP_INDEX_PAUSE		= 0x1101,	// PC/XT pause, not pause media player
	KBDMAP_INDEX_PRINT_SCREEN	= 0x1102,
	KBDMAP_INDEX_ATTENTION		= 0x1103,
	KBDMAP_INDEX_REFRESH		= 0x1104,
	KBDMAP_INDEX_NEW		= 0x1105,
	KBDMAP_INDEX_EXIT		= 0x1106,
//	KBDMAP_INDEX_SAVE		= 0x1107,
	KBDMAP_INDEX_STOP_OR_BREAK	= 0x1108,	// PC/XT break, not stop player/loading
	KBDMAP_INDEX_SEARCH		= 0x1109,	// not find
	KBDMAP_INDEX_BOOKMARKS		= 0x110A,
	KBDMAP_INDEX_STOP_LOADING	= 0x110B,
	KBDMAP_INDEX_EXECUTE		= 0x110C,
	KBDMAP_INDEX_MENU		= 0x110D,	// not Windows keyboard's popup menu
	KBDMAP_INDEX_OPEN		= 0x110E,
//	KBDMAP_INDEX_CLOSE		= 0x110F,

	// Application Commands keypad part 3
	KBDMAP_INDEX_SELECT		= 0x1200,
	KBDMAP_INDEX_STOP		= 0x1201,	// Linux evdev stop
	KBDMAP_INDEX_AGAIN		= 0x1202,
	KBDMAP_INDEX_CANCEL		= 0x1203,
	KBDMAP_INDEX_CLEAR		= 0x1204,
	KBDMAP_INDEX_PRIOR		= 0x1205,
	KBDMAP_INDEX_APP_RETURN		= 0x1206,
	KBDMAP_INDEX_SEPARATOR		= 0x1207,
	KBDMAP_INDEX_OUT		= 0x1208,
	KBDMAP_INDEX_OPER		= 0x1209,
	KBDMAP_INDEX_CLEAR_OR_AGAIN	= 0x120A,
	KBDMAP_INDEX_PROPERTIES		= 0x120B,
	KBDMAP_INDEX_EXSEL		= 0x120C,

};

#endif
