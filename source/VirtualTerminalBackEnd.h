/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_VIRTUALTERMINALBACKEND_H)
#define INCLUDE_VIRTUALTERMINALBACKEND_H

#include <cstdio>
#include <vector>
#include "FileStar.h"
#include "FileDescriptorOwner.h"
#include "CharacterCell.h"

class VirtualTerminalBackEnd
{
public:
	typedef unsigned short coordinate;
	VirtualTerminalBackEnd(const char * dirname, FILE * buffer_file = nullptr, int input_fd = -1);
	VirtualTerminalBackEnd();
	~VirtualTerminalBackEnd();
	struct xy {
		coordinate x, y;
		xy(coordinate xp, coordinate yp) : x(xp), y(yp) {}
		xy() : x(0U), y(0U) {}
	} ;
	struct wh {
		coordinate w, h;
		wh(coordinate wp, coordinate hp) : w(wp), h(hp) {}
		wh() : w(0U), h(0U) {}
	} ;
	const char * query_dir_name() const { return dir_name; }
	void set_dir_name(const char * n) { dir_name = n; }
	void set_buffer_file(FILE *);
	void set_input_fd(int);
	int query_buffer_fd() const { if (FILE * f = buffer_file.operator FILE *()) return fileno(f); else return -1; }
	int query_input_fd() const { return input_fd.get(); }
	FILE * query_buffer_file() const { return buffer_file; }
	const struct wh & query_size() const { return size; }
	const struct xy & query_cursor() const { return cursor; }
	CursorSprite::glyph_type query_cursor_glyph() const { return cursor_glyph; }
	CursorSprite::attribute_type query_cursor_attributes() const { return cursor_attributes; }
	PointerSprite::attribute_type query_pointer_attributes() const { return pointer_attributes; }
	ScreenFlags::flag_type query_screen_flags() const { return screen_flags; }
	bool query_reload_needed() const { return reload_needed; }
	void set_reload_needed() { reload_needed = true; }
	void reload();
	void calculate_visible_rectangle(const struct wh & area, struct xy & origin, struct wh & margin) const;
	CharacterCell & at(coordinate y, coordinate x) { return cells[static_cast<std::size_t>(y) * size.w + x]; }
	void WriteInputMessage(uint32_t);
	bool MessageAvailable() const { return message_pending > 0U; }
	void FlushMessages();
	bool query_polling_for_write() const { return polling_for_write; }
	void set_polling_for_write(bool v) { polling_for_write = v; }

protected:
	VirtualTerminalBackEnd(const VirtualTerminalBackEnd & c);
	enum { CELL_LENGTH = 16U, HEADER_LENGTH = 16U };

	void move_cursor(coordinate y, coordinate x);
	void resize(coordinate, coordinate);

	const char * dir_name;
	char display_stdio_buffer[128U * 1024U];
	FileStar buffer_file;
	FileDescriptorOwner input_fd;
	char message_buffer[4096];
	std::size_t message_pending;
	bool polling_for_write, reload_needed;
	struct xy cursor;
	struct wh size;
	CursorSprite::glyph_type cursor_glyph;
	CursorSprite::attribute_type cursor_attributes;
	PointerSprite::attribute_type pointer_attributes;
	ScreenFlags::flag_type screen_flags;
	std::vector<CharacterCell> cells;
};

#endif
