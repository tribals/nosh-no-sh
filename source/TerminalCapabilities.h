/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_TERMINALCAPABILITIES_H)
#define INCLUDE_TERMINALCAPABILITIES_H

struct ProcessEnvironment;

/// \brief Express terminal capabilities
///
/// See the manual page of this class for details.
class TerminalCapabilities {
public:
	TerminalCapabilities(const ProcessEnvironment &);
	static bool permit_fake_truecolour;
	enum { NO_COLOURS, ECMA_8_COLOURS, ECMA_16_COLOURS, INDEXED_COLOUR_FAULTY, ISO_INDEXED_COLOUR, DIRECT_COLOUR_FAULTY, ISO_DIRECT_COLOUR } colour_level;
	enum { NO_SCUSR, ORIGINAL_DECSCUSR, XTERM_DECSCUSR, EXTENDED_DECSCUSR, LINUX_SCUSR } cursor_shape_command;
	/// \brief standards non-conformance, deficiencies, and bugs
	/// @{
	bool lacks_pending_wrap, lacks_NEL, lacks_RI, lacks_IND, lacks_CTC, lacks_HPA, lacks_REP, lacks_invisible, lacks_strikethrough, lacks_reverse_off;
	bool faulty_reverse_video, faulty_inverse_erase, faulty_SP_REP;
	bool linux_editing_keypad, interix_function_keys, teken_function_keys, sco_function_keys, rxvt_function_keys, linux_function_keys;
	/// @}
	/// \brief DEC and other private modes and control sequences
	/// @{
	bool use_DECPrivateMode, use_SCOPrivateMode, use_DECSTR, use_DECST8C, use_DECLocator, use_DECSNLS, use_DECSCPP, use_DECSLRM, use_DECNKM;
	bool has_DECECM, initial_DECECM;
	bool has_DTTerm_DECSLPP_extensions, has_XTerm1006Mouse, has_square_mode;
	/// @}
	/// \brief other augmentations
	/// @{
	bool reset_sets_tabs, has_extended_underline;
	/// @}
};

#endif
