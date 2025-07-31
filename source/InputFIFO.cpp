/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstddef>
#include <cstring>
#include <unistd.h>
#include "FileDescriptorOwner.h"
#include "InputFIFO.h"

InputFIFO::InputFIFO(int i) :
	FileDescriptorOwner(i),
	input_read(0U)
{
}

void
InputFIFO::ReadInput(
	int n		///< number of characters available; can be <= 0 erroneously
) {
	do {
		const ssize_t l(read(fd, input_buffer + input_read, sizeof input_buffer - input_read));
		if (0 >= l) break;
		input_read += l;
		if (l >= n) break;
		n -= l;
	} while (n > 0);
}

uint32_t
InputFIFO::PullMessage()
{
	uint32_t b(0);
	if (input_read >= sizeof b) {
		std::memcpy(&b, input_buffer, sizeof b);
		input_read -= sizeof b;
		std::memmove(input_buffer, input_buffer + sizeof b, input_read);
	}
	return b;
}
