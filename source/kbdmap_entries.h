/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_KBDMAP_ENTRIES_H)
#define INCLUDE_KBDMAP_ENTRIES_H

#include "kbdmap.h"

/* Keyboard map entry shorthands ********************************************
// **************************************************************************
*/

// Individual action mappings

#define	NOOP(x)	 (((x) & 0x00FFFFFF) << 0U)
#define UCSA(x) ((((x) & 0x00FFFFFF) << 0U) | KBDMAP_ACTION_UCS3)
#define MMNT(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_MODIFIER | KBDMAP_MODIFIER_CMD_MOMENTARY)
#define LTCH(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_MODIFIER | KBDMAP_MODIFIER_CMD_LATCH)
#define LOCK(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_MODIFIER | KBDMAP_MODIFIER_CMD_LOCK)
#define SCRN(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_SCREEN)
#define SYST(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_SYSTEM)
#define CONS(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_CONSUMER)
#define EXTE(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_EXTENDED)
#define EXTN(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_EXTENDED1)
#define FUNC(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_FUNCTION)
#define FUNK(x) ((((x) & 0x0000FFFF) << 8U) | KBDMAP_ACTION_FUNCTION1)

#endif
