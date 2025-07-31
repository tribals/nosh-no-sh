/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstdio>
#include <vector>
#include <cstring>
#include <unistd.h>
#include "FileStar.h"
#include "FileDescriptorOwner.h"
#include "CharacterCell.h"
#include "VirtualTerminalBackEnd.h"

VirtualTerminalBackEnd::VirtualTerminalBackEnd(
) :
	dir_name(nullptr),
	buffer_file(nullptr),
	input_fd(-1),
	message_pending(0U),
	polling_for_write(false),
	reload_needed(true),
	cursor(),
	size(),
	cursor_glyph(CursorSprite::BLOCK),
	cursor_attributes(CursorSprite::VISIBLE),
	pointer_attributes(0),
	screen_flags(0),
	cells()
{
	if (buffer_file)
		std::setvbuf(buffer_file, display_stdio_buffer, _IOFBF, sizeof display_stdio_buffer);
}

VirtualTerminalBackEnd::VirtualTerminalBackEnd(
	const char * dirname,
	FILE * b,
	int d
) :
	dir_name(dirname),
	buffer_file(b),
	input_fd(d),
	message_pending(0U),
	polling_for_write(false),
	reload_needed(true),
	cursor(),
	size(),
	cursor_glyph(CursorSprite::BLOCK),
	cursor_attributes(CursorSprite::VISIBLE),
	pointer_attributes(0),
	screen_flags(0),
	cells()
{
	if (buffer_file)
		std::setvbuf(buffer_file, display_stdio_buffer, _IOFBF, sizeof display_stdio_buffer);
}

VirtualTerminalBackEnd::~VirtualTerminalBackEnd()
{
}

void
VirtualTerminalBackEnd::set_buffer_file(FILE * f)
{
	if (f == buffer_file) return;
	reload_needed = true;
	buffer_file = f;
	if (buffer_file)
		std::setvbuf(buffer_file, display_stdio_buffer, _IOFBF, sizeof display_stdio_buffer);
}

void
VirtualTerminalBackEnd::set_input_fd(int fd)
{
	if (input_fd.get() == fd) return;
	polling_for_write = false;
	input_fd.reset(fd);
}

void
VirtualTerminalBackEnd::move_cursor(
	coordinate y,
	coordinate x
) {
	if (y >= size.h) y = size.h - 1;
	if (x >= size.w) x = size.w - 1;
	cursor.y = y;
	cursor.x = x;
}

void
VirtualTerminalBackEnd::resize(
	coordinate new_h,
	coordinate new_w
) {
	if (size.h == new_h && size.w == new_w) return;
	size.h = new_h;
	size.w = new_w;
	const std::size_t s(static_cast<std::size_t>(size.h) * size.w);
	if (cells.size() == s) return;
	cells.resize(s);
}

void
VirtualTerminalBackEnd::calculate_visible_rectangle(
	const struct wh & n,
	struct xy & origin,
	struct wh & margin
) const {
	// Restrict the maximum visible area to the size of the buffer.
	if (n.h > size.h) { margin.h = size.h; } else { margin.h = n.h; }
	if (n.w > size.w) { margin.w = size.w; } else { margin.w = n.w; }
	// Keep the visible area in the buffer.
	if (origin.y + margin.h > size.h) origin.y = size.h - margin.h;
	if (origin.x + margin.w > size.w) origin.x = size.w - margin.w;
	// When programs repaint the screen the cursor is instantaneously all over the place, leading to the window scrolling all over the shop.
	// But some programs, like vim, make the cursor invisible during the repaint in order to reduce cursor flicker.
	// We take advantage of this by only scrolling the screen to include the cursor position if the cursor is actually visible.
	if (CursorSprite::VISIBLE & cursor_attributes) {
		// The window includes the cursor position.
		if (origin.y > cursor.y || margin.h < 1U) origin.y = cursor.y; else if (origin.y + margin.h <= cursor.y) origin.y = cursor.y - margin.h + 1;
		if (origin.x > cursor.x || margin.w < 1U) origin.x = cursor.x; else if (origin.x + margin.w <= cursor.x) origin.x = cursor.x - margin.w + 1;
	}
}

/// \brief Pull the display buffer from file into the memory buffer, but don't output anything.
void
VirtualTerminalBackEnd::reload ()
{
	if (!buffer_file) { reload_needed = false; return; }

	// The stdio buffers may well be out of synch, so we need to reset them.
#if defined(__LINUX__) || defined(__linux__)
	std::fflush(buffer_file);
#endif
	std::rewind(buffer_file);
	uint8_t header0[4] = { 0, 0, 0, 0 };
	std::fread(header0, sizeof header0, 1U, buffer_file);
	uint16_t header1[4] = { 0, 0, 0, 0 };
	std::fread(header1, sizeof header1, 1U, buffer_file);
	uint8_t header2[4] = { 0, 0, 0, 0 };
	std::fread(header2, sizeof header2, 1U, buffer_file);

	// Don't fseek() if we can avoid it; it causes duplicate VERY LARGE reads to re-fill the stdio buffer.
	if (HEADER_LENGTH != ftello(buffer_file))
		std::fseek(buffer_file, HEADER_LENGTH, SEEK_SET);

	resize(header1[1], header1[0]);
	move_cursor(header1[3], header1[2]);
	cursor_glyph = static_cast<CursorSprite::glyph_type>(header2[0] & 0x0F);
	cursor_attributes = static_cast<CursorSprite::attribute_type >(header2[1] & 0x0F);
	pointer_attributes = static_cast<PointerSprite::attribute_type >(header2[2] & 0x0F);
	screen_flags = static_cast<ScreenFlags::flag_type>(header2[2] >> 4);

	for (unsigned row(0); row < size.h; ++row) {
		for (unsigned col(0); col < size.w; ++col) {
			unsigned char b[CELL_LENGTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			std::fread(b, sizeof b, 1U, buffer_file);
			uint32_t wc;
			std::memcpy(&wc, &b[8], 4);
			const CharacterCell::attribute_type a((static_cast<unsigned short>(b[13]) << 8U) + b[12]);
			const CharacterCell::colour_type fg(b[0], b[1], b[2], b[3]);
			const CharacterCell::colour_type bg(b[4], b[5], b[6], b[7]);
			CharacterCell cc(wc, a, fg, bg);
			at(row, col) = cc;
		}
	}

	reload_needed = false;
}

void
VirtualTerminalBackEnd::WriteInputMessage(uint32_t m)
{
	if (sizeof m > (sizeof message_buffer - message_pending)) return;
	std::memmove(message_buffer + message_pending, &m, sizeof m);
	message_pending += sizeof m;
}

void
VirtualTerminalBackEnd::FlushMessages()
{
	if (0 > input_fd.get()) { message_pending = 0; return; }
	const ssize_t l(write(input_fd.get(), message_buffer, message_pending));
	if (l > 0) {
		std::memmove(message_buffer, message_buffer + l, message_pending - l);
		message_pending -= l;
	}
}
