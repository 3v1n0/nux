/*
* Copyright 2010-2012 Inalogic® Inc.
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License, as
* published by the  Free Software Foundation; either version 2.1 or 3.0
* of the License.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranties of
* MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
* PURPOSE.  See the applicable version of the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of both the GNU Lesser General Public
* License along with this program. If not, see <http://www.gnu.org/licenses/>
*
* Authored by: Brandon Schaefer <brandontschaefer@gmail.com>
*              Jay Taoko <jaytaoko@inalogic.com>
*              Marco Trevisan <marco.trevisan@canonical.com>
*
*/

#ifndef TEXT_ENTRY_COMPOSE_SEQS_H
#define TEXT_ENTRY_COMPOSE_SEQS_H

namespace nux
{

 /* This list has been generated using the script ./tools/compose_key_list_generator.py
  * with this file as input:
  * http://cgit.freedesktop.org/xorg/lib/libX11/plain/nls/en_US.UTF-8/Compose.pre
  */
struct ComposeSequence
{
  const static unsigned int MAX_SYMBOLS = 8;
  KeySym symbols[MAX_SYMBOLS];
  const char* result;
};

static const size_t COMPOSE_SEQUENCES_SIZE = 4444;

static const ComposeSequence COMPOSE_SEQUENCES[] = {
  {{XK_dead_hook, XK_B, XK_VoidSymbol}, "Ɓ"}, // U0181 | LATIN CAPITAL LETTER B WITH HOOK
  {{XK_Multi_key, XK_f, XK_l, XK_VoidSymbol}, "ﬂ"}, // Ufb02 | LATIN SMALL LIGATURE FL
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"}, // U1F85 | GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾅ"},
  {{XK_dead_belowdot, XK_B, XK_VoidSymbol}, "Ḅ"}, // U1E04 | LATIN CAPITAL LETTER B WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_B, XK_VoidSymbol}, "Ḅ"},
  {{XK_dead_acute, XK_C, XK_VoidSymbol}, "Ć"}, // U0106 | LATIN CAPITAL LETTER C WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_C, XK_VoidSymbol}, "Ć"},
  {{XK_Multi_key, XK_apostrophe, XK_C, XK_VoidSymbol}, "Ć"},
  {{XK_Multi_key, XK_C, XK_apostrophe, XK_VoidSymbol}, "Ć"},
  {{XK_dead_caron, XK_minus, XK_VoidSymbol}, "₋"}, // U208B | SUBSCRIPT MINUS
  {{XK_dead_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἂ"}, // U1F0A | GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἂ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἂ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἂ"},
  {{XK_dead_doublegrave, XK_O, XK_VoidSymbol}, "Ȍ"}, // U020C | LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
  {{XK_dead_abovedot, XK_y, XK_VoidSymbol}, "ẏ"}, // U1E8F | LATIN SMALL LETTER Y WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_y, XK_VoidSymbol}, "ẏ"},
  {{XK_Multi_key, XK_C, XK_equal, XK_VoidSymbol}, "€"}, // EuroSign | EURO SIGN
  {{XK_Multi_key, XK_equal, XK_C, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_c, XK_equal, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_equal, XK_c, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_E, XK_equal, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_equal, XK_E, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_e, XK_equal, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_equal, XK_e, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_Cyrillic_ES, XK_equal, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_equal, XK_Cyrillic_ES, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_Cyrillic_IE, XK_equal, XK_VoidSymbol}, "€"},
  {{XK_Multi_key, XK_equal, XK_Cyrillic_IE, XK_VoidSymbol}, "€"},
  {{XK_dead_currency, XK_e, XK_VoidSymbol}, "€"},
  {{XK_dead_hook, XK_F, XK_VoidSymbol}, "Ƒ"}, // U0191 | LATIN CAPITAL LETTER F WITH HOOK
  {{XK_dead_voiced_sound, XK_kana_HE, XK_VoidSymbol}, "ベ"}, // U30D9 | KATAKANA LETTER BE
  {{XK_Multi_key, XK_slash, XK_Cyrillic_ghe, XK_VoidSymbol}, "ғ"}, // U0493 | CYRILLIC SMALL LETTER GHE WITH STROKE
  {{XK_Multi_key, XK_KP_Divide, XK_Cyrillic_ghe, XK_VoidSymbol}, "ғ"},
  {{XK_Multi_key, XK_parenleft, XK_L, XK_parenright, XK_VoidSymbol}, "Ⓛ"}, // U24C1 | CIRCLED LATIN CAPITAL LETTER L
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"}, // U1F95 | GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾕ"},
  {{XK_dead_grave, XK_Emacron, XK_VoidSymbol}, "Ḕ"}, // U1E14 | LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Emacron, XK_VoidSymbol}, "Ḕ"},
  {{XK_dead_grave, XK_dead_macron, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_dead_grave, XK_Multi_key, XK_macron, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_dead_grave, XK_Multi_key, XK_underscore, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_Multi_key, XK_grave, XK_dead_macron, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_Multi_key, XK_grave, XK_macron, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_Multi_key, XK_grave, XK_underscore, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_dead_macron, XK_Egrave, XK_VoidSymbol}, "Ḕ"},
  {{XK_dead_macron, XK_dead_grave, XK_E, XK_VoidSymbol}, "Ḕ"},
  {{XK_Multi_key, XK_N, XK_o, XK_VoidSymbol}, "№"}, // numerosign | NUMERO SIGN
  {{XK_Multi_key, XK_N, XK_O, XK_VoidSymbol}, "№"},
  {{XK_Multi_key, XK_Cyrillic_EN, XK_Cyrillic_o, XK_VoidSymbol}, "№"},
  {{XK_Multi_key, XK_Cyrillic_EN, XK_Cyrillic_O, XK_VoidSymbol}, "№"},
  {{XK_dead_acute, XK_nobreakspace, XK_VoidSymbol}, "́"}, // U0301 | COMBINING ACUTE ACCENT
  {{XK_dead_grave, XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἒ"}, // U1F1A | GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἒ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἒ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἒ"},
  {{XK_dead_macron, XK_Cyrillic_O, XK_VoidSymbol}, "О̄"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_O, XK_VoidSymbol}, "О̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_O, XK_VoidSymbol}, "О̄"},
  {{XK_dead_horn, XK_o, XK_VoidSymbol}, "ơ"}, // U01A1 | LATIN SMALL LETTER O WITH HORN
  {{XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ơ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_0, XK_parenright, XK_VoidSymbol}, "㉚"}, // U325A | CIRCLED NUMBER THIRTY
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㉚"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_0, XK_parenright, XK_VoidSymbol}, "㉚"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㉚"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"}, // U1FA5 | GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾥ"},
  {{XK_dead_belowdot, XK_H, XK_VoidSymbol}, "Ḥ"}, // U1E24 | LATIN CAPITAL LETTER H WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_H, XK_VoidSymbol}, "Ḥ"},
  {{XK_dead_stroke, XK_H, XK_VoidSymbol}, "Ħ"}, // U0126 | LATIN CAPITAL LETTER H WITH STROKE
  {{XK_Multi_key, XK_slash, XK_H, XK_VoidSymbol}, "Ħ"},
  {{XK_Multi_key, XK_KP_Divide, XK_H, XK_VoidSymbol}, "Ħ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_SE, XK_parenright, XK_VoidSymbol}, "㋝"}, // U32DD | CIRCLED KATAKANA SE
  {{XK_Multi_key, XK_d, XK_equal, XK_VoidSymbol}, "₫"}, // U20ab | DONG SIGN
  {{XK_Multi_key, XK_equal, XK_d, XK_VoidSymbol}, "₫"},
  {{XK_dead_currency, XK_d, XK_VoidSymbol}, "₫"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἢ"}, // U1F2A | GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἢ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἢ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἢ"},
  {{XK_dead_acute, XK_Greek_epsilon, XK_VoidSymbol}, "έ"}, // U03AD | GREEK SMALL LETTER EPSILON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_epsilon, XK_VoidSymbol}, "έ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_epsilon, XK_VoidSymbol}, "έ"},
  {{XK_Multi_key, XK_Greek_epsilon, XK_apostrophe, XK_VoidSymbol}, "έ"},
  {{XK_dead_macron, XK_Otilde, XK_VoidSymbol}, "Ȭ"}, // U022C | LATIN CAPITAL LETTER O WITH TILDE AND MACRON
  {{XK_Multi_key, XK_macron, XK_Otilde, XK_VoidSymbol}, "Ȭ"},
  {{XK_Multi_key, XK_underscore, XK_Otilde, XK_VoidSymbol}, "Ȭ"},
  {{XK_dead_macron, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_dead_macron, XK_Multi_key, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_Multi_key, XK_macron, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_Multi_key, XK_macron, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_Multi_key, XK_underscore, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_Multi_key, XK_underscore, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_dead_tilde, XK_Omacron, XK_VoidSymbol}, "Ȭ"},
  {{XK_dead_tilde, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ȭ"},
  {{XK_dead_acute, XK_abreve, XK_VoidSymbol}, "ắ"}, // U1EAF | LATIN SMALL LETTER A WITH BREVE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_abreve, XK_VoidSymbol}, "ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_abreve, XK_VoidSymbol}, "ắ"},
  {{XK_dead_acute, XK_dead_breve, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_dead_acute, XK_Multi_key, XK_U, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_dead_acute, XK_Multi_key, XK_b, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_Multi_key, XK_acute, XK_dead_breve, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_Multi_key, XK_acute, XK_b, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_breve, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_b, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_dead_breve, XK_aacute, XK_VoidSymbol}, "ắ"},
  {{XK_dead_breve, XK_dead_acute, XK_a, XK_VoidSymbol}, "ắ"},
  {{XK_dead_stroke, XK_U, XK_VoidSymbol}, "Ʉ"}, // U0244 | LATIN CAPITAL LETTER U BAR
  {{XK_Multi_key, XK_percent, XK_o, XK_VoidSymbol}, "‰"}, // U2030 | PER MILLE SIGN
  {{XK_dead_belowmacron, XK_K, XK_VoidSymbol}, "Ḵ"}, // U1E34 | LATIN CAPITAL LETTER K WITH LINE BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_w, XK_VoidSymbol}, "ʷ"}, // U02B7 | MODIFIER LETTER SMALL W
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_w, XK_VoidSymbol}, "ʷ"},
  {{XK_dead_cedilla, XK_K, XK_VoidSymbol}, "Ķ"}, // U0136 | LATIN CAPITAL LETTER K WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_K, XK_VoidSymbol}, "Ķ"},
  {{XK_Multi_key, XK_K, XK_comma, XK_VoidSymbol}, "Ķ"},
  {{XK_Multi_key, XK_cedilla, XK_K, XK_VoidSymbol}, "Ķ"},
  {{XK_dead_acute, XK_Cyrillic_u, XK_VoidSymbol}, "у́"}, // CYRILLIC SMALL LETTER U WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_u, XK_VoidSymbol}, "у́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_u, XK_VoidSymbol}, "у́"},
  {{XK_Multi_key, XK_greater, XK_greater, XK_VoidSymbol}, "»"}, // guillemotright | RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
  {{XK_dead_grave, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἲ"}, // U1F3A | GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἲ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἲ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἲ"},
  {{XK_dead_stroke, XK_c, XK_VoidSymbol}, "ȼ"}, // U023C | LATIN SMALL LETTER C WITH STROKE
  {{XK_dead_acute, XK_ecircumflex, XK_VoidSymbol}, "ế"}, // U1EBF | LATIN SMALL LETTER E WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_ecircumflex, XK_VoidSymbol}, "ế"},
  {{XK_Multi_key, XK_apostrophe, XK_ecircumflex, XK_VoidSymbol}, "ế"},
  {{XK_dead_acute, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_dead_circumflex, XK_eacute, XK_VoidSymbol}, "ế"},
  {{XK_dead_circumflex, XK_dead_acute, XK_e, XK_VoidSymbol}, "ế"},
  {{XK_dead_voiced_sound, XK_kana_KO, XK_VoidSymbol}, "ゴ"}, // U30B4 | KATAKANA LETTER GO
  {{XK_Multi_key, XK_A, XK_T, XK_VoidSymbol}, "@"}, // at | COMMERCIAL AT
  {{XK_Multi_key, XK_parenleft, XK_N, XK_parenright, XK_VoidSymbol}, "Ⓝ"}, // U24C3 | CIRCLED LATIN CAPITAL LETTER N
  {{XK_dead_abovering, XK_nobreakspace, XK_VoidSymbol}, "̊"}, // U030A | COMBINING RING ABOVE
  {{XK_dead_abovedot, XK_N, XK_VoidSymbol}, "Ṅ"}, // U1E44 | LATIN CAPITAL LETTER N WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_N, XK_VoidSymbol}, "Ṅ"},
  {{XK_dead_caron, XK_space, XK_VoidSymbol}, "ˇ"}, // caron | CARON
  {{XK_dead_caron, XK_dead_caron, XK_VoidSymbol}, "ˇ"},
  {{XK_Multi_key, XK_space, XK_less, XK_VoidSymbol}, "ˇ"},
  {{XK_Multi_key, XK_less, XK_space, XK_VoidSymbol}, "ˇ"},
  {{XK_dead_cedilla, XK_n, XK_VoidSymbol}, "ņ"}, // U0146 | LATIN SMALL LETTER N WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_n, XK_VoidSymbol}, "ņ"},
  {{XK_Multi_key, XK_n, XK_comma, XK_VoidSymbol}, "ņ"},
  {{XK_Multi_key, XK_cedilla, XK_n, XK_VoidSymbol}, "ņ"},
  {{XK_dead_diaeresis, XK_E, XK_VoidSymbol}, "Ë"}, // Ediaeresis | LATIN CAPITAL LETTER E WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_E, XK_VoidSymbol}, "Ë"},
  {{XK_Multi_key, XK_E, XK_quotedbl, XK_VoidSymbol}, "Ë"},
  {{XK_Multi_key, XK_diaeresis, XK_E, XK_VoidSymbol}, "Ë"},
  {{XK_Multi_key, XK_E, XK_diaeresis, XK_VoidSymbol}, "Ë"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὂ"}, // U1F4A | GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὂ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὂ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὂ"},
  {{XK_dead_acute, XK_Greek_upsilon, XK_VoidSymbol}, "ύ"}, // U03CD | GREEK SMALL LETTER UPSILON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_upsilon, XK_VoidSymbol}, "ύ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_upsilon, XK_VoidSymbol}, "ύ"},
  {{XK_Multi_key, XK_Greek_upsilon, XK_apostrophe, XK_VoidSymbol}, "ύ"},
  {{XK_dead_grave, XK_nobreakspace, XK_VoidSymbol}, "̀"}, // U0300 | COMBINING GRAVE ACCENT
  {{XK_dead_hook, XK_o, XK_VoidSymbol}, "ỏ"}, // U1ECF | LATIN SMALL LETTER O WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_o, XK_VoidSymbol}, "ỏ"},
  {{XK_dead_caron, XK_O, XK_VoidSymbol}, "Ǒ"}, // U01D1 | LATIN CAPITAL LETTER O WITH CARON
  {{XK_Multi_key, XK_c, XK_O, XK_VoidSymbol}, "Ǒ"},
  {{XK_dead_diaeresis, XK_Cyrillic_a, XK_VoidSymbol}, "ӓ"}, // U04D3 | CYRILLIC SMALL LETTER A WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_a, XK_VoidSymbol}, "ӓ"},
  {{XK_dead_acute, XK_P, XK_VoidSymbol}, "Ṕ"}, // U1E54 | LATIN CAPITAL LETTER P WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_P, XK_VoidSymbol}, "Ṕ"},
  {{XK_Multi_key, XK_apostrophe, XK_P, XK_VoidSymbol}, "Ṕ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_KU, XK_parenright, XK_VoidSymbol}, "㋗"}, // U32D7 | CIRCLED KATAKANA KU
  {{XK_dead_cedilla, XK_R, XK_VoidSymbol}, "Ŗ"}, // U0156 | LATIN CAPITAL LETTER R WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_R, XK_VoidSymbol}, "Ŗ"},
  {{XK_Multi_key, XK_R, XK_comma, XK_VoidSymbol}, "Ŗ"},
  {{XK_Multi_key, XK_cedilla, XK_R, XK_VoidSymbol}, "Ŗ"},
  {{XK_dead_circumflex, XK_U, XK_VoidSymbol}, "Û"}, // Ucircumflex | LATIN CAPITAL LETTER U WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_U, XK_VoidSymbol}, "Û"},
  {{XK_Multi_key, XK_U, XK_asciicircum, XK_VoidSymbol}, "Û"},
  {{XK_Multi_key, XK_greater, XK_U, XK_VoidSymbol}, "Û"},
  {{XK_Multi_key, XK_U, XK_greater, XK_VoidSymbol}, "Û"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_2, XK_parenright, XK_VoidSymbol}, "㉜"}, // U325C | CIRCLED NUMBER THIRTY TWO
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㉜"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㉜"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_2, XK_parenright, XK_VoidSymbol}, "㉜"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㉜"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㉜"},
  {{XK_dead_hook, XK_ohorn, XK_VoidSymbol}, "ở"}, // U1EDF | LATIN SMALL LETTER O WITH HORN AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_ohorn, XK_VoidSymbol}, "ở"},
  {{XK_dead_hook, XK_dead_horn, XK_o, XK_VoidSymbol}, "ở"},
  {{XK_dead_hook, XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ở"},
  {{XK_Multi_key, XK_question, XK_dead_horn, XK_o, XK_VoidSymbol}, "ở"},
  {{XK_Multi_key, XK_question, XK_plus, XK_o, XK_VoidSymbol}, "ở"},
  {{XK_dead_horn, XK_ohook, XK_VoidSymbol}, "ở"},
  {{XK_dead_horn, XK_dead_hook, XK_o, XK_VoidSymbol}, "ở"},
  {{XK_dead_macron, XK_dead_abovedot, XK_a, XK_VoidSymbol}, "ǡ"}, // U01E1 | LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_period, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_Multi_key, XK_macron, XK_dead_abovedot, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_Multi_key, XK_macron, XK_period, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_Multi_key, XK_underscore, XK_dead_abovedot, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_Multi_key, XK_underscore, XK_period, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_dead_abovedot, XK_amacron, XK_VoidSymbol}, "ǡ"},
  {{XK_dead_abovedot, XK_dead_macron, XK_a, XK_VoidSymbol}, "ǡ"},
  {{XK_dead_grave, XK_space, XK_VoidSymbol}, "`"}, // grave | GRAVE ACCENT
  {{XK_dead_grave, XK_dead_grave, XK_VoidSymbol}, "`"},
  {{XK_Multi_key, XK_grave, XK_space, XK_VoidSymbol}, "`"},
  {{XK_Multi_key, XK_space, XK_grave, XK_VoidSymbol}, "`"},
  {{XK_dead_macron, XK_Cyrillic_i, XK_VoidSymbol}, "ӣ"}, // U04E3 | CYRILLIC SMALL LETTER I WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_i, XK_VoidSymbol}, "ӣ"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_i, XK_VoidSymbol}, "ӣ"},
  {{XK_dead_dasia, XK_Greek_rho, XK_VoidSymbol}, "ῥ"}, // U1FE5 | GREEK SMALL LETTER RHO WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_rho, XK_VoidSymbol}, "ῥ"},
  {{XK_dead_abovedot, XK_Sacute, XK_VoidSymbol}, "Ṥ"}, // U1E64 | LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
  {{XK_Multi_key, XK_period, XK_Sacute, XK_VoidSymbol}, "Ṥ"},
  {{XK_dead_abovedot, XK_dead_acute, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_acute, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_apostrophe, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_Multi_key, XK_period, XK_dead_acute, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_Multi_key, XK_period, XK_acute, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_Multi_key, XK_period, XK_apostrophe, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_dead_acute, XK_Sabovedot, XK_VoidSymbol}, "Ṥ"},
  {{XK_dead_acute, XK_dead_abovedot, XK_S, XK_VoidSymbol}, "Ṥ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_NE, XK_parenright, XK_VoidSymbol}, "㋧"}, // U32E7 | CIRCLED KATAKANA NE
  {{XK_dead_stroke, XK_T, XK_VoidSymbol}, "Ŧ"}, // U0166 | LATIN CAPITAL LETTER T WITH STROKE
  {{XK_Multi_key, XK_slash, XK_T, XK_VoidSymbol}, "Ŧ"},
  {{XK_Multi_key, XK_KP_Divide, XK_T, XK_VoidSymbol}, "Ŧ"},
  {{XK_Multi_key, XK_T, XK_slash, XK_VoidSymbol}, "Ŧ"},
  {{XK_Multi_key, XK_T, XK_minus, XK_VoidSymbol}, "Ŧ"},
  {{XK_Multi_key, XK_parenleft, XK_9, XK_parenright, XK_VoidSymbol}, "⑨"}, // U2468 | CIRCLED DIGIT NINE
  {{XK_Multi_key, XK_parenleft, XK_KP_9, XK_parenright, XK_VoidSymbol}, "⑨"},
  {{XK_dead_diaeresis, XK_e, XK_VoidSymbol}, "ë"}, // ediaeresis | LATIN SMALL LETTER E WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_e, XK_VoidSymbol}, "ë"},
  {{XK_Multi_key, XK_e, XK_quotedbl, XK_VoidSymbol}, "ë"},
  {{XK_Multi_key, XK_diaeresis, XK_e, XK_VoidSymbol}, "ë"},
  {{XK_Multi_key, XK_e, XK_diaeresis, XK_VoidSymbol}, "ë"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὢ"}, // U1F6A | GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὢ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὢ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὢ"},
  {{XK_dead_tilde, XK_uhorn, XK_VoidSymbol}, "ữ"}, // U1EEF | LATIN SMALL LETTER U WITH HORN AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_uhorn, XK_VoidSymbol}, "ữ"},
  {{XK_dead_tilde, XK_dead_horn, XK_u, XK_VoidSymbol}, "ữ"},
  {{XK_dead_tilde, XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ữ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_horn, XK_u, XK_VoidSymbol}, "ữ"},
  {{XK_Multi_key, XK_asciitilde, XK_plus, XK_u, XK_VoidSymbol}, "ữ"},
  {{XK_dead_horn, XK_utilde, XK_VoidSymbol}, "ữ"},
  {{XK_dead_horn, XK_dead_tilde, XK_u, XK_VoidSymbol}, "ữ"},
  {{XK_dead_voiced_sound, XK_kana_SE, XK_VoidSymbol}, "ゼ"}, // U30BC | KATAKANA LETTER ZE
  {{XK_dead_circumflex, XK_0, XK_VoidSymbol}, "⁰"}, // U2070 | SUPERSCRIPT ZERO
  {{XK_Multi_key, XK_asciicircum, XK_0, XK_VoidSymbol}, "⁰"},
  {{XK_dead_circumflex, XK_KP_0, XK_VoidSymbol}, "⁰"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_0, XK_VoidSymbol}, "⁰"},
  {{XK_dead_doubleacute, XK_Cyrillic_u, XK_VoidSymbol}, "ӳ"}, // U04F3 | CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_Cyrillic_u, XK_VoidSymbol}, "ӳ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_NA, XK_parenright, XK_VoidSymbol}, "㋤"}, // U32E4 | CIRCLED KATAKANA NA
  {{XK_dead_belowtilde, XK_U, XK_VoidSymbol}, "Ṵ"}, // U1E74 | LATIN CAPITAL LETTER U WITH TILDE BELOW
  {{XK_Multi_key, XK_parenleft, XK_kana_RI, XK_parenright, XK_VoidSymbol}, "㋷"}, // U32F7 | CIRCLED KATAKANA RI
  {{XK_dead_circumflex, XK_Y, XK_VoidSymbol}, "Ŷ"}, // U0176 | LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_Y, XK_VoidSymbol}, "Ŷ"},
  {{XK_Multi_key, XK_Y, XK_asciicircum, XK_VoidSymbol}, "Ŷ"},
  {{XK_dead_circumflex, XK_u, XK_VoidSymbol}, "û"}, // ucircumflex | LATIN SMALL LETTER U WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_u, XK_VoidSymbol}, "û"},
  {{XK_Multi_key, XK_u, XK_asciicircum, XK_VoidSymbol}, "û"},
  {{XK_Multi_key, XK_greater, XK_u, XK_VoidSymbol}, "û"},
  {{XK_Multi_key, XK_u, XK_greater, XK_VoidSymbol}, "û"},
  {{XK_dead_grave, XK_Greek_upsilon, XK_VoidSymbol}, "ὺ"}, // U1F7A | GREEK SMALL LETTER UPSILON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_upsilon, XK_VoidSymbol}, "ὺ"},
  {{XK_dead_hook, XK_r, XK_VoidSymbol}, "ɼ"}, // U027C | LATIN SMALL LETTER R WITH LONG LEG
  {{XK_dead_currency, XK_U, XK_VoidSymbol}, "圓"}, // U5713 | YUAN / WEN
  {{XK_dead_macron, XK_a, XK_VoidSymbol}, "ā"}, // U0101 | LATIN SMALL LETTER A WITH MACRON
  {{XK_Multi_key, XK_macron, XK_a, XK_VoidSymbol}, "ā"},
  {{XK_Multi_key, XK_underscore, XK_a, XK_VoidSymbol}, "ā"},
  {{XK_Multi_key, XK_a, XK_underscore, XK_VoidSymbol}, "ā"},
  {{XK_Multi_key, XK_minus, XK_a, XK_VoidSymbol}, "ā"},
  {{XK_Multi_key, XK_a, XK_minus, XK_VoidSymbol}, "ā"},
  {{XK_Multi_key, XK_underscore, XK_0, XK_VoidSymbol}, "₀"}, // U2080 | SUBSCRIPT ZERO
  {{XK_Multi_key, XK_underscore, XK_KP_0, XK_VoidSymbol}, "₀"},
  {{XK_dead_caron, XK_0, XK_VoidSymbol}, "₀"},
  {{XK_dead_acute, XK_Cyrillic_GHE, XK_VoidSymbol}, "Ѓ"}, // U0403 | CYRILLIC CAPITAL LETTER GJE
  {{XK_Multi_key, XK_acute, XK_Cyrillic_GHE, XK_VoidSymbol}, "Ѓ"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_GHE, XK_VoidSymbol}, "Ѓ"},
  {{XK_dead_abovedot, XK_T, XK_VoidSymbol}, "Ṫ"}, // U1E6A | LATIN CAPITAL LETTER T WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_T, XK_VoidSymbol}, "Ṫ"},
  {{XK_Multi_key, XK_T, XK_period, XK_VoidSymbol}, "Ṫ"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"}, // U1F05 | GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἅ"},
  {{XK_dead_diaeresis, XK_W, XK_VoidSymbol}, "Ẅ"}, // U1E84 | LATIN CAPITAL LETTER W WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_W, XK_VoidSymbol}, "Ẅ"},
  {{XK_dead_invertedbreve, XK_e, XK_VoidSymbol}, "ȇ"}, // U0207 | LATIN SMALL LETTER E WITH INVERTED BREVE
  {{XK_dead_grave, XK_Ecircumflex, XK_VoidSymbol}, "Ề"}, // U1EC0 | LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Ecircumflex, XK_VoidSymbol}, "Ề"},
  {{XK_dead_grave, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ề"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ề"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ề"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ề"},
  {{XK_dead_circumflex, XK_Egrave, XK_VoidSymbol}, "Ề"},
  {{XK_dead_circumflex, XK_dead_grave, XK_E, XK_VoidSymbol}, "Ề"},
  {{XK_dead_abovedot, XK_E, XK_VoidSymbol}, "Ė"}, // U0116 | LATIN CAPITAL LETTER E WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_E, XK_VoidSymbol}, "Ė"},
  {{XK_Multi_key, XK_E, XK_period, XK_VoidSymbol}, "Ė"},
  {{XK_dead_tilde, XK_equal, XK_VoidSymbol}, "≃"}, // similarequal | ASYMPTOTICALLY EQUAL TO
  {{XK_dead_belowmacron, XK_d, XK_VoidSymbol}, "ḏ"}, // U1E0F | LATIN SMALL LETTER D WITH LINE BELOW
  {{XK_dead_stroke, XK_d, XK_VoidSymbol}, "đ"}, // dstroke | LATIN SMALL LETTER D WITH STROKE
  {{XK_Multi_key, XK_minus, XK_d, XK_VoidSymbol}, "đ"},
  {{XK_Multi_key, XK_d, XK_minus, XK_VoidSymbol}, "đ"},
  {{XK_Multi_key, XK_slash, XK_d, XK_VoidSymbol}, "đ"},
  {{XK_Multi_key, XK_KP_Divide, XK_d, XK_VoidSymbol}, "đ"},
  {{XK_dead_voiced_sound, XK_kana_WA, XK_VoidSymbol}, "ヷ"}, // U30F7 | KATAKANA LETTER VA
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"}, // U1F15 | GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἕ"},
  {{XK_dead_belowmacron, XK_Z, XK_VoidSymbol}, "Ẕ"}, // U1E94 | LATIN CAPITAL LETTER Z WITH LINE BELOW
  {{XK_dead_invertedbreve, XK_u, XK_VoidSymbol}, "ȗ"}, // U0217 | LATIN SMALL LETTER U WITH INVERTED BREVE
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"}, // U1F9A | GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾚ"},
  {{XK_dead_abovedot, XK_f, XK_VoidSymbol}, "ḟ"}, // U1E1F | LATIN SMALL LETTER F WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_f, XK_VoidSymbol}, "ḟ"},
  {{XK_Multi_key, XK_f, XK_period, XK_VoidSymbol}, "ḟ"},
  {{XK_dead_abovedot, XK_g, XK_VoidSymbol}, "ġ"}, // U0121 | LATIN SMALL LETTER G WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_g, XK_VoidSymbol}, "ġ"},
  {{XK_Multi_key, XK_g, XK_period, XK_VoidSymbol}, "ġ"},
  {{XK_Multi_key, XK_space, XK_space, XK_VoidSymbol}, " "}, // nobreakspace | NO-BREAK SPACE
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἥ"}, // U1F25 | GREEK SMALL LETTER ETA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἥ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἥ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἥ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἥ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἥ"},
  {{XK_dead_acute, XK_Acircumflex, XK_VoidSymbol}, "Ấ"}, // U1EA4 | LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Acircumflex, XK_VoidSymbol}, "Ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_Acircumflex, XK_VoidSymbol}, "Ấ"},
  {{XK_dead_acute, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_dead_circumflex, XK_Aacute, XK_VoidSymbol}, "Ấ"},
  {{XK_dead_circumflex, XK_dead_acute, XK_A, XK_VoidSymbol}, "Ấ"},
  {{XK_dead_abovedot, XK_a, XK_VoidSymbol}, "ȧ"}, // U0227 | LATIN SMALL LETTER A WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_a, XK_VoidSymbol}, "ȧ"},
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"}, // U1FAA | GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾪ"},
  {{XK_dead_acute, XK_idiaeresis, XK_VoidSymbol}, "ḯ"}, // U1E2F | LATIN SMALL LETTER I WITH DIAERESIS AND ACUTE
  {{XK_Multi_key, XK_acute, XK_idiaeresis, XK_VoidSymbol}, "ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_idiaeresis, XK_VoidSymbol}, "ḯ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_dead_diaeresis, XK_iacute, XK_VoidSymbol}, "ḯ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_i, XK_VoidSymbol}, "ḯ"},
  {{XK_dead_abovedot, XK_i, XK_VoidSymbol}, "ı"}, // U0131 | LATIN SMALL LETTER DOTLESS I
  {{XK_Multi_key, XK_i, XK_period, XK_VoidSymbol}, "ı"},
  {{XK_Multi_key, XK_period, XK_i, XK_VoidSymbol}, "ı"},
  {{XK_dead_abovering, XK_space, XK_VoidSymbol}, "°"}, // degree | DEGREE SIGN
  {{XK_dead_abovering, XK_dead_abovering, XK_VoidSymbol}, "°"},
  {{XK_Multi_key, XK_o, XK_o, XK_VoidSymbol}, "°"},
  {{XK_Multi_key, XK_asterisk, XK_0, XK_VoidSymbol}, "°"},
  {{XK_Multi_key, XK_0, XK_asterisk, XK_VoidSymbol}, "°"},
  {{XK_dead_acute, XK_Cyrillic_ie, XK_VoidSymbol}, "е́"}, // CYRILLIC SMALL LETTER IE WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_ie, XK_VoidSymbol}, "е́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_ie, XK_VoidSymbol}, "е́"},
  {{XK_dead_tilde, XK_less, XK_VoidSymbol}, "≲"}, // U2272 | LESS-THAN OR EQUIVALENT TO
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἵ"}, // U1F35 | GREEK SMALL LETTER IOTA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἵ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἵ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἵ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἵ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἵ"},
  {{XK_dead_tilde, XK_Abreve, XK_VoidSymbol}, "Ẵ"}, // U1EB4 | LATIN CAPITAL LETTER A WITH BREVE AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Abreve, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_tilde, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_tilde, XK_Multi_key, XK_U, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_tilde, XK_Multi_key, XK_b, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_Multi_key, XK_asciitilde, XK_b, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_breve, XK_Atilde, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_breve, XK_dead_tilde, XK_A, XK_VoidSymbol}, "Ẵ"},
  {{XK_dead_grave, XK_Omacron, XK_VoidSymbol}, "Ṑ"}, // U1E50 | LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Omacron, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_grave, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_grave, XK_Multi_key, XK_macron, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_grave, XK_Multi_key, XK_underscore, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_Multi_key, XK_grave, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_Multi_key, XK_grave, XK_macron, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_Multi_key, XK_grave, XK_underscore, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_macron, XK_Ograve, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_macron, XK_dead_grave, XK_O, XK_VoidSymbol}, "Ṑ"},
  {{XK_dead_stroke, XK_z, XK_VoidSymbol}, "ƶ"}, // U01B6 | LATIN SMALL LETTER Z WITH STROKE
  {{XK_Multi_key, XK_slash, XK_z, XK_VoidSymbol}, "ƶ"},
  {{XK_Multi_key, XK_KP_Divide, XK_z, XK_VoidSymbol}, "ƶ"},
  {{XK_Multi_key, XK_parenleft, XK_C, XK_parenright, XK_VoidSymbol}, "Ⓒ"}, // U24B8 | CIRCLED LATIN CAPITAL LETTER C
  {{XK_dead_grave, XK_Greek_ALPHA, XK_VoidSymbol}, "Ὰ"}, // U1FBA | GREEK CAPITAL LETTER ALPHA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_ALPHA, XK_VoidSymbol}, "Ὰ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_7, XK_parenright, XK_VoidSymbol}, "㊼"}, // U32BC | CIRCLED NUMBER FORTY SEVEN
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㊼"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_7, XK_parenright, XK_VoidSymbol}, "㊼"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㊼"},
  {{XK_dead_acute, XK_m, XK_VoidSymbol}, "ḿ"}, // U1E3F | LATIN SMALL LETTER M WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_m, XK_VoidSymbol}, "ḿ"},
  {{XK_Multi_key, XK_apostrophe, XK_m, XK_VoidSymbol}, "ḿ"},
  {{XK_dead_stroke, XK_L, XK_VoidSymbol}, "Ł"}, // U0141 | LATIN CAPITAL LETTER L WITH STROKE
  {{XK_Multi_key, XK_slash, XK_L, XK_VoidSymbol}, "Ł"},
  {{XK_Multi_key, XK_L, XK_slash, XK_VoidSymbol}, "Ł"},
  {{XK_Multi_key, XK_KP_Divide, XK_L, XK_VoidSymbol}, "Ł"},
  {{XK_dead_grave, XK_A, XK_VoidSymbol}, "À"}, // Agrave | LATIN CAPITAL LETTER A WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_A, XK_VoidSymbol}, "À"},
  {{XK_Multi_key, XK_A, XK_grave, XK_VoidSymbol}, "À"},
  {{XK_dead_belowcircumflex, XK_E, XK_VoidSymbol}, "Ḙ"}, // U1E18 | LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
  {{XK_dead_tilde, XK_Ecircumflex, XK_VoidSymbol}, "Ễ"}, // U1EC4 | LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Ecircumflex, XK_VoidSymbol}, "Ễ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ễ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ễ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ễ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ễ"},
  {{XK_dead_circumflex, XK_Etilde, XK_VoidSymbol}, "Ễ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_E, XK_VoidSymbol}, "Ễ"},
  {{XK_dead_voiced_sound, XK_kana_FU, XK_VoidSymbol}, "ブ"}, // U30D6 | KATAKANA LETTER BU
  {{XK_Multi_key, XK_parenleft, XK_S, XK_parenright, XK_VoidSymbol}, "Ⓢ"}, // U24C8 | CIRCLED LATIN CAPITAL LETTER S
  {{XK_dead_voiced_sound, XK_kana_HA, XK_VoidSymbol}, "バ"}, // U30D0 | KATAKANA LETTER BA
  {{XK_dead_grave, XK_Greek_ETA, XK_VoidSymbol}, "Ὴ"}, // U1FCA | GREEK CAPITAL LETTER ETA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_ETA, XK_VoidSymbol}, "Ὴ"},
  {{XK_Multi_key, XK_F, XK_r, XK_VoidSymbol}, "₣"}, // U20a3 | FRENCH FRANC SIGN
  {{XK_dead_currency, XK_F, XK_VoidSymbol}, "₣"},
  {{XK_dead_diaeresis, XK_otilde, XK_VoidSymbol}, "ṏ"}, // U1E4F | LATIN SMALL LETTER O WITH TILDE AND DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_otilde, XK_VoidSymbol}, "ṏ"},
  {{XK_dead_diaeresis, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ṏ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_asciitilde, XK_o, XK_VoidSymbol}, "ṏ"},
  {{XK_Multi_key, XK_quotedbl, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ṏ"},
  {{XK_Multi_key, XK_quotedbl, XK_asciitilde, XK_o, XK_VoidSymbol}, "ṏ"},
  {{XK_dead_tilde, XK_odiaeresis, XK_VoidSymbol}, "ṏ"},
  {{XK_dead_tilde, XK_dead_diaeresis, XK_o, XK_VoidSymbol}, "ṏ"},
  {{XK_dead_grave, XK_Cyrillic_u, XK_VoidSymbol}, "у̀"}, // CYRILLIC SMALL LETTER U WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_u, XK_VoidSymbol}, "у̀"},
  {{XK_dead_doubleacute, XK_o, XK_VoidSymbol}, "ő"}, // U0151 | LATIN SMALL LETTER O WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_o, XK_VoidSymbol}, "ő"},
  {{XK_Multi_key, XK_D, XK_H, XK_VoidSymbol}, "Ð"}, // ETH | LATIN CAPITAL LETTER ETH
  {{XK_dead_acute, XK_Cyrillic_ghe, XK_VoidSymbol}, "ѓ"}, // U0453 | CYRILLIC SMALL LETTER GJE
  {{XK_Multi_key, XK_acute, XK_Cyrillic_ghe, XK_VoidSymbol}, "ѓ"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_ghe, XK_VoidSymbol}, "ѓ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_9, XK_parenright, XK_VoidSymbol}, "㉙"}, // U3259 | CIRCLED NUMBER TWENTY NINE
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㉙"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_9, XK_parenright, XK_VoidSymbol}, "㉙"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㉙"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_9, XK_parenright, XK_VoidSymbol}, "㉙"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㉙"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"}, // U1F55 | GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὕ"},
  {{XK_dead_hook, XK_Ocircumflex, XK_VoidSymbol}, "Ổ"}, // U1ED4 | LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Ocircumflex, XK_VoidSymbol}, "Ổ"},
  {{XK_dead_hook, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ổ"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ổ"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ổ"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ổ"},
  {{XK_dead_circumflex, XK_Ohook, XK_VoidSymbol}, "Ổ"},
  {{XK_dead_circumflex, XK_dead_hook, XK_O, XK_VoidSymbol}, "Ổ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_7, XK_parenright, XK_VoidSymbol}, "㉗"}, // U3257 | CIRCLED NUMBER TWENTY SEVEN
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㉗"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_7, XK_parenright, XK_VoidSymbol}, "㉗"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㉗"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_7, XK_parenright, XK_VoidSymbol}, "㉗"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㉗"},
  {{XK_dead_macron, XK_udiaeresis, XK_VoidSymbol}, "ǖ"}, // U01D6 | LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_udiaeresis, XK_VoidSymbol}, "ǖ"},
  {{XK_Multi_key, XK_underscore, XK_udiaeresis, XK_VoidSymbol}, "ǖ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǖ"},
  {{XK_dead_macron, XK_v, XK_VoidSymbol}, "ǖ"},
  {{XK_dead_invertedbreve, XK_Cyrillic_i, XK_VoidSymbol}, "и̑"}, // CYRILLIC SMALL LETTER I WITH COMBINING INVERTED BREVE
  {{XK_Multi_key, XK_parenleft, XK_i, XK_parenright, XK_VoidSymbol}, "ⓘ"}, // U24D8 | CIRCLED LATIN SMALL LETTER I
  {{XK_Multi_key, XK_parenleft, XK_parenleft, XK_VoidSymbol}, "["}, // bracketleft | LEFT SQUARE BRACKET
  {{XK_dead_grave, XK_Greek_IOTA, XK_VoidSymbol}, "Ὶ"}, // U1FDA | GREEK CAPITAL LETTER IOTA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_IOTA, XK_VoidSymbol}, "Ὶ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_SU, XK_parenright, XK_VoidSymbol}, "㋜"}, // U32DC | CIRCLED KATAKANA SU
  {{XK_dead_belowmacron, XK_r, XK_VoidSymbol}, "ṟ"}, // U1E5F | LATIN SMALL LETTER R WITH LINE BELOW
  {{XK_dead_caron, XK_s, XK_VoidSymbol}, "š"}, // U0161 | LATIN SMALL LETTER S WITH CARON
  {{XK_Multi_key, XK_c, XK_s, XK_VoidSymbol}, "š"},
  {{XK_Multi_key, XK_less, XK_s, XK_VoidSymbol}, "š"},
  {{XK_Multi_key, XK_s, XK_less, XK_VoidSymbol}, "š"},
  {{XK_dead_grave, XK_a, XK_VoidSymbol}, "à"}, // agrave | LATIN SMALL LETTER A WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_a, XK_VoidSymbol}, "à"},
  {{XK_Multi_key, XK_a, XK_grave, XK_VoidSymbol}, "à"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_parenright, XK_VoidSymbol}, "④"}, // U2463 | CIRCLED DIGIT FOUR
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_parenright, XK_VoidSymbol}, "④"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὥ"}, // U1F65 | GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὥ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὥ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὥ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὥ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὥ"},
  {{XK_dead_belowdot, XK_U, XK_VoidSymbol}, "Ụ"}, // U1EE4 | LATIN CAPITAL LETTER U WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_U, XK_VoidSymbol}, "Ụ"},
  {{XK_dead_caron, XK_G, XK_VoidSymbol}, "Ǧ"}, // U01E6 | LATIN CAPITAL LETTER G WITH CARON
  {{XK_Multi_key, XK_c, XK_G, XK_VoidSymbol}, "Ǧ"},
  {{XK_Multi_key, XK_parenleft, XK_y, XK_parenright, XK_VoidSymbol}, "ⓨ"}, // U24E8 | CIRCLED LATIN SMALL LETTER Y
  {{XK_dead_grave, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὺ"}, // U1FEA | GREEK CAPITAL LETTER UPSILON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὺ"},
  {{XK_Multi_key, XK_bar, XK_asciitilde, XK_VoidSymbol}, "⍭"}, // U236d | | ~ APL FUNCTIONAL SYMBOL STILE TILDE
  {{XK_Multi_key, XK_asciitilde, XK_bar, XK_VoidSymbol}, "⍭"},
  {{XK_Multi_key, XK_parenleft, XK_kana_HE, XK_parenright, XK_VoidSymbol}, "㋬"}, // U32EC | CIRCLED KATAKANA HE
  {{XK_Multi_key, XK_numbersign, XK_numbersign, XK_VoidSymbol}, "♯"}, // U266f | MUSIC SHARP SIGN
  {{XK_dead_doubleacute, XK_u, XK_VoidSymbol}, "ű"}, // U0171 | LATIN SMALL LETTER U WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_u, XK_VoidSymbol}, "ű"},
  {{XK_Multi_key, XK_d, XK_h, XK_VoidSymbol}, "ð"}, // eth | LATIN SMALL LETTER ETH
  {{XK_Multi_key, XK_parenleft, XK_2, XK_0, XK_parenright, XK_VoidSymbol}, "⑳"}, // U2473 | CIRCLED NUMBER TWENTY
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⑳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_0, XK_parenright, XK_VoidSymbol}, "⑳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⑳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_0, XK_parenright, XK_VoidSymbol}, "⑳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⑳"},
  {{XK_Multi_key, XK_equal, XK_greater, XK_VoidSymbol}, "⇒"}, // U21D2 | RIGHTWARDS DOUBLE ARROW
  {{XK_dead_belowdot, XK_Y, XK_VoidSymbol}, "Ỵ"}, // U1EF4 | LATIN CAPITAL LETTER Y WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_Y, XK_VoidSymbol}, "Ỵ"},
  {{XK_dead_diaeresis, XK_dead_belowdiaeresis, XK_equal, XK_VoidSymbol}, "⩷"}, // U2A77 | EQUALS SIGN WITH TWO DOTS ABOVE AND TWO DOTS BELOW
  {{XK_dead_belowdiaeresis, XK_dead_diaeresis, XK_equal, XK_VoidSymbol}, "⩷"},
  {{XK_dead_invertedbreve, XK_Cyrillic_o, XK_VoidSymbol}, "о̑"}, // CYRILLIC SMALL LETTER O WITH COMBINING INVERTED BREVE
  {{XK_dead_diaeresis, XK_Cyrillic_YERU, XK_VoidSymbol}, "Ӹ"}, // U04F8 | CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_YERU, XK_VoidSymbol}, "Ӹ"},
  {{XK_Multi_key, XK_parenleft, XK_minus, XK_VoidSymbol}, "{"}, // braceleft | LEFT CURLY BRACKET
  {{XK_Multi_key, XK_minus, XK_parenleft, XK_VoidSymbol}, "{"},
  {{XK_dead_grave, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὼ"}, // U1FFA | GREEK CAPITAL LETTER OMEGA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὼ"},
  {{XK_Multi_key, XK_less, XK_less, XK_VoidSymbol}, "«"}, // guillemotleft | LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
  {{XK_dead_belowdot, XK_v, XK_VoidSymbol}, "ṿ"}, // U1E7F | LATIN SMALL LETTER V WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_v, XK_VoidSymbol}, "ṿ"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_3, XK_parenright, XK_VoidSymbol}, "⑬"}, // U246C | CIRCLED NUMBER THIRTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_3, XK_parenright, XK_VoidSymbol}, "⑬"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_3, XK_parenright, XK_VoidSymbol}, "⑬"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_3, XK_parenright, XK_VoidSymbol}, "⑬"},
  {{XK_Multi_key, XK_underscore, XK_1, XK_VoidSymbol}, "₁"}, // U2081 | SUBSCRIPT ONE
  {{XK_Multi_key, XK_underscore, XK_KP_1, XK_VoidSymbol}, "₁"},
  {{XK_dead_caron, XK_1, XK_VoidSymbol}, "₁"},
  {{XK_dead_macron, XK_A, XK_VoidSymbol}, "Ā"}, // U0100 | LATIN CAPITAL LETTER A WITH MACRON
  {{XK_Multi_key, XK_macron, XK_A, XK_VoidSymbol}, "Ā"},
  {{XK_Multi_key, XK_underscore, XK_A, XK_VoidSymbol}, "Ā"},
  {{XK_Multi_key, XK_A, XK_underscore, XK_VoidSymbol}, "Ā"},
  {{XK_Multi_key, XK_minus, XK_A, XK_VoidSymbol}, "Ā"},
  {{XK_Multi_key, XK_A, XK_minus, XK_VoidSymbol}, "Ā"},
  {{XK_dead_invertedbreve, XK_A, XK_VoidSymbol}, "Ȃ"}, // U0202 | LATIN CAPITAL LETTER A WITH INVERTED BREVE
  {{XK_dead_diaeresis, XK_w, XK_VoidSymbol}, "ẅ"}, // U1E85 | LATIN SMALL LETTER W WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_w, XK_VoidSymbol}, "ẅ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"}, // U1F04 | GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἄ"},
  {{XK_dead_abovedot, XK_D, XK_VoidSymbol}, "Ḋ"}, // U1E0A | LATIN CAPITAL LETTER D WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_D, XK_VoidSymbol}, "Ḋ"},
  {{XK_Multi_key, XK_D, XK_period, XK_VoidSymbol}, "Ḋ"},
  {{XK_dead_caron, XK_nobreakspace, XK_VoidSymbol}, "̌"}, // U030C | COMBINING CARON
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"}, // U1F8F | GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾏ"},
  {{XK_dead_breve, XK_Cyrillic_U, XK_VoidSymbol}, "Ў"}, // U040E | CYRILLIC CAPITAL LETTER SHORT U
  {{XK_Multi_key, XK_U, XK_Cyrillic_U, XK_VoidSymbol}, "Ў"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_U, XK_VoidSymbol}, "Ў"},
  {{XK_dead_stroke, XK_D, XK_VoidSymbol}, "Đ"}, // Dstroke | LATIN CAPITAL LETTER D WITH STROKE
  {{XK_Multi_key, XK_minus, XK_D, XK_VoidSymbol}, "Đ"},
  {{XK_Multi_key, XK_D, XK_minus, XK_VoidSymbol}, "Đ"},
  {{XK_Multi_key, XK_slash, XK_D, XK_VoidSymbol}, "Đ"},
  {{XK_Multi_key, XK_KP_Divide, XK_D, XK_VoidSymbol}, "Đ"},
  {{XK_dead_invertedbreve, XK_R, XK_VoidSymbol}, "Ȓ"}, // U0212 | LATIN CAPITAL LETTER R WITH INVERTED BREVE
  {{XK_dead_belowmacron, XK_z, XK_VoidSymbol}, "ẕ"}, // U1E95 | LATIN SMALL LETTER Z WITH LINE BELOW
  {{XK_dead_acute, XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"}, // U1F14 | GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἔ"},
  {{XK_dead_circumflex, XK_dead_belowdot, XK_O, XK_VoidSymbol}, "Ộ"}, // U1ED8 | LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_O, XK_VoidSymbol}, "Ộ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_O, XK_VoidSymbol}, "Ộ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_O, XK_VoidSymbol}, "Ộ"},
  {{XK_dead_belowdot, XK_Ocircumflex, XK_VoidSymbol}, "Ộ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ộ"},
  {{XK_Multi_key, XK_braceleft, XK_braceright, XK_VoidSymbol}, "∅"}, // U2205 | EMPTY SET
  {{XK_dead_belowtilde, XK_E, XK_VoidSymbol}, "Ḛ"}, // U1E1A | LATIN CAPITAL LETTER E WITH TILDE BELOW
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"}, // U1F9F | GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾟ"},
  {{XK_Multi_key, XK_C, XK_slash, XK_VoidSymbol}, "₡"}, // U20a1 | COLON SIGN
  {{XK_Multi_key, XK_slash, XK_C, XK_VoidSymbol}, "₡"},
  {{XK_dead_currency, XK_C, XK_VoidSymbol}, "₡"},
  {{XK_dead_abovedot, XK_G, XK_VoidSymbol}, "Ġ"}, // U0120 | LATIN CAPITAL LETTER G WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_G, XK_VoidSymbol}, "Ġ"},
  {{XK_Multi_key, XK_G, XK_period, XK_VoidSymbol}, "Ġ"},
  {{XK_dead_acute, XK_acircumflex, XK_VoidSymbol}, "ấ"}, // U1EA5 | LATIN SMALL LETTER A WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_acircumflex, XK_VoidSymbol}, "ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_acircumflex, XK_VoidSymbol}, "ấ"},
  {{XK_dead_acute, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_dead_circumflex, XK_aacute, XK_VoidSymbol}, "ấ"},
  {{XK_dead_circumflex, XK_dead_acute, XK_a, XK_VoidSymbol}, "ấ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἤ"}, // U1F24 | GREEK SMALL LETTER ETA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἤ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἤ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἤ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἤ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἤ"},
  {{XK_Multi_key, XK_period, XK_period, XK_VoidSymbol}, "…"}, // ellipsis | HORIZONTAL ELLIPSIS
  {{XK_Multi_key, XK_5, XK_8, XK_VoidSymbol}, "⅝"}, // U215D | VULGAR FRACTION FIVE EIGHTHS
  {{XK_Multi_key, XK_parenleft, XK_kana_KE, XK_parenright, XK_VoidSymbol}, "㋘"}, // U32D8 | CIRCLED KATAKANA KE
  {{XK_dead_belowbreve, XK_H, XK_VoidSymbol}, "Ḫ"}, // U1E2A | LATIN CAPITAL LETTER H WITH BREVE BELOW
  {{XK_dead_currency, XK_A, XK_VoidSymbol}, "₳"}, // U20B3 | AUSTRAL SIGN
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"}, // U1FAF | GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾯ"},
  {{XK_Multi_key, XK_parenleft, XK_D, XK_parenright, XK_VoidSymbol}, "Ⓓ"}, // U24B9 | CIRCLED LATIN CAPITAL LETTER D
  {{XK_Multi_key, XK_plus, XK_minus, XK_VoidSymbol}, "±"}, // plusminus | PLUS-MINUS SIGN
  {{XK_Multi_key, XK_minus, XK_plus, XK_VoidSymbol}, "±"},
  {{XK_dead_abovedot, XK_I, XK_VoidSymbol}, "İ"}, // U0130 | LATIN CAPITAL LETTER I WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_I, XK_VoidSymbol}, "İ"},
  {{XK_Multi_key, XK_I, XK_period, XK_VoidSymbol}, "İ"},
  {{XK_dead_macron, XK_Y, XK_VoidSymbol}, "Ȳ"}, // U0232 | LATIN CAPITAL LETTER Y WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Y, XK_VoidSymbol}, "Ȳ"},
  {{XK_Multi_key, XK_underscore, XK_Y, XK_VoidSymbol}, "Ȳ"},
  {{XK_dead_tilde, XK_abreve, XK_VoidSymbol}, "ẵ"}, // U1EB5 | LATIN SMALL LETTER A WITH BREVE AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_abreve, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_tilde, XK_dead_breve, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_tilde, XK_Multi_key, XK_U, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_tilde, XK_Multi_key, XK_b, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_breve, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_Multi_key, XK_asciitilde, XK_b, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_breve, XK_atilde, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_breve, XK_dead_tilde, XK_a, XK_VoidSymbol}, "ẵ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἴ"}, // U1F34 | GREEK SMALL LETTER IOTA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἴ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἴ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἴ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἴ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἴ"},
  {{XK_Multi_key, XK_3, XK_5, XK_VoidSymbol}, "⅗"}, // U2157 | VULGAR FRACTION THREE FIFTHS
  {{XK_dead_stroke, XK_2, XK_VoidSymbol}, "ƻ"}, // U01BB | LATIN LETTER TWO WITH STROKE
  {{XK_Multi_key, XK_colon, XK_parenright, XK_VoidSymbol}, "☺"}, // U263A | WHITE SMILING FACE
  {{XK_Multi_key, XK_parenleft, XK_4, XK_8, XK_parenright, XK_VoidSymbol}, "㊽"}, // U32BD | CIRCLED NUMBER FORTY EIGHT
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㊽"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_8, XK_parenright, XK_VoidSymbol}, "㊽"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㊽"},
  {{XK_dead_acute, XK_A, XK_VoidSymbol}, "Á"}, // Aacute | LATIN CAPITAL LETTER A WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_A, XK_VoidSymbol}, "Á"},
  {{XK_Multi_key, XK_A, XK_acute, XK_VoidSymbol}, "Á"},
  {{XK_Multi_key, XK_apostrophe, XK_A, XK_VoidSymbol}, "Á"},
  {{XK_Multi_key, XK_A, XK_apostrophe, XK_VoidSymbol}, "Á"},
  {{XK_dead_abovedot, XK_l, XK_VoidSymbol}, "ŀ"}, // U0140 | LATIN SMALL LETTER L WITH MIDDLE DOT
  {{XK_dead_tilde, XK_ecircumflex, XK_VoidSymbol}, "ễ"}, // U1EC5 | LATIN SMALL LETTER E WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_ecircumflex, XK_VoidSymbol}, "ễ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ễ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_e, XK_VoidSymbol}, "ễ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ễ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_e, XK_VoidSymbol}, "ễ"},
  {{XK_dead_circumflex, XK_etilde, XK_VoidSymbol}, "ễ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_e, XK_VoidSymbol}, "ễ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"}, // U1F44 | GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὄ"},
  {{XK_Multi_key, XK_parenleft, XK_T, XK_parenright, XK_VoidSymbol}, "Ⓣ"}, // U24C9 | CIRCLED LATIN CAPITAL LETTER T
  {{XK_dead_stroke, XK_P, XK_VoidSymbol}, "Ᵽ"}, // U2C63 | LATIN CAPITAL LETTER P WITH STROKE
  {{XK_dead_belowcircumflex, XK_N, XK_VoidSymbol}, "Ṋ"}, // U1E4A | LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
  {{XK_dead_invertedbreve, XK_Cyrillic_ie, XK_VoidSymbol}, "е̑"}, // CYRILLIC SMALL LETTER IE WITH COMBINING INVERTED BREVE
  {{XK_dead_tilde, XK_N, XK_VoidSymbol}, "Ñ"}, // Ntilde | LATIN CAPITAL LETTER N WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_N, XK_VoidSymbol}, "Ñ"},
  {{XK_Multi_key, XK_N, XK_asciitilde, XK_VoidSymbol}, "Ñ"},
  {{XK_dead_doubleacute, XK_O, XK_VoidSymbol}, "Ő"}, // U0150 | LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_O, XK_VoidSymbol}, "Ő"},
  {{XK_dead_doublegrave, XK_Cyrillic_ie, XK_VoidSymbol}, "е̏"}, // CYRILLIC SMALL LETTER IE WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_ie, XK_VoidSymbol}, "е̏"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_2, XK_parenright, XK_VoidSymbol}, "㉒"}, // U3252 | CIRCLED NUMBER TWENTY TWO
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_2, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_2, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㉒"},
  {{XK_dead_hook, XK_ocircumflex, XK_VoidSymbol}, "ổ"}, // U1ED5 | LATIN SMALL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_ocircumflex, XK_VoidSymbol}, "ổ"},
  {{XK_dead_hook, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ổ"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_o, XK_VoidSymbol}, "ổ"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ổ"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_o, XK_VoidSymbol}, "ổ"},
  {{XK_dead_circumflex, XK_ohook, XK_VoidSymbol}, "ổ"},
  {{XK_dead_circumflex, XK_dead_hook, XK_o, XK_VoidSymbol}, "ổ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"}, // U1F54 | GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὔ"},
  {{XK_dead_acute, XK_Cyrillic_o, XK_VoidSymbol}, "о́"}, // CYRILLIC SMALL LETTER O WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_o, XK_VoidSymbol}, "о́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_o, XK_VoidSymbol}, "о́"},
  {{XK_Multi_key, XK_parenleft, XK_j, XK_parenright, XK_VoidSymbol}, "ⓙ"}, // U24D9 | CIRCLED LATIN SMALL LETTER J
  {{XK_dead_macron, XK_Cyrillic_ie, XK_VoidSymbol}, "е̄"}, // CYRILLIC SMALL LETTER IE WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_ie, XK_VoidSymbol}, "е̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_ie, XK_VoidSymbol}, "е̄"},
  {{XK_dead_grave, XK_Udiaeresis, XK_VoidSymbol}, "Ǜ"}, // U01DB | LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Udiaeresis, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_grave, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_grave, XK_Multi_key, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǜ"},
  {{XK_Multi_key, XK_grave, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǜ"},
  {{XK_Multi_key, XK_grave, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_diaeresis, XK_Ugrave, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_grave, XK_V, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_diaeresis, XK_dead_grave, XK_U, XK_VoidSymbol}, "Ǜ"},
  {{XK_dead_belowdot, XK_R, XK_VoidSymbol}, "Ṛ"}, // U1E5A | LATIN CAPITAL LETTER R WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_R, XK_VoidSymbol}, "Ṛ"},
  {{XK_dead_doubleacute, XK_space, XK_VoidSymbol}, "˝"}, // U2dd | DOUBLE ACUTE ACCENT
  {{XK_dead_doubleacute, XK_dead_doubleacute, XK_VoidSymbol}, "˝"},
  {{XK_dead_circumflex, XK_Cyrillic_o, XK_VoidSymbol}, "о̂"}, // CYRILLIC SMALL LETTER O WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_o, XK_VoidSymbol}, "о̂"},
  {{XK_dead_breve, XK_Cyrillic_u, XK_VoidSymbol}, "ў"}, // U045E | CYRILLIC SMALL LETTER SHORT U
  {{XK_Multi_key, XK_U, XK_Cyrillic_u, XK_VoidSymbol}, "ў"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_u, XK_VoidSymbol}, "ў"},
  {{XK_dead_acute, XK_a, XK_VoidSymbol}, "á"}, // aacute | LATIN SMALL LETTER A WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_a, XK_VoidSymbol}, "á"},
  {{XK_Multi_key, XK_a, XK_acute, XK_VoidSymbol}, "á"},
  {{XK_Multi_key, XK_apostrophe, XK_a, XK_VoidSymbol}, "á"},
  {{XK_Multi_key, XK_a, XK_apostrophe, XK_VoidSymbol}, "á"},
  {{XK_dead_caron, XK_S, XK_VoidSymbol}, "Š"}, // U0160 | LATIN CAPITAL LETTER S WITH CARON
  {{XK_Multi_key, XK_c, XK_S, XK_VoidSymbol}, "Š"},
  {{XK_Multi_key, XK_less, XK_S, XK_VoidSymbol}, "Š"},
  {{XK_Multi_key, XK_S, XK_less, XK_VoidSymbol}, "Š"},
  {{XK_dead_belowdot, XK_u, XK_VoidSymbol}, "ụ"}, // U1EE5 | LATIN SMALL LETTER U WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_u, XK_VoidSymbol}, "ụ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὤ"}, // U1F64 | GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὤ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὤ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὤ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὤ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὤ"},
  {{XK_Multi_key, XK_parenleft, XK_z, XK_parenright, XK_VoidSymbol}, "ⓩ"}, // U24E9 | CIRCLED LATIN SMALL LETTER Z
  {{XK_dead_ogonek, XK_o, XK_VoidSymbol}, "ǫ"}, // U01EB | LATIN SMALL LETTER O WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_o, XK_VoidSymbol}, "ǫ"},
  {{XK_Multi_key, XK_o, XK_semicolon, XK_VoidSymbol}, "ǫ"},
  {{XK_Multi_key, XK_comma, XK_o, XK_VoidSymbol}, "ǫ"},
  {{XK_Multi_key, XK_o, XK_comma, XK_VoidSymbol}, "ǫ"},
  {{XK_Multi_key, XK_numbersign, XK_e, XK_VoidSymbol}, "♪"}, // U266a | EIGHTH NOTE
  {{XK_Multi_key, XK_parenleft, XK_kana_HO, XK_parenright, XK_VoidSymbol}, "㋭"}, // U32ED | CIRCLED KATAKANA HO
  {{XK_Multi_key, XK_0, XK_asciitilde, XK_VoidSymbol}, "⍬"}, // U236c | 0 ~ APL FUNCTIONAL SYMBOL ZILDE
  {{XK_Multi_key, XK_asciitilde, XK_0, XK_VoidSymbol}, "⍬"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_5, XK_parenright, XK_VoidSymbol}, "⑮"}, // U246E | CIRCLED NUMBER FIFTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_5, XK_parenright, XK_VoidSymbol}, "⑮"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_5, XK_parenright, XK_VoidSymbol}, "⑮"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_5, XK_parenright, XK_VoidSymbol}, "⑮"},
  {{XK_dead_tilde, XK_n, XK_VoidSymbol}, "ñ"}, // ntilde | LATIN SMALL LETTER N WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_n, XK_VoidSymbol}, "ñ"},
  {{XK_Multi_key, XK_n, XK_asciitilde, XK_VoidSymbol}, "ñ"},
  {{XK_dead_doubleacute, XK_U, XK_VoidSymbol}, "Ű"}, // U0170 | LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_U, XK_VoidSymbol}, "Ű"},
  {{XK_dead_semivoiced_sound, XK_kana_HI, XK_VoidSymbol}, "ピ"}, // U30D4 | KATAKANA LETTER PI
  {{XK_dead_hook, XK_n, XK_VoidSymbol}, "ɲ"}, // U0272 | LATIN SMALL LETTER N WITH LEFT HOOK
  {{XK_dead_belowdot, XK_y, XK_VoidSymbol}, "ỵ"}, // U1EF5 | LATIN SMALL LETTER Y WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_y, XK_VoidSymbol}, "ỵ"},
  {{XK_dead_grave, XK_Greek_eta, XK_VoidSymbol}, "ὴ"}, // U1F74 | GREEK SMALL LETTER ETA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_eta, XK_VoidSymbol}, "ὴ"},
  {{XK_dead_circumflex, XK_6, XK_VoidSymbol}, "⁶"}, // U2076 | SUPERSCRIPT SIX
  {{XK_Multi_key, XK_asciicircum, XK_6, XK_VoidSymbol}, "⁶"},
  {{XK_dead_circumflex, XK_KP_6, XK_VoidSymbol}, "⁶"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_6, XK_VoidSymbol}, "⁶"},
  {{XK_dead_diaeresis, XK_Cyrillic_yeru, XK_VoidSymbol}, "ӹ"}, // U04F9 | CYRILLIC SMALL LETTER YERU WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_yeru, XK_VoidSymbol}, "ӹ"},
  {{XK_dead_acute, XK_aring, XK_VoidSymbol}, "ǻ"}, // U01FB | LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_aring, XK_VoidSymbol}, "ǻ"},
  {{XK_Multi_key, XK_apostrophe, XK_aring, XK_VoidSymbol}, "ǻ"},
  {{XK_dead_acute, XK_dead_abovering, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_dead_acute, XK_Multi_key, XK_o, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_Multi_key, XK_acute, XK_dead_abovering, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_abovering, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_Multi_key, XK_asterisk, XK_apostrophe, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_dead_abovering, XK_aacute, XK_VoidSymbol}, "ǻ"},
  {{XK_dead_abovering, XK_dead_acute, XK_a, XK_VoidSymbol}, "ǻ"},
  {{XK_dead_diaeresis, XK_Umacron, XK_VoidSymbol}, "Ṻ"}, // U1E7A | LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Umacron, XK_VoidSymbol}, "Ṻ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_macron, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_underscore, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_dead_macron, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_macron, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_underscore, XK_U, XK_VoidSymbol}, "Ṻ"},
  {{XK_Multi_key, XK_parenleft, XK_w, XK_parenright, XK_VoidSymbol}, "ⓦ"}, // U24E6 | CIRCLED LATIN SMALL LETTER W
  {{XK_dead_abovedot, XK_t, XK_VoidSymbol}, "ṫ"}, // U1E6B | LATIN SMALL LETTER T WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_t, XK_VoidSymbol}, "ṫ"},
  {{XK_Multi_key, XK_t, XK_period, XK_VoidSymbol}, "ṫ"},
  {{XK_dead_stroke, XK_b, XK_VoidSymbol}, "ƀ"}, // U0180 | LATIN SMALL LETTER B WITH STROKE
  {{XK_Multi_key, XK_slash, XK_b, XK_VoidSymbol}, "ƀ"},
  {{XK_Multi_key, XK_KP_Divide, XK_b, XK_VoidSymbol}, "ƀ"},
  {{XK_dead_hook, XK_s, XK_VoidSymbol}, "ʂ"}, // U0282 | LATIN SMALL LETTER S WITH HOOK
  {{XK_dead_belowdot, XK_b, XK_VoidSymbol}, "ḅ"}, // U1E05 | LATIN SMALL LETTER B WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_b, XK_VoidSymbol}, "ḅ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"}, // U1F84 | GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾄ"},
  {{XK_dead_abovedot, XK_nobreakspace, XK_VoidSymbol}, "̇"}, // U0307 | COMBINING DOT ABOVE
  {{XK_Multi_key, XK_underscore, XK_6, XK_VoidSymbol}, "₆"}, // U2086 | SUBSCRIPT SIX
  {{XK_Multi_key, XK_underscore, XK_KP_6, XK_VoidSymbol}, "₆"},
  {{XK_dead_caron, XK_6, XK_VoidSymbol}, "₆"},
  {{XK_dead_invertedbreve, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̑"}, // CYRILLIC CAPITAL LETTER IE WITH COMBINING INVERTED BREVE
  {{XK_dead_abovedot, XK_c, XK_VoidSymbol}, "ċ"}, // U010B | LATIN SMALL LETTER C WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_c, XK_VoidSymbol}, "ċ"},
  {{XK_Multi_key, XK_c, XK_period, XK_VoidSymbol}, "ċ"},
  {{XK_dead_abovedot, XK_X, XK_VoidSymbol}, "Ẋ"}, // U1E8A | LATIN CAPITAL LETTER X WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_X, XK_VoidSymbol}, "Ẋ"},
  {{XK_dead_doublegrave, XK_o, XK_VoidSymbol}, "ȍ"}, // U020D | LATIN SMALL LETTER O WITH DOUBLE GRAVE
  {{XK_dead_acute, XK_Greek_OMICRON, XK_VoidSymbol}, "Ό"}, // U038C | GREEK CAPITAL LETTER OMICRON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_OMICRON, XK_VoidSymbol}, "Ό"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_OMICRON, XK_VoidSymbol}, "Ό"},
  {{XK_Multi_key, XK_Greek_OMICRON, XK_apostrophe, XK_VoidSymbol}, "Ό"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἇ"}, // U1F0F | GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἇ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἇ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἇ"},
  {{XK_Multi_key, XK_less, XK_minus, XK_VoidSymbol}, "←"}, // U2190 | LEFTWARDS ARROW
  {{XK_dead_grave, XK_emacron, XK_VoidSymbol}, "ḕ"}, // U1E15 | LATIN SMALL LETTER E WITH MACRON AND GRAVE
  {{XK_Multi_key, XK_grave, XK_emacron, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_grave, XK_dead_macron, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_grave, XK_Multi_key, XK_macron, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_grave, XK_Multi_key, XK_underscore, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_Multi_key, XK_grave, XK_dead_macron, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_Multi_key, XK_grave, XK_macron, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_Multi_key, XK_grave, XK_underscore, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_macron, XK_egrave, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_macron, XK_dead_grave, XK_e, XK_VoidSymbol}, "ḕ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"}, // U1F94 | GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾔ"},
  {{XK_dead_currency, XK_THORN, XK_VoidSymbol}, "৲"}, // U09F2 | BENGALI RUPEE MARK
  {{XK_dead_currency, XK_thorn, XK_VoidSymbol}, "৲"},
  {{XK_dead_breve, XK_Cyrillic_I, XK_VoidSymbol}, "Й"}, // U0419 | CYRILLIC CAPITAL LETTER SHORT I
  {{XK_Multi_key, XK_U, XK_Cyrillic_I, XK_VoidSymbol}, "Й"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_I, XK_VoidSymbol}, "Й"},
  {{XK_dead_caron, XK_e, XK_VoidSymbol}, "ě"}, // U011B | LATIN SMALL LETTER E WITH CARON
  {{XK_Multi_key, XK_c, XK_e, XK_VoidSymbol}, "ě"},
  {{XK_Multi_key, XK_less, XK_e, XK_VoidSymbol}, "ě"},
  {{XK_Multi_key, XK_e, XK_less, XK_VoidSymbol}, "ě"},
  {{XK_Multi_key, XK_4, XK_5, XK_VoidSymbol}, "⅘"}, // U2158 | VULGAR FRACTION FOUR FIFTHS
  {{XK_Multi_key, XK_slash, XK_Cyrillic_KA, XK_VoidSymbol}, "Ҟ"}, // U049E | CYRILLIC CAPITAL LETTER KA WITH STROKE
  {{XK_Multi_key, XK_KP_Divide, XK_Cyrillic_KA, XK_VoidSymbol}, "Ҟ"},
  {{XK_dead_horn, XK_O, XK_VoidSymbol}, "Ơ"}, // U01A0 | LATIN CAPITAL LETTER O WITH HORN
  {{XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ơ"},
  {{XK_dead_belowdot, XK_h, XK_VoidSymbol}, "ḥ"}, // U1E25 | LATIN SMALL LETTER H WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_h, XK_VoidSymbol}, "ḥ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"}, // U1FA4 | GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾤ"},
  {{XK_dead_cedilla, XK_nobreakspace, XK_VoidSymbol}, "̧"}, // U0327 | COMBINING CEDILLA
  {{XK_Multi_key, XK_N, XK_equal, XK_VoidSymbol}, "₦"}, // U20a6 | NAIRA SIGN
  {{XK_Multi_key, XK_equal, XK_N, XK_VoidSymbol}, "₦"},
  {{XK_dead_currency, XK_N, XK_VoidSymbol}, "₦"},
  {{XK_dead_currency, XK_n, XK_VoidSymbol}, "₦"},
  {{XK_dead_macron, XK_i, XK_VoidSymbol}, "ī"}, // U012B | LATIN SMALL LETTER I WITH MACRON
  {{XK_Multi_key, XK_macron, XK_i, XK_VoidSymbol}, "ī"},
  {{XK_Multi_key, XK_underscore, XK_i, XK_VoidSymbol}, "ī"},
  {{XK_Multi_key, XK_i, XK_underscore, XK_VoidSymbol}, "ī"},
  {{XK_Multi_key, XK_minus, XK_i, XK_VoidSymbol}, "ī"},
  {{XK_Multi_key, XK_i, XK_minus, XK_VoidSymbol}, "ī"},
  {{XK_dead_tilde, XK_Acircumflex, XK_VoidSymbol}, "Ẫ"}, // U1EAA | LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Acircumflex, XK_VoidSymbol}, "Ẫ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ẫ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ẫ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ẫ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ẫ"},
  {{XK_dead_circumflex, XK_Atilde, XK_VoidSymbol}, "Ẫ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_A, XK_VoidSymbol}, "Ẫ"},
  {{XK_dead_macron, XK_otilde, XK_VoidSymbol}, "ȭ"}, // U022D | LATIN SMALL LETTER O WITH TILDE AND MACRON
  {{XK_Multi_key, XK_macron, XK_otilde, XK_VoidSymbol}, "ȭ"},
  {{XK_Multi_key, XK_underscore, XK_otilde, XK_VoidSymbol}, "ȭ"},
  {{XK_dead_macron, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_dead_macron, XK_Multi_key, XK_asciitilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_Multi_key, XK_macron, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_Multi_key, XK_macron, XK_asciitilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_Multi_key, XK_underscore, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_Multi_key, XK_underscore, XK_asciitilde, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_dead_tilde, XK_omacron, XK_VoidSymbol}, "ȭ"},
  {{XK_dead_tilde, XK_dead_macron, XK_o, XK_VoidSymbol}, "ȭ"},
  {{XK_dead_acute, XK_Greek_alpha, XK_VoidSymbol}, "ά"}, // U03AC | GREEK SMALL LETTER ALPHA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_alpha, XK_VoidSymbol}, "ά"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_alpha, XK_VoidSymbol}, "ά"},
  {{XK_Multi_key, XK_Greek_alpha, XK_apostrophe, XK_VoidSymbol}, "ά"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἧ"}, // U1F2F | GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἧ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἧ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἧ"},
  {{XK_dead_horn, XK_u, XK_VoidSymbol}, "ư"}, // U01B0 | LATIN SMALL LETTER U WITH HORN
  {{XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ư"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_j, XK_VoidSymbol}, "ʲ"}, // U02B2 | MODIFIER LETTER SMALL J
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_j, XK_VoidSymbol}, "ʲ"},
  {{XK_dead_belowmacron, XK_k, XK_VoidSymbol}, "ḵ"}, // U1E35 | LATIN SMALL LETTER K WITH LINE BELOW
  {{XK_dead_iota, XK_Greek_alphaaccent, XK_VoidSymbol}, "ᾴ"}, // U1FB4 | GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_alphaaccent, XK_VoidSymbol}, "ᾴ"},
  {{XK_dead_iota, XK_dead_acute, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_Greek_alpha, XK_VoidSymbol}, "ᾴ"},
  {{XK_Multi_key, XK_bracketleft, XK_bracketright, XK_VoidSymbol}, "⌷"}, // U2337 | [ ] APL FUNCTIONAL SYMBOL SQUISH QUAD
  {{XK_Multi_key, XK_bracketright, XK_bracketleft, XK_VoidSymbol}, "⌷"},
  {{XK_Multi_key, XK_p, XK_exclam, XK_VoidSymbol}, "¶"}, // paragraph | PILCROW SIGN
  {{XK_Multi_key, XK_P, XK_exclam, XK_VoidSymbol}, "¶"},
  {{XK_Multi_key, XK_P, XK_P, XK_VoidSymbol}, "¶"},
  {{XK_dead_breve, XK_Cyrillic_i, XK_VoidSymbol}, "й"}, // U0439 | CYRILLIC SMALL LETTER SHORT I
  {{XK_Multi_key, XK_U, XK_Cyrillic_i, XK_VoidSymbol}, "й"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_i, XK_VoidSymbol}, "й"},
  {{XK_dead_diaeresis, XK_space, XK_VoidSymbol}, "\""}, // quotedbl | REVERSE SOLIDUS
  {{XK_dead_cedilla, XK_L, XK_VoidSymbol}, "Ļ"}, // U013B | LATIN CAPITAL LETTER L WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_L, XK_VoidSymbol}, "Ļ"},
  {{XK_Multi_key, XK_L, XK_comma, XK_VoidSymbol}, "Ļ"},
  {{XK_Multi_key, XK_cedilla, XK_L, XK_VoidSymbol}, "Ļ"},
  {{XK_dead_hook, XK_E, XK_VoidSymbol}, "Ẻ"}, // U1EBA | LATIN CAPITAL LETTER E WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_E, XK_VoidSymbol}, "Ẻ"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἷ"}, // U1F3F | GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἷ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἷ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἷ"},
  {{XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ô"}, // ocircumflex | LATIN SMALL LETTER O WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_o, XK_VoidSymbol}, "ô"},
  {{XK_Multi_key, XK_o, XK_asciicircum, XK_VoidSymbol}, "ô"},
  {{XK_Multi_key, XK_greater, XK_o, XK_VoidSymbol}, "ô"},
  {{XK_Multi_key, XK_o, XK_greater, XK_VoidSymbol}, "ô"},
  {{XK_dead_hook, XK_W, XK_VoidSymbol}, "Ⱳ"}, // U2C72 | LATIN CAPITAL LETTER W WITH HOOK
  {{XK_dead_abovedot, XK_n, XK_VoidSymbol}, "ṅ"}, // U1E45 | LATIN SMALL LETTER N WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_n, XK_VoidSymbol}, "ṅ"},
  {{XK_dead_iota, XK_Greek_etaaccent, XK_VoidSymbol}, "ῄ"}, // U1FC4 | GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_etaaccent, XK_VoidSymbol}, "ῄ"},
  {{XK_dead_iota, XK_dead_acute, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_Greek_eta, XK_VoidSymbol}, "ῄ"},
  {{XK_Multi_key, XK_A, XK_E, XK_VoidSymbol}, "Æ"}, // AE | LATIN CAPITAL LETTER AE
  {{XK_Multi_key, XK_3, XK_8, XK_VoidSymbol}, "⅜"}, // U215C | VULGAR FRACTION THREE EIGHTHS
  {{XK_Multi_key, XK_n, XK_g, XK_VoidSymbol}, "ŋ"}, // U014B | LATIN SMALL LETTER ENG
  {{XK_dead_belowdot, XK_I, XK_VoidSymbol}, "Ị"}, // U1ECA | LATIN CAPITAL LETTER I WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_I, XK_VoidSymbol}, "Ị"},
  {{XK_dead_stroke, XK_r, XK_VoidSymbol}, "ɍ"}, // U024D | LATIN SMALL LETTER R WITH STROKE
  {{XK_dead_acute, XK_Greek_omicron, XK_VoidSymbol}, "ό"}, // U03CC | GREEK SMALL LETTER OMICRON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_omicron, XK_VoidSymbol}, "ό"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_omicron, XK_VoidSymbol}, "ό"},
  {{XK_Multi_key, XK_Greek_omicron, XK_apostrophe, XK_VoidSymbol}, "ό"},
  {{XK_dead_acute, XK_Otilde, XK_VoidSymbol}, "Ṍ"}, // U1E4C | LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Otilde, XK_VoidSymbol}, "Ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_Otilde, XK_VoidSymbol}, "Ṍ"},
  {{XK_dead_acute, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_Multi_key, XK_acute, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_Multi_key, XK_acute, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_dead_tilde, XK_Oacute, XK_VoidSymbol}, "Ṍ"},
  {{XK_dead_tilde, XK_dead_acute, XK_O, XK_VoidSymbol}, "Ṍ"},
  {{XK_dead_caron, XK_i, XK_VoidSymbol}, "ǐ"}, // U01D0 | LATIN SMALL LETTER I WITH CARON
  {{XK_Multi_key, XK_c, XK_i, XK_VoidSymbol}, "ǐ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_U, XK_parenright, XK_VoidSymbol}, "㋒"}, // U32D2 | CIRCLED KATAKANA U
  {{XK_dead_acute, XK_p, XK_VoidSymbol}, "ṕ"}, // U1E55 | LATIN SMALL LETTER P WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_p, XK_VoidSymbol}, "ṕ"},
  {{XK_Multi_key, XK_apostrophe, XK_p, XK_VoidSymbol}, "ṕ"},
  {{XK_dead_diaeresis, XK_O, XK_VoidSymbol}, "Ö"}, // Odiaeresis | LATIN CAPITAL LETTER O WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_O, XK_VoidSymbol}, "Ö"},
  {{XK_Multi_key, XK_O, XK_quotedbl, XK_VoidSymbol}, "Ö"},
  {{XK_Multi_key, XK_diaeresis, XK_O, XK_VoidSymbol}, "Ö"},
  {{XK_Multi_key, XK_O, XK_diaeresis, XK_VoidSymbol}, "Ö"},
  {{XK_dead_hook, XK_z, XK_VoidSymbol}, "ȥ"}, // U0225 | LATIN SMALL LETTER Z WITH HOOK
  {{XK_dead_acute, XK_s, XK_VoidSymbol}, "ś"}, // U015B | LATIN SMALL LETTER S WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_s, XK_VoidSymbol}, "ś"},
  {{XK_Multi_key, XK_apostrophe, XK_s, XK_VoidSymbol}, "ś"},
  {{XK_Multi_key, XK_s, XK_apostrophe, XK_VoidSymbol}, "ś"},
  {{XK_dead_acute, XK_Ohorn, XK_VoidSymbol}, "Ớ"}, // U1EDA | LATIN CAPITAL LETTER O WITH HORN AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Ohorn, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_Ohorn, XK_VoidSymbol}, "Ớ"},
  {{XK_dead_acute, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_dead_acute, XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_acute, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_acute, XK_plus, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_plus, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_dead_horn, XK_Oacute, XK_VoidSymbol}, "Ớ"},
  {{XK_dead_horn, XK_dead_acute, XK_O, XK_VoidSymbol}, "Ớ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_3, XK_parenright, XK_VoidSymbol}, "㉝"}, // U325D | CIRCLED NUMBER THIRTY THREE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㉝"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_3, XK_parenright, XK_VoidSymbol}, "㉝"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㉝"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὗ"}, // U1F5F | GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὗ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὗ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὗ"},
  {{XK_dead_diaeresis, XK_Cyrillic_ZE, XK_VoidSymbol}, "Ӟ"}, // U04DE | CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_ZE, XK_VoidSymbol}, "Ӟ"},
  {{XK_Multi_key, XK_1, XK_8, XK_VoidSymbol}, "⅛"}, // U215B | VULGAR FRACTION ONE EIGHTH
  {{XK_dead_macron, XK_dead_abovedot, XK_A, XK_VoidSymbol}, "Ǡ"}, // U01E0 | LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_period, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_Multi_key, XK_macron, XK_dead_abovedot, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_Multi_key, XK_macron, XK_period, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_Multi_key, XK_underscore, XK_dead_abovedot, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_Multi_key, XK_underscore, XK_period, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_dead_abovedot, XK_Amacron, XK_VoidSymbol}, "Ǡ"},
  {{XK_dead_abovedot, XK_dead_macron, XK_A, XK_VoidSymbol}, "Ǡ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_s, XK_VoidSymbol}, "ˢ"}, // U02E2 | MODIFIER LETTER SMALL S
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_s, XK_VoidSymbol}, "ˢ"},
  {{XK_Multi_key, XK_less, XK_3, XK_VoidSymbol}, "♥"}, // U2665 | BLACK HEART SUIT
  {{XK_dead_psili, XK_Greek_rho, XK_VoidSymbol}, "ῤ"}, // U1FE4 | GREEK SMALL LETTER RHO WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_rho, XK_VoidSymbol}, "ῤ"},
  {{XK_Multi_key, XK_a, XK_e, XK_VoidSymbol}, "æ"}, // ae | LATIN SMALL LETTER AE
  {{XK_Multi_key, XK_parenleft, XK_1, XK_0, XK_parenright, XK_VoidSymbol}, "⑩"}, // U2469 | CIRCLED NUMBER TEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⑩"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_0, XK_parenright, XK_VoidSymbol}, "⑩"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⑩"},
  {{XK_dead_acute, XK_Cyrillic_IE, XK_VoidSymbol}, "Е́"}, // CYRILLIC CAPITAL LETTER IE WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_IE, XK_VoidSymbol}, "Е́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_IE, XK_VoidSymbol}, "Е́"},
  {{XK_dead_macron, XK_u, XK_VoidSymbol}, "ū"}, // U016B | LATIN SMALL LETTER U WITH MACRON
  {{XK_Multi_key, XK_macron, XK_u, XK_VoidSymbol}, "ū"},
  {{XK_Multi_key, XK_underscore, XK_u, XK_VoidSymbol}, "ū"},
  {{XK_Multi_key, XK_u, XK_underscore, XK_VoidSymbol}, "ū"},
  {{XK_Multi_key, XK_minus, XK_u, XK_VoidSymbol}, "ū"},
  {{XK_Multi_key, XK_u, XK_minus, XK_VoidSymbol}, "ū"},
  {{XK_Multi_key, XK_exclam, XK_asciicircum, XK_VoidSymbol}, "¦"}, // brokenbar | BROKEN BAR
  {{XK_Multi_key, XK_parenleft, XK_d, XK_parenright, XK_VoidSymbol}, "ⓓ"}, // U24D3 | CIRCLED LATIN SMALL LETTER D
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὧ"}, // U1F6F | GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὧ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὧ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὧ"},
  {{XK_dead_macron, XK_Cyrillic_U, XK_VoidSymbol}, "Ӯ"}, // U04EE | CYRILLIC CAPITAL LETTER U WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_U, XK_VoidSymbol}, "Ӯ"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_U, XK_VoidSymbol}, "Ӯ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_i, XK_VoidSymbol}, "ⁱ"}, // U2071 | SUPERSCRIPT LATIN SMALL LETTER I
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_i, XK_VoidSymbol}, "ⁱ"},
  {{XK_dead_caron, XK_j, XK_VoidSymbol}, "ǰ"}, // U01F0 | LATIN SMALL LETTER J WITH CARON
  {{XK_Multi_key, XK_c, XK_j, XK_VoidSymbol}, "ǰ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_MO, XK_parenright, XK_VoidSymbol}, "㋲"}, // U32F2 | CIRCLED KATAKANA MO
  {{XK_dead_belowtilde, XK_u, XK_VoidSymbol}, "ṵ"}, // U1E75 | LATIN SMALL LETTER U WITH TILDE BELOW
  {{XK_dead_iota, XK_Greek_omegaaccent, XK_VoidSymbol}, "ῴ"}, // U1FF4 | GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_omegaaccent, XK_VoidSymbol}, "ῴ"},
  {{XK_dead_iota, XK_dead_acute, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_Greek_omega, XK_VoidSymbol}, "ῴ"},
  {{XK_dead_diaeresis, XK_o, XK_VoidSymbol}, "ö"}, // odiaeresis | LATIN SMALL LETTER O WITH DIAERESIS
  {{XK_Multi_key, XK_o, XK_diaeresis, XK_VoidSymbol}, "ö"},
  {{XK_Multi_key, XK_diaeresis, XK_o, XK_VoidSymbol}, "ö"},
  {{XK_Multi_key, XK_quotedbl, XK_o, XK_VoidSymbol}, "ö"},
  {{XK_Multi_key, XK_o, XK_quotedbl, XK_VoidSymbol}, "ö"},
  {{XK_dead_invertedbreve, XK_Cyrillic_A, XK_VoidSymbol}, "А̑"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING INVERTED BREVE
  {{XK_dead_abovedot, XK_Z, XK_VoidSymbol}, "Ż"}, // U017B | LATIN CAPITAL LETTER Z WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_Z, XK_VoidSymbol}, "Ż"},
  {{XK_Multi_key, XK_Z, XK_period, XK_VoidSymbol}, "Ż"},
  {{XK_dead_doublegrave, XK_A, XK_VoidSymbol}, "Ȁ"}, // U0200 | LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
  {{XK_dead_acute, XK_w, XK_VoidSymbol}, "ẃ"}, // U1E83 | LATIN SMALL LETTER W WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_w, XK_VoidSymbol}, "ẃ"},
  {{XK_Multi_key, XK_apostrophe, XK_w, XK_VoidSymbol}, "ẃ"},
  {{XK_dead_breve, XK_A, XK_VoidSymbol}, "Ă"}, // U0102 | LATIN CAPITAL LETTER A WITH BREVE
  {{XK_Multi_key, XK_U, XK_A, XK_VoidSymbol}, "Ă"},
  {{XK_Multi_key, XK_u, XK_A, XK_VoidSymbol}, "Ă"},
  {{XK_Multi_key, XK_b, XK_A, XK_VoidSymbol}, "Ă"},
  {{XK_Multi_key, XK_A, XK_parenleft, XK_VoidSymbol}, "Ă"},
  {{XK_dead_doublegrave, XK_Cyrillic_u, XK_VoidSymbol}, "у̏"}, // CYRILLIC SMALL LETTER U WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_u, XK_VoidSymbol}, "у̏"},
  {{XK_Multi_key, XK_underscore, XK_7, XK_VoidSymbol}, "₇"}, // U2087 | SUBSCRIPT SEVEN
  {{XK_Multi_key, XK_underscore, XK_KP_7, XK_VoidSymbol}, "₇"},
  {{XK_dead_caron, XK_7, XK_VoidSymbol}, "₇"},
  {{XK_Multi_key, XK_d, XK_i, XK_VoidSymbol}, "⌀"}, // U2300 | DIAMETER SIGN
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾉ"}, // U1F89 | GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾉ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾉ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾉ"},
  {{XK_dead_acute, XK_Ccedilla, XK_VoidSymbol}, "Ḉ"}, // U1E08 | LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Ccedilla, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_Ccedilla, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_acute, XK_dead_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_acute, XK_Multi_key, XK_comma, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_acute, XK_Multi_key, XK_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_acute, XK_dead_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_acute, XK_comma, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_acute, XK_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_cedilla, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_cedilla, XK_Cacute, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_cedilla, XK_dead_acute, XK_C, XK_VoidSymbol}, "Ḉ"},
  {{XK_dead_hook, XK_v, XK_VoidSymbol}, "ʋ"}, // U028B | LATIN SMALL LETTER V WITH HOOK
  {{XK_Multi_key, XK_2, XK_5, XK_VoidSymbol}, "⅖"}, // U2156 | VULGAR FRACTION TWO FIFTHS
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἆ"}, // U1F0E | GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἆ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἆ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἆ"},
  {{XK_dead_doublegrave, XK_R, XK_VoidSymbol}, "Ȑ"}, // U0210 | LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
  {{XK_dead_belowdot, XK_z, XK_VoidSymbol}, "ẓ"}, // U1E93 | LATIN SMALL LETTER Z WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_z, XK_VoidSymbol}, "ẓ"},
  {{XK_dead_macron, XK_E, XK_VoidSymbol}, "Ē"}, // U0112 | LATIN CAPITAL LETTER E WITH MACRON
  {{XK_Multi_key, XK_macron, XK_E, XK_VoidSymbol}, "Ē"},
  {{XK_Multi_key, XK_underscore, XK_E, XK_VoidSymbol}, "Ē"},
  {{XK_Multi_key, XK_E, XK_underscore, XK_VoidSymbol}, "Ē"},
  {{XK_Multi_key, XK_minus, XK_E, XK_VoidSymbol}, "Ē"},
  {{XK_Multi_key, XK_E, XK_minus, XK_VoidSymbol}, "Ē"},
  {{XK_Multi_key, XK_F, XK_U, XK_VoidSymbol}, "🖕"}, // U1F595 | REVERSED HAND WITH MIDDLE FINGER EXTENDED
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾙ"}, // U1F99 | GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾙ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾙ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾙ"},
  {{XK_Multi_key, XK_question, XK_exclam, XK_VoidSymbol}, "⸘"}, // U2E18 | INVERTED INTERROBANG
  {{XK_dead_invertedbreve, XK_Cyrillic_u, XK_VoidSymbol}, "у̑"}, // CYRILLIC SMALL LETTER U WITH COMBINING INVERTED BREVE
  {{XK_dead_belowtilde, XK_i, XK_VoidSymbol}, "ḭ"}, // U1E2D | LATIN SMALL LETTER I WITH TILDE BELOW
  {{XK_Multi_key, XK_1, XK_7, XK_VoidSymbol}, "⅐"}, // U2150 | VULGAR FRACTION ONE SEVENTH
  {{XK_Multi_key, XK_less, XK_quotedbl, XK_VoidSymbol}, "“"}, // U201c | LEFT DOUBLE QUOTATION MARK
  {{XK_Multi_key, XK_quotedbl, XK_less, XK_VoidSymbol}, "“"},
  {{XK_Multi_key, XK_slash, XK_Cyrillic_ka, XK_VoidSymbol}, "ҟ"}, // U049F | CYRILLIC SMALL LETTER KA WITH STROKE
  {{XK_Multi_key, XK_KP_Divide, XK_Cyrillic_ka, XK_VoidSymbol}, "ҟ"},
  {{XK_dead_hook, XK_a, XK_VoidSymbol}, "ả"}, // U1EA3 | LATIN SMALL LETTER A WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_a, XK_VoidSymbol}, "ả"},
  {{XK_dead_cedilla, XK_G, XK_VoidSymbol}, "Ģ"}, // U0122 | LATIN CAPITAL LETTER G WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_G, XK_VoidSymbol}, "Ģ"},
  {{XK_Multi_key, XK_G, XK_comma, XK_VoidSymbol}, "Ģ"},
  {{XK_Multi_key, XK_cedilla, XK_G, XK_VoidSymbol}, "Ģ"},
  {{XK_Multi_key, XK_P, XK_t, XK_VoidSymbol}, "₧"}, // U20a7 | PESETA SIGN
  {{XK_dead_currency, XK_P, XK_VoidSymbol}, "₧"},
  {{XK_dead_belowcomma, XK_nobreakspace, XK_VoidSymbol}, "̦"}, // U0326 | COMBINING COMMA BELOW
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾩ"}, // U1FA9 | GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾩ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾩ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾩ"},
  {{XK_dead_cedilla, XK_H, XK_VoidSymbol}, "Ḩ"}, // U1E28 | LATIN CAPITAL LETTER H WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_H, XK_VoidSymbol}, "Ḩ"},
  {{XK_Multi_key, XK_H, XK_comma, XK_VoidSymbol}, "Ḩ"},
  {{XK_Multi_key, XK_cedilla, XK_H, XK_VoidSymbol}, "Ḩ"},
  {{XK_dead_currency, XK_G, XK_VoidSymbol}, "₲"}, // U20B2 | GUARANI SIGN
  {{XK_dead_currency, XK_g, XK_VoidSymbol}, "₲"},
  {{XK_dead_belowcomma, XK_dead_belowcomma, XK_VoidSymbol}, ","}, // comma | COMMA
  {{XK_dead_belowcomma, XK_space, XK_VoidSymbol}, ","},
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἦ"}, // U1F2E | GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἦ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἦ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἦ"},
  {{XK_dead_hook, XK_b, XK_VoidSymbol}, "ɓ"}, // U0253 | LATIN SMALL LETTER B WITH HOOK
  {{XK_dead_macron, XK_dead_abovedot, XK_O, XK_VoidSymbol}, "Ȱ"}, // U0230 | LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_period, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_Multi_key, XK_macron, XK_dead_abovedot, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_Multi_key, XK_macron, XK_period, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_Multi_key, XK_underscore, XK_dead_abovedot, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_Multi_key, XK_underscore, XK_period, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_dead_abovedot, XK_Omacron, XK_VoidSymbol}, "Ȱ"},
  {{XK_dead_abovedot, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ȱ"},
  {{XK_dead_hook, XK_abreve, XK_VoidSymbol}, "ẳ"}, // U1EB3 | LATIN SMALL LETTER A WITH BREVE AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_abreve, XK_VoidSymbol}, "ẳ"},
  {{XK_dead_hook, XK_dead_breve, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_dead_hook, XK_Multi_key, XK_U, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_dead_hook, XK_Multi_key, XK_b, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_Multi_key, XK_question, XK_dead_breve, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_Multi_key, XK_question, XK_b, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_dead_breve, XK_ahook, XK_VoidSymbol}, "ẳ"},
  {{XK_dead_breve, XK_dead_hook, XK_a, XK_VoidSymbol}, "ẳ"},
  {{XK_Multi_key, XK_I, XK_J, XK_VoidSymbol}, "Ĳ"}, // U0132 | LATIN CAPITAL LIGATURE IJ
  {{XK_Multi_key, XK_I, XK_j, XK_VoidSymbol}, "Ĳ"},
  {{XK_dead_voiced_sound, XK_kana_KI, XK_VoidSymbol}, "ギ"}, // U30AE | KATAKANA LETTER GI
  {{XK_Multi_key, XK_period, XK_minus, XK_VoidSymbol}, "·"}, // periodcentered | MIDDLE DOT
  {{XK_Multi_key, XK_period, XK_asciicircum, XK_VoidSymbol}, "·"},
  {{XK_Multi_key, XK_asciicircum, XK_period, XK_VoidSymbol}, "·"},
  {{XK_dead_macron, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾱ"}, // U1FB9 | GREEK CAPITAL LETTER ALPHA WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾱ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾱ"},
  {{XK_dead_macron, XK_dead_belowdot, XK_L, XK_VoidSymbol}, "Ḹ"}, // U1E38 | LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_exclam, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_Multi_key, XK_macron, XK_dead_belowdot, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_Multi_key, XK_macron, XK_exclam, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_Multi_key, XK_underscore, XK_dead_belowdot, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_Multi_key, XK_underscore, XK_exclam, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_dead_belowdot, XK_dead_macron, XK_L, XK_VoidSymbol}, "Ḹ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_6, XK_parenright, XK_VoidSymbol}, "㊻"}, // U32BB | CIRCLED NUMBER FORTY SIX
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㊻"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_6, XK_parenright, XK_VoidSymbol}, "㊻"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㊻"},
  {{XK_Multi_key, XK_7, XK_8, XK_VoidSymbol}, "⅞"}, // U215E | VULGAR FRACTION SEVEN EIGHTHS
  {{XK_Multi_key, XK_parenleft, XK_J, XK_parenright, XK_VoidSymbol}, "Ⓙ"}, // U24BF | CIRCLED LATIN CAPITAL LETTER J
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἶ"}, // U1F3E | GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἶ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἶ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἶ"},
  {{XK_dead_hook, XK_ecircumflex, XK_VoidSymbol}, "ể"}, // U1EC3 | LATIN SMALL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_ecircumflex, XK_VoidSymbol}, "ể"},
  {{XK_dead_hook, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ể"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_e, XK_VoidSymbol}, "ể"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ể"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_e, XK_VoidSymbol}, "ể"},
  {{XK_dead_circumflex, XK_ehook, XK_VoidSymbol}, "ể"},
  {{XK_dead_circumflex, XK_dead_hook, XK_e, XK_VoidSymbol}, "ể"},
  {{XK_dead_stroke, XK_l, XK_VoidSymbol}, "ł"}, // U0142 | LATIN SMALL LETTER L WITH STROKE
  {{XK_Multi_key, XK_slash, XK_l, XK_VoidSymbol}, "ł"},
  {{XK_Multi_key, XK_l, XK_slash, XK_VoidSymbol}, "ł"},
  {{XK_Multi_key, XK_KP_Divide, XK_l, XK_VoidSymbol}, "ł"},
  {{XK_dead_cedilla, XK_C, XK_VoidSymbol}, "Ç"}, // Ccedilla | LATIN CAPITAL LETTER C WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_C, XK_VoidSymbol}, "Ç"},
  {{XK_Multi_key, XK_C, XK_comma, XK_VoidSymbol}, "Ç"},
  {{XK_Multi_key, XK_cedilla, XK_C, XK_VoidSymbol}, "Ç"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_parenright, XK_VoidSymbol}, "①"}, // U2460 | CIRCLED DIGIT ONE
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_parenright, XK_VoidSymbol}, "①"},
  {{XK_dead_belowmacron, XK_N, XK_VoidSymbol}, "Ṉ"}, // U1E48 | LATIN CAPITAL LETTER N WITH LINE BELOW
  {{XK_dead_voiced_sound, XK_kana_SA, XK_VoidSymbol}, "ザ"}, // U30B6 | KATAKANA LETTER ZA
  {{XK_dead_caron, XK_A, XK_VoidSymbol}, "Ǎ"}, // U01CD | LATIN CAPITAL LETTER A WITH CARON
  {{XK_Multi_key, XK_c, XK_A, XK_VoidSymbol}, "Ǎ"},
  {{XK_Multi_key, XK_parenleft, XK_t, XK_parenright, XK_VoidSymbol}, "ⓣ"}, // U24E3 | CIRCLED LATIN SMALL LETTER T
  {{XK_Multi_key, XK_parenleft, XK_Z, XK_parenright, XK_VoidSymbol}, "Ⓩ"}, // U24CF | CIRCLED LATIN CAPITAL LETTER Z
  {{XK_dead_grave, XK_ocircumflex, XK_VoidSymbol}, "ồ"}, // U1ED3 | LATIN SMALL LETTER O WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_ocircumflex, XK_VoidSymbol}, "ồ"},
  {{XK_dead_grave, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ồ"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_o, XK_VoidSymbol}, "ồ"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ồ"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_o, XK_VoidSymbol}, "ồ"},
  {{XK_dead_circumflex, XK_ograve, XK_VoidSymbol}, "ồ"},
  {{XK_dead_circumflex, XK_dead_grave, XK_o, XK_VoidSymbol}, "ồ"},
  {{XK_Multi_key, XK_O, XK_E, XK_VoidSymbol}, "Œ"}, // OE | LATIN CAPITAL LIGATURE OE
  {{XK_Multi_key, XK_x, XK_x, XK_VoidSymbol}, "×"}, // multiply | MULTIPLICATION SIGN
  {{XK_dead_macron, XK_Greek_IOTA, XK_VoidSymbol}, "Ῑ"}, // U1FD9 | GREEK CAPITAL LETTER IOTA WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_IOTA, XK_VoidSymbol}, "Ῑ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_IOTA, XK_VoidSymbol}, "Ῑ"},
  {{XK_dead_abovedot, XK_R, XK_VoidSymbol}, "Ṙ"}, // U1E58 | LATIN CAPITAL LETTER R WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_R, XK_VoidSymbol}, "Ṙ"},
  {{XK_dead_ogonek, XK_space, XK_VoidSymbol}, "˛"}, // ogonek | OGONEK
  {{XK_dead_ogonek, XK_dead_ogonek, XK_VoidSymbol}, "˛"},
  {{XK_Multi_key, XK_slash, XK_slash, XK_VoidSymbol}, "\\"}, // backslash | REVERSE SOLIDUS
  {{XK_Multi_key, XK_slash, XK_less, XK_VoidSymbol}, "\\"},
  {{XK_Multi_key, XK_less, XK_slash, XK_VoidSymbol}, "\\"},
  {{XK_dead_diaeresis, XK_Cyrillic_ze, XK_VoidSymbol}, "ӟ"}, // U04DF | CYRILLIC SMALL LETTER ZE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_ze, XK_VoidSymbol}, "ӟ"},
  {{XK_dead_voiced_sound, XK_kana_TO, XK_VoidSymbol}, "ド"}, // U30C9 | KATAKANA LETTER DO
  {{XK_Multi_key, XK_slash, XK_equal, XK_VoidSymbol}, "≠"}, // U2260 | NOT EQUAL TO
  {{XK_Multi_key, XK_equal, XK_slash, XK_VoidSymbol}, "≠"},
  {{XK_dead_stroke, XK_equal, XK_VoidSymbol}, "≠"},
  {{XK_dead_belowdot, XK_ohorn, XK_VoidSymbol}, "ợ"}, // U1EE3 | LATIN SMALL LETTER O WITH HORN AND DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_ohorn, XK_VoidSymbol}, "ợ"},
  {{XK_dead_belowdot, XK_dead_horn, XK_o, XK_VoidSymbol}, "ợ"},
  {{XK_dead_belowdot, XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ợ"},
  {{XK_Multi_key, XK_exclam, XK_dead_horn, XK_o, XK_VoidSymbol}, "ợ"},
  {{XK_Multi_key, XK_exclam, XK_plus, XK_o, XK_VoidSymbol}, "ợ"},
  {{XK_dead_horn, XK_obelowdot, XK_VoidSymbol}, "ợ"},
  {{XK_dead_horn, XK_dead_belowdot, XK_o, XK_VoidSymbol}, "ợ"},
  {{XK_dead_cedilla, XK_T, XK_VoidSymbol}, "Ţ"}, // U0162 | LATIN CAPITAL LETTER T WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_T, XK_VoidSymbol}, "Ţ"},
  {{XK_Multi_key, XK_T, XK_comma, XK_VoidSymbol}, "Ţ"},
  {{XK_Multi_key, XK_cedilla, XK_T, XK_VoidSymbol}, "Ţ"},
  {{XK_Multi_key, XK_parenleft, XK_5, XK_parenright, XK_VoidSymbol}, "⑤"}, // U2464 | CIRCLED DIGIT FIVE
  {{XK_Multi_key, XK_parenleft, XK_KP_5, XK_parenright, XK_VoidSymbol}, "⑤"},
  {{XK_dead_cedilla, XK_c, XK_VoidSymbol}, "ç"}, // ccedilla | LATIN SMALL LETTER C WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_c, XK_VoidSymbol}, "ç"},
  {{XK_Multi_key, XK_c, XK_comma, XK_VoidSymbol}, "ç"},
  {{XK_Multi_key, XK_cedilla, XK_c, XK_VoidSymbol}, "ç"},
  {{XK_dead_macron, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῡ"}, // U1FE9 | GREEK CAPITAL LETTER UPSILON WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῡ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῡ"},
  {{XK_dead_abovedot, XK_dead_belowdot, XK_S, XK_VoidSymbol}, "Ṩ"}, // U1E68 | LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
  {{XK_dead_abovedot, XK_Multi_key, XK_exclam, XK_S, XK_VoidSymbol}, "Ṩ"},
  {{XK_Multi_key, XK_period, XK_dead_belowdot, XK_S, XK_VoidSymbol}, "Ṩ"},
  {{XK_Multi_key, XK_period, XK_exclam, XK_S, XK_VoidSymbol}, "Ṩ"},
  {{XK_dead_belowdot, XK_Sabovedot, XK_VoidSymbol}, "Ṩ"},
  {{XK_dead_belowdot, XK_dead_abovedot, XK_S, XK_VoidSymbol}, "Ṩ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_FU, XK_parenright, XK_VoidSymbol}, "㋫"}, // U32EB | CIRCLED KATAKANA HU
  {{XK_dead_macron, XK_dead_ogonek, XK_o, XK_VoidSymbol}, "ǭ"}, // U01ED | LATIN SMALL LETTER O WITH OGONEK AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_semicolon, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_Multi_key, XK_macron, XK_dead_ogonek, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_Multi_key, XK_macron, XK_semicolon, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_Multi_key, XK_underscore, XK_dead_ogonek, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_Multi_key, XK_underscore, XK_semicolon, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_dead_ogonek, XK_omacron, XK_VoidSymbol}, "ǭ"},
  {{XK_dead_ogonek, XK_dead_macron, XK_o, XK_VoidSymbol}, "ǭ"},
  {{XK_dead_macron, XK_Cyrillic_u, XK_VoidSymbol}, "ӯ"}, // U04EF | CYRILLIC SMALL LETTER U WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_u, XK_VoidSymbol}, "ӯ"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_u, XK_VoidSymbol}, "ӯ"},
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὦ"}, // U1F6E | GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὦ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὦ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὦ"},
  {{XK_dead_grave, XK_y, XK_VoidSymbol}, "ỳ"}, // U1EF3 | LATIN SMALL LETTER Y WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_y, XK_VoidSymbol}, "ỳ"},
  {{XK_dead_ogonek, XK_U, XK_VoidSymbol}, "Ų"}, // U0172 | LATIN CAPITAL LETTER U WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_U, XK_VoidSymbol}, "Ų"},
  {{XK_Multi_key, XK_U, XK_semicolon, XK_VoidSymbol}, "Ų"},
  {{XK_Multi_key, XK_comma, XK_U, XK_VoidSymbol}, "Ų"},
  {{XK_Multi_key, XK_U, XK_comma, XK_VoidSymbol}, "Ų"},
  {{XK_Multi_key, XK_slash, XK_minus, XK_VoidSymbol}, "⌿"}, // U233f | / - APL FUNCTIONAL SYMBOL SLASH BAR
  {{XK_Multi_key, XK_minus, XK_slash, XK_VoidSymbol}, "⌿"},
  {{XK_Multi_key, XK_colon, XK_minus, XK_VoidSymbol}, "÷"}, // division | DIVISION SIGN
  {{XK_Multi_key, XK_minus, XK_colon, XK_VoidSymbol}, "÷"},
  {{XK_dead_acute, XK_Utilde, XK_VoidSymbol}, "Ṹ"}, // U1E78 | LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Utilde, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_Utilde, XK_VoidSymbol}, "Ṹ"},
  {{XK_dead_acute, XK_dead_tilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciitilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_acute, XK_dead_tilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_acute, XK_asciitilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_tilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciitilde, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_dead_tilde, XK_Uacute, XK_VoidSymbol}, "Ṹ"},
  {{XK_dead_tilde, XK_dead_acute, XK_U, XK_VoidSymbol}, "Ṹ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_WA, XK_parenright, XK_VoidSymbol}, "㋻"}, // U32FB | CIRCLED KATAKANA WA
  {{XK_Multi_key, XK_parenleft, XK_I, XK_parenright, XK_VoidSymbol}, "Ⓘ"}, // U24BE | CIRCLED LATIN CAPITAL LETTER I
  {{XK_dead_acute, XK_ae, XK_VoidSymbol}, "ǽ"}, // U01FD | LATIN SMALL LETTER AE WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_ae, XK_VoidSymbol}, "ǽ"},
  {{XK_Multi_key, XK_apostrophe, XK_ae, XK_VoidSymbol}, "ǽ"},
  {{XK_Multi_key, XK_slash, XK_asciicircum, XK_VoidSymbol}, "|"}, // bar | VERTICAL LINE
  {{XK_Multi_key, XK_asciicircum, XK_slash, XK_VoidSymbol}, "|"},
  {{XK_Multi_key, XK_V, XK_L, XK_VoidSymbol}, "|"},
  {{XK_Multi_key, XK_L, XK_V, XK_VoidSymbol}, "|"},
  {{XK_Multi_key, XK_v, XK_l, XK_VoidSymbol}, "|"},
  {{XK_Multi_key, XK_l, XK_v, XK_VoidSymbol}, "|"},
  {{XK_dead_invertedbreve, XK_Cyrillic_a, XK_VoidSymbol}, "а̑"}, // CYRILLIC SMALL LETTER A WITH COMBINING INVERTED BREVE
  {{XK_Multi_key, XK_f, XK_i, XK_VoidSymbol}, "ﬁ"}, // Ufb01 | LATIN SMALL LIGATURE FI
  {{XK_dead_abovedot, XK_b, XK_VoidSymbol}, "ḃ"}, // U1E03 | LATIN SMALL LETTER B WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_b, XK_VoidSymbol}, "ḃ"},
  {{XK_Multi_key, XK_b, XK_period, XK_VoidSymbol}, "ḃ"},
  {{XK_dead_acute, XK_Greek_ALPHA, XK_VoidSymbol}, "Ά"}, // U0386 | GREEK CAPITAL LETTER ALPHA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_ALPHA, XK_VoidSymbol}, "Ά"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_ALPHA, XK_VoidSymbol}, "Ά"},
  {{XK_Multi_key, XK_Greek_ALPHA, XK_apostrophe, XK_VoidSymbol}, "Ά"},
  {{XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἁ"}, // U1F09 | GREEK CAPITAL LETTER ALPHA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἁ"},
  {{XK_dead_belowdot, XK_W, XK_VoidSymbol}, "Ẉ"}, // U1E88 | LATIN CAPITAL LETTER W WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_W, XK_VoidSymbol}, "Ẉ"},
  {{XK_dead_invertedbreve, XK_i, XK_VoidSymbol}, "ȋ"}, // U020B | LATIN SMALL LETTER I WITH INVERTED BREVE
  {{XK_dead_caron, XK_c, XK_VoidSymbol}, "č"}, // U010D | LATIN SMALL LETTER C WITH CARON
  {{XK_Multi_key, XK_c, XK_c, XK_VoidSymbol}, "č"},
  {{XK_Multi_key, XK_less, XK_c, XK_VoidSymbol}, "č"},
  {{XK_Multi_key, XK_c, XK_less, XK_VoidSymbol}, "č"},
  {{XK_Multi_key, XK_underscore, XK_equal, XK_VoidSymbol}, "₌"}, // U208C | SUBSCRIPT EQUALS SIGN
  {{XK_Multi_key, XK_underscore, XK_KP_Equal, XK_VoidSymbol}, "₌"},
  {{XK_dead_caron, XK_equal, XK_VoidSymbol}, "₌"},
  {{XK_dead_belowdot, XK_T, XK_VoidSymbol}, "Ṭ"}, // U1E6C | LATIN CAPITAL LETTER T WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_T, XK_VoidSymbol}, "Ṭ"},
  {{XK_Multi_key, XK_1, XK_3, XK_VoidSymbol}, "⅓"}, // U2153 | VULGAR FRACTION ONE THIRD
  {{XK_dead_belowcircumflex, XK_d, XK_VoidSymbol}, "ḓ"}, // U1E13 | LATIN SMALL LETTER D WITH CIRCUMFLEX BELOW
  {{XK_Multi_key, XK_minus, XK_greater, XK_VoidSymbol}, "→"}, // U2192 | RIGHTWARDS ARROW
  {{XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἑ"}, // U1F19 | GREEK CAPITAL LETTER EPSILON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἑ"},
  {{XK_dead_abovering, XK_w, XK_VoidSymbol}, "ẘ"}, // U1E98 | LATIN SMALL LETTER W WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_w, XK_VoidSymbol}, "ẘ"},
  {{XK_dead_belowcomma, XK_t, XK_VoidSymbol}, "ț"}, // U021B | LATIN SMALL LETTER T WITH COMMA BELOW
  {{XK_Multi_key, XK_semicolon, XK_t, XK_VoidSymbol}, "ț"},
  {{XK_Multi_key, XK_t, XK_semicolon, XK_VoidSymbol}, "ț"},
  {{XK_dead_circumflex, XK_g, XK_VoidSymbol}, "ĝ"}, // U011D | LATIN SMALL LETTER G WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_g, XK_VoidSymbol}, "ĝ"},
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"}, // U1F9E | GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾞ"},
  {{XK_dead_hook, XK_q, XK_VoidSymbol}, "ʠ"}, // U02A0 | LATIN SMALL LETTER Q WITH HOOK
  {{XK_dead_abovedot, XK_h, XK_VoidSymbol}, "ḣ"}, // U1E23 | LATIN SMALL LETTER H WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_h, XK_VoidSymbol}, "ḣ"},
  {{XK_dead_acute, XK_space, XK_VoidSymbol}, "'"}, // apostrophe | APOSTROPHE
  {{XK_Multi_key, XK_apostrophe, XK_space, XK_VoidSymbol}, "'"},
  {{XK_Multi_key, XK_space, XK_apostrophe, XK_VoidSymbol}, "'"},
  {{XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἡ"}, // U1F29 | GREEK CAPITAL LETTER ETA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἡ"},
  {{XK_dead_hook, XK_Acircumflex, XK_VoidSymbol}, "Ẩ"}, // U1EA8 | LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Acircumflex, XK_VoidSymbol}, "Ẩ"},
  {{XK_dead_hook, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ẩ"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ẩ"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ẩ"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ẩ"},
  {{XK_dead_circumflex, XK_Ahook, XK_VoidSymbol}, "Ẩ"},
  {{XK_dead_circumflex, XK_dead_hook, XK_A, XK_VoidSymbol}, "Ẩ"},
  {{XK_dead_macron, XK_odiaeresis, XK_VoidSymbol}, "ȫ"}, // U022B | LATIN SMALL LETTER O WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_odiaeresis, XK_VoidSymbol}, "ȫ"},
  {{XK_Multi_key, XK_underscore, XK_odiaeresis, XK_VoidSymbol}, "ȫ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_dead_diaeresis, XK_omacron, XK_VoidSymbol}, "ȫ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_o, XK_VoidSymbol}, "ȫ"},
  {{XK_dead_breve, XK_i, XK_VoidSymbol}, "ĭ"}, // U012D | LATIN SMALL LETTER I WITH BREVE
  {{XK_Multi_key, XK_U, XK_i, XK_VoidSymbol}, "ĭ"},
  {{XK_Multi_key, XK_b, XK_i, XK_VoidSymbol}, "ĭ"},
  {{XK_Multi_key, XK_comma, XK_minus, XK_VoidSymbol}, "¬"}, // notsign | NOT SIGN
  {{XK_Multi_key, XK_minus, XK_comma, XK_VoidSymbol}, "¬"},
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"}, // U1FAE | GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾮ"},
  {{XK_dead_semivoiced_sound, XK_kana_HA, XK_VoidSymbol}, "パ"}, // U30D1 | KATAKANA LETTER PA
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_h, XK_VoidSymbol}, "ʰ"}, // U02B0 | MODIFIER LETTER SMALL H
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_h, XK_VoidSymbol}, "ʰ"},
  {{XK_dead_belowdot, XK_k, XK_VoidSymbol}, "ḳ"}, // U1E33 | LATIN SMALL LETTER K WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_k, XK_VoidSymbol}, "ḳ"},
  {{XK_dead_hook, XK_V, XK_VoidSymbol}, "Ʋ"}, // U01B2 | LATIN CAPITAL LETTER V WITH HOOK
  {{XK_dead_doublegrave, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̏"}, // CYRILLIC CAPITAL LETTER IE WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̏"},
  {{XK_dead_hook, XK_w, XK_VoidSymbol}, "ⱳ"}, // U2C73 | LATIN SMALL LETTER W WITH HOOK
  {{XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἱ"}, // U1F39 | GREEK CAPITAL LETTER IOTA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἱ"},
  {{XK_dead_belowdot, XK_E, XK_VoidSymbol}, "Ẹ"}, // U1EB8 | LATIN CAPITAL LETTER E WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_E, XK_VoidSymbol}, "Ẹ"},
  {{XK_dead_stroke, XK_C, XK_VoidSymbol}, "Ȼ"}, // U023B | LATIN CAPITAL LETTER C WITH STROKE
  {{XK_Multi_key, XK_8, XK_8, XK_VoidSymbol}, "∞"}, // U221e | 8 8 INFINITY
  {{XK_dead_caron, XK_L, XK_VoidSymbol}, "Ľ"}, // U013D | LATIN CAPITAL LETTER L WITH CARON
  {{XK_Multi_key, XK_c, XK_L, XK_VoidSymbol}, "Ľ"},
  {{XK_Multi_key, XK_less, XK_L, XK_VoidSymbol}, "Ľ"},
  {{XK_Multi_key, XK_L, XK_less, XK_VoidSymbol}, "Ľ"},
  {{XK_Multi_key, XK_1, XK_4, XK_VoidSymbol}, "¼"}, // onequarter | VULGAR FRACTION ONE QUARTER
  {{XK_dead_semivoiced_sound, XK_kana_HO, XK_VoidSymbol}, "ポ"}, // U30DD | KATAKANA LETTER PO
  {{XK_dead_belowdot, XK_m, XK_VoidSymbol}, "ṃ"}, // U1E43 | LATIN SMALL LETTER M WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_m, XK_VoidSymbol}, "ṃ"},
  {{XK_Multi_key, XK_parenleft, XK_O, XK_parenright, XK_VoidSymbol}, "Ⓞ"}, // U24C4 | CIRCLED LATIN CAPITAL LETTER O
  {{XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὁ"}, // U1F49 | GREEK CAPITAL LETTER OMICRON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὁ"},
  {{XK_dead_hook, XK_I, XK_VoidSymbol}, "Ỉ"}, // U1EC8 | LATIN CAPITAL LETTER I WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_I, XK_VoidSymbol}, "Ỉ"},
  {{XK_dead_macron, XK_o, XK_VoidSymbol}, "ō"}, // U014D | LATIN SMALL LETTER O WITH MACRON
  {{XK_Multi_key, XK_macron, XK_o, XK_VoidSymbol}, "ō"},
  {{XK_Multi_key, XK_underscore, XK_o, XK_VoidSymbol}, "ō"},
  {{XK_Multi_key, XK_o, XK_underscore, XK_VoidSymbol}, "ō"},
  {{XK_Multi_key, XK_minus, XK_o, XK_VoidSymbol}, "ō"},
  {{XK_Multi_key, XK_o, XK_minus, XK_VoidSymbol}, "ō"},
  {{XK_dead_grave, XK_I, XK_VoidSymbol}, "Ì"}, // Igrave | LATIN CAPITAL LETTER I WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_I, XK_VoidSymbol}, "Ì"},
  {{XK_Multi_key, XK_I, XK_grave, XK_VoidSymbol}, "Ì"},
  {{XK_Multi_key, XK_parenleft, XK_kana_A, XK_parenright, XK_VoidSymbol}, "㋐"}, // U32D0 | CIRCLED KATAKANA A
  {{XK_dead_acute, XK_omacron, XK_VoidSymbol}, "ṓ"}, // U1E53 | LATIN SMALL LETTER O WITH MACRON AND ACUTE
  {{XK_Multi_key, XK_acute, XK_omacron, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_omacron, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_acute, XK_dead_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_acute, XK_Multi_key, XK_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_acute, XK_Multi_key, XK_underscore, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_acute, XK_dead_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_acute, XK_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_acute, XK_underscore, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_macron, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_underscore, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_macron, XK_oacute, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_macron, XK_dead_acute, XK_o, XK_VoidSymbol}, "ṓ"},
  {{XK_dead_caron, XK_o, XK_VoidSymbol}, "ǒ"}, // U01D2 | LATIN SMALL LETTER O WITH CARON
  {{XK_Multi_key, XK_c, XK_o, XK_VoidSymbol}, "ǒ"},
  {{XK_Multi_key, XK_1, XK_5, XK_VoidSymbol}, "⅕"}, // U2155 | VULGAR FRACTION ONE FIFTH
  {{XK_Multi_key, XK_parenleft, XK_e, XK_parenright, XK_VoidSymbol}, "ⓔ"}, // U24D4 | CIRCLED LATIN SMALL LETTER E
  {{XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὑ"}, // U1F59 | GREEK CAPITAL LETTER UPSILON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὑ"},
  {{XK_Multi_key, XK_m, XK_slash, XK_VoidSymbol}, "₥"}, // U20a5 | MILL SIGN
  {{XK_Multi_key, XK_slash, XK_m, XK_VoidSymbol}, "₥"},
  {{XK_dead_currency, XK_m, XK_VoidSymbol}, "₥"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_1, XK_parenright, XK_VoidSymbol}, "㉛"}, // U325B | CIRCLED NUMBER THIRTY ONE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㉛"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_1, XK_parenright, XK_VoidSymbol}, "㉛"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㉛"},
  {{XK_Multi_key, XK_parenleft, XK_Y, XK_parenright, XK_VoidSymbol}, "Ⓨ"}, // U24CE | CIRCLED LATIN CAPITAL LETTER Y
  {{XK_dead_circumflex, XK_s, XK_VoidSymbol}, "ŝ"}, // U015D | LATIN SMALL LETTER S WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_s, XK_VoidSymbol}, "ŝ"},
  {{XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ü"}, // Udiaeresis | LATIN CAPITAL LETTER U WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ü"},
  {{XK_Multi_key, XK_U, XK_quotedbl, XK_VoidSymbol}, "Ü"},
  {{XK_Multi_key, XK_diaeresis, XK_U, XK_VoidSymbol}, "Ü"},
  {{XK_Multi_key, XK_U, XK_diaeresis, XK_VoidSymbol}, "Ü"},
  {{XK_Multi_key, XK_parenleft, XK_kana_CHI, XK_parenright, XK_VoidSymbol}, "㋠"}, // U32E0 | CIRCLED KATAKANA TI
  {{XK_dead_belowdot, XK_s, XK_VoidSymbol}, "ṣ"}, // U1E63 | LATIN SMALL LETTER S WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_s, XK_VoidSymbol}, "ṣ"},
  {{XK_dead_macron, XK_AE, XK_VoidSymbol}, "Ǣ"}, // U01E2 | LATIN CAPITAL LETTER AE WITH MACRON
  {{XK_Multi_key, XK_macron, XK_AE, XK_VoidSymbol}, "Ǣ"},
  {{XK_Multi_key, XK_underscore, XK_AE, XK_VoidSymbol}, "Ǣ"},
  {{XK_dead_circumflex, XK_Cyrillic_u, XK_VoidSymbol}, "у̂"}, // CYRILLIC SMALL LETTER U WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_u, XK_VoidSymbol}, "у̂"},
  {{XK_dead_diaeresis, XK_Cyrillic_I, XK_VoidSymbol}, "Ӥ"}, // U04E4 | CYRILLIC CAPITAL LETTER I WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_I, XK_VoidSymbol}, "Ӥ"},
  {{XK_dead_invertedbreve, XK_Cyrillic_er, XK_VoidSymbol}, "р̑"}, // CYRILLIC SMALL LETTER ER WITH COMBINING INVERTED BREVE
  {{XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὡ"}, // U1F69 | GREEK CAPITAL LETTER OMEGA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὡ"},
  {{XK_dead_acute, XK_Uhorn, XK_VoidSymbol}, "Ứ"}, // U1EE8 | LATIN CAPITAL LETTER U WITH HORN AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Uhorn, XK_VoidSymbol}, "Ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_Uhorn, XK_VoidSymbol}, "Ứ"},
  {{XK_dead_acute, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_dead_acute, XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_Multi_key, XK_acute, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_Multi_key, XK_acute, XK_plus, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_plus, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_dead_horn, XK_Uacute, XK_VoidSymbol}, "Ứ"},
  {{XK_dead_horn, XK_dead_acute, XK_U, XK_VoidSymbol}, "Ứ"},
  {{XK_dead_currency, XK_u, XK_VoidSymbol}, "元"}, // U5143 | YUAN / WEN
  {{XK_dead_belowtilde, XK_plus, XK_VoidSymbol}, "⨦"}, // U2A26 | PLUS SIGN WITH TILDE BELOW
  {{XK_dead_breve, XK_u, XK_VoidSymbol}, "ŭ"}, // U016D | LATIN SMALL LETTER U WITH BREVE
  {{XK_Multi_key, XK_U, XK_u, XK_VoidSymbol}, "ŭ"},
  {{XK_Multi_key, XK_u, XK_u, XK_VoidSymbol}, "ŭ"},
  {{XK_Multi_key, XK_b, XK_u, XK_VoidSymbol}, "ŭ"},
  {{XK_dead_grave, XK_i, XK_VoidSymbol}, "ì"}, // igrave | LATIN SMALL LETTER I WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_i, XK_VoidSymbol}, "ì"},
  {{XK_Multi_key, XK_i, XK_grave, XK_VoidSymbol}, "ì"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_6, XK_parenright, XK_VoidSymbol}, "⑯"}, // U246F | CIRCLED NUMBER SIXTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_6, XK_parenright, XK_VoidSymbol}, "⑯"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_6, XK_parenright, XK_VoidSymbol}, "⑯"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_6, XK_parenright, XK_VoidSymbol}, "⑯"},
  {{XK_dead_belowring, XK_bar, XK_VoidSymbol}, "⫰"}, // U2AF0 | VERTICAL LINE WITH CIRCLE BELOW
  {{XK_dead_belowdiaeresis, XK_u, XK_VoidSymbol}, "ṳ"}, // U1E73 | LATIN SMALL LETTER U WITH DIAERESIS BELOW
  {{XK_dead_grave, XK_Ocircumflex, XK_VoidSymbol}, "Ồ"}, // U1ED2 | LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Ocircumflex, XK_VoidSymbol}, "Ồ"},
  {{XK_dead_grave, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ồ"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ồ"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ồ"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ồ"},
  {{XK_dead_circumflex, XK_Ograve, XK_VoidSymbol}, "Ồ"},
  {{XK_dead_circumflex, XK_dead_grave, XK_O, XK_VoidSymbol}, "Ồ"},
  {{XK_dead_diaeresis, XK_Cyrillic_CHE, XK_VoidSymbol}, "Ӵ"}, // U04F4 | CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_CHE, XK_VoidSymbol}, "Ӵ"},
  {{XK_dead_circumflex, XK_7, XK_VoidSymbol}, "⁷"}, // U2077 | SUPERSCRIPT SEVEN
  {{XK_Multi_key, XK_asciicircum, XK_7, XK_VoidSymbol}, "⁷"},
  {{XK_dead_circumflex, XK_KP_7, XK_VoidSymbol}, "⁷"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_7, XK_VoidSymbol}, "⁷"},
  {{XK_dead_tilde, XK_Y, XK_VoidSymbol}, "Ỹ"}, // U1EF8 | LATIN CAPITAL LETTER Y WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Y, XK_VoidSymbol}, "Ỹ"},
  {{XK_dead_voiced_sound, XK_kana_HO, XK_VoidSymbol}, "ボ"}, // U30DC | KATAKANA LETTER BO
  {{XK_dead_caron, XK_Z, XK_VoidSymbol}, "Ž"}, // U017D | LATIN CAPITAL LETTER Z WITH CARON
  {{XK_Multi_key, XK_c, XK_Z, XK_VoidSymbol}, "Ž"},
  {{XK_Multi_key, XK_v, XK_Z, XK_VoidSymbol}, "Ž"},
  {{XK_Multi_key, XK_less, XK_Z, XK_VoidSymbol}, "Ž"},
  {{XK_Multi_key, XK_Z, XK_less, XK_VoidSymbol}, "Ž"},
  {{XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ü"}, // udiaeresis | LATIN SMALL LETTER U WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_u, XK_VoidSymbol}, "ü"},
  {{XK_Multi_key, XK_u, XK_quotedbl, XK_VoidSymbol}, "ü"},
  {{XK_Multi_key, XK_diaeresis, XK_u, XK_VoidSymbol}, "ü"},
  {{XK_Multi_key, XK_u, XK_diaeresis, XK_VoidSymbol}, "ü"},
  {{XK_dead_circumflex, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̂"}, // CYRILLIC CAPITAL LETTER IE WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̂"},
  {{XK_Multi_key, XK_f, XK_f, XK_VoidSymbol}, "ﬀ"}, // Ufb00 | LATIN SMALL LIGATURE FF
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"}, // U1F83 | GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾃ"},
  {{XK_dead_hook, XK_C, XK_VoidSymbol}, "Ƈ"}, // U0187 | LATIN CAPITAL LETTER C WITH HOOK
  {{XK_dead_invertedbreve, XK_E, XK_VoidSymbol}, "Ȇ"}, // U0206 | LATIN CAPITAL LETTER E WITH INVERTED BREVE
  {{XK_dead_belowdot, XK_w, XK_VoidSymbol}, "ẉ"}, // U1E89 | LATIN SMALL LETTER W WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_w, XK_VoidSymbol}, "ẉ"},
  {{XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἀ"}, // U1F08 | GREEK CAPITAL LETTER ALPHA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἀ"},
  {{XK_Multi_key, XK_parenleft, XK_g, XK_parenright, XK_VoidSymbol}, "ⓖ"}, // U24D6 | CIRCLED LATIN SMALL LETTER G
  {{XK_Multi_key, XK_underscore, XK_parenleft, XK_VoidSymbol}, "₍"}, // U208D | SUBSCRIPT LEFT PARENTHESIS
  {{XK_dead_caron, XK_parenleft, XK_VoidSymbol}, "₍"},
  {{XK_dead_caron, XK_C, XK_VoidSymbol}, "Č"}, // U010C | LATIN CAPITAL LETTER C WITH CARON
  {{XK_Multi_key, XK_c, XK_C, XK_VoidSymbol}, "Č"},
  {{XK_Multi_key, XK_less, XK_C, XK_VoidSymbol}, "Č"},
  {{XK_Multi_key, XK_C, XK_less, XK_VoidSymbol}, "Č"},
  {{XK_dead_belowmacron, XK_D, XK_VoidSymbol}, "Ḏ"}, // U1E0E | LATIN CAPITAL LETTER D WITH LINE BELOW
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"}, // U1F93 | GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾓ"},
  {{XK_dead_stroke, XK_I, XK_VoidSymbol}, "Ɨ"}, // U0197 | LATIN CAPITAL LETTER I WITH STROKE
  {{XK_Multi_key, XK_slash, XK_I, XK_VoidSymbol}, "Ɨ"},
  {{XK_Multi_key, XK_KP_Divide, XK_I, XK_VoidSymbol}, "Ɨ"},
  {{XK_dead_invertedbreve, XK_U, XK_VoidSymbol}, "Ȗ"}, // U0216 | LATIN CAPITAL LETTER U WITH INVERTED BREVE
  {{XK_dead_abovering, XK_y, XK_VoidSymbol}, "ẙ"}, // U1E99 | LATIN SMALL LETTER Y WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_y, XK_VoidSymbol}, "ẙ"},
  {{XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἐ"}, // U1F18 | GREEK CAPITAL LETTER EPSILON WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἐ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_MU, XK_parenright, XK_VoidSymbol}, "㋰"}, // U32F0 | CIRCLED KATAKANA MU
  {{XK_dead_circumflex, XK_G, XK_VoidSymbol}, "Ĝ"}, // U011C | LATIN CAPITAL LETTER G WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_G, XK_VoidSymbol}, "Ĝ"},
  {{XK_dead_abovedot, XK_F, XK_VoidSymbol}, "Ḟ"}, // U1E1E | LATIN CAPITAL LETTER F WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_F, XK_VoidSymbol}, "Ḟ"},
  {{XK_Multi_key, XK_F, XK_period, XK_VoidSymbol}, "Ḟ"},
  {{XK_dead_circumflex, XK_Cyrillic_I, XK_VoidSymbol}, "И̂"}, // CYRILLIC CAPITAL LETTER I WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_I, XK_VoidSymbol}, "И̂"},
  {{XK_dead_circumflex, XK_Cyrillic_A, XK_VoidSymbol}, "А̂"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_A, XK_VoidSymbol}, "А̂"},
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"}, // U1FA3 | GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾣ"},
  {{XK_Multi_key, XK_period, XK_equal, XK_VoidSymbol}, "•"}, // enfilledcircbullet | BULLET
  {{XK_dead_abovedot, XK_A, XK_VoidSymbol}, "Ȧ"}, // U0226 | LATIN CAPITAL LETTER A WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_A, XK_VoidSymbol}, "Ȧ"},
  {{XK_dead_hook, XK_acircumflex, XK_VoidSymbol}, "ẩ"}, // U1EA9 | LATIN SMALL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_acircumflex, XK_VoidSymbol}, "ẩ"},
  {{XK_dead_hook, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ẩ"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_a, XK_VoidSymbol}, "ẩ"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ẩ"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_a, XK_VoidSymbol}, "ẩ"},
  {{XK_dead_circumflex, XK_ahook, XK_VoidSymbol}, "ẩ"},
  {{XK_dead_circumflex, XK_dead_hook, XK_a, XK_VoidSymbol}, "ẩ"},
  {{XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἠ"}, // U1F28 | GREEK CAPITAL LETTER ETA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἠ"},
  {{XK_dead_diaeresis, XK_Greek_UPSILON, XK_VoidSymbol}, "Ϋ"}, // U03AB | GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
  {{XK_Multi_key, XK_quotedbl, XK_Greek_UPSILON, XK_VoidSymbol}, "Ϋ"},
  {{XK_Multi_key, XK_Greek_UPSILON, XK_quotedbl, XK_VoidSymbol}, "Ϋ"},
  {{XK_dead_currency, XK_Y, XK_VoidSymbol}, "円"}, // U5186 | YEN
  {{XK_Multi_key, XK_minus, XK_minus, XK_space, XK_VoidSymbol}, "­"}, // U00ad | SOFT HYPHEN
  {{XK_dead_breve, XK_I, XK_VoidSymbol}, "Ĭ"}, // U012C | LATIN CAPITAL LETTER I WITH BREVE
  {{XK_Multi_key, XK_U, XK_I, XK_VoidSymbol}, "Ĭ"},
  {{XK_Multi_key, XK_b, XK_I, XK_VoidSymbol}, "Ĭ"},
  {{XK_dead_acute, XK_Idiaeresis, XK_VoidSymbol}, "Ḯ"}, // U1E2E | LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Idiaeresis, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_Idiaeresis, XK_VoidSymbol}, "Ḯ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_dead_diaeresis, XK_Iacute, XK_VoidSymbol}, "Ḯ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_I, XK_VoidSymbol}, "Ḯ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_6, XK_parenright, XK_VoidSymbol}, "㊱"}, // U32B1 | CIRCLED NUMBER THIRTY SIX
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㊱"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_6, XK_parenright, XK_VoidSymbol}, "㊱"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㊱"},
  {{XK_dead_iota, XK_Greek_alpha, XK_VoidSymbol}, "ᾳ"}, // U1FB3 | GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_alpha, XK_VoidSymbol}, "ᾳ"},
  {{XK_dead_acute, XK_Cyrillic_A, XK_VoidSymbol}, "А́"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_A, XK_VoidSymbol}, "А́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_A, XK_VoidSymbol}, "А́"},
  {{XK_dead_macron, XK_Cyrillic_A, XK_VoidSymbol}, "А̄"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_A, XK_VoidSymbol}, "А̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_A, XK_VoidSymbol}, "А̄"},
  {{XK_dead_belowdot, XK_e, XK_VoidSymbol}, "ẹ"}, // U1EB9 | LATIN SMALL LETTER E WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_e, XK_VoidSymbol}, "ẹ"},
  {{XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἰ"}, // U1F38 | GREEK CAPITAL LETTER IOTA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἰ"},
  {{XK_Multi_key, XK_parenleft, XK_o, XK_parenright, XK_VoidSymbol}, "ⓞ"}, // U24DE | CIRCLED LATIN SMALL LETTER O
  {{XK_Multi_key, XK_1, XK_2, XK_VoidSymbol}, "½"}, // onehalf | VULGAR FRACTION ONE HALF
  {{XK_dead_cedilla, XK_l, XK_VoidSymbol}, "ļ"}, // U013C | LATIN SMALL LETTER L WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_l, XK_VoidSymbol}, "ļ"},
  {{XK_Multi_key, XK_l, XK_comma, XK_VoidSymbol}, "ļ"},
  {{XK_Multi_key, XK_cedilla, XK_l, XK_VoidSymbol}, "ļ"},
  {{XK_dead_acute, XK_M, XK_VoidSymbol}, "Ḿ"}, // U1E3E | LATIN CAPITAL LETTER M WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_M, XK_VoidSymbol}, "Ḿ"},
  {{XK_Multi_key, XK_apostrophe, XK_M, XK_VoidSymbol}, "Ḿ"},
  {{XK_Multi_key, XK_backslash, XK_minus, XK_VoidSymbol}, "⍀"}, // U2340 | \ - APL FUNCTIONAL SYMBOL BACKSLASH BAR
  {{XK_Multi_key, XK_minus, XK_backslash, XK_VoidSymbol}, "⍀"},
  {{XK_dead_iota, XK_Greek_eta, XK_VoidSymbol}, "ῃ"}, // U1FC3 | GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_eta, XK_VoidSymbol}, "ῃ"},
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"}, // U1F8A | GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾊ"},
  {{XK_Multi_key, XK_parenleft, XK_P, XK_parenright, XK_VoidSymbol}, "Ⓟ"}, // U24C5 | CIRCLED LATIN CAPITAL LETTER P
  {{XK_dead_currency, XK_K, XK_VoidSymbol}, "₭"}, // U20AD | KIP SIGN
  {{XK_dead_currency, XK_k, XK_VoidSymbol}, "₭"},
  {{XK_dead_hook, XK_i, XK_VoidSymbol}, "ỉ"}, // U1EC9 | LATIN SMALL LETTER I WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_i, XK_VoidSymbol}, "ỉ"},
  {{XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὀ"}, // U1F48 | GREEK CAPITAL LETTER OMICRON WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὀ"},
  {{XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ϋ"}, // U03CB | GREEK SMALL LETTER UPSILON WITH DIALYTIKA
  {{XK_Multi_key, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ϋ"},
  {{XK_Multi_key, XK_Greek_upsilon, XK_quotedbl, XK_VoidSymbol}, "ϋ"},
  {{XK_dead_acute, XK_I, XK_VoidSymbol}, "Í"}, // Iacute | LATIN CAPITAL LETTER I WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_I, XK_VoidSymbol}, "Í"},
  {{XK_Multi_key, XK_I, XK_acute, XK_VoidSymbol}, "Í"},
  {{XK_Multi_key, XK_apostrophe, XK_I, XK_VoidSymbol}, "Í"},
  {{XK_Multi_key, XK_I, XK_apostrophe, XK_VoidSymbol}, "Í"},
  {{XK_dead_macron, XK_O, XK_VoidSymbol}, "Ō"}, // U014C | LATIN CAPITAL LETTER O WITH MACRON
  {{XK_Multi_key, XK_macron, XK_O, XK_VoidSymbol}, "Ō"},
  {{XK_Multi_key, XK_underscore, XK_O, XK_VoidSymbol}, "Ō"},
  {{XK_Multi_key, XK_O, XK_underscore, XK_VoidSymbol}, "Ō"},
  {{XK_Multi_key, XK_minus, XK_O, XK_VoidSymbol}, "Ō"},
  {{XK_Multi_key, XK_O, XK_minus, XK_VoidSymbol}, "Ō"},
  {{XK_dead_diaeresis, XK_Otilde, XK_VoidSymbol}, "Ṏ"}, // U1E4E | LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Otilde, XK_VoidSymbol}, "Ṏ"},
  {{XK_dead_diaeresis, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ṏ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ṏ"},
  {{XK_Multi_key, XK_quotedbl, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ṏ"},
  {{XK_Multi_key, XK_quotedbl, XK_asciitilde, XK_O, XK_VoidSymbol}, "Ṏ"},
  {{XK_dead_tilde, XK_Odiaeresis, XK_VoidSymbol}, "Ṏ"},
  {{XK_dead_tilde, XK_dead_diaeresis, XK_O, XK_VoidSymbol}, "Ṏ"},
  {{XK_Multi_key, XK_parenleft, XK_b, XK_parenright, XK_VoidSymbol}, "ⓑ"}, // U24D1 | CIRCLED LATIN SMALL LETTER B
  {{XK_Multi_key, XK_parenleft, XK_kana_TE, XK_parenright, XK_VoidSymbol}, "㋢"}, // U32E2 | CIRCLED KATAKANA TE
  {{XK_Multi_key, XK_parenleft, XK_f, XK_parenright, XK_VoidSymbol}, "ⓕ"}, // U24D5 | CIRCLED LATIN SMALL LETTER F
  {{XK_dead_acute, XK_Udiaeresis, XK_VoidSymbol}, "Ǘ"}, // U01D7 | LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Udiaeresis, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_Udiaeresis, XK_VoidSymbol}, "Ǘ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_dead_acute, XK_V, XK_VoidSymbol}, "Ǘ"},
  {{XK_dead_diaeresis, XK_Uacute, XK_VoidSymbol}, "Ǘ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_U, XK_VoidSymbol}, "Ǘ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_6, XK_parenright, XK_VoidSymbol}, "㉖"}, // U3256 | CIRCLED NUMBER TWENTY SIX
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㉖"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_6, XK_parenright, XK_VoidSymbol}, "㉖"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㉖"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_6, XK_parenright, XK_VoidSymbol}, "㉖"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_6, XK_parenright, XK_VoidSymbol}, "㉖"},
  {{XK_dead_circumflex, XK_dead_belowdot, XK_o, XK_VoidSymbol}, "ộ"}, // U1ED9 | LATIN SMALL LETTER O WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_o, XK_VoidSymbol}, "ộ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_o, XK_VoidSymbol}, "ộ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_o, XK_VoidSymbol}, "ộ"},
  {{XK_dead_belowdot, XK_ocircumflex, XK_VoidSymbol}, "ộ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ộ"},
  {{XK_dead_abovedot, XK_sacute, XK_VoidSymbol}, "ṥ"}, // U1E65 | LATIN SMALL LETTER S WITH ACUTE AND DOT ABOVE
  {{XK_Multi_key, XK_period, XK_sacute, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_abovedot, XK_dead_acute, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_acute, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_apostrophe, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_Multi_key, XK_period, XK_dead_acute, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_Multi_key, XK_period, XK_acute, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_Multi_key, XK_period, XK_apostrophe, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_acute, XK_sabovedot, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_acute, XK_dead_abovedot, XK_s, XK_VoidSymbol}, "ṥ"},
  {{XK_dead_acute, XK_Y, XK_VoidSymbol}, "Ý"}, // Yacute | LATIN CAPITAL LETTER Y WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_Y, XK_VoidSymbol}, "Ý"},
  {{XK_Multi_key, XK_Y, XK_acute, XK_VoidSymbol}, "Ý"},
  {{XK_Multi_key, XK_apostrophe, XK_Y, XK_VoidSymbol}, "Ý"},
  {{XK_Multi_key, XK_Y, XK_apostrophe, XK_VoidSymbol}, "Ý"},
  {{XK_dead_circumflex, XK_S, XK_VoidSymbol}, "Ŝ"}, // U015C | LATIN CAPITAL LETTER S WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_S, XK_VoidSymbol}, "Ŝ"},
  {{XK_dead_belowmacron, XK_R, XK_VoidSymbol}, "Ṟ"}, // U1E5E | LATIN CAPITAL LETTER R WITH LINE BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_l, XK_VoidSymbol}, "ˡ"}, // U02E1 | MODIFIER LETTER SMALL L
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_l, XK_VoidSymbol}, "ˡ"},
  {{XK_dead_diaeresis, XK_Cyrillic_i, XK_VoidSymbol}, "ӥ"}, // U04E5 | CYRILLIC SMALL LETTER I WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_i, XK_VoidSymbol}, "ӥ"},
  {{XK_dead_caron, XK_g, XK_VoidSymbol}, "ǧ"}, // U01E7 | LATIN SMALL LETTER G WITH CARON
  {{XK_Multi_key, XK_c, XK_g, XK_VoidSymbol}, "ǧ"},
  {{XK_dead_belowdot, XK_equal, XK_VoidSymbol}, "⩦"}, // U2A66 | EQUALS SIGN WITH DOT BELOW
  {{XK_dead_acute, XK_uhorn, XK_VoidSymbol}, "ứ"}, // U1EE9 | LATIN SMALL LETTER U WITH HORN AND ACUTE
  {{XK_Multi_key, XK_acute, XK_uhorn, XK_VoidSymbol}, "ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_uhorn, XK_VoidSymbol}, "ứ"},
  {{XK_dead_acute, XK_dead_horn, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_dead_acute, XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_Multi_key, XK_acute, XK_dead_horn, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_Multi_key, XK_acute, XK_plus, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_horn, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_Multi_key, XK_apostrophe, XK_plus, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_dead_horn, XK_uacute, XK_VoidSymbol}, "ứ"},
  {{XK_dead_horn, XK_dead_acute, XK_u, XK_VoidSymbol}, "ứ"},
  {{XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὠ"}, // U1F68 | GREEK CAPITAL LETTER OMEGA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὠ"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_1, XK_parenright, XK_VoidSymbol}, "⑪"}, // U246A | CIRCLED NUMBER ELEVEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_1, XK_parenright, XK_VoidSymbol}, "⑪"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_1, XK_parenright, XK_VoidSymbol}, "⑪"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_1, XK_parenright, XK_VoidSymbol}, "⑪"},
  {{XK_dead_acute, XK_i, XK_VoidSymbol}, "í"}, // iacute | LATIN SMALL LETTER I WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_i, XK_VoidSymbol}, "í"},
  {{XK_Multi_key, XK_i, XK_acute, XK_VoidSymbol}, "í"},
  {{XK_Multi_key, XK_apostrophe, XK_i, XK_VoidSymbol}, "í"},
  {{XK_Multi_key, XK_i, XK_apostrophe, XK_VoidSymbol}, "í"},
  {{XK_dead_breve, XK_U, XK_VoidSymbol}, "Ŭ"}, // U016C | LATIN CAPITAL LETTER U WITH BREVE
  {{XK_Multi_key, XK_U, XK_U, XK_VoidSymbol}, "Ŭ"},
  {{XK_Multi_key, XK_u, XK_U, XK_VoidSymbol}, "Ŭ"},
  {{XK_Multi_key, XK_b, XK_U, XK_VoidSymbol}, "Ŭ"},
  {{XK_Multi_key, XK_numbersign, XK_f, XK_VoidSymbol}, "♮"}, // U266e | MUSIC NATURAL SIGN
  {{XK_Multi_key, XK_parenleft, XK_kana_ME, XK_parenright, XK_VoidSymbol}, "㋱"}, // U32F1 | CIRCLED KATAKANA ME
  {{XK_dead_iota, XK_Greek_omega, XK_VoidSymbol}, "ῳ"}, // U1FF3 | GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_omega, XK_VoidSymbol}, "ῳ"},
  {{XK_dead_diaeresis, XK_Cyrillic_che, XK_VoidSymbol}, "ӵ"}, // U04F5 | CYRILLIC SMALL LETTER CHE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_che, XK_VoidSymbol}, "ӵ"},
  {{XK_dead_tilde, XK_y, XK_VoidSymbol}, "ỹ"}, // U1EF9 | LATIN SMALL LETTER Y WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_y, XK_VoidSymbol}, "ỹ"},
  {{XK_dead_grave, XK_Greek_omicron, XK_VoidSymbol}, "ὸ"}, // U1F78 | GREEK SMALL LETTER OMICRON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_omicron, XK_VoidSymbol}, "ὸ"},
  {{XK_dead_acute, XK_Cyrillic_I, XK_VoidSymbol}, "И́"}, // CYRILLIC CAPITAL LETTER I WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_I, XK_VoidSymbol}, "И́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_I, XK_VoidSymbol}, "И́"},
  {{XK_dead_acute, XK_y, XK_VoidSymbol}, "ý"}, // yacute | LATIN SMALL LETTER Y WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_y, XK_VoidSymbol}, "ý"},
  {{XK_Multi_key, XK_y, XK_acute, XK_VoidSymbol}, "ý"},
  {{XK_Multi_key, XK_apostrophe, XK_y, XK_VoidSymbol}, "ý"},
  {{XK_Multi_key, XK_y, XK_apostrophe, XK_VoidSymbol}, "ý"},
  {{XK_dead_abovedot, XK_z, XK_VoidSymbol}, "ż"}, // U017C | LATIN SMALL LETTER Z WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_z, XK_VoidSymbol}, "ż"},
  {{XK_Multi_key, XK_z, XK_period, XK_VoidSymbol}, "ż"},
  {{XK_dead_voiced_sound, XK_kana_CHI, XK_VoidSymbol}, "ヂ"}, // U30C2 | KATAKANA LETTER DI
  {{XK_dead_belowdot, XK_V, XK_VoidSymbol}, "Ṿ"}, // U1E7E | LATIN CAPITAL LETTER V WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_V, XK_VoidSymbol}, "Ṿ"},
  {{XK_dead_doublegrave, XK_a, XK_VoidSymbol}, "ȁ"}, // U0201 | LATIN SMALL LETTER A WITH DOUBLE GRAVE
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἃ"}, // U1F03 | GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἃ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἃ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἃ"},
  {{XK_dead_grave, XK_Uhorn, XK_VoidSymbol}, "Ừ"}, // U1EEA | LATIN CAPITAL LETTER U WITH HORN AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Uhorn, XK_VoidSymbol}, "Ừ"},
  {{XK_dead_grave, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ừ"},
  {{XK_dead_grave, XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ừ"},
  {{XK_Multi_key, XK_grave, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ừ"},
  {{XK_Multi_key, XK_grave, XK_plus, XK_U, XK_VoidSymbol}, "Ừ"},
  {{XK_dead_horn, XK_Ugrave, XK_VoidSymbol}, "Ừ"},
  {{XK_dead_horn, XK_dead_grave, XK_U, XK_VoidSymbol}, "Ừ"},
  {{XK_dead_acute, XK_c, XK_VoidSymbol}, "ć"}, // U0107 | LATIN SMALL LETTER C WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_c, XK_VoidSymbol}, "ć"},
  {{XK_Multi_key, XK_apostrophe, XK_c, XK_VoidSymbol}, "ć"},
  {{XK_Multi_key, XK_c, XK_apostrophe, XK_VoidSymbol}, "ć"},
  {{XK_dead_acute, XK_ccedilla, XK_VoidSymbol}, "ḉ"}, // U1E09 | LATIN SMALL LETTER C WITH CEDILLA AND ACUTE
  {{XK_Multi_key, XK_acute, XK_ccedilla, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_ccedilla, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_acute, XK_dead_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_acute, XK_Multi_key, XK_comma, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_acute, XK_Multi_key, XK_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_acute, XK_dead_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_acute, XK_comma, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_acute, XK_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_Multi_key, XK_apostrophe, XK_cedilla, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_cedilla, XK_cacute, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_cedilla, XK_dead_acute, XK_c, XK_VoidSymbol}, "ḉ"},
  {{XK_dead_belowdot, XK_t, XK_VoidSymbol}, "ṭ"}, // U1E6D | LATIN SMALL LETTER T WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_t, XK_VoidSymbol}, "ṭ"},
  {{XK_dead_doubleacute, XK_nobreakspace, XK_VoidSymbol}, "̋"}, // U030B | COMBINING DOUBLE ACUTE ACCENT
  {{XK_dead_abovedot, XK_Y, XK_VoidSymbol}, "Ẏ"}, // U1E8E | LATIN CAPITAL LETTER Y WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_Y, XK_VoidSymbol}, "Ẏ"},
  {{XK_dead_doublegrave, XK_r, XK_VoidSymbol}, "ȑ"}, // U0211 | LATIN SMALL LETTER R WITH DOUBLE GRAVE
  {{XK_dead_acute, XK_Greek_iotadieresis, XK_VoidSymbol}, "ΐ"}, // U0390 | GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_iotadieresis, XK_VoidSymbol}, "ΐ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_iotadieresis, XK_VoidSymbol}, "ΐ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_Greek_accentdieresis, XK_Greek_iota, XK_VoidSymbol}, "ΐ"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἓ"}, // U1F13 | GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἓ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἓ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἓ"},
  {{XK_dead_acute, XK_Cyrillic_a, XK_VoidSymbol}, "а́"}, // CYRILLIC SMALL LETTER A WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_a, XK_VoidSymbol}, "а́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_a, XK_VoidSymbol}, "а́"},
  {{XK_dead_grave, XK_Cyrillic_A, XK_VoidSymbol}, "А̀"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_A, XK_VoidSymbol}, "А̀"},
  {{XK_dead_abovedot, XK_e, XK_VoidSymbol}, "ė"}, // U0117 | LATIN SMALL LETTER E WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_e, XK_VoidSymbol}, "ė"},
  {{XK_Multi_key, XK_e, XK_period, XK_VoidSymbol}, "ė"},
  {{XK_dead_belowcircumflex, XK_e, XK_VoidSymbol}, "ḙ"}, // U1E19 | LATIN SMALL LETTER E WITH CIRCUMFLEX BELOW
  {{XK_dead_iota, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾘ"}, // U1F98 | GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾘ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾘ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾘ"},
  {{XK_dead_horn, XK_dead_horn, XK_VoidSymbol}, "̛"}, // U031B | COMBINING HORN
  {{XK_dead_horn, XK_nobreakspace, XK_VoidSymbol}, "̛"},
  {{XK_dead_horn, XK_space, XK_VoidSymbol}, "̛"},
  {{XK_Multi_key, XK_greater, XK_quotedbl, XK_VoidSymbol}, "”"}, // U201d | RIGHT DOUBLE QUOTATION MARK
  {{XK_Multi_key, XK_quotedbl, XK_greater, XK_VoidSymbol}, "”"},
  {{XK_dead_doublegrave, XK_Cyrillic_a, XK_VoidSymbol}, "а̏"}, // CYRILLIC SMALL LETTER A WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_a, XK_VoidSymbol}, "а̏"},
  {{XK_Multi_key, XK_S, XK_S, XK_VoidSymbol}, "ẞ"}, // U1e9e | LATIN CAPITAL LETTER SHARP S
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἣ"}, // U1F23 | GREEK SMALL LETTER ETA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἣ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἣ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἣ"},
  {{XK_Multi_key, XK_C, XK_bar, XK_VoidSymbol}, "¢"}, // cent | CENT SIGN
  {{XK_Multi_key, XK_bar, XK_C, XK_VoidSymbol}, "¢"},
  {{XK_Multi_key, XK_c, XK_bar, XK_VoidSymbol}, "¢"},
  {{XK_Multi_key, XK_bar, XK_c, XK_VoidSymbol}, "¢"},
  {{XK_Multi_key, XK_c, XK_slash, XK_VoidSymbol}, "¢"},
  {{XK_Multi_key, XK_slash, XK_c, XK_VoidSymbol}, "¢"},
  {{XK_dead_currency, XK_c, XK_VoidSymbol}, "¢"},
  {{XK_dead_doublegrave, XK_Cyrillic_U, XK_VoidSymbol}, "У̏"}, // CYRILLIC CAPITAL LETTER U WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_U, XK_VoidSymbol}, "У̏"},
  {{XK_dead_stroke, XK_h, XK_VoidSymbol}, "ħ"}, // U0127 | LATIN SMALL LETTER H WITH STROKE
  {{XK_Multi_key, XK_slash, XK_h, XK_VoidSymbol}, "ħ"},
  {{XK_Multi_key, XK_KP_Divide, XK_h, XK_VoidSymbol}, "ħ"},
  {{XK_dead_cedilla, XK_h, XK_VoidSymbol}, "ḩ"}, // U1E29 | LATIN SMALL LETTER H WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_h, XK_VoidSymbol}, "ḩ"},
  {{XK_Multi_key, XK_h, XK_comma, XK_VoidSymbol}, "ḩ"},
  {{XK_Multi_key, XK_cedilla, XK_h, XK_VoidSymbol}, "ḩ"},
  {{XK_dead_iota, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾨ"}, // U1FA8 | GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾨ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾨ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾨ"},
  {{XK_dead_acute, XK_Cyrillic_U, XK_VoidSymbol}, "У́"}, // CYRILLIC CAPITAL LETTER U WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_U, XK_VoidSymbol}, "У́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_U, XK_VoidSymbol}, "У́"},
  {{XK_dead_stroke, XK_E, XK_VoidSymbol}, "Ɇ"}, // U0246 | LATIN CAPITAL LETTER E WITH STROKE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_7, XK_parenright, XK_VoidSymbol}, "㊲"}, // U32B2 | CIRCLED NUMBER THIRTY SEVEN
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㊲"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_7, XK_parenright, XK_VoidSymbol}, "㊲"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_7, XK_parenright, XK_VoidSymbol}, "㊲"},
  {{XK_dead_hook, XK_T, XK_VoidSymbol}, "Ƭ"}, // U01AC | LATIN CAPITAL LETTER T WITH HOOK
  {{XK_dead_acute, XK_Abreve, XK_VoidSymbol}, "Ắ"}, // U1EAE | LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Abreve, XK_VoidSymbol}, "Ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_Abreve, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_acute, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_acute, XK_Multi_key, XK_U, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_acute, XK_Multi_key, XK_b, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_Multi_key, XK_acute, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_Multi_key, XK_acute, XK_b, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_Multi_key, XK_apostrophe, XK_b, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_breve, XK_Aacute, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_breve, XK_dead_acute, XK_A, XK_VoidSymbol}, "Ắ"},
  {{XK_dead_macron, XK_dead_abovedot, XK_o, XK_VoidSymbol}, "ȱ"}, // U0231 | LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_period, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_Multi_key, XK_macron, XK_dead_abovedot, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_Multi_key, XK_macron, XK_period, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_Multi_key, XK_underscore, XK_dead_abovedot, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_Multi_key, XK_underscore, XK_period, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_dead_abovedot, XK_omacron, XK_VoidSymbol}, "ȱ"},
  {{XK_dead_abovedot, XK_dead_macron, XK_o, XK_VoidSymbol}, "ȱ"},
  {{XK_dead_acute, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ΰ"}, // U03B0 | GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ΰ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ΰ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_Greek_accentdieresis, XK_Greek_upsilon, XK_VoidSymbol}, "ΰ"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἳ"}, // U1F33 | GREEK SMALL LETTER IOTA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἳ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἳ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἳ"},
  {{XK_dead_circumflex, XK_2, XK_VoidSymbol}, "²"}, // twosuperior | SUPERSCRIPT TWO
  {{XK_Multi_key, XK_asciicircum, XK_2, XK_VoidSymbol}, "²"},
  {{XK_Multi_key, XK_2, XK_asciicircum, XK_VoidSymbol}, "²"},
  {{XK_dead_circumflex, XK_KP_Space, XK_VoidSymbol}, "²"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_Space, XK_VoidSymbol}, "²"},
  {{XK_dead_circumflex, XK_KP_2, XK_VoidSymbol}, "²"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_2, XK_VoidSymbol}, "²"},
  {{XK_Multi_key, XK_parenleft, XK_kana_RA, XK_parenright, XK_VoidSymbol}, "㋶"}, // U32F6 | CIRCLED KATAKANA RA
  {{XK_dead_cedilla, XK_k, XK_VoidSymbol}, "ķ"}, // U0137 | LATIN SMALL LETTER K WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_k, XK_VoidSymbol}, "ķ"},
  {{XK_Multi_key, XK_k, XK_comma, XK_VoidSymbol}, "ķ"},
  {{XK_Multi_key, XK_cedilla, XK_k, XK_VoidSymbol}, "ķ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_1, XK_parenright, XK_VoidSymbol}, "㊶"}, // U32B6 | CIRCLED NUMBER FORTY ONE
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㊶"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_1, XK_parenright, XK_VoidSymbol}, "㊶"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㊶"},
  {{XK_Multi_key, XK_colon, XK_parenleft, XK_VoidSymbol}, "☹"}, // U2639 | WHITE FROWNING FACE
  {{XK_dead_breve, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾰ"}, // U1FB8 | GREEK CAPITAL LETTER ALPHA WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾰ"},
  {{XK_Multi_key, XK_b, XK_Greek_ALPHA, XK_VoidSymbol}, "Ᾰ"},
  {{XK_dead_invertedbreve, XK_Cyrillic_U, XK_VoidSymbol}, "У̑"}, // CYRILLIC CAPITAL LETTER U WITH COMBINING INVERTED BREVE
  {{XK_Multi_key, XK_parenleft, XK_E, XK_parenright, XK_VoidSymbol}, "Ⓔ"}, // U24BA | CIRCLED LATIN CAPITAL LETTER E
  {{XK_Multi_key, XK_exclam, XK_question, XK_VoidSymbol}, "‽"}, // U203D | INTERROBANG
  {{XK_dead_acute, XK_Ecircumflex, XK_VoidSymbol}, "Ế"}, // U1EBE | LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Ecircumflex, XK_VoidSymbol}, "Ế"},
  {{XK_Multi_key, XK_apostrophe, XK_Ecircumflex, XK_VoidSymbol}, "Ế"},
  {{XK_dead_acute, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_dead_circumflex, XK_Eacute, XK_VoidSymbol}, "Ế"},
  {{XK_dead_circumflex, XK_dead_acute, XK_E, XK_VoidSymbol}, "Ế"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὃ"}, // U1F43 | GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὃ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὃ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὃ"},
  {{XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Â"}, // Acircumflex | LATIN CAPITAL LETTER A WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_A, XK_VoidSymbol}, "Â"},
  {{XK_Multi_key, XK_A, XK_asciicircum, XK_VoidSymbol}, "Â"},
  {{XK_Multi_key, XK_greater, XK_A, XK_VoidSymbol}, "Â"},
  {{XK_Multi_key, XK_A, XK_greater, XK_VoidSymbol}, "Â"},
  {{XK_dead_caron, XK_N, XK_VoidSymbol}, "Ň"}, // U0147 | LATIN CAPITAL LETTER N WITH CARON
  {{XK_Multi_key, XK_c, XK_N, XK_VoidSymbol}, "Ň"},
  {{XK_Multi_key, XK_less, XK_N, XK_VoidSymbol}, "Ň"},
  {{XK_Multi_key, XK_N, XK_less, XK_VoidSymbol}, "Ň"},
  {{XK_Multi_key, XK_C, XK_E, XK_VoidSymbol}, "₠"}, // U20a0 | EURO-CURRENCY SIGN
  {{XK_dead_currency, XK_E, XK_VoidSymbol}, "₠"},
  {{XK_dead_belowmacron, XK_n, XK_VoidSymbol}, "ṉ"}, // U1E49 | LATIN SMALL LETTER N WITH LINE BELOW
  {{XK_dead_grave, XK_Greek_EPSILON, XK_VoidSymbol}, "Ὲ"}, // U1FC8 | GREEK CAPITAL LETTER EPSILON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_EPSILON, XK_VoidSymbol}, "Ὲ"},
  {{XK_Multi_key, XK_parenleft, XK_U, XK_parenright, XK_VoidSymbol}, "Ⓤ"}, // U24CA | CIRCLED LATIN CAPITAL LETTER U
  {{XK_dead_hook, XK_O, XK_VoidSymbol}, "Ỏ"}, // U1ECE | LATIN CAPITAL LETTER O WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_O, XK_VoidSymbol}, "Ỏ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_1, XK_parenright, XK_VoidSymbol}, "㉑"}, // U3251 | CIRCLED NUMBER TWENTY ONE
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㉑"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_1, XK_parenright, XK_VoidSymbol}, "㉑"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㉑"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_1, XK_parenright, XK_VoidSymbol}, "㉑"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_1, XK_parenright, XK_VoidSymbol}, "㉑"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὓ"}, // U1F53 | GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὓ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὓ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὓ"},
  {{XK_dead_grave, XK_O, XK_VoidSymbol}, "Ò"}, // Ograve | LATIN CAPITAL LETTER O WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_O, XK_VoidSymbol}, "Ò"},
  {{XK_Multi_key, XK_O, XK_grave, XK_VoidSymbol}, "Ò"},
  {{XK_dead_voiced_sound, XK_kana_SO, XK_VoidSymbol}, "ゾ"}, // U30BE | KATAKANA LETTER ZO
  {{XK_dead_cedilla, XK_r, XK_VoidSymbol}, "ŗ"}, // U0157 | LATIN SMALL LETTER R WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_r, XK_VoidSymbol}, "ŗ"},
  {{XK_Multi_key, XK_r, XK_comma, XK_VoidSymbol}, "ŗ"},
  {{XK_Multi_key, XK_cedilla, XK_r, XK_VoidSymbol}, "ŗ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_KI, XK_parenright, XK_VoidSymbol}, "㋖"}, // U32D6 | CIRCLED KATAKANA KI
  {{XK_dead_abovedot, XK_r, XK_VoidSymbol}, "ṙ"}, // U1E59 | LATIN SMALL LETTER R WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_r, XK_VoidSymbol}, "ṙ"},
  {{XK_dead_breve, XK_Greek_IOTA, XK_VoidSymbol}, "Ῐ"}, // U1FD8 | GREEK CAPITAL LETTER IOTA WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_IOTA, XK_VoidSymbol}, "Ῐ"},
  {{XK_Multi_key, XK_b, XK_Greek_IOTA, XK_VoidSymbol}, "Ῐ"},
  {{XK_Multi_key, XK_parenleft, XK_k, XK_parenright, XK_VoidSymbol}, "ⓚ"}, // U24DA | CIRCLED LATIN SMALL LETTER K
  {{XK_Multi_key, XK_parenright, XK_parenright, XK_VoidSymbol}, "]"}, // bracketright | RIGHT SQUARE BRACKET
  {{XK_dead_grave, XK_udiaeresis, XK_VoidSymbol}, "ǜ"}, // U01DC | LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
  {{XK_Multi_key, XK_grave, XK_udiaeresis, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_grave, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_grave, XK_Multi_key, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǜ"},
  {{XK_Multi_key, XK_grave, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǜ"},
  {{XK_Multi_key, XK_grave, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_diaeresis, XK_ugrave, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_grave, XK_v, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_diaeresis, XK_dead_grave, XK_u, XK_VoidSymbol}, "ǜ"},
  {{XK_dead_hook, XK_Ohorn, XK_VoidSymbol}, "Ở"}, // U1EDE | LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Ohorn, XK_VoidSymbol}, "Ở"},
  {{XK_dead_hook, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ở"},
  {{XK_dead_hook, XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ở"},
  {{XK_Multi_key, XK_question, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ở"},
  {{XK_Multi_key, XK_question, XK_plus, XK_O, XK_VoidSymbol}, "Ở"},
  {{XK_dead_horn, XK_Ohook, XK_VoidSymbol}, "Ở"},
  {{XK_dead_horn, XK_dead_hook, XK_O, XK_VoidSymbol}, "Ở"},
  {{XK_Multi_key, XK_equal, XK_underscore, XK_VoidSymbol}, "≡"}, // U2261 | = _ IDENTICAL TO
  {{XK_Multi_key, XK_parenleft, XK_kana_I, XK_parenright, XK_VoidSymbol}, "㋑"}, // U32D1 | CIRCLED KATAKANA I
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὣ"}, // U1F63 | GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὣ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὣ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὣ"},
  {{XK_dead_circumflex, XK_a, XK_VoidSymbol}, "â"}, // acircumflex | LATIN SMALL LETTER A WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_a, XK_VoidSymbol}, "â"},
  {{XK_Multi_key, XK_a, XK_asciicircum, XK_VoidSymbol}, "â"},
  {{XK_Multi_key, XK_greater, XK_a, XK_VoidSymbol}, "â"},
  {{XK_Multi_key, XK_a, XK_greater, XK_VoidSymbol}, "â"},
  {{XK_Multi_key, XK_parenleft, XK_6, XK_parenright, XK_VoidSymbol}, "⑥"}, // U2465 | CIRCLED DIGIT SIX
  {{XK_Multi_key, XK_parenleft, XK_KP_6, XK_parenright, XK_VoidSymbol}, "⑥"},
  {{XK_dead_stroke, XK_t, XK_VoidSymbol}, "ŧ"}, // U0167 | LATIN SMALL LETTER T WITH STROKE
  {{XK_Multi_key, XK_slash, XK_t, XK_VoidSymbol}, "ŧ"},
  {{XK_Multi_key, XK_KP_Divide, XK_t, XK_VoidSymbol}, "ŧ"},
  {{XK_Multi_key, XK_t, XK_slash, XK_VoidSymbol}, "ŧ"},
  {{XK_Multi_key, XK_t, XK_minus, XK_VoidSymbol}, "ŧ"},
  {{XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὐ"}, // U1F50 | GREEK SMALL LETTER UPSILON WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὐ"},
  {{XK_Multi_key, XK_numbersign, XK_q, XK_VoidSymbol}, "♩"}, // U2669 | QUARTER NOTE
  {{XK_dead_breve, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῠ"}, // U1FE8 | GREEK CAPITAL LETTER UPSILON WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῠ"},
  {{XK_Multi_key, XK_b, XK_Greek_UPSILON, XK_VoidSymbol}, "Ῠ"},
  {{XK_Multi_key, XK_parenleft, XK_0, XK_parenright, XK_VoidSymbol}, "⓪"}, // U24EA | CIRCLED DIGIT ZERO
  {{XK_Multi_key, XK_parenleft, XK_KP_0, XK_parenright, XK_VoidSymbol}, "⓪"},
  {{XK_dead_macron, XK_dead_ogonek, XK_O, XK_VoidSymbol}, "Ǭ"}, // U01EC | LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_semicolon, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_Multi_key, XK_macron, XK_dead_ogonek, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_Multi_key, XK_macron, XK_semicolon, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_Multi_key, XK_underscore, XK_dead_ogonek, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_Multi_key, XK_underscore, XK_semicolon, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_dead_ogonek, XK_Omacron, XK_VoidSymbol}, "Ǭ"},
  {{XK_dead_ogonek, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ǭ"},
  {{XK_dead_voiced_sound, XK_kana_HI, XK_VoidSymbol}, "ビ"}, // U30D3 | KATAKANA LETTER BI
  {{XK_dead_tilde, XK_Uhorn, XK_VoidSymbol}, "Ữ"}, // U1EEE | LATIN CAPITAL LETTER U WITH HORN AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Uhorn, XK_VoidSymbol}, "Ữ"},
  {{XK_dead_tilde, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ữ"},
  {{XK_dead_tilde, XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ữ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ữ"},
  {{XK_Multi_key, XK_asciitilde, XK_plus, XK_U, XK_VoidSymbol}, "Ữ"},
  {{XK_dead_horn, XK_Utilde, XK_VoidSymbol}, "Ữ"},
  {{XK_dead_horn, XK_dead_tilde, XK_U, XK_VoidSymbol}, "Ữ"},
  {{XK_dead_hook, XK_m, XK_VoidSymbol}, "ɱ"}, // U0271 | LATIN SMALL LETTER M WITH HOOK
  {{XK_dead_currency, XK_M, XK_VoidSymbol}, "ℳ"}, // U2133 | SCRIPT CAPITAL M
  {{XK_dead_circumflex, XK_Cyrillic_ie, XK_VoidSymbol}, "е̂"}, // CYRILLIC SMALL LETTER IE WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_ie, XK_VoidSymbol}, "е̂"},
  {{XK_dead_grave, XK_o, XK_VoidSymbol}, "ò"}, // ograve | LATIN SMALL LETTER O WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_o, XK_VoidSymbol}, "ò"},
  {{XK_Multi_key, XK_o, XK_grave, XK_VoidSymbol}, "ò"},
  {{XK_dead_circumflex, XK_y, XK_VoidSymbol}, "ŷ"}, // U0177 | LATIN SMALL LETTER Y WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_y, XK_VoidSymbol}, "ŷ"},
  {{XK_Multi_key, XK_y, XK_asciicircum, XK_VoidSymbol}, "ŷ"},
  {{XK_Multi_key, XK_R, XK_s, XK_VoidSymbol}, "₨"}, // U20a8 | RUPEE SIGN
  {{XK_dead_currency, XK_R, XK_VoidSymbol}, "₨"},
  {{XK_dead_acute, XK_utilde, XK_VoidSymbol}, "ṹ"}, // U1E79 | LATIN SMALL LETTER U WITH TILDE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_utilde, XK_VoidSymbol}, "ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_utilde, XK_VoidSymbol}, "ṹ"},
  {{XK_dead_acute, XK_dead_tilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciitilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_Multi_key, XK_acute, XK_dead_tilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_Multi_key, XK_acute, XK_asciitilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_tilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciitilde, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_dead_tilde, XK_uacute, XK_VoidSymbol}, "ṹ"},
  {{XK_dead_tilde, XK_dead_acute, XK_u, XK_VoidSymbol}, "ṹ"},
  {{XK_dead_grave, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὸ"}, // U1FF8 | GREEK CAPITAL LETTER OMICRON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὸ"},
  {{XK_dead_belowdot, XK_Uhorn, XK_VoidSymbol}, "Ự"}, // U1EF0 | LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_Uhorn, XK_VoidSymbol}, "Ự"},
  {{XK_dead_belowdot, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ự"},
  {{XK_dead_belowdot, XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ự"},
  {{XK_Multi_key, XK_exclam, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ự"},
  {{XK_Multi_key, XK_exclam, XK_plus, XK_U, XK_VoidSymbol}, "Ự"},
  {{XK_dead_horn, XK_Ubelowdot, XK_VoidSymbol}, "Ự"},
  {{XK_dead_horn, XK_dead_belowdot, XK_U, XK_VoidSymbol}, "Ự"},
  {{XK_Multi_key, XK_parenright, XK_minus, XK_VoidSymbol}, "}"}, // braceright | RIGHT CURLY BRACKET
  {{XK_Multi_key, XK_minus, XK_parenright, XK_VoidSymbol}, "}"},
  {{XK_dead_acute, XK_AE, XK_VoidSymbol}, "Ǽ"}, // U01FC | LATIN CAPITAL LETTER AE WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_AE, XK_VoidSymbol}, "Ǽ"},
  {{XK_Multi_key, XK_apostrophe, XK_AE, XK_VoidSymbol}, "Ǽ"},
  {{XK_Multi_key, XK_2, XK_3, XK_VoidSymbol}, "⅔"}, // U2154 | VULGAR FRACTION TWO THIRDS
  {{XK_dead_grave, XK_Cyrillic_IE, XK_VoidSymbol}, "Ѐ"}, // U0400 | CYRILLIC CAPITAL LETTER IE WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_Cyrillic_IE, XK_VoidSymbol}, "Ѐ"},
  {{XK_Multi_key, XK_underscore, XK_3, XK_VoidSymbol}, "₃"}, // U2083 | SUBSCRIPT THREE
  {{XK_Multi_key, XK_underscore, XK_KP_3, XK_VoidSymbol}, "₃"},
  {{XK_dead_caron, XK_3, XK_VoidSymbol}, "₃"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἂ"}, // U1F02 | GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἂ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἂ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἂ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_space, XK_VoidSymbol}, "΅"}, // U0385 | GREEK DIALYTIKA TONOS
  {{XK_dead_diaeresis, XK_dead_acute, XK_space, XK_VoidSymbol}, "΅"},
  {{XK_Multi_key, XK_diaeresis, XK_dead_acute, XK_VoidSymbol}, "΅"},
  {{XK_Multi_key, XK_diaeresis, XK_acute, XK_VoidSymbol}, "΅"},
  {{XK_Multi_key, XK_diaeresis, XK_apostrophe, XK_VoidSymbol}, "΅"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_space, XK_VoidSymbol}, "΅"},
  {{XK_dead_doublegrave, XK_E, XK_VoidSymbol}, "Ȅ"}, // U0204 | LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
  {{XK_dead_abovedot, XK_w, XK_VoidSymbol}, "ẇ"}, // U1E87 | LATIN SMALL LETTER W WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_w, XK_VoidSymbol}, "ẇ"},
  {{XK_Multi_key, XK_0, XK_3, XK_VoidSymbol}, "↉"}, // U2189 | VULGAR FRACTION ZERO THIRDS
  {{XK_Multi_key, XK_space, XK_period, XK_VoidSymbol}, " "}, // U2008 | PUNCTUATION SPACE
  {{XK_dead_grave, XK_Cyrillic_U, XK_VoidSymbol}, "У̀"}, // CYRILLIC CAPITAL LETTER U WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_U, XK_VoidSymbol}, "У̀"},
  {{XK_dead_currency, XK_f, XK_VoidSymbol}, "ƒ"}, // function | LATIN SMALL LETTER F WITH HOOK
  {{XK_dead_hook, XK_f, XK_VoidSymbol}, "ƒ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"}, // U1F8D | GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾍ"},
  {{XK_dead_belowdot, XK_D, XK_VoidSymbol}, "Ḍ"}, // U1E0C | LATIN CAPITAL LETTER D WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_D, XK_VoidSymbol}, "Ḍ"},
  {{XK_dead_caron, XK_D, XK_VoidSymbol}, "Ď"}, // U010E | LATIN CAPITAL LETTER D WITH CARON
  {{XK_Multi_key, XK_c, XK_D, XK_VoidSymbol}, "Ď"},
  {{XK_Multi_key, XK_less, XK_D, XK_VoidSymbol}, "Ď"},
  {{XK_Multi_key, XK_D, XK_less, XK_VoidSymbol}, "Ď"},
  {{XK_Multi_key, XK_parenleft, XK_kana_KO, XK_parenright, XK_VoidSymbol}, "㋙"}, // U32D9 | CIRCLED KATAKANA KO
  {{XK_dead_grave, XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἒ"}, // U1F12 | GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἒ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἒ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἒ"},
  {{XK_dead_doublegrave, XK_U, XK_VoidSymbol}, "Ȕ"}, // U0214 | LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
  {{XK_dead_diaeresis, XK_t, XK_VoidSymbol}, "ẗ"}, // U1E97 | LATIN SMALL LETTER T WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_t, XK_VoidSymbol}, "ẗ"},
  {{XK_Multi_key, XK_underscore, XK_apostrophe, XK_VoidSymbol}, "⍘"}, // U2358 | _ ' APL FUNCTIONAL SYMBOL QUOTE UNDERBAR
  {{XK_dead_hook, XK_k, XK_VoidSymbol}, "ƙ"}, // U0199 | LATIN SMALL LETTER K WITH HOOK
  {{XK_Multi_key, XK_less, XK_apostrophe, XK_VoidSymbol}, "‘"}, // U2018 | LEFT SINGLE QUOTATION MARK
  {{XK_Multi_key, XK_apostrophe, XK_less, XK_VoidSymbol}, "‘"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"}, // U1F9D | GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾝ"},
  {{XK_dead_breve, XK_dead_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"}, // U1E1C | LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
  {{XK_dead_breve, XK_Multi_key, XK_comma, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_dead_breve, XK_Multi_key, XK_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_U, XK_dead_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_U, XK_space, XK_comma, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_U, XK_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_b, XK_dead_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_b, XK_comma, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_Multi_key, XK_b, XK_cedilla, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_dead_cedilla, XK_dead_breve, XK_E, XK_VoidSymbol}, "Ḝ"},
  {{XK_dead_breve, XK_G, XK_VoidSymbol}, "Ğ"}, // U011E | LATIN CAPITAL LETTER G WITH BREVE
  {{XK_Multi_key, XK_U, XK_G, XK_VoidSymbol}, "Ğ"},
  {{XK_Multi_key, XK_G, XK_U, XK_VoidSymbol}, "Ğ"},
  {{XK_Multi_key, XK_b, XK_G, XK_VoidSymbol}, "Ğ"},
  {{XK_Multi_key, XK_breve, XK_G, XK_VoidSymbol}, "Ğ"},
  {{XK_Multi_key, XK_G, XK_breve, XK_VoidSymbol}, "Ğ"},
  {{XK_Multi_key, XK_G, XK_parenleft, XK_VoidSymbol}, "Ğ"},
  {{XK_dead_hook, XK_d, XK_VoidSymbol}, "ɗ"}, // U0257 | LATIN SMALL LETTER D WITH HOOK
  {{XK_Multi_key, XK_L, XK_minus, XK_VoidSymbol}, "£"}, // sterling | POUND SIGN
  {{XK_Multi_key, XK_minus, XK_L, XK_VoidSymbol}, "£"},
  {{XK_Multi_key, XK_l, XK_minus, XK_VoidSymbol}, "£"},
  {{XK_Multi_key, XK_minus, XK_l, XK_VoidSymbol}, "£"},
  {{XK_dead_currency, XK_l, XK_VoidSymbol}, "£"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἢ"}, // U1F22 | GREEK SMALL LETTER ETA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἢ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἢ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἢ"},
  {{XK_dead_hook, XK_N, XK_VoidSymbol}, "Ɲ"}, // U019D | LATIN CAPITAL LETTER N WITH LEFT HOOK
  {{XK_dead_hook, XK_Z, XK_VoidSymbol}, "Ȥ"}, // U0224 | LATIN CAPITAL LETTER Z WITH HOOK
  {{XK_dead_grave, XK_acircumflex, XK_VoidSymbol}, "ầ"}, // U1EA7 | LATIN SMALL LETTER A WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_acircumflex, XK_VoidSymbol}, "ầ"},
  {{XK_dead_grave, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ầ"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_a, XK_VoidSymbol}, "ầ"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ầ"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_a, XK_VoidSymbol}, "ầ"},
  {{XK_dead_circumflex, XK_agrave, XK_VoidSymbol}, "ầ"},
  {{XK_dead_circumflex, XK_dead_grave, XK_a, XK_VoidSymbol}, "ầ"},
  {{XK_dead_voiced_sound, XK_kana_KU, XK_VoidSymbol}, "グ"}, // U30B0 | KATAKANA LETTER GU
  {{XK_Multi_key, XK_parenleft, XK_n, XK_parenright, XK_VoidSymbol}, "ⓝ"}, // U24DD | CIRCLED LATIN SMALL LETTER N
  {{XK_dead_breve, XK_nobreakspace, XK_VoidSymbol}, "̆"}, // U0306 | COMBINING BREVE
  {{XK_dead_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"}, // U1FAD | GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾭ"},
  {{XK_dead_belowtilde, XK_I, XK_VoidSymbol}, "Ḭ"}, // U1E2C | LATIN CAPITAL LETTER I WITH TILDE BELOW
  {{XK_dead_ogonek, XK_I, XK_VoidSymbol}, "Į"}, // U012E | LATIN CAPITAL LETTER I WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_I, XK_VoidSymbol}, "Į"},
  {{XK_Multi_key, XK_I, XK_semicolon, XK_VoidSymbol}, "Į"},
  {{XK_Multi_key, XK_comma, XK_I, XK_VoidSymbol}, "Į"},
  {{XK_Multi_key, XK_I, XK_comma, XK_VoidSymbol}, "Į"},
  {{XK_dead_circumflex, XK_3, XK_VoidSymbol}, "³"}, // threesuperior | SUPERSCRIPT THREE
  {{XK_Multi_key, XK_asciicircum, XK_3, XK_VoidSymbol}, "³"},
  {{XK_Multi_key, XK_3, XK_asciicircum, XK_VoidSymbol}, "³"},
  {{XK_dead_circumflex, XK_KP_3, XK_VoidSymbol}, "³"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_3, XK_VoidSymbol}, "³"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἲ"}, // U1F32 | GREEK SMALL LETTER IOTA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἲ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἲ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἲ"},
  {{XK_Multi_key, XK_colon, XK_period, XK_VoidSymbol}, "∴"}, // therefore | : . THEREFORE
  {{XK_dead_breve, XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ặ"}, // U1EB7 | LATIN SMALL LETTER A WITH BREVE AND DOT BELOW
  {{XK_dead_breve, XK_Multi_key, XK_exclam, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_Multi_key, XK_U, XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_Multi_key, XK_U, XK_exclam, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_Multi_key, XK_b, XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_Multi_key, XK_b, XK_exclam, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_dead_belowdot, XK_abreve, XK_VoidSymbol}, "ặ"},
  {{XK_dead_belowdot, XK_dead_breve, XK_a, XK_VoidSymbol}, "ặ"},
  {{XK_dead_doublegrave, XK_Cyrillic_o, XK_VoidSymbol}, "о̏"}, // CYRILLIC SMALL LETTER O WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_o, XK_VoidSymbol}, "о̏"},
  {{XK_dead_grave, XK_Cyrillic_o, XK_VoidSymbol}, "о̀"}, // CYRILLIC SMALL LETTER O WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_o, XK_VoidSymbol}, "о̀"},
  {{XK_dead_currency, XK_T, XK_VoidSymbol}, "₮"}, // U20AE | TUGRIK SIGN
  {{XK_Multi_key, XK_parenleft, XK_F, XK_parenright, XK_VoidSymbol}, "Ⓕ"}, // U24BB | CIRCLED LATIN CAPITAL LETTER F
  {{XK_dead_macron, XK_Cyrillic_o, XK_VoidSymbol}, "о̄"}, // CYRILLIC SMALL LETTER O WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_o, XK_VoidSymbol}, "о̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_o, XK_VoidSymbol}, "о̄"},
  {{XK_dead_belowcircumflex, XK_L, XK_VoidSymbol}, "Ḽ"}, // U1E3C | LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
  {{XK_Multi_key, XK_parenleft, XK_5, XK_0, XK_parenright, XK_VoidSymbol}, "㊿"}, // U32BF | CIRCLED NUMBER FIFTY
  {{XK_Multi_key, XK_parenleft, XK_5, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㊿"},
  {{XK_Multi_key, XK_parenleft, XK_KP_5, XK_0, XK_parenright, XK_VoidSymbol}, "㊿"},
  {{XK_Multi_key, XK_parenleft, XK_KP_5, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㊿"},
  {{XK_dead_caron, XK_l, XK_VoidSymbol}, "ľ"}, // U013E | LATIN SMALL LETTER L WITH CARON
  {{XK_Multi_key, XK_c, XK_l, XK_VoidSymbol}, "ľ"},
  {{XK_Multi_key, XK_less, XK_l, XK_VoidSymbol}, "ľ"},
  {{XK_Multi_key, XK_l, XK_less, XK_VoidSymbol}, "ľ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_TSU, XK_parenright, XK_VoidSymbol}, "㋡"}, // U32E1 | CIRCLED KATAKANA TU
  {{XK_dead_tilde, XK_A, XK_VoidSymbol}, "Ã"}, // Atilde | LATIN CAPITAL LETTER A WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_A, XK_VoidSymbol}, "Ã"},
  {{XK_Multi_key, XK_A, XK_asciitilde, XK_VoidSymbol}, "Ã"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὂ"}, // U1F42 | GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὂ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὂ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὂ"},
  {{XK_dead_abovedot, XK_j, XK_VoidSymbol}, "ȷ"}, // U0237 | LATIN SMALL LETTER DOTLESS J
  {{XK_dead_circumflex, XK_dead_belowdot, XK_e, XK_VoidSymbol}, "ệ"}, // U1EC7 | LATIN SMALL LETTER E WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_e, XK_VoidSymbol}, "ệ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_e, XK_VoidSymbol}, "ệ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_e, XK_VoidSymbol}, "ệ"},
  {{XK_dead_belowdot, XK_ecircumflex, XK_VoidSymbol}, "ệ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ệ"},
  {{XK_Multi_key, XK_parenleft, XK_V, XK_parenright, XK_VoidSymbol}, "Ⓥ"}, // U24CB | CIRCLED LATIN CAPITAL LETTER V
  {{XK_Multi_key, XK_backslash, XK_o, XK_slash, XK_VoidSymbol}, "🙌"}, // PERSON RAISING BOTH HANDS IN CELEBRATION
  {{XK_dead_breve, XK_O, XK_VoidSymbol}, "Ŏ"}, // U014E | LATIN CAPITAL LETTER O WITH BREVE
  {{XK_Multi_key, XK_U, XK_O, XK_VoidSymbol}, "Ŏ"},
  {{XK_Multi_key, XK_b, XK_O, XK_VoidSymbol}, "Ŏ"},
  {{XK_dead_grave, XK_Cyrillic_ie, XK_VoidSymbol}, "ѐ"}, // U0450 | CYRILLIC SMALL LETTER IE WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_Cyrillic_ie, XK_VoidSymbol}, "ѐ"},
  {{XK_dead_acute, XK_O, XK_VoidSymbol}, "Ó"}, // Oacute | LATIN CAPITAL LETTER O WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_O, XK_VoidSymbol}, "Ó"},
  {{XK_Multi_key, XK_O, XK_acute, XK_VoidSymbol}, "Ó"},
  {{XK_Multi_key, XK_apostrophe, XK_O, XK_VoidSymbol}, "Ó"},
  {{XK_Multi_key, XK_O, XK_apostrophe, XK_VoidSymbol}, "Ó"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὒ"}, // U1F52 | GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὒ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὒ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὒ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_4, XK_parenright, XK_VoidSymbol}, "㉔"}, // U3254 | CIRCLED NUMBER TWENTY FOUR
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㉔"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_4, XK_parenright, XK_VoidSymbol}, "㉔"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㉔"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_4, XK_parenright, XK_VoidSymbol}, "㉔"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㉔"},
  {{XK_dead_tilde, XK_ocircumflex, XK_VoidSymbol}, "ỗ"}, // U1ED7 | LATIN SMALL LETTER O WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_ocircumflex, XK_VoidSymbol}, "ỗ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ỗ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_o, XK_VoidSymbol}, "ỗ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ỗ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_o, XK_VoidSymbol}, "ỗ"},
  {{XK_dead_circumflex, XK_otilde, XK_VoidSymbol}, "ỗ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ỗ"},
  {{XK_dead_voiced_sound, XK_kana_SHI, XK_VoidSymbol}, "ジ"}, // U30B8 | KATAKANA LETTER ZI
  {{XK_dead_caron, XK_Udiaeresis, XK_VoidSymbol}, "Ǚ"}, // U01D9 | LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
  {{XK_Multi_key, XK_c, XK_Udiaeresis, XK_VoidSymbol}, "Ǚ"},
  {{XK_dead_caron, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǚ"},
  {{XK_dead_caron, XK_Multi_key, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǚ"},
  {{XK_Multi_key, XK_c, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǚ"},
  {{XK_Multi_key, XK_c, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǚ"},
  {{XK_dead_caron, XK_V, XK_VoidSymbol}, "Ǚ"},
  {{XK_dead_diaeresis, XK_dead_caron, XK_U, XK_VoidSymbol}, "Ǚ"},
  {{XK_Multi_key, XK_parenleft, XK_v, XK_parenright, XK_VoidSymbol}, "ⓥ"}, // U24E5 | CIRCLED LATIN SMALL LETTER V
  {{XK_Multi_key, XK_parenleft, XK_l, XK_parenright, XK_VoidSymbol}, "ⓛ"}, // U24DB | CIRCLED LATIN SMALL LETTER L
  {{XK_Multi_key, XK_parenleft, XK_4, XK_2, XK_parenright, XK_VoidSymbol}, "㊷"}, // U32B7 | CIRCLED NUMBER FORTY TWO
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㊷"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㊷"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_2, XK_parenright, XK_VoidSymbol}, "㊷"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "㊷"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_2, XK_parenright, XK_VoidSymbol}, "㊷"},
  {{XK_dead_macron, XK_dead_belowdot, XK_R, XK_VoidSymbol}, "Ṝ"}, // U1E5C | LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_exclam, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_Multi_key, XK_macron, XK_dead_belowdot, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_Multi_key, XK_macron, XK_exclam, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_Multi_key, XK_underscore, XK_dead_belowdot, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_Multi_key, XK_underscore, XK_exclam, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_dead_belowdot, XK_dead_macron, XK_R, XK_VoidSymbol}, "Ṝ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_TA, XK_parenright, XK_VoidSymbol}, "㋟"}, // U32DF | CIRCLED KATAKANA TA
  {{XK_dead_cedilla, XK_S, XK_VoidSymbol}, "Ş"}, // U015E | LATIN CAPITAL LETTER S WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_S, XK_VoidSymbol}, "Ş"},
  {{XK_Multi_key, XK_S, XK_comma, XK_VoidSymbol}, "Ş"},
  {{XK_Multi_key, XK_cedilla, XK_S, XK_VoidSymbol}, "Ş"},
  {{XK_Multi_key, XK_1, XK_1, XK_0, XK_VoidSymbol}, "⅒"}, // U2152 | VULGAR FRACTION ONE TENTH
  {{XK_dead_tilde, XK_a, XK_VoidSymbol}, "ã"}, // atilde | LATIN SMALL LETTER A WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_a, XK_VoidSymbol}, "ã"},
  {{XK_Multi_key, XK_a, XK_asciitilde, XK_VoidSymbol}, "ã"},
  {{XK_dead_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὢ"}, // U1F62 | GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὢ"},
  {{XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὢ"},
  {{XK_Multi_key, XK_grave, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὢ"},
  {{XK_Multi_key, XK_less, XK_equal, XK_VoidSymbol}, "≤"}, // U2264 | LESS-THAN OR EQUAL TO
  {{XK_Multi_key, XK_less, XK_underscore, XK_VoidSymbol}, "≤"},
  {{XK_Multi_key, XK_underscore, XK_less, XK_VoidSymbol}, "≤"},
  {{XK_dead_hook, XK_u, XK_VoidSymbol}, "ủ"}, // U1EE7 | LATIN SMALL LETTER U WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_u, XK_VoidSymbol}, "ủ"},
  {{XK_dead_caron, XK_k, XK_VoidSymbol}, "ǩ"}, // U01E9 | LATIN SMALL LETTER K WITH CARON
  {{XK_Multi_key, XK_c, XK_k, XK_VoidSymbol}, "ǩ"},
  {{XK_dead_stroke, XK_R, XK_VoidSymbol}, "Ɍ"}, // U024C | LATIN CAPITAL LETTER R WITH STROKE
  {{XK_dead_hook, XK_h, XK_VoidSymbol}, "ɦ"}, // U0266 | LATIN SMALL LETTER H WITH HOOK
  {{XK_Multi_key, XK_diaeresis, XK_dead_grave, XK_VoidSymbol}, "῭"}, // U1FED | GREEK DIALYTIKA AND VARIA
  {{XK_Multi_key, XK_diaeresis, XK_grave, XK_VoidSymbol}, "῭"},
  {{XK_Multi_key, XK_numbersign, XK_S, XK_VoidSymbol}, "♬"}, // U266c | BEAMED SIXTEENTH NOTES
  {{XK_Multi_key, XK_parenleft, XK_kana_MI, XK_parenright, XK_VoidSymbol}, "㋯"}, // U32EF | CIRCLED KATAKANA MI
  {{XK_dead_abovering, XK_U, XK_VoidSymbol}, "Ů"}, // U016E | LATIN CAPITAL LETTER U WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_U, XK_VoidSymbol}, "Ů"},
  {{XK_Multi_key, XK_asterisk, XK_U, XK_VoidSymbol}, "Ů"},
  {{XK_Multi_key, XK_U, XK_asterisk, XK_VoidSymbol}, "Ů"},
  {{XK_dead_tilde, XK_nobreakspace, XK_VoidSymbol}, "̃"}, // U0303 | COMBINING TILDE
  {{XK_Multi_key, XK_parenleft, XK_1, XK_7, XK_parenright, XK_VoidSymbol}, "⑰"}, // U2470 | CIRCLED NUMBER SEVENTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_7, XK_parenright, XK_VoidSymbol}, "⑰"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_7, XK_parenright, XK_VoidSymbol}, "⑰"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_7, XK_parenright, XK_VoidSymbol}, "⑰"},
  {{XK_dead_acute, XK_o, XK_VoidSymbol}, "ó"}, // oacute | LATIN SMALL LETTER O WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_o, XK_VoidSymbol}, "ó"},
  {{XK_Multi_key, XK_o, XK_acute, XK_VoidSymbol}, "ó"},
  {{XK_Multi_key, XK_apostrophe, XK_o, XK_VoidSymbol}, "ó"},
  {{XK_Multi_key, XK_o, XK_apostrophe, XK_VoidSymbol}, "ó"},
  {{XK_dead_grave, XK_Greek_epsilon, XK_VoidSymbol}, "ὲ"}, // U1F72 | GREEK SMALL LETTER EPSILON WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_epsilon, XK_VoidSymbol}, "ὲ"},
  {{XK_dead_hook, XK_schwa, XK_VoidSymbol}, "ɚ"}, // U025A | LATIN SMALL LETTER SCHWA WITH HOOK
  {{XK_dead_currency, XK_b, XK_VoidSymbol}, "฿"}, // Thai_baht | THAI CURRENCY SYMBOL BAHT
  {{XK_dead_hook, XK_y, XK_VoidSymbol}, "ỷ"}, // U1EF7 | LATIN SMALL LETTER Y WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_y, XK_VoidSymbol}, "ỷ"},
  {{XK_dead_grave, XK_n, XK_VoidSymbol}, "ǹ"}, // U01F9 | LATIN SMALL LETTER N WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_n, XK_VoidSymbol}, "ǹ"},
  {{XK_dead_circumflex, XK_8, XK_VoidSymbol}, "⁸"}, // U2078 | SUPERSCRIPT EIGHT
  {{XK_Multi_key, XK_asciicircum, XK_8, XK_VoidSymbol}, "⁸"},
  {{XK_dead_circumflex, XK_KP_8, XK_VoidSymbol}, "⁸"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_8, XK_VoidSymbol}, "⁸"},
  {{XK_dead_iota, XK_space, XK_VoidSymbol}, "ͺ"}, // U37a | GREEK YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_iota, XK_VoidSymbol}, "ͺ"},
  {{XK_dead_tilde, XK_V, XK_VoidSymbol}, "Ṽ"}, // U1E7C | LATIN CAPITAL LETTER V WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_V, XK_VoidSymbol}, "Ṽ"},
  {{XK_dead_caron, XK_z, XK_VoidSymbol}, "ž"}, // U017E | LATIN SMALL LETTER Z WITH CARON
  {{XK_Multi_key, XK_c, XK_z, XK_VoidSymbol}, "ž"},
  {{XK_Multi_key, XK_v, XK_z, XK_VoidSymbol}, "ž"},
  {{XK_Multi_key, XK_less, XK_z, XK_VoidSymbol}, "ž"},
  {{XK_Multi_key, XK_z, XK_less, XK_VoidSymbol}, "ž"},
  {{XK_dead_circumflex, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̂"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̂"},
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"}, // U1F82 | GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾂ"},
  {{XK_dead_hook, XK_t, XK_VoidSymbol}, "ƭ"}, // U01AD | LATIN SMALL LETTER T WITH HOOK
  {{XK_dead_belowmacron, XK_b, XK_VoidSymbol}, "ḇ"}, // U1E07 | LATIN SMALL LETTER B WITH LINE BELOW
  {{XK_dead_voiced_sound, XK_kana_TA, XK_VoidSymbol}, "ダ"}, // U30C0 | KATAKANA LETTER DA
  {{XK_dead_circumflex, XK_c, XK_VoidSymbol}, "ĉ"}, // U0109 | LATIN SMALL LETTER C WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_c, XK_VoidSymbol}, "ĉ"},
  {{XK_Multi_key, XK_underscore, XK_8, XK_VoidSymbol}, "₈"}, // U2088 | SUBSCRIPT EIGHT
  {{XK_Multi_key, XK_underscore, XK_KP_8, XK_VoidSymbol}, "₈"},
  {{XK_dead_caron, XK_8, XK_VoidSymbol}, "₈"},
  {{XK_dead_acute, XK_Greek_IOTA, XK_VoidSymbol}, "Ί"}, // U038A | GREEK CAPITAL LETTER IOTA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_IOTA, XK_VoidSymbol}, "Ί"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_IOTA, XK_VoidSymbol}, "Ί"},
  {{XK_Multi_key, XK_Greek_IOTA, XK_apostrophe, XK_VoidSymbol}, "Ί"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"}, // U1F0D | GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἅ"},
  {{XK_dead_diaeresis, XK_X, XK_VoidSymbol}, "Ẍ"}, // U1E8C | LATIN CAPITAL LETTER X WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_X, XK_VoidSymbol}, "Ẍ"},
  {{XK_dead_invertedbreve, XK_o, XK_VoidSymbol}, "ȏ"}, // U020F | LATIN SMALL LETTER O WITH INVERTED BREVE
  {{XK_dead_circumflex, XK_parenright, XK_VoidSymbol}, "⁾"}, // U207E | SUPERSCRIPT RIGHT PARENTHESIS
  {{XK_Multi_key, XK_asciicircum, XK_parenright, XK_VoidSymbol}, "⁾"},
  {{XK_Multi_key, XK_minus, XK_minus, XK_period, XK_VoidSymbol}, "–"}, // U2013 | EN DASH
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"}, // U1F92 | GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾒ"},
  {{XK_dead_acute, XK_emacron, XK_VoidSymbol}, "ḗ"}, // U1E17 | LATIN SMALL LETTER E WITH MACRON AND ACUTE
  {{XK_Multi_key, XK_acute, XK_emacron, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_emacron, XK_VoidSymbol}, "ḗ"},
  {{XK_dead_acute, XK_dead_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_dead_acute, XK_Multi_key, XK_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_dead_acute, XK_Multi_key, XK_underscore, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_acute, XK_dead_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_acute, XK_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_acute, XK_underscore, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_macron, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_underscore, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_dead_macron, XK_eacute, XK_VoidSymbol}, "ḗ"},
  {{XK_dead_macron, XK_dead_acute, XK_e, XK_VoidSymbol}, "ḗ"},
  {{XK_Multi_key, XK_L, XK_L, XK_A, XK_P, XK_VoidSymbol}, "🖖"}, // U1F596 | RAISED HAND WITH PART BETWEEN MIDDLE AND RING FINGERS
  {{XK_dead_ogonek, XK_e, XK_VoidSymbol}, "ę"}, // U0119 | LATIN SMALL LETTER E WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_e, XK_VoidSymbol}, "ę"},
  {{XK_Multi_key, XK_e, XK_semicolon, XK_VoidSymbol}, "ę"},
  {{XK_Multi_key, XK_comma, XK_e, XK_VoidSymbol}, "ę"},
  {{XK_Multi_key, XK_e, XK_comma, XK_VoidSymbol}, "ę"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"}, // U1F45 | GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὅ"},
  {{XK_dead_belowmacron, XK_T, XK_VoidSymbol}, "Ṯ"}, // U1E6E | LATIN CAPITAL LETTER T WITH LINE BELOW
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"}, // U1F1D | GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἕ"},
  {{XK_dead_caron, XK_h, XK_VoidSymbol}, "ȟ"}, // U021F | LATIN SMALL LETTER H WITH CARON
  {{XK_Multi_key, XK_c, XK_h, XK_VoidSymbol}, "ȟ"},
  {{XK_dead_currency, XK_O, XK_VoidSymbol}, "૱"}, // U0AF1 | GUJARATI RUPEE SIGN
  {{XK_Multi_key, XK_plus, XK_plus, XK_VoidSymbol}, "#"}, // numbersign | NUMBER SIGN
  {{XK_dead_iota, XK_dead_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"}, // U1FA2 | GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾢ"},
  {{XK_dead_stroke, XK_e, XK_VoidSymbol}, "ɇ"}, // U0247 | LATIN SMALL LETTER E WITH STROKE
  {{XK_dead_diaeresis, XK_h, XK_VoidSymbol}, "ḧ"}, // U1E27 | LATIN SMALL LETTER H WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_h, XK_VoidSymbol}, "ḧ"},
  {{XK_dead_grave, XK_Greek_alpha, XK_VoidSymbol}, "ὰ"}, // U1F70 | GREEK SMALL LETTER ALPHA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_alpha, XK_VoidSymbol}, "ὰ"},
  {{XK_dead_tilde, XK_i, XK_VoidSymbol}, "ĩ"}, // U0129 | LATIN SMALL LETTER I WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_i, XK_VoidSymbol}, "ĩ"},
  {{XK_Multi_key, XK_i, XK_asciitilde, XK_VoidSymbol}, "ĩ"},
  {{XK_dead_diaeresis, XK_dead_diaeresis, XK_VoidSymbol}, "¨"}, // diaeresis | DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_quotedbl, XK_VoidSymbol}, "¨"},
  {{XK_dead_diaeresis, XK_Greek_IOTA, XK_VoidSymbol}, "Ϊ"}, // U03AA | GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
  {{XK_Multi_key, XK_quotedbl, XK_Greek_IOTA, XK_VoidSymbol}, "Ϊ"},
  {{XK_Multi_key, XK_Greek_IOTA, XK_quotedbl, XK_VoidSymbol}, "Ϊ"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"}, // U1F2D | GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἥ"},
  {{XK_dead_circumflex, XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ậ"}, // U1EAC | LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_A, XK_VoidSymbol}, "Ậ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ậ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_A, XK_VoidSymbol}, "Ậ"},
  {{XK_dead_belowdot, XK_Acircumflex, XK_VoidSymbol}, "Ậ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ậ"},
  {{XK_dead_abovedot, XK_o, XK_VoidSymbol}, "ȯ"}, // U022F | LATIN SMALL LETTER O WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_o, XK_VoidSymbol}, "ȯ"},
  {{XK_dead_iota, XK_dead_grave, XK_Greek_alpha, XK_VoidSymbol}, "ᾲ"}, // U1FB2 | GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_Greek_alpha, XK_VoidSymbol}, "ᾲ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_Greek_alpha, XK_VoidSymbol}, "ᾲ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_Greek_alpha, XK_VoidSymbol}, "ᾲ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_9, XK_parenright, XK_VoidSymbol}, "㊴"}, // U32B4 | CIRCLED NUMBER THIRTY NINE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㊴"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_9, XK_parenright, XK_VoidSymbol}, "㊴"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㊴"},
  {{XK_dead_belowdot, XK_l, XK_VoidSymbol}, "ḷ"}, // U1E37 | LATIN SMALL LETTER L WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_l, XK_VoidSymbol}, "ḷ"},
  {{XK_dead_acute, XK_L, XK_VoidSymbol}, "Ĺ"}, // U0139 | LATIN CAPITAL LETTER L WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_L, XK_VoidSymbol}, "Ĺ"},
  {{XK_Multi_key, XK_apostrophe, XK_L, XK_VoidSymbol}, "Ĺ"},
  {{XK_Multi_key, XK_L, XK_apostrophe, XK_VoidSymbol}, "Ĺ"},
  {{XK_dead_cedilla, XK_space, XK_VoidSymbol}, "¸"}, // cedilla | CEDILLA
  {{XK_dead_cedilla, XK_dead_cedilla, XK_VoidSymbol}, "¸"},
  {{XK_Multi_key, XK_comma, XK_space, XK_VoidSymbol}, "¸"},
  {{XK_Multi_key, XK_space, XK_comma, XK_VoidSymbol}, "¸"},
  {{XK_Multi_key, XK_comma, XK_comma, XK_VoidSymbol}, "¸"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"}, // U1F3D | GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἵ"},
  {{XK_dead_tilde, XK_E, XK_VoidSymbol}, "Ẽ"}, // U1EBC | LATIN CAPITAL LETTER E WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_E, XK_VoidSymbol}, "Ẽ"},
  {{XK_Multi_key, XK_parenleft, XK_K, XK_parenright, XK_VoidSymbol}, "Ⓚ"}, // U24C0 | CIRCLED LATIN CAPITAL LETTER K
  {{XK_dead_iota, XK_dead_grave, XK_Greek_eta, XK_VoidSymbol}, "ῂ"}, // U1FC2 | GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_Greek_eta, XK_VoidSymbol}, "ῂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_Greek_eta, XK_VoidSymbol}, "ῂ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_Greek_eta, XK_VoidSymbol}, "ῂ"},
  {{XK_Multi_key, XK_less, XK_greater, XK_VoidSymbol}, "⋄"}, // U22c4 | < > DIAMOND OPERATOR
  {{XK_Multi_key, XK_greater, XK_less, XK_VoidSymbol}, "⋄"},
  {{XK_dead_belowdot, XK_n, XK_VoidSymbol}, "ṇ"}, // U1E47 | LATIN SMALL LETTER N WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_n, XK_VoidSymbol}, "ṇ"},
  {{XK_dead_grave, XK_E, XK_VoidSymbol}, "È"}, // Egrave | LATIN CAPITAL LETTER E WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_E, XK_VoidSymbol}, "È"},
  {{XK_Multi_key, XK_E, XK_grave, XK_VoidSymbol}, "È"},
  {{XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ϊ"}, // U03CA | GREEK SMALL LETTER IOTA WITH DIALYTIKA
  {{XK_Multi_key, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ϊ"},
  {{XK_Multi_key, XK_Greek_iota, XK_quotedbl, XK_VoidSymbol}, "ϊ"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"}, // U1F4D | GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὅ"},
  {{XK_dead_belowdot, XK_O, XK_VoidSymbol}, "Ọ"}, // U1ECC | LATIN CAPITAL LETTER O WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_O, XK_VoidSymbol}, "Ọ"},
  {{XK_dead_stroke, XK_y, XK_VoidSymbol}, "ɏ"}, // U024F | LATIN SMALL LETTER Y WITH STROKE
  {{XK_dead_caron, XK_a, XK_VoidSymbol}, "ǎ"}, // U01CE | LATIN SMALL LETTER A WITH CARON
  {{XK_Multi_key, XK_c, XK_a, XK_VoidSymbol}, "ǎ"},
  {{XK_dead_breve, XK_Cyrillic_A, XK_VoidSymbol}, "Ӑ"}, // U04D0 | CYRILLIC CAPITAL LETTER A WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_A, XK_VoidSymbol}, "Ӑ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_A, XK_VoidSymbol}, "Ӑ"},
  {{XK_dead_grave, XK_Greek_iotadieresis, XK_VoidSymbol}, "ῒ"}, // U1FD2 | GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_iotadieresis, XK_VoidSymbol}, "ῒ"},
  {{XK_dead_grave, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ῒ"},
  {{XK_dead_grave, XK_Multi_key, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ῒ"},
  {{XK_Multi_key, XK_grave, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ῒ"},
  {{XK_Multi_key, XK_grave, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ῒ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_O, XK_parenright, XK_VoidSymbol}, "㋔"}, // U32D4 | CIRCLED KATAKANA O
  {{XK_dead_abovedot, XK_p, XK_VoidSymbol}, "ṗ"}, // U1E57 | LATIN SMALL LETTER P WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_p, XK_VoidSymbol}, "ṗ"},
  {{XK_Multi_key, XK_p, XK_period, XK_VoidSymbol}, "ṗ"},
  {{XK_dead_caron, XK_r, XK_VoidSymbol}, "ř"}, // U0159 | LATIN SMALL LETTER R WITH CARON
  {{XK_Multi_key, XK_c, XK_r, XK_VoidSymbol}, "ř"},
  {{XK_Multi_key, XK_less, XK_r, XK_VoidSymbol}, "ř"},
  {{XK_Multi_key, XK_r, XK_less, XK_VoidSymbol}, "ř"},
  {{XK_dead_stroke, XK_O, XK_VoidSymbol}, "Ø"}, // Oslash | LATIN CAPITAL LETTER O WITH STROKE
  {{XK_Multi_key, XK_slash, XK_O, XK_VoidSymbol}, "Ø"},
  {{XK_Multi_key, XK_O, XK_slash, XK_VoidSymbol}, "Ø"},
  {{XK_Multi_key, XK_KP_Divide, XK_O, XK_VoidSymbol}, "Ø"},
  {{XK_dead_acute, XK_Cyrillic_ER, XK_VoidSymbol}, "Р́"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_ER, XK_VoidSymbol}, "Р́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_ER, XK_VoidSymbol}, "Р́"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"}, // U1F5D | GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὕ"},
  {{XK_dead_grave, XK_Ohorn, XK_VoidSymbol}, "Ờ"}, // U1EDC | LATIN CAPITAL LETTER O WITH HORN AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Ohorn, XK_VoidSymbol}, "Ờ"},
  {{XK_dead_grave, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ờ"},
  {{XK_dead_grave, XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ờ"},
  {{XK_Multi_key, XK_grave, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ờ"},
  {{XK_Multi_key, XK_grave, XK_plus, XK_O, XK_VoidSymbol}, "Ờ"},
  {{XK_dead_horn, XK_Ograve, XK_VoidSymbol}, "Ờ"},
  {{XK_dead_horn, XK_dead_grave, XK_O, XK_VoidSymbol}, "Ờ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_5, XK_parenright, XK_VoidSymbol}, "㉟"}, // U325F | CIRCLED NUMBER THIRTY FIVE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㉟"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_5, XK_parenright, XK_VoidSymbol}, "㉟"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㉟"},
  {{XK_dead_macron, XK_Adiaeresis, XK_VoidSymbol}, "Ǟ"}, // U01DE | LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_Adiaeresis, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_underscore, XK_Adiaeresis, XK_VoidSymbol}, "Ǟ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_dead_diaeresis, XK_Amacron, XK_VoidSymbol}, "Ǟ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_A, XK_VoidSymbol}, "Ǟ"},
  {{XK_Multi_key, XK_1, XK_9, XK_VoidSymbol}, "⅑"}, // U2151 | VULGAR FRACTION ONE NINTH
  {{XK_dead_grave, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ῢ"}, // U1FE2 | GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ῢ"},
  {{XK_dead_grave, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ῢ"},
  {{XK_dead_grave, XK_Multi_key, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ῢ"},
  {{XK_Multi_key, XK_grave, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ῢ"},
  {{XK_Multi_key, XK_grave, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ῢ"},
  {{XK_Multi_key, XK_s, XK_o, XK_VoidSymbol}, "§"}, // section | SECTION SIGN
  {{XK_Multi_key, XK_o, XK_s, XK_VoidSymbol}, "§"},
  {{XK_Multi_key, XK_S, XK_O, XK_VoidSymbol}, "§"},
  {{XK_Multi_key, XK_O, XK_S, XK_VoidSymbol}, "§"},
  {{XK_Multi_key, XK_s, XK_exclam, XK_VoidSymbol}, "§"},
  {{XK_Multi_key, XK_S, XK_exclam, XK_VoidSymbol}, "§"},
  {{XK_Multi_key, XK_Cyrillic_pe, XK_Cyrillic_a, XK_VoidSymbol}, "§"},
  {{XK_dead_abovedot, XK_scaron, XK_VoidSymbol}, "ṧ"}, // U1E67 | LATIN SMALL LETTER S WITH CARON AND DOT ABOVE
  {{XK_Multi_key, XK_period, XK_scaron, XK_VoidSymbol}, "ṧ"},
  {{XK_dead_abovedot, XK_dead_caron, XK_s, XK_VoidSymbol}, "ṧ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_c, XK_s, XK_VoidSymbol}, "ṧ"},
  {{XK_Multi_key, XK_period, XK_dead_caron, XK_s, XK_VoidSymbol}, "ṧ"},
  {{XK_dead_caron, XK_sabovedot, XK_VoidSymbol}, "ṧ"},
  {{XK_dead_caron, XK_dead_abovedot, XK_s, XK_VoidSymbol}, "ṧ"},
  {{XK_Multi_key, XK_parenleft, XK_a, XK_parenright, XK_VoidSymbol}, "ⓐ"}, // U24D0 | CIRCLED LATIN SMALL LETTER A
  {{XK_dead_tilde, XK_u, XK_VoidSymbol}, "ũ"}, // U0169 | LATIN SMALL LETTER U WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_u, XK_VoidSymbol}, "ũ"},
  {{XK_Multi_key, XK_u, XK_asciitilde, XK_VoidSymbol}, "ũ"},
  {{XK_dead_grave, XK_e, XK_VoidSymbol}, "è"}, // egrave | LATIN SMALL LETTER E WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_e, XK_VoidSymbol}, "è"},
  {{XK_Multi_key, XK_e, XK_grave, XK_VoidSymbol}, "è"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_2, XK_parenright, XK_VoidSymbol}, "⑫"}, // U246B | CIRCLED NUMBER TWELVE
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "⑫"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_2, XK_parenright, XK_VoidSymbol}, "⑫"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_2, XK_parenright, XK_VoidSymbol}, "⑫"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "⑫"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_2, XK_parenright, XK_VoidSymbol}, "⑫"},
  {{XK_dead_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"}, // U1F6D | GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"},
  {{XK_Multi_key, XK_acute, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"},
  {{XK_Multi_key, XK_acute, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὥ"},
  {{XK_dead_hook, XK_Uhorn, XK_VoidSymbol}, "Ử"}, // U1EEC | LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Uhorn, XK_VoidSymbol}, "Ử"},
  {{XK_dead_hook, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ử"},
  {{XK_dead_hook, XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ử"},
  {{XK_Multi_key, XK_question, XK_dead_horn, XK_U, XK_VoidSymbol}, "Ử"},
  {{XK_Multi_key, XK_question, XK_plus, XK_U, XK_VoidSymbol}, "Ử"},
  {{XK_dead_horn, XK_Uhook, XK_VoidSymbol}, "Ử"},
  {{XK_dead_horn, XK_dead_hook, XK_U, XK_VoidSymbol}, "Ử"},
  {{XK_dead_stroke, XK_greater, XK_VoidSymbol}, "≯"}, // U226F | NOT GREATER-THAN
  {{XK_dead_diaeresis, XK_Cyrillic_U, XK_VoidSymbol}, "Ӱ"}, // U04F0 | CYRILLIC CAPITAL LETTER U WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_U, XK_VoidSymbol}, "Ӱ"},
  {{XK_dead_iota, XK_dead_grave, XK_Greek_omega, XK_VoidSymbol}, "ῲ"}, // U1FF2 | GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_Greek_omega, XK_VoidSymbol}, "ῲ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_Greek_omega, XK_VoidSymbol}, "ῲ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_Greek_omega, XK_VoidSymbol}, "ῲ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_YU, XK_parenright, XK_VoidSymbol}, "㋴"}, // U32F4 | CIRCLED KATAKANA YU
  {{XK_dead_belowcircumflex, XK_u, XK_VoidSymbol}, "ṷ"}, // U1E77 | LATIN SMALL LETTER U WITH CIRCUMFLEX BELOW
  {{XK_dead_hook, XK_M, XK_VoidSymbol}, "Ɱ"}, // U2C6E | LATIN CAPITAL LETTER M WITH HOOK
  {{XK_dead_acute, XK_Z, XK_VoidSymbol}, "Ź"}, // U0179 | LATIN CAPITAL LETTER Z WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_Z, XK_VoidSymbol}, "Ź"},
  {{XK_Multi_key, XK_apostrophe, XK_Z, XK_VoidSymbol}, "Ź"},
  {{XK_Multi_key, XK_Z, XK_apostrophe, XK_VoidSymbol}, "Ź"},
  {{XK_dead_stroke, XK_o, XK_VoidSymbol}, "ø"}, // oslash | LATIN SMALL LETTER O WITH STROKE
  {{XK_Multi_key, XK_slash, XK_o, XK_VoidSymbol}, "ø"},
  {{XK_Multi_key, XK_o, XK_slash, XK_VoidSymbol}, "ø"},
  {{XK_Multi_key, XK_KP_Divide, XK_o, XK_VoidSymbol}, "ø"},
  {{XK_dead_acute, XK_Ooblique, XK_VoidSymbol}, "Ǿ"}, // U01FE | LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Ooblique, XK_VoidSymbol}, "Ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_Ooblique, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_acute, XK_dead_stroke, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_acute, XK_Multi_key, XK_slash, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_Multi_key, XK_acute, XK_slash, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_slash, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_acute, XK_Multi_key, XK_KP_Divide, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_Multi_key, XK_acute, XK_KP_Divide, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_KP_Divide, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_stroke, XK_dead_acute, XK_O, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_stroke, XK_Oacute, XK_VoidSymbol}, "Ǿ"},
  {{XK_dead_abovedot, XK_B, XK_VoidSymbol}, "Ḃ"}, // U1E02 | LATIN CAPITAL LETTER B WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_B, XK_VoidSymbol}, "Ḃ"},
  {{XK_Multi_key, XK_B, XK_period, XK_VoidSymbol}, "Ḃ"},
  {{XK_Multi_key, XK_F, XK_l, XK_VoidSymbol}, "ﬄ"}, // Ufb04 | LATIN SMALL LIGATURE FFL
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"}, // U1F87 | GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾇ"},
  {{XK_Multi_key, XK_underscore, XK_9, XK_VoidSymbol}, "₉"}, // U2089 | SUBSCRIPT NINE
  {{XK_Multi_key, XK_underscore, XK_KP_9, XK_VoidSymbol}, "₉"},
  {{XK_dead_caron, XK_9, XK_VoidSymbol}, "₉"},
  {{XK_dead_circumflex, XK_C, XK_VoidSymbol}, "Ĉ"}, // U0108 | LATIN CAPITAL LETTER C WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_C, XK_VoidSymbol}, "Ĉ"},
  {{XK_dead_invertedbreve, XK_I, XK_VoidSymbol}, "Ȋ"}, // U020A | LATIN CAPITAL LETTER I WITH INVERTED BREVE
  {{XK_dead_diaeresis, XK_x, XK_VoidSymbol}, "ẍ"}, // U1E8D | LATIN SMALL LETTER X WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_x, XK_VoidSymbol}, "ẍ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"}, // U1F0C | GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἄ"},
  {{XK_dead_acute, XK_Greek_OMEGA, XK_VoidSymbol}, "Ώ"}, // U038F | GREEK CAPITAL LETTER OMEGA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_OMEGA, XK_VoidSymbol}, "Ώ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_OMEGA, XK_VoidSymbol}, "Ώ"},
  {{XK_Multi_key, XK_Greek_OMEGA, XK_apostrophe, XK_VoidSymbol}, "Ώ"},
  {{XK_Multi_key, XK_1, XK_6, XK_VoidSymbol}, "⅙"}, // U2159 | VULGAR FRACTION ONE SIXTH
  {{XK_dead_hook, XK_G, XK_VoidSymbol}, "Ɠ"}, // U0193 | LATIN CAPITAL LETTER G WITH HOOK
  {{XK_dead_belowcircumflex, XK_D, XK_VoidSymbol}, "Ḓ"}, // U1E12 | LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
  {{XK_dead_currency, XK_D, XK_VoidSymbol}, "₯"}, // U20AF | DRACHMA SIGN
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"}, // U1F97 | GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾗ"},
  {{XK_dead_ogonek, XK_E, XK_VoidSymbol}, "Ę"}, // U0118 | LATIN CAPITAL LETTER E WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_E, XK_VoidSymbol}, "Ę"},
  {{XK_Multi_key, XK_E, XK_semicolon, XK_VoidSymbol}, "Ę"},
  {{XK_Multi_key, XK_comma, XK_E, XK_VoidSymbol}, "Ę"},
  {{XK_Multi_key, XK_E, XK_comma, XK_VoidSymbol}, "Ę"},
  {{XK_dead_belowcomma, XK_T, XK_VoidSymbol}, "Ț"}, // U021A | LATIN CAPITAL LETTER T WITH COMMA BELOW
  {{XK_Multi_key, XK_semicolon, XK_T, XK_VoidSymbol}, "Ț"},
  {{XK_Multi_key, XK_T, XK_semicolon, XK_VoidSymbol}, "Ț"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"}, // U1F1C | GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἔ"},
  {{XK_Multi_key, XK_comma, XK_quotedbl, XK_VoidSymbol}, "„"}, // U201e | DOUBLE LOW-9 QUOTATION MARK
  {{XK_Multi_key, XK_quotedbl, XK_comma, XK_VoidSymbol}, "„"},
  {{XK_dead_abovedot, XK_H, XK_VoidSymbol}, "Ḣ"}, // U1E22 | LATIN CAPITAL LETTER H WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_H, XK_VoidSymbol}, "Ḣ"},
  {{XK_dead_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"}, // U1FA7 | GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾧ"},
  {{XK_Multi_key, XK_W, XK_equal, XK_VoidSymbol}, "₩"}, // U20a9 | WON SIGN
  {{XK_Multi_key, XK_equal, XK_W, XK_VoidSymbol}, "₩"},
  {{XK_dead_currency, XK_W, XK_VoidSymbol}, "₩"},
  {{XK_dead_currency, XK_w, XK_VoidSymbol}, "₩"},
  {{XK_dead_tilde, XK_I, XK_VoidSymbol}, "Ĩ"}, // U0128 | LATIN CAPITAL LETTER I WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_I, XK_VoidSymbol}, "Ĩ"},
  {{XK_Multi_key, XK_I, XK_asciitilde, XK_VoidSymbol}, "Ĩ"},
  {{XK_dead_macron, XK_Odiaeresis, XK_VoidSymbol}, "Ȫ"}, // U022A | LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_Odiaeresis, XK_VoidSymbol}, "Ȫ"},
  {{XK_Multi_key, XK_underscore, XK_Odiaeresis, XK_VoidSymbol}, "Ȫ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_dead_diaeresis, XK_Omacron, XK_VoidSymbol}, "Ȫ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ȫ"},
  {{XK_dead_circumflex, XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ậ"}, // U1EAD | LATIN SMALL LETTER A WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_a, XK_VoidSymbol}, "ậ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ậ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_a, XK_VoidSymbol}, "ậ"},
  {{XK_dead_belowdot, XK_acircumflex, XK_VoidSymbol}, "ậ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ậ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"}, // U1F2C | GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "Ἤ"},
  {{XK_dead_acute, XK_Greek_iota, XK_VoidSymbol}, "ί"}, // U03AF | GREEK SMALL LETTER IOTA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_iota, XK_VoidSymbol}, "ί"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_iota, XK_VoidSymbol}, "ί"},
  {{XK_dead_belowdot, XK_K, XK_VoidSymbol}, "Ḳ"}, // U1E32 | LATIN CAPITAL LETTER K WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_K, XK_VoidSymbol}, "Ḳ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_0, XK_parenright, XK_VoidSymbol}, "㊵"}, // U32B5 | CIRCLED NUMBER FORTY
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㊵"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_0, XK_parenright, XK_VoidSymbol}, "㊵"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_0, XK_parenright, XK_VoidSymbol}, "㊵"},
  {{XK_Multi_key, XK_underscore, XK_2, XK_VoidSymbol}, "₂"}, // U2082 | SUBSCRIPT TWO
  {{XK_Multi_key, XK_underscore, XK_KP_Space, XK_VoidSymbol}, "₂"},
  {{XK_Multi_key, XK_underscore, XK_KP_2, XK_VoidSymbol}, "₂"},
  {{XK_dead_caron, XK_2, XK_VoidSymbol}, "₂"},
  {{XK_dead_iota, XK_dead_tilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾷ"}, // U1FB7 | GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾷ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾷ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾷ"},
  {{XK_dead_iota, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾈ"}, // U1F88 | GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾈ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾈ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾈ"},
  {{XK_Multi_key, XK_R, XK_equal, XK_VoidSymbol}, "₹"}, // U20b9 | INDIAN RUPEE SIGN
  {{XK_Multi_key, XK_equal, XK_R, XK_VoidSymbol}, "₹"},
  {{XK_Multi_key, XK_r, XK_equal, XK_VoidSymbol}, "₹"},
  {{XK_Multi_key, XK_equal, XK_r, XK_VoidSymbol}, "₹"},
  {{XK_Multi_key, XK_k, XK_k, XK_VoidSymbol}, "ĸ"}, // U0138 | LATIN SMALL LETTER KRA
  {{XK_dead_stroke, XK_A, XK_VoidSymbol}, "Ⱥ"}, // U023A | LATIN CAPITAL LETTER A WITH STROKE
  {{XK_dead_tilde, XK_e, XK_VoidSymbol}, "ẽ"}, // U1EBD | LATIN SMALL LETTER E WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_e, XK_VoidSymbol}, "ẽ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"}, // U1F3C | GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_IOTA, XK_VoidSymbol}, "Ἴ"},
  {{XK_dead_breve, XK_Cyrillic_ZHE, XK_VoidSymbol}, "Ӂ"}, // U04C1 | CYRILLIC CAPITAL LETTER ZHE WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_ZHE, XK_VoidSymbol}, "Ӂ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_ZHE, XK_VoidSymbol}, "Ӂ"},
  {{XK_dead_belowdot, XK_M, XK_VoidSymbol}, "Ṃ"}, // U1E42 | LATIN CAPITAL LETTER M WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_M, XK_VoidSymbol}, "Ṃ"},
  {{XK_dead_diaeresis, XK_acute, XK_VoidSymbol}, "̈́"}, // U0344 | COMBINING GREEK DIALYTIKA TONOS
  {{XK_dead_diaeresis, XK_apostrophe, XK_VoidSymbol}, "̈́"},
  {{XK_Multi_key, XK_quotedbl, XK_dead_acute, XK_VoidSymbol}, "̈́"},
  {{XK_Multi_key, XK_quotedbl, XK_acute, XK_VoidSymbol}, "̈́"},
  {{XK_Multi_key, XK_quotedbl, XK_apostrophe, XK_VoidSymbol}, "̈́"},
  {{XK_dead_iota, XK_dead_tilde, XK_Greek_eta, XK_VoidSymbol}, "ῇ"}, // U1FC7 | GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_Greek_eta, XK_VoidSymbol}, "ῇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_Greek_eta, XK_VoidSymbol}, "ῇ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_Greek_eta, XK_VoidSymbol}, "ῇ"},
  {{XK_Multi_key, XK_parenleft, XK_q, XK_parenright, XK_VoidSymbol}, "ⓠ"}, // U24E0 | CIRCLED LATIN SMALL LETTER Q
  {{XK_dead_acute, XK_E, XK_VoidSymbol}, "É"}, // Eacute | LATIN CAPITAL LETTER E WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_E, XK_VoidSymbol}, "É"},
  {{XK_Multi_key, XK_E, XK_acute, XK_VoidSymbol}, "É"},
  {{XK_Multi_key, XK_apostrophe, XK_E, XK_VoidSymbol}, "É"},
  {{XK_Multi_key, XK_E, XK_apostrophe, XK_VoidSymbol}, "É"},
  {{XK_dead_caron, XK_n, XK_VoidSymbol}, "ň"}, // U0148 | LATIN SMALL LETTER N WITH CARON
  {{XK_Multi_key, XK_c, XK_n, XK_VoidSymbol}, "ň"},
  {{XK_Multi_key, XK_less, XK_n, XK_VoidSymbol}, "ň"},
  {{XK_Multi_key, XK_n, XK_less, XK_VoidSymbol}, "ň"},
  {{XK_dead_macron, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̄"}, // CYRILLIC CAPITAL LETTER IE WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_IE, XK_VoidSymbol}, "Е̄"},
  {{XK_dead_belowdot, XK_o, XK_VoidSymbol}, "ọ"}, // U1ECD | LATIN SMALL LETTER O WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_o, XK_VoidSymbol}, "ọ"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"}, // U1F4C | GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὄ"},
  {{XK_dead_breve, XK_Cyrillic_a, XK_VoidSymbol}, "ӑ"}, // U04D1 | CYRILLIC SMALL LETTER A WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_a, XK_VoidSymbol}, "ӑ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_a, XK_VoidSymbol}, "ӑ"},
  {{XK_dead_caron, XK_U, XK_VoidSymbol}, "Ǔ"}, // U01D3 | LATIN CAPITAL LETTER U WITH CARON
  {{XK_Multi_key, XK_c, XK_U, XK_VoidSymbol}, "Ǔ"},
  {{XK_dead_acute, XK_Omacron, XK_VoidSymbol}, "Ṓ"}, // U1E52 | LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Omacron, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_Omacron, XK_VoidSymbol}, "Ṓ"},
  {{XK_dead_acute, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_dead_acute, XK_Multi_key, XK_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_dead_acute, XK_Multi_key, XK_underscore, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_acute, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_acute, XK_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_acute, XK_underscore, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_macron, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_apostrophe, XK_underscore, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_dead_macron, XK_Oacute, XK_VoidSymbol}, "Ṓ"},
  {{XK_dead_macron, XK_dead_acute, XK_O, XK_VoidSymbol}, "Ṓ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_KA, XK_parenright, XK_VoidSymbol}, "㋕"}, // U32D5 | CIRCLED KATAKANA KA
  {{XK_dead_tilde, XK_Greek_iotadieresis, XK_VoidSymbol}, "ῗ"}, // U1FD7 | GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_iotadieresis, XK_VoidSymbol}, "ῗ"},
  {{XK_dead_tilde, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ῗ"},
  {{XK_dead_tilde, XK_Multi_key, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ῗ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_diaeresis, XK_Greek_iota, XK_VoidSymbol}, "ῗ"},
  {{XK_Multi_key, XK_asciitilde, XK_quotedbl, XK_Greek_iota, XK_VoidSymbol}, "ῗ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_3, XK_parenright, XK_VoidSymbol}, "㊸"}, // U32B8 | CIRCLED NUMBER FORTY THREE
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㊸"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_3, XK_parenright, XK_VoidSymbol}, "㊸"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㊸"},
  {{XK_dead_grave, XK_U, XK_VoidSymbol}, "Ù"}, // Ugrave | LATIN CAPITAL LETTER U WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_U, XK_VoidSymbol}, "Ù"},
  {{XK_Multi_key, XK_U, XK_grave, XK_VoidSymbol}, "Ù"},
  {{XK_dead_caron, XK_R, XK_VoidSymbol}, "Ř"}, // U0158 | LATIN CAPITAL LETTER R WITH CARON
  {{XK_Multi_key, XK_c, XK_R, XK_VoidSymbol}, "Ř"},
  {{XK_Multi_key, XK_less, XK_R, XK_VoidSymbol}, "Ř"},
  {{XK_Multi_key, XK_R, XK_less, XK_VoidSymbol}, "Ř"},
  {{XK_dead_invertedbreve, XK_Cyrillic_I, XK_VoidSymbol}, "И̑"}, // CYRILLIC CAPITAL LETTER I WITH COMBINING INVERTED BREVE
  {{XK_Multi_key, XK_parenleft, XK_3, XK_parenright, XK_VoidSymbol}, "③"}, // U2462 | CIRCLED DIGIT THREE
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_parenright, XK_VoidSymbol}, "③"},
  {{XK_dead_grave, XK_ohorn, XK_VoidSymbol}, "ờ"}, // U1EDD | LATIN SMALL LETTER O WITH HORN AND GRAVE
  {{XK_Multi_key, XK_grave, XK_ohorn, XK_VoidSymbol}, "ờ"},
  {{XK_dead_grave, XK_dead_horn, XK_o, XK_VoidSymbol}, "ờ"},
  {{XK_dead_grave, XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ờ"},
  {{XK_Multi_key, XK_grave, XK_dead_horn, XK_o, XK_VoidSymbol}, "ờ"},
  {{XK_Multi_key, XK_grave, XK_plus, XK_o, XK_VoidSymbol}, "ờ"},
  {{XK_dead_horn, XK_ograve, XK_VoidSymbol}, "ờ"},
  {{XK_dead_horn, XK_dead_grave, XK_o, XK_VoidSymbol}, "ờ"},
  {{XK_dead_circumflex, XK_space, XK_VoidSymbol}, "^"}, // asciicircum | CIRCUMFLEX ACCENT
  {{XK_dead_circumflex, XK_dead_circumflex, XK_VoidSymbol}, "^"},
  {{XK_Multi_key, XK_asciicircum, XK_space, XK_VoidSymbol}, "^"},
  {{XK_Multi_key, XK_space, XK_asciicircum, XK_VoidSymbol}, "^"},
  {{XK_Multi_key, XK_greater, XK_space, XK_VoidSymbol}, "^"},
  {{XK_Multi_key, XK_space, XK_greater, XK_VoidSymbol}, "^"},
  {{XK_Multi_key, XK_parenleft, XK_r, XK_parenright, XK_VoidSymbol}, "ⓡ"}, // U24E1 | CIRCLED LATIN SMALL LETTER R
  {{XK_dead_circumflex, XK_Cyrillic_a, XK_VoidSymbol}, "а̂"}, // CYRILLIC SMALL LETTER A WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_a, XK_VoidSymbol}, "а̂"},
  {{XK_dead_macron, XK_ae, XK_VoidSymbol}, "ǣ"}, // U01E3 | LATIN SMALL LETTER AE WITH MACRON
  {{XK_Multi_key, XK_macron, XK_ae, XK_VoidSymbol}, "ǣ"},
  {{XK_Multi_key, XK_underscore, XK_ae, XK_VoidSymbol}, "ǣ"},
  {{XK_dead_belowdot, XK_S, XK_VoidSymbol}, "Ṣ"}, // U1E62 | LATIN CAPITAL LETTER S WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_S, XK_VoidSymbol}, "Ṣ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_NI, XK_parenright, XK_VoidSymbol}, "㋥"}, // U32E5 | CIRCLED KATAKANA NI
  {{XK_dead_tilde, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ῧ"}, // U1FE7 | GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_upsilondieresis, XK_VoidSymbol}, "ῧ"},
  {{XK_dead_tilde, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ῧ"},
  {{XK_dead_tilde, XK_Multi_key, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ῧ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_diaeresis, XK_Greek_upsilon, XK_VoidSymbol}, "ῧ"},
  {{XK_Multi_key, XK_asciitilde, XK_quotedbl, XK_Greek_upsilon, XK_VoidSymbol}, "ῧ"},
  {{XK_Multi_key, XK_parenleft, XK_7, XK_parenright, XK_VoidSymbol}, "⑦"}, // U2466 | CIRCLED DIGIT SEVEN
  {{XK_Multi_key, XK_parenleft, XK_KP_7, XK_parenright, XK_VoidSymbol}, "⑦"},
  {{XK_dead_acute, XK_e, XK_VoidSymbol}, "é"}, // eacute | LATIN SMALL LETTER E WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_e, XK_VoidSymbol}, "é"},
  {{XK_Multi_key, XK_e, XK_acute, XK_VoidSymbol}, "é"},
  {{XK_Multi_key, XK_apostrophe, XK_e, XK_VoidSymbol}, "é"},
  {{XK_Multi_key, XK_e, XK_apostrophe, XK_VoidSymbol}, "é"},
  {{XK_dead_tilde, XK_U, XK_VoidSymbol}, "Ũ"}, // U0168 | LATIN CAPITAL LETTER U WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_U, XK_VoidSymbol}, "Ũ"},
  {{XK_Multi_key, XK_U, XK_asciitilde, XK_VoidSymbol}, "Ũ"},
  {{XK_dead_hook, XK_uhorn, XK_VoidSymbol}, "ử"}, // U1EED | LATIN SMALL LETTER U WITH HORN AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_uhorn, XK_VoidSymbol}, "ử"},
  {{XK_dead_hook, XK_dead_horn, XK_u, XK_VoidSymbol}, "ử"},
  {{XK_dead_hook, XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ử"},
  {{XK_Multi_key, XK_question, XK_dead_horn, XK_u, XK_VoidSymbol}, "ử"},
  {{XK_Multi_key, XK_question, XK_plus, XK_u, XK_VoidSymbol}, "ử"},
  {{XK_dead_horn, XK_uhook, XK_VoidSymbol}, "ử"},
  {{XK_dead_horn, XK_dead_hook, XK_u, XK_VoidSymbol}, "ử"},
  {{XK_dead_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"}, // U1F6C | GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
  {{XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"},
  {{XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"},
  {{XK_Multi_key, XK_acute, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"},
  {{XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὤ"},
  {{XK_dead_diaeresis, XK_Cyrillic_u, XK_VoidSymbol}, "ӱ"}, // U04F1 | CYRILLIC SMALL LETTER U WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_u, XK_VoidSymbol}, "ӱ"},
  {{XK_dead_currency, XK_t, XK_VoidSymbol}, "৳"}, // U09F3 | BENGALI RUPEE SIGN
  {{XK_dead_belowdiaeresis, XK_U, XK_VoidSymbol}, "Ṳ"}, // U1E72 | LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
  {{XK_Multi_key, XK_parenleft, XK_kana_YO, XK_parenright, XK_VoidSymbol}, "㋵"}, // U32F5 | CIRCLED KATAKANA YO
  {{XK_dead_iota, XK_dead_tilde, XK_Greek_omega, XK_VoidSymbol}, "ῷ"}, // U1FF7 | GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_Greek_omega, XK_VoidSymbol}, "ῷ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_Greek_omega, XK_VoidSymbol}, "ῷ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_Greek_omega, XK_VoidSymbol}, "ῷ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_8, XK_parenright, XK_VoidSymbol}, "㊳"}, // U32B3 | CIRCLED NUMBER THIRTY EIGHT
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㊳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_8, XK_parenright, XK_VoidSymbol}, "㊳"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㊳"},
  {{XK_dead_grave, XK_u, XK_VoidSymbol}, "ù"}, // ugrave | LATIN SMALL LETTER U WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_u, XK_VoidSymbol}, "ù"},
  {{XK_Multi_key, XK_u, XK_grave, XK_VoidSymbol}, "ù"},
  {{XK_dead_diaeresis, XK_Y, XK_VoidSymbol}, "Ÿ"}, // U0178 | LATIN CAPITAL LETTER Y WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Y, XK_VoidSymbol}, "Ÿ"},
  {{XK_Multi_key, XK_Y, XK_quotedbl, XK_VoidSymbol}, "Ÿ"},
  {{XK_Multi_key, XK_diaeresis, XK_Y, XK_VoidSymbol}, "Ÿ"},
  {{XK_Multi_key, XK_Y, XK_diaeresis, XK_VoidSymbol}, "Ÿ"},
  {{XK_dead_grave, XK_Greek_omega, XK_VoidSymbol}, "ὼ"}, // U1F7C | GREEK SMALL LETTER OMEGA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_omega, XK_VoidSymbol}, "ὼ"},
  {{XK_dead_tilde, XK_space, XK_VoidSymbol}, "~"}, // asciitilde | TILDE
  {{XK_dead_tilde, XK_dead_tilde, XK_VoidSymbol}, "~"},
  {{XK_Multi_key, XK_asciitilde, XK_space, XK_VoidSymbol}, "~"},
  {{XK_Multi_key, XK_space, XK_asciitilde, XK_VoidSymbol}, "~"},
  {{XK_Multi_key, XK_minus, XK_space, XK_VoidSymbol}, "~"},
  {{XK_Multi_key, XK_space, XK_minus, XK_VoidSymbol}, "~"},
  {{XK_dead_diaeresis, XK_Cyrillic_IE, XK_VoidSymbol}, "Ё"}, // U0401 | CYRILLIC CAPITAL LETTER IO
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_IE, XK_VoidSymbol}, "Ё"},
  {{XK_dead_breve, XK_a, XK_VoidSymbol}, "ă"}, // U0103 | LATIN SMALL LETTER A WITH BREVE
  {{XK_Multi_key, XK_U, XK_a, XK_VoidSymbol}, "ă"},
  {{XK_Multi_key, XK_u, XK_a, XK_VoidSymbol}, "ă"},
  {{XK_Multi_key, XK_b, XK_a, XK_VoidSymbol}, "ă"},
  {{XK_Multi_key, XK_a, XK_parenleft, XK_VoidSymbol}, "ă"},
  {{XK_dead_acute, XK_W, XK_VoidSymbol}, "Ẃ"}, // U1E82 | LATIN CAPITAL LETTER W WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_W, XK_VoidSymbol}, "Ẃ"},
  {{XK_Multi_key, XK_apostrophe, XK_W, XK_VoidSymbol}, "Ẃ"},
  {{XK_dead_doublegrave, XK_e, XK_VoidSymbol}, "ȅ"}, // U0205 | LATIN SMALL LETTER E WITH DOUBLE GRAVE
  {{XK_dead_macron, XK_Cyrillic_a, XK_VoidSymbol}, "а̄"}, // CYRILLIC SMALL LETTER A WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_a, XK_VoidSymbol}, "а̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_a, XK_VoidSymbol}, "а̄"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἇ"}, // U1F07 | GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἇ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἇ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἇ"},
  {{XK_dead_hook, XK_c, XK_VoidSymbol}, "ƈ"}, // U0188 | LATIN SMALL LETTER C WITH HOOK
  {{XK_dead_belowdot, XK_d, XK_VoidSymbol}, "ḍ"}, // U1E0D | LATIN SMALL LETTER D WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_d, XK_VoidSymbol}, "ḍ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"}, // U1F8C | GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾌ"},
  {{XK_Multi_key, XK_underscore, XK_parenright, XK_VoidSymbol}, "₎"}, // U208E | SUBSCRIPT RIGHT PARENTHESIS
  {{XK_dead_caron, XK_parenright, XK_VoidSymbol}, "₎"},
  {{XK_dead_macron, XK_e, XK_VoidSymbol}, "ē"}, // U0113 | LATIN SMALL LETTER E WITH MACRON
  {{XK_Multi_key, XK_macron, XK_e, XK_VoidSymbol}, "ē"},
  {{XK_Multi_key, XK_underscore, XK_e, XK_VoidSymbol}, "ē"},
  {{XK_Multi_key, XK_e, XK_underscore, XK_VoidSymbol}, "ē"},
  {{XK_Multi_key, XK_minus, XK_e, XK_VoidSymbol}, "ē"},
  {{XK_Multi_key, XK_e, XK_minus, XK_VoidSymbol}, "ē"},
  {{XK_dead_belowdot, XK_Z, XK_VoidSymbol}, "Ẓ"}, // U1E92 | LATIN CAPITAL LETTER Z WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_Z, XK_VoidSymbol}, "Ẓ"},
  {{XK_dead_doublegrave, XK_u, XK_VoidSymbol}, "ȕ"}, // U0215 | LATIN SMALL LETTER U WITH DOUBLE GRAVE
  {{XK_dead_belowmacron, XK_t, XK_VoidSymbol}, "ṯ"}, // U1E6F | LATIN SMALL LETTER T WITH LINE BELOW
  {{XK_dead_currency, XK_B, XK_VoidSymbol}, "₱"}, // U20B1 | PESO SIGN
  {{XK_Multi_key, XK_greater, XK_apostrophe, XK_VoidSymbol}, "’"}, // U2019 | RIGHT SINGLE QUOTATION MARK
  {{XK_Multi_key, XK_apostrophe, XK_greater, XK_VoidSymbol}, "’"},
  {{XK_dead_hook, XK_K, XK_VoidSymbol}, "Ƙ"}, // U0198 | LATIN CAPITAL LETTER K WITH HOOK
  {{XK_dead_breve, XK_dead_cedilla, XK_e, XK_VoidSymbol}, "ḝ"}, // U1E1D | LATIN SMALL LETTER E WITH CEDILLA AND BREVE
  {{XK_dead_breve, XK_Multi_key, XK_comma, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_dead_breve, XK_Multi_key, XK_cedilla, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_U, XK_dead_cedilla, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_U, XK_space, XK_comma, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_U, XK_cedilla, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_b, XK_dead_cedilla, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_b, XK_comma, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_Multi_key, XK_b, XK_cedilla, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_dead_cedilla, XK_dead_breve, XK_e, XK_VoidSymbol}, "ḝ"},
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"}, // U1F9C | GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_ETA, XK_VoidSymbol}, "ᾜ"},
  {{XK_dead_cedilla, XK_g, XK_VoidSymbol}, "ģ"}, // U0123 | LATIN SMALL LETTER G WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_g, XK_VoidSymbol}, "ģ"},
  {{XK_Multi_key, XK_g, XK_comma, XK_VoidSymbol}, "ģ"},
  {{XK_Multi_key, XK_cedilla, XK_g, XK_VoidSymbol}, "ģ"},
  {{XK_dead_hook, XK_A, XK_VoidSymbol}, "Ả"}, // U1EA2 | LATIN CAPITAL LETTER A WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_A, XK_VoidSymbol}, "Ả"},
  {{XK_dead_belowdot, XK_plus, XK_VoidSymbol}, "⨥"}, // U2A25 | PLUS SIGN WITH DOT BELOW
  {{XK_dead_voiced_sound, XK_kana_TE, XK_VoidSymbol}, "デ"}, // U30C7 | KATAKANA LETTER DE
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἧ"}, // U1F27 | GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἧ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἧ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἧ"},
  {{XK_dead_circumflex, XK_Cyrillic_i, XK_VoidSymbol}, "и̂"}, // CYRILLIC SMALL LETTER I WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_i, XK_VoidSymbol}, "и̂"},
  {{XK_dead_acute, XK_Cyrillic_i, XK_VoidSymbol}, "и́"}, // CYRILLIC SMALL LETTER I WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_i, XK_VoidSymbol}, "и́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_i, XK_VoidSymbol}, "и́"},
  {{XK_Multi_key, XK_C, XK_C, XK_C, XK_P, XK_VoidSymbol}, "☭"}, // U262D | HAMMER AND SICKLE
  {{XK_dead_iota, XK_dead_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"}, // U1FAC | GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_acute, XK_Multi_key, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_dead_iota, XK_Multi_key, XK_acute, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_dead_iota, XK_Multi_key, XK_apostrophe, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_acute, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_acute, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_dead_psili, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_Greek_iota, XK_apostrophe, XK_parenright, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾬ"},
  {{XK_Multi_key, XK_o, XK_r, XK_VoidSymbol}, "®"}, // registered | REGISTERED SIGN
  {{XK_Multi_key, XK_o, XK_R, XK_VoidSymbol}, "®"},
  {{XK_Multi_key, XK_O, XK_r, XK_VoidSymbol}, "®"},
  {{XK_Multi_key, XK_O, XK_R, XK_VoidSymbol}, "®"},
  {{XK_Multi_key, XK_R, XK_o, XK_VoidSymbol}, "®"},
  {{XK_Multi_key, XK_R, XK_O, XK_VoidSymbol}, "®"},
  {{XK_Multi_key, XK_i, XK_j, XK_VoidSymbol}, "ĳ"}, // U0133 | LATIN SMALL LIGATURE IJ
  {{XK_dead_hook, XK_Abreve, XK_VoidSymbol}, "Ẳ"}, // U1EB2 | LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Abreve, XK_VoidSymbol}, "Ẳ"},
  {{XK_dead_hook, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_dead_hook, XK_Multi_key, XK_U, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_dead_hook, XK_Multi_key, XK_b, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_Multi_key, XK_question, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_Multi_key, XK_question, XK_b, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_dead_breve, XK_Ahook, XK_VoidSymbol}, "Ẳ"},
  {{XK_dead_breve, XK_dead_hook, XK_A, XK_VoidSymbol}, "Ẳ"},
  {{XK_Multi_key, XK_period, XK_colon, XK_VoidSymbol}, "∵"}, // because | . : BECAUSE
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἷ"}, // U1F37 | GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἷ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἷ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἷ"},
  {{XK_Multi_key, XK_O, XK_A, XK_VoidSymbol}, "Ⓐ"}, // U24B6 | CIRCLED LATIN CAPITAL LETTER A
  {{XK_Multi_key, XK_parenleft, XK_A, XK_parenright, XK_VoidSymbol}, "Ⓐ"},
  {{XK_Multi_key, XK_period, XK_less, XK_VoidSymbol}, "‹"}, // U2039 | SINGLE LEFT-POINTING ANGLE QUOTATION MARK
  {{XK_Multi_key, XK_parenleft, XK_4, XK_5, XK_parenright, XK_VoidSymbol}, "㊺"}, // U32BA | CIRCLED NUMBER FORTY FIVE
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㊺"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_5, XK_parenright, XK_VoidSymbol}, "㊺"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㊺"},
  {{XK_dead_belowcircumflex, XK_l, XK_VoidSymbol}, "ḽ"}, // U1E3D | LATIN SMALL LETTER L WITH CIRCUMFLEX BELOW
  {{XK_dead_iota, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾼ"}, // U1FBC | GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾼ"},
  {{XK_dead_grave, XK_Cyrillic_a, XK_VoidSymbol}, "а̀"}, // CYRILLIC SMALL LETTER A WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_a, XK_VoidSymbol}, "а̀"},
  {{XK_Multi_key, XK_3, XK_4, XK_VoidSymbol}, "¾"}, // threequarters | VULGAR FRACTION THREE QUARTERS
  {{XK_dead_acute, XK_N, XK_VoidSymbol}, "Ń"}, // U0143 | LATIN CAPITAL LETTER N WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_N, XK_VoidSymbol}, "Ń"},
  {{XK_Multi_key, XK_apostrophe, XK_N, XK_VoidSymbol}, "Ń"},
  {{XK_Multi_key, XK_N, XK_apostrophe, XK_VoidSymbol}, "Ń"},
  {{XK_dead_hook, XK_Ecircumflex, XK_VoidSymbol}, "Ể"}, // U1EC2 | LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Ecircumflex, XK_VoidSymbol}, "Ể"},
  {{XK_dead_hook, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ể"},
  {{XK_dead_hook, XK_Multi_key, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ể"},
  {{XK_Multi_key, XK_question, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ể"},
  {{XK_Multi_key, XK_question, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ể"},
  {{XK_dead_circumflex, XK_Ehook, XK_VoidSymbol}, "Ể"},
  {{XK_dead_circumflex, XK_dead_hook, XK_E, XK_VoidSymbol}, "Ể"},
  {{XK_Multi_key, XK_parenleft, XK_Q, XK_parenright, XK_VoidSymbol}, "Ⓠ"}, // U24C6 | CIRCLED LATIN CAPITAL LETTER Q
  {{XK_dead_invertedbreve, XK_Cyrillic_O, XK_VoidSymbol}, "О̑"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING INVERTED BREVE
  {{XK_dead_acute, XK_otilde, XK_VoidSymbol}, "ṍ"}, // U1E4D | LATIN SMALL LETTER O WITH TILDE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_otilde, XK_VoidSymbol}, "ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_otilde, XK_VoidSymbol}, "ṍ"},
  {{XK_dead_acute, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_dead_acute, XK_Multi_key, XK_asciitilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_Multi_key, XK_acute, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_Multi_key, XK_acute, XK_asciitilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_Multi_key, XK_apostrophe, XK_asciitilde, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_dead_tilde, XK_oacute, XK_VoidSymbol}, "ṍ"},
  {{XK_dead_tilde, XK_dead_acute, XK_o, XK_VoidSymbol}, "ṍ"},
  {{XK_dead_iota, XK_Greek_ETA, XK_VoidSymbol}, "ῌ"}, // U1FCC | GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_ETA, XK_VoidSymbol}, "ῌ"},
  {{XK_dead_circumflex, XK_I, XK_VoidSymbol}, "Î"}, // Icircumflex | LATIN CAPITAL LETTER I WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_I, XK_VoidSymbol}, "Î"},
  {{XK_Multi_key, XK_I, XK_asciicircum, XK_VoidSymbol}, "Î"},
  {{XK_Multi_key, XK_greater, XK_I, XK_VoidSymbol}, "Î"},
  {{XK_Multi_key, XK_I, XK_greater, XK_VoidSymbol}, "Î"},
  {{XK_dead_diaeresis, XK_Cyrillic_ie, XK_VoidSymbol}, "ё"}, // U0451 | CYRILLIC SMALL LETTER IO
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_ie, XK_VoidSymbol}, "ё"},
  {{XK_Multi_key, XK_o, XK_e, XK_VoidSymbol}, "œ"}, // oe | LATIN SMALL LIGATURE OE
  {{XK_Multi_key, XK_C, XK_r, XK_VoidSymbol}, "₢"}, // U20a2 | CRUZEIRO SIGN
  {{XK_dead_currency, XK_r, XK_VoidSymbol}, "₢"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_5, XK_parenright, XK_VoidSymbol}, "㉕"}, // U3255 | CIRCLED NUMBER TWENTY FIVE
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㉕"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_5, XK_parenright, XK_VoidSymbol}, "㉕"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㉕"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_5, XK_parenright, XK_VoidSymbol}, "㉕"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_5, XK_parenright, XK_VoidSymbol}, "㉕"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὗ"}, // U1F57 | GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὗ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὗ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὗ"},
  {{XK_dead_breve, XK_Cyrillic_IE, XK_VoidSymbol}, "Ӗ"}, // U04D6 | CYRILLIC CAPITAL LETTER IE WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_IE, XK_VoidSymbol}, "Ӗ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_IE, XK_VoidSymbol}, "Ӗ"},
  {{XK_dead_grave, XK_Cyrillic_O, XK_VoidSymbol}, "О̀"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_O, XK_VoidSymbol}, "О̀"},
  {{XK_dead_acute, XK_udiaeresis, XK_VoidSymbol}, "ǘ"}, // U01D8 | LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
  {{XK_Multi_key, XK_acute, XK_udiaeresis, XK_VoidSymbol}, "ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_udiaeresis, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_acute, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_acute, XK_Multi_key, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_Multi_key, XK_acute, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_Multi_key, XK_acute, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_Multi_key, XK_apostrophe, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_acute, XK_v, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_diaeresis, XK_uacute, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_diaeresis, XK_dead_acute, XK_u, XK_VoidSymbol}, "ǘ"},
  {{XK_dead_circumflex, XK_Cyrillic_O, XK_VoidSymbol}, "О̂"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_O, XK_VoidSymbol}, "О̂"},
  {{XK_Multi_key, XK_parenleft, XK_kana_SA, XK_parenright, XK_VoidSymbol}, "㋚"}, // U32DA | CIRCLED KATAKANA SA
  {{XK_dead_macron, XK_dead_belowdot, XK_r, XK_VoidSymbol}, "ṝ"}, // U1E5D | LATIN SMALL LETTER R WITH DOT BELOW AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_exclam, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_Multi_key, XK_macron, XK_dead_belowdot, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_Multi_key, XK_macron, XK_exclam, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_Multi_key, XK_underscore, XK_dead_belowdot, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_Multi_key, XK_underscore, XK_exclam, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_dead_belowdot, XK_dead_macron, XK_r, XK_VoidSymbol}, "ṝ"},
  {{XK_dead_circumflex, XK_minus, XK_VoidSymbol}, "⁻"}, // U207B | SUPERSCRIPT MINUS
  {{XK_Multi_key, XK_T, XK_H, XK_VoidSymbol}, "Þ"}, // THORN | LATIN CAPITAL LETTER THORN
  {{XK_Multi_key, XK_parenleft, XK_2, XK_parenright, XK_VoidSymbol}, "②"}, // U2461 | CIRCLED DIGIT TWO
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_parenright, XK_VoidSymbol}, "②"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_parenright, XK_VoidSymbol}, "②"},
  {{XK_dead_voiced_sound, XK_kana_KA, XK_VoidSymbol}, "ガ"}, // U30AC | KATAKANA LETTER GA
  {{XK_dead_cedilla, XK_t, XK_VoidSymbol}, "ţ"}, // U0163 | LATIN SMALL LETTER T WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_t, XK_VoidSymbol}, "ţ"},
  {{XK_Multi_key, XK_t, XK_comma, XK_VoidSymbol}, "ţ"},
  {{XK_Multi_key, XK_cedilla, XK_t, XK_VoidSymbol}, "ţ"},
  {{XK_dead_belowdot, XK_Ohorn, XK_VoidSymbol}, "Ợ"}, // U1EE2 | LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_Ohorn, XK_VoidSymbol}, "Ợ"},
  {{XK_dead_belowdot, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ợ"},
  {{XK_dead_belowdot, XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ợ"},
  {{XK_Multi_key, XK_exclam, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ợ"},
  {{XK_Multi_key, XK_exclam, XK_plus, XK_O, XK_VoidSymbol}, "Ợ"},
  {{XK_dead_horn, XK_Obelowdot, XK_VoidSymbol}, "Ợ"},
  {{XK_dead_horn, XK_dead_belowdot, XK_O, XK_VoidSymbol}, "Ợ"},
  {{XK_Multi_key, XK_greater, XK_equal, XK_VoidSymbol}, "≥"}, // U2265 | GREATER-THAN OR EQUAL TO
  {{XK_Multi_key, XK_greater, XK_underscore, XK_VoidSymbol}, "≥"},
  {{XK_Multi_key, XK_underscore, XK_greater, XK_VoidSymbol}, "≥"},
  {{XK_dead_tilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὧ"}, // U1F67 | GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὧ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὧ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὧ"},
  {{XK_dead_diaeresis, XK_Cyrillic_O, XK_VoidSymbol}, "Ӧ"}, // U04E6 | CYRILLIC CAPITAL LETTER O WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_O, XK_VoidSymbol}, "Ӧ"},
  {{XK_dead_caron, XK_K, XK_VoidSymbol}, "Ǩ"}, // U01E8 | LATIN CAPITAL LETTER K WITH CARON
  {{XK_Multi_key, XK_c, XK_K, XK_VoidSymbol}, "Ǩ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_HI, XK_parenright, XK_VoidSymbol}, "㋪"}, // U32EA | CIRCLED KATAKANA HI
  {{XK_Multi_key, XK_numbersign, XK_b, XK_VoidSymbol}, "♭"}, // U266d | MUSIC FLAT SIGN
  {{XK_dead_dasia, XK_Greek_RHO, XK_VoidSymbol}, "Ῥ"}, // U1FEC | GREEK CAPITAL LETTER RHO WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_RHO, XK_VoidSymbol}, "Ῥ"},
  {{XK_dead_circumflex, XK_i, XK_VoidSymbol}, "î"}, // icircumflex | LATIN SMALL LETTER I WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_i, XK_VoidSymbol}, "î"},
  {{XK_Multi_key, XK_i, XK_asciicircum, XK_VoidSymbol}, "î"},
  {{XK_Multi_key, XK_greater, XK_i, XK_VoidSymbol}, "î"},
  {{XK_Multi_key, XK_i, XK_greater, XK_VoidSymbol}, "î"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_8, XK_parenright, XK_VoidSymbol}, "⑱"}, // U2471 | CIRCLED NUMBER EIGHTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_8, XK_parenright, XK_VoidSymbol}, "⑱"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_8, XK_parenright, XK_VoidSymbol}, "⑱"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_8, XK_parenright, XK_VoidSymbol}, "⑱"},
  {{XK_dead_ogonek, XK_u, XK_VoidSymbol}, "ų"}, // U0173 | LATIN SMALL LETTER U WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_u, XK_VoidSymbol}, "ų"},
  {{XK_Multi_key, XK_u, XK_semicolon, XK_VoidSymbol}, "ų"},
  {{XK_Multi_key, XK_comma, XK_u, XK_VoidSymbol}, "ų"},
  {{XK_Multi_key, XK_u, XK_comma, XK_VoidSymbol}, "ų"},
  {{XK_dead_grave, XK_Y, XK_VoidSymbol}, "Ỳ"}, // U1EF2 | LATIN CAPITAL LETTER Y WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_Y, XK_VoidSymbol}, "Ỳ"},
  {{XK_dead_circumflex, XK_Cyrillic_U, XK_VoidSymbol}, "У̂"}, // CYRILLIC CAPITAL LETTER U WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_U, XK_VoidSymbol}, "У̂"},
  {{XK_dead_circumflex, XK_9, XK_VoidSymbol}, "⁹"}, // U2079 | SUPERSCRIPT NINE
  {{XK_Multi_key, XK_asciicircum, XK_9, XK_VoidSymbol}, "⁹"},
  {{XK_dead_circumflex, XK_KP_9, XK_VoidSymbol}, "⁹"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_9, XK_VoidSymbol}, "⁹"},
  {{XK_dead_grave, XK_N, XK_VoidSymbol}, "Ǹ"}, // U01F8 | LATIN CAPITAL LETTER N WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_N, XK_VoidSymbol}, "Ǹ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_RO, XK_parenright, XK_VoidSymbol}, "㋺"}, // U32FA | CIRCLED KATAKANA RO
  {{XK_dead_tilde, XK_v, XK_VoidSymbol}, "ṽ"}, // U1E7D | LATIN SMALL LETTER V WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_v, XK_VoidSymbol}, "ṽ"},
  {{XK_dead_iota, XK_Greek_OMEGA, XK_VoidSymbol}, "ῼ"}, // U1FFC | GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
  {{XK_Multi_key, XK_Greek_iota, XK_Greek_OMEGA, XK_VoidSymbol}, "ῼ"},
  {{XK_Multi_key, XK_t, XK_h, XK_VoidSymbol}, "þ"}, // thorn | LATIN SMALL LETTER THORN
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾁ"}, // U1F81 | GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾁ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ᾁ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ᾁ"},
  {{XK_dead_belowring, XK_A, XK_VoidSymbol}, "Ḁ"}, // U1E00 | LATIN CAPITAL LETTER A WITH RING BELOW
  {{XK_dead_currency, XK_s, XK_VoidSymbol}, "₪"}, // NewSheqelSign | NEW SHEQEL SIGN
  {{XK_Multi_key, XK_parenleft, XK_h, XK_parenright, XK_VoidSymbol}, "ⓗ"}, // U24D7 | CIRCLED LATIN SMALL LETTER H
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἆ"}, // U1F06 | GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἆ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἆ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἆ"},
  {{XK_dead_acute, XK_Greek_ETA, XK_VoidSymbol}, "Ή"}, // U0389 | GREEK CAPITAL LETTER ETA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_ETA, XK_VoidSymbol}, "Ή"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_ETA, XK_VoidSymbol}, "Ή"},
  {{XK_Multi_key, XK_Greek_ETA, XK_apostrophe, XK_VoidSymbol}, "Ή"},
  {{XK_dead_doublegrave, XK_I, XK_VoidSymbol}, "Ȉ"}, // U0208 | LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
  {{XK_dead_abovedot, XK_x, XK_VoidSymbol}, "ẋ"}, // U1E8B | LATIN SMALL LETTER X WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_x, XK_VoidSymbol}, "ẋ"},
  {{XK_dead_abovedot, XK_C, XK_VoidSymbol}, "Ċ"}, // U010A | LATIN CAPITAL LETTER C WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_C, XK_VoidSymbol}, "Ċ"},
  {{XK_Multi_key, XK_C, XK_period, XK_VoidSymbol}, "Ċ"},
  {{XK_dead_acute, XK_Cyrillic_KA, XK_VoidSymbol}, "Ќ"}, // U040C | CYRILLIC CAPITAL LETTER KJE
  {{XK_Multi_key, XK_acute, XK_Cyrillic_KA, XK_VoidSymbol}, "Ќ"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_KA, XK_VoidSymbol}, "Ќ"},
  {{XK_dead_circumflex, XK_Cyrillic_er, XK_VoidSymbol}, "р̂"}, // CYRILLIC SMALL LETTER ER WITH COMBINING CIRCUMFLEX ACCENT
  {{XK_Multi_key, XK_asciicircum, XK_Cyrillic_er, XK_VoidSymbol}, "р̂"},
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾑ"}, // U1F91 | GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾑ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ᾑ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ᾑ"},
  {{XK_dead_cedilla, XK_D, XK_VoidSymbol}, "Ḑ"}, // U1E10 | LATIN CAPITAL LETTER D WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_D, XK_VoidSymbol}, "Ḑ"},
  {{XK_Multi_key, XK_D, XK_comma, XK_VoidSymbol}, "Ḑ"},
  {{XK_Multi_key, XK_cedilla, XK_D, XK_VoidSymbol}, "Ḑ"},
  {{XK_Multi_key, XK_minus, XK_minus, XK_minus, XK_VoidSymbol}, "—"}, // U2014 | EM DASH
  {{XK_dead_cedilla, XK_ColonSign, XK_VoidSymbol}, "₵"}, // U20B5 | CEDI SIGN
  {{XK_dead_cedilla, XK_cent, XK_VoidSymbol}, "₵"},
  {{XK_dead_currency, XK_Ccedilla, XK_VoidSymbol}, "₵"},
  {{XK_dead_currency, XK_ccedilla, XK_VoidSymbol}, "₵"},
  {{XK_dead_cedilla, XK_dead_currency, XK_C, XK_VoidSymbol}, "₵"},
  {{XK_dead_currency, XK_dead_cedilla, XK_C, XK_VoidSymbol}, "₵"},
  {{XK_dead_cedilla, XK_dead_currency, XK_c, XK_VoidSymbol}, "₵"},
  {{XK_dead_currency, XK_dead_cedilla, XK_c, XK_VoidSymbol}, "₵"},
  {{XK_dead_belowcomma, XK_S, XK_VoidSymbol}, "Ș"}, // U0218 | LATIN CAPITAL LETTER S WITH COMMA BELOW
  {{XK_Multi_key, XK_semicolon, XK_S, XK_VoidSymbol}, "Ș"},
  {{XK_Multi_key, XK_S, XK_semicolon, XK_VoidSymbol}, "Ș"},
  {{XK_dead_abovedot, XK_Multi_key, XK_f, XK_s, XK_VoidSymbol}, "ẛ"}, // U1e9b | LATIN SMALL LETTER LONG S WITH DOT ABOVE
  {{XK_Multi_key, XK_dead_abovedot, XK_f, XK_s, XK_VoidSymbol}, "ẛ"},
  {{XK_dead_caron, XK_E, XK_VoidSymbol}, "Ě"}, // U011A | LATIN CAPITAL LETTER E WITH CARON
  {{XK_Multi_key, XK_c, XK_E, XK_VoidSymbol}, "Ě"},
  {{XK_Multi_key, XK_less, XK_E, XK_VoidSymbol}, "Ě"},
  {{XK_Multi_key, XK_E, XK_less, XK_VoidSymbol}, "Ě"},
  {{XK_Multi_key, XK_parenleft, XK_kana_SHI, XK_parenright, XK_VoidSymbol}, "㋛"}, // U32DB | CIRCLED KATAKANA SI
  {{XK_dead_macron, XK_nobreakspace, XK_VoidSymbol}, "̄"}, // U0304 | COMBINING MACRON
  {{XK_dead_iota, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾡ"}, // U1FA1 | GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾡ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ᾡ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ᾡ"},
  {{XK_dead_macron, XK_G, XK_VoidSymbol}, "Ḡ"}, // U1E20 | LATIN CAPITAL LETTER G WITH MACRON
  {{XK_Multi_key, XK_macron, XK_G, XK_VoidSymbol}, "Ḡ"},
  {{XK_Multi_key, XK_underscore, XK_G, XK_VoidSymbol}, "Ḡ"},
  {{XK_Multi_key, XK_5, XK_6, XK_VoidSymbol}, "⅚"}, // U215A | VULGAR FRACTION FIVE SIXTHS
  {{XK_dead_hook, XK_p, XK_VoidSymbol}, "ƥ"}, // U01A5 | LATIN SMALL LETTER P WITH HOOK
  {{XK_dead_currency, XK_S, XK_VoidSymbol}, "$"}, // dollar | DOLLAR SIGN
  {{XK_Multi_key, XK_parenleft, XK_kana_SO, XK_parenright, XK_VoidSymbol}, "㋞"}, // U32DE | CIRCLED KATAKANA SO
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἦ"}, // U1F26 | GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἦ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἦ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἦ"},
  {{XK_dead_cedilla, XK_E, XK_VoidSymbol}, "Ȩ"}, // U0228 | LATIN CAPITAL LETTER E WITH CEDILLA
  {{XK_Multi_key, XK_cedilla, XK_E, XK_VoidSymbol}, "Ȩ"},
  {{XK_dead_tilde, XK_acircumflex, XK_VoidSymbol}, "ẫ"}, // U1EAB | LATIN SMALL LETTER A WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_acircumflex, XK_VoidSymbol}, "ẫ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ẫ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_a, XK_VoidSymbol}, "ẫ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_a, XK_VoidSymbol}, "ẫ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_a, XK_VoidSymbol}, "ẫ"},
  {{XK_dead_circumflex, XK_atilde, XK_VoidSymbol}, "ẫ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_a, XK_VoidSymbol}, "ẫ"},
  {{XK_dead_macron, XK_I, XK_VoidSymbol}, "Ī"}, // U012A | LATIN CAPITAL LETTER I WITH MACRON
  {{XK_Multi_key, XK_macron, XK_I, XK_VoidSymbol}, "Ī"},
  {{XK_Multi_key, XK_underscore, XK_I, XK_VoidSymbol}, "Ī"},
  {{XK_Multi_key, XK_I, XK_underscore, XK_VoidSymbol}, "Ī"},
  {{XK_Multi_key, XK_minus, XK_I, XK_VoidSymbol}, "Ī"},
  {{XK_Multi_key, XK_I, XK_minus, XK_VoidSymbol}, "Ī"},
  {{XK_dead_macron, XK_space, XK_VoidSymbol}, "¯"}, // macron | MACRON
  {{XK_dead_macron, XK_dead_macron, XK_VoidSymbol}, "¯"},
  {{XK_Multi_key, XK_minus, XK_asciicircum, XK_VoidSymbol}, "¯"},
  {{XK_Multi_key, XK_asciicircum, XK_minus, XK_VoidSymbol}, "¯"},
  {{XK_Multi_key, XK_underscore, XK_underscore, XK_VoidSymbol}, "¯"},
  {{XK_Multi_key, XK_underscore, XK_asciicircum, XK_VoidSymbol}, "¯"},
  {{XK_dead_macron, XK_Greek_alpha, XK_VoidSymbol}, "ᾱ"}, // U1FB1 | GREEK SMALL LETTER ALPHA WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_alpha, XK_VoidSymbol}, "ᾱ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_alpha, XK_VoidSymbol}, "ᾱ"},
  {{XK_dead_acute, XK_K, XK_VoidSymbol}, "Ḱ"}, // U1E30 | LATIN CAPITAL LETTER K WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_K, XK_VoidSymbol}, "Ḱ"},
  {{XK_Multi_key, XK_apostrophe, XK_K, XK_VoidSymbol}, "Ḱ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_r, XK_VoidSymbol}, "ʳ"}, // U02B3 | MODIFIER LETTER SMALL R
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_r, XK_VoidSymbol}, "ʳ"},
  {{XK_dead_voiced_sound, XK_kana_KE, XK_VoidSymbol}, "ゲ"}, // U30B2 | KATAKANA LETTER GE
  {{XK_dead_stroke, XK_Z, XK_VoidSymbol}, "Ƶ"}, // U01B5 | LATIN CAPITAL LETTER Z WITH STROKE
  {{XK_Multi_key, XK_slash, XK_Z, XK_VoidSymbol}, "Ƶ"},
  {{XK_Multi_key, XK_KP_Divide, XK_Z, XK_VoidSymbol}, "Ƶ"},
  {{XK_Multi_key, XK_parenleft, XK_p, XK_parenright, XK_VoidSymbol}, "ⓟ"}, // U24DF | CIRCLED LATIN SMALL LETTER P
  {{XK_Multi_key, XK_parenleft, XK_B, XK_parenright, XK_VoidSymbol}, "Ⓑ"}, // U24B7 | CIRCLED LATIN CAPITAL LETTER B
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἶ"}, // U1F36 | GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἶ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἶ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἶ"},
  {{XK_dead_hook, XK_e, XK_VoidSymbol}, "ẻ"}, // U1EBB | LATIN SMALL LETTER E WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_e, XK_VoidSymbol}, "ẻ"},
  {{XK_dead_acute, XK_l, XK_VoidSymbol}, "ĺ"}, // U013A | LATIN SMALL LETTER L WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_l, XK_VoidSymbol}, "ĺ"},
  {{XK_Multi_key, XK_apostrophe, XK_l, XK_VoidSymbol}, "ĺ"},
  {{XK_Multi_key, XK_l, XK_apostrophe, XK_VoidSymbol}, "ĺ"},
  {{XK_dead_voiced_sound, XK_kana_TSU, XK_VoidSymbol}, "ヅ"}, // U30C5 | KATAKANA LETTER DU
  {{XK_Multi_key, XK_question, XK_question, XK_VoidSymbol}, "¿"}, // questiondown | INVERTED QUESTION MARK
  {{XK_Multi_key, XK_diaeresis, XK_dead_tilde, XK_VoidSymbol}, "῁"}, // U1FC1 | GREEK DIALYTIKA AND PERISPOMENI
  {{XK_Multi_key, XK_diaeresis, XK_asciitilde, XK_VoidSymbol}, "῁"},
  {{XK_dead_abovedot, XK_M, XK_VoidSymbol}, "Ṁ"}, // U1E40 | LATIN CAPITAL LETTER M WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_M, XK_VoidSymbol}, "Ṁ"},
  {{XK_Multi_key, XK_M, XK_period, XK_VoidSymbol}, "Ṁ"},
  {{XK_Multi_key, XK_parenleft, XK_R, XK_parenright, XK_VoidSymbol}, "Ⓡ"}, // U24C7 | CIRCLED LATIN CAPITAL LETTER R
  {{XK_dead_hook, XK_g, XK_VoidSymbol}, "ɠ"}, // U0260 | LATIN SMALL LETTER G WITH HOOK
  {{XK_dead_stroke, XK_J, XK_VoidSymbol}, "Ɉ"}, // U0248 | LATIN CAPITAL LETTER J WITH STROKE
  {{XK_dead_belowdot, XK_i, XK_VoidSymbol}, "ị"}, // U1ECB | LATIN SMALL LETTER I WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_i, XK_VoidSymbol}, "ị"},
  {{XK_Multi_key, XK_N, XK_G, XK_VoidSymbol}, "Ŋ"}, // U014A | LATIN CAPITAL LETTER ENG
  {{XK_Multi_key, XK_parenleft, XK_kana_TO, XK_parenright, XK_VoidSymbol}, "㋣"}, // U32E3 | CIRCLED KATAKANA TO
  {{XK_dead_diaeresis, XK_I, XK_VoidSymbol}, "Ï"}, // Idiaeresis | LATIN CAPITAL LETTER I WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_I, XK_VoidSymbol}, "Ï"},
  {{XK_Multi_key, XK_I, XK_quotedbl, XK_VoidSymbol}, "Ï"},
  {{XK_Multi_key, XK_diaeresis, XK_I, XK_VoidSymbol}, "Ï"},
  {{XK_Multi_key, XK_I, XK_diaeresis, XK_VoidSymbol}, "Ï"},
  {{XK_dead_macron, XK_Greek_iota, XK_VoidSymbol}, "ῑ"}, // U1FD1 | GREEK SMALL LETTER IOTA WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_iota, XK_VoidSymbol}, "ῑ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_iota, XK_VoidSymbol}, "ῑ"},
  {{XK_dead_macron, XK_dead_belowdot, XK_l, XK_VoidSymbol}, "ḹ"}, // U1E39 | LATIN SMALL LETTER L WITH DOT BELOW AND MACRON
  {{XK_dead_macron, XK_Multi_key, XK_exclam, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_Multi_key, XK_macron, XK_dead_belowdot, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_Multi_key, XK_macron, XK_exclam, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_Multi_key, XK_underscore, XK_dead_belowdot, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_Multi_key, XK_underscore, XK_exclam, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_dead_belowdot, XK_dead_macron, XK_l, XK_VoidSymbol}, "ḹ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_E, XK_parenright, XK_VoidSymbol}, "㋓"}, // U32D3 | CIRCLED KATAKANA E
  {{XK_dead_macron, XK_Udiaeresis, XK_VoidSymbol}, "Ǖ"}, // U01D5 | LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_Udiaeresis, XK_VoidSymbol}, "Ǖ"},
  {{XK_Multi_key, XK_underscore, XK_Udiaeresis, XK_VoidSymbol}, "Ǖ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_U, XK_VoidSymbol}, "Ǖ"},
  {{XK_dead_macron, XK_V, XK_VoidSymbol}, "Ǖ"},
  {{XK_dead_breve, XK_Cyrillic_ie, XK_VoidSymbol}, "ӗ"}, // U04D7 | CYRILLIC SMALL LETTER IE WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_ie, XK_VoidSymbol}, "ӗ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_ie, XK_VoidSymbol}, "ӗ"},
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὖ"}, // U1F56 | GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὖ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_upsilon, XK_VoidSymbol}, "ὖ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_upsilon, XK_VoidSymbol}, "ὖ"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_8, XK_parenright, XK_VoidSymbol}, "㉘"}, // U3258 | CIRCLED NUMBER TWENTY EIGHT
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㉘"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_8, XK_parenright, XK_VoidSymbol}, "㉘"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㉘"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_8, XK_parenright, XK_VoidSymbol}, "㉘"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_8, XK_parenright, XK_VoidSymbol}, "㉘"},
  {{XK_dead_acute, XK_ohorn, XK_VoidSymbol}, "ớ"}, // U1EDB | LATIN SMALL LETTER O WITH HORN AND ACUTE
  {{XK_Multi_key, XK_acute, XK_ohorn, XK_VoidSymbol}, "ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_ohorn, XK_VoidSymbol}, "ớ"},
  {{XK_dead_acute, XK_dead_horn, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_dead_acute, XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_Multi_key, XK_acute, XK_dead_horn, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_Multi_key, XK_acute, XK_plus, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_horn, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_Multi_key, XK_apostrophe, XK_plus, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_dead_horn, XK_oacute, XK_VoidSymbol}, "ớ"},
  {{XK_dead_horn, XK_dead_acute, XK_o, XK_VoidSymbol}, "ớ"},
  {{XK_dead_acute, XK_S, XK_VoidSymbol}, "Ś"}, // U015A | LATIN CAPITAL LETTER S WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_S, XK_VoidSymbol}, "Ś"},
  {{XK_Multi_key, XK_apostrophe, XK_S, XK_VoidSymbol}, "Ś"},
  {{XK_Multi_key, XK_S, XK_apostrophe, XK_VoidSymbol}, "Ś"},
  {{XK_dead_acute, XK_Cyrillic_ka, XK_VoidSymbol}, "ќ"}, // U045C | CYRILLIC SMALL LETTER KJE
  {{XK_Multi_key, XK_acute, XK_Cyrillic_ka, XK_VoidSymbol}, "ќ"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_ka, XK_VoidSymbol}, "ќ"},
  {{XK_Multi_key, XK_s, XK_s, XK_VoidSymbol}, "ß"}, // ssharp | LATIN SMALL LETTER SHARP S
  {{XK_dead_macron, XK_Greek_upsilon, XK_VoidSymbol}, "ῡ"}, // U1FE1 | GREEK SMALL LETTER UPSILON WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Greek_upsilon, XK_VoidSymbol}, "ῡ"},
  {{XK_Multi_key, XK_underscore, XK_Greek_upsilon, XK_VoidSymbol}, "ῡ"},
  {{XK_dead_abovedot, XK_S, XK_VoidSymbol}, "Ṡ"}, // U1E60 | LATIN CAPITAL LETTER S WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_S, XK_VoidSymbol}, "Ṡ"},
  {{XK_Multi_key, XK_S, XK_period, XK_VoidSymbol}, "Ṡ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_x, XK_VoidSymbol}, "ˣ"}, // U02E3 | MODIFIER LETTER SMALL X
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_x, XK_VoidSymbol}, "ˣ"},
  {{XK_dead_voiced_sound, XK_kana_SU, XK_VoidSymbol}, "ズ"}, // U30BA | KATAKANA LETTER ZU
  {{XK_dead_stroke, XK_g, XK_VoidSymbol}, "ǥ"}, // U01E5 | LATIN SMALL LETTER G WITH STROKE
  {{XK_Multi_key, XK_slash, XK_g, XK_VoidSymbol}, "ǥ"},
  {{XK_Multi_key, XK_KP_Divide, XK_g, XK_VoidSymbol}, "ǥ"},
  {{XK_Multi_key, XK_parenleft, XK_x, XK_parenright, XK_VoidSymbol}, "ⓧ"}, // U24E7 | CIRCLED LATIN SMALL LETTER X
  {{XK_dead_diaeresis, XK_Cyrillic_o, XK_VoidSymbol}, "ӧ"}, // U04E7 | CYRILLIC SMALL LETTER O WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_o, XK_VoidSymbol}, "ӧ"},
  {{XK_dead_tilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὦ"}, // U1F66 | GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI
  {{XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὦ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὦ"},
  {{XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὦ"},
  {{XK_dead_stroke, XK_i, XK_VoidSymbol}, "ɨ"}, // U0268 | LATIN SMALL LETTER I WITH STROKE
  {{XK_Multi_key, XK_slash, XK_i, XK_VoidSymbol}, "ɨ"},
  {{XK_Multi_key, XK_KP_Divide, XK_i, XK_VoidSymbol}, "ɨ"},
  {{XK_dead_grave, XK_uhorn, XK_VoidSymbol}, "ừ"}, // U1EEB | LATIN SMALL LETTER U WITH HORN AND GRAVE
  {{XK_Multi_key, XK_grave, XK_uhorn, XK_VoidSymbol}, "ừ"},
  {{XK_dead_grave, XK_dead_horn, XK_u, XK_VoidSymbol}, "ừ"},
  {{XK_dead_grave, XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ừ"},
  {{XK_Multi_key, XK_grave, XK_dead_horn, XK_u, XK_VoidSymbol}, "ừ"},
  {{XK_Multi_key, XK_grave, XK_plus, XK_u, XK_VoidSymbol}, "ừ"},
  {{XK_dead_horn, XK_ugrave, XK_VoidSymbol}, "ừ"},
  {{XK_dead_horn, XK_dead_grave, XK_u, XK_VoidSymbol}, "ừ"},
  {{XK_dead_macron, XK_U, XK_VoidSymbol}, "Ū"}, // U016A | LATIN CAPITAL LETTER U WITH MACRON
  {{XK_Multi_key, XK_macron, XK_U, XK_VoidSymbol}, "Ū"},
  {{XK_Multi_key, XK_underscore, XK_U, XK_VoidSymbol}, "Ū"},
  {{XK_Multi_key, XK_U, XK_underscore, XK_VoidSymbol}, "Ū"},
  {{XK_Multi_key, XK_minus, XK_U, XK_VoidSymbol}, "Ū"},
  {{XK_Multi_key, XK_U, XK_minus, XK_VoidSymbol}, "Ū"},
  {{XK_dead_circumflex, XK_Multi_key, XK_S, XK_M, XK_VoidSymbol}, "℠"}, // U2120 | SERVICE MARK
  {{XK_Multi_key, XK_S, XK_M, XK_VoidSymbol}, "℠"},
  {{XK_dead_circumflex, XK_Multi_key, XK_s, XK_M, XK_VoidSymbol}, "℠"},
  {{XK_Multi_key, XK_s, XK_M, XK_VoidSymbol}, "℠"},
  {{XK_dead_circumflex, XK_Multi_key, XK_S, XK_m, XK_VoidSymbol}, "℠"},
  {{XK_Multi_key, XK_S, XK_m, XK_VoidSymbol}, "℠"},
  {{XK_dead_circumflex, XK_Multi_key, XK_s, XK_m, XK_VoidSymbol}, "℠"},
  {{XK_Multi_key, XK_s, XK_m, XK_VoidSymbol}, "℠"},
  {{XK_dead_diaeresis, XK_i, XK_VoidSymbol}, "ï"}, // idiaeresis | LATIN SMALL LETTER I WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_i, XK_VoidSymbol}, "ï"},
  {{XK_Multi_key, XK_i, XK_quotedbl, XK_VoidSymbol}, "ï"},
  {{XK_Multi_key, XK_diaeresis, XK_i, XK_VoidSymbol}, "ï"},
  {{XK_Multi_key, XK_i, XK_diaeresis, XK_VoidSymbol}, "ï"},
  {{XK_Multi_key, XK_semicolon, XK_underscore, XK_VoidSymbol}, "⍮"}, // U236e | ; _ APL FUNCTIONAL SYMBOL SEMICOLON UNDERBAR
  {{XK_dead_belowcircumflex, XK_T, XK_VoidSymbol}, "Ṱ"}, // U1E70 | LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
  {{XK_Multi_key, XK_parenleft, XK_kana_YA, XK_parenright, XK_VoidSymbol}, "㋳"}, // U32F3 | CIRCLED KATAKANA YA
  {{XK_Multi_key, XK_parenleft, XK_u, XK_parenright, XK_VoidSymbol}, "ⓤ"}, // U24E4 | CIRCLED LATIN SMALL LETTER U
  {{XK_dead_acute, XK_g, XK_VoidSymbol}, "ǵ"}, // U01F5 | LATIN SMALL LETTER G WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_g, XK_VoidSymbol}, "ǵ"},
  {{XK_Multi_key, XK_apostrophe, XK_g, XK_VoidSymbol}, "ǵ"},
  {{XK_dead_circumflex, XK_4, XK_VoidSymbol}, "⁴"}, // U2074 | SUPERSCRIPT FOUR
  {{XK_Multi_key, XK_asciicircum, XK_4, XK_VoidSymbol}, "⁴"},
  {{XK_dead_circumflex, XK_KP_4, XK_VoidSymbol}, "⁴"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_4, XK_VoidSymbol}, "⁴"},
  {{XK_dead_grave, XK_Greek_iota, XK_VoidSymbol}, "ὶ"}, // U1F76 | GREEK SMALL LETTER IOTA WITH VARIA
  {{XK_Multi_key, XK_grave, XK_Greek_iota, XK_VoidSymbol}, "ὶ"},
  {{XK_dead_currency, XK_o, XK_VoidSymbol}, "௹"}, // U0BF9 | TAMIL RUPEE SIGN
  {{XK_dead_acute, XK_z, XK_VoidSymbol}, "ź"}, // U017A | LATIN SMALL LETTER Z WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_z, XK_VoidSymbol}, "ź"},
  {{XK_Multi_key, XK_apostrophe, XK_z, XK_VoidSymbol}, "ź"},
  {{XK_Multi_key, XK_z, XK_apostrophe, XK_VoidSymbol}, "ź"},
  {{XK_dead_diaeresis, XK_y, XK_VoidSymbol}, "ÿ"}, // ydiaeresis | LATIN SMALL LETTER Y WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_y, XK_VoidSymbol}, "ÿ"},
  {{XK_Multi_key, XK_y, XK_quotedbl, XK_VoidSymbol}, "ÿ"},
  {{XK_Multi_key, XK_diaeresis, XK_y, XK_VoidSymbol}, "ÿ"},
  {{XK_Multi_key, XK_y, XK_diaeresis, XK_VoidSymbol}, "ÿ"},
  {{XK_dead_dasia, XK_Greek_alpha, XK_VoidSymbol}, "ἁ"}, // U1F01 | GREEK SMALL LETTER ALPHA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_alpha, XK_VoidSymbol}, "ἁ"},
  {{XK_dead_grave, XK_W, XK_VoidSymbol}, "Ẁ"}, // U1E80 | LATIN CAPITAL LETTER W WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_W, XK_VoidSymbol}, "Ẁ"},
  {{XK_dead_invertedbreve, XK_a, XK_VoidSymbol}, "ȃ"}, // U0203 | LATIN SMALL LETTER A WITH INVERTED BREVE
  {{XK_dead_ogonek, XK_a, XK_VoidSymbol}, "ą"}, // U0105 | LATIN SMALL LETTER A WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_a, XK_VoidSymbol}, "ą"},
  {{XK_Multi_key, XK_a, XK_semicolon, XK_VoidSymbol}, "ą"},
  {{XK_Multi_key, XK_comma, XK_a, XK_VoidSymbol}, "ą"},
  {{XK_Multi_key, XK_a, XK_comma, XK_VoidSymbol}, "ą"},
  {{XK_Multi_key, XK_underscore, XK_4, XK_VoidSymbol}, "₄"}, // U2084 | SUBSCRIPT FOUR
  {{XK_Multi_key, XK_underscore, XK_KP_4, XK_VoidSymbol}, "₄"},
  {{XK_dead_caron, XK_4, XK_VoidSymbol}, "₄"},
  {{XK_dead_diaeresis, XK_Ukrainian_I, XK_VoidSymbol}, "Ї"}, // U0407 | CYRILLIC CAPITAL LETTER YI
  {{XK_Multi_key, XK_quotedbl, XK_Ukrainian_I, XK_VoidSymbol}, "Ї"},
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"}, // U1F86 | GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾆ"},
  {{XK_dead_hook, XK_dead_hook, XK_VoidSymbol}, "̉"}, // U0309 | COMBINING HOOK ABOVE
  {{XK_dead_hook, XK_nobreakspace, XK_VoidSymbol}, "̉"},
  {{XK_dead_hook, XK_space, XK_VoidSymbol}, "̉"},
  {{XK_dead_doublegrave, XK_Cyrillic_i, XK_VoidSymbol}, "и̏"}, // CYRILLIC SMALL LETTER I WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_i, XK_VoidSymbol}, "и̏"},
  {{XK_dead_abovedot, XK_d, XK_VoidSymbol}, "ḋ"}, // U1E0B | LATIN SMALL LETTER D WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_d, XK_VoidSymbol}, "ḋ"},
  {{XK_Multi_key, XK_d, XK_period, XK_VoidSymbol}, "ḋ"},
  {{XK_dead_hook, XK_D, XK_VoidSymbol}, "Ɗ"}, // U018A | LATIN CAPITAL LETTER D WITH HOOK
  {{XK_dead_acute, XK_Greek_UPSILON, XK_VoidSymbol}, "Ύ"}, // U038E | GREEK CAPITAL LETTER UPSILON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_UPSILON, XK_VoidSymbol}, "Ύ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_UPSILON, XK_VoidSymbol}, "Ύ"},
  {{XK_Multi_key, XK_Greek_UPSILON, XK_apostrophe, XK_VoidSymbol}, "Ύ"},
  {{XK_dead_dasia, XK_Greek_epsilon, XK_VoidSymbol}, "ἑ"}, // U1F11 | GREEK SMALL LETTER EPSILON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_epsilon, XK_VoidSymbol}, "ἑ"},
  {{XK_dead_circumflex, XK_Z, XK_VoidSymbol}, "Ẑ"}, // U1E90 | LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_Z, XK_VoidSymbol}, "Ẑ"},
  {{XK_dead_invertedbreve, XK_r, XK_VoidSymbol}, "ȓ"}, // U0213 | LATIN SMALL LETTER R WITH INVERTED BREVE
  {{XK_Multi_key, XK_parenleft, XK_M, XK_parenright, XK_VoidSymbol}, "Ⓜ"}, // U24C2 | CIRCLED LATIN CAPITAL LETTER M
  {{XK_dead_breve, XK_e, XK_VoidSymbol}, "ĕ"}, // U0115 | LATIN SMALL LETTER E WITH BREVE
  {{XK_Multi_key, XK_U, XK_e, XK_VoidSymbol}, "ĕ"},
  {{XK_Multi_key, XK_b, XK_e, XK_VoidSymbol}, "ĕ"},
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"}, // U1F96 | GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾖ"},
  {{XK_dead_belowtilde, XK_e, XK_VoidSymbol}, "ḛ"}, // U1E1B | LATIN SMALL LETTER E WITH TILDE BELOW
  {{XK_dead_dasia, XK_Greek_eta, XK_VoidSymbol}, "ἡ"}, // U1F21 | GREEK SMALL LETTER ETA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_eta, XK_VoidSymbol}, "ἡ"},
  {{XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ạ"}, // U1EA0 | LATIN CAPITAL LETTER A WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_A, XK_VoidSymbol}, "Ạ"},
  {{XK_Multi_key, XK_v, XK_slash, XK_VoidSymbol}, "√"}, // U221a | v / SQUARE ROOT
  {{XK_Multi_key, XK_slash, XK_v, XK_VoidSymbol}, "√"},
  {{XK_dead_circumflex, XK_h, XK_VoidSymbol}, "ĥ"}, // U0125 | LATIN SMALL LETTER H WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_h, XK_VoidSymbol}, "ĥ"},
  {{XK_Multi_key, XK_L, XK_equal, XK_VoidSymbol}, "₤"}, // U20a4 | LIRA SIGN
  {{XK_Multi_key, XK_equal, XK_L, XK_VoidSymbol}, "₤"},
  {{XK_dead_currency, XK_L, XK_VoidSymbol}, "₤"},
  {{XK_Multi_key, XK_parenleft, XK_m, XK_parenright, XK_VoidSymbol}, "ⓜ"}, // U24DC | CIRCLED LATIN SMALL LETTER M
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"}, // U1FA6 | GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾦ"},
  {{XK_dead_belowbreve, XK_h, XK_VoidSymbol}, "ḫ"}, // U1E2B | LATIN SMALL LETTER H WITH BREVE BELOW
  {{XK_dead_semivoiced_sound, XK_kana_FU, XK_VoidSymbol}, "プ"}, // U30D7 | KATAKANA LETTER PU
  {{XK_dead_stroke, XK_dead_stroke, XK_VoidSymbol}, "/"}, // slash | SOLIDUS
  {{XK_dead_stroke, XK_space, XK_VoidSymbol}, "/"},
  {{XK_dead_acute, XK_Greek_eta, XK_VoidSymbol}, "ή"}, // U03AE | GREEK SMALL LETTER ETA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_eta, XK_VoidSymbol}, "ή"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_eta, XK_VoidSymbol}, "ή"},
  {{XK_Multi_key, XK_Greek_eta, XK_apostrophe, XK_VoidSymbol}, "ή"},
  {{XK_dead_dasia, XK_Greek_iota, XK_VoidSymbol}, "ἱ"}, // U1F31 | GREEK SMALL LETTER IOTA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_iota, XK_VoidSymbol}, "ἱ"},
  {{XK_dead_grave, XK_Abreve, XK_VoidSymbol}, "Ằ"}, // U1EB0 | LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Abreve, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_grave, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_grave, XK_Multi_key, XK_U, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_grave, XK_Multi_key, XK_b, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_Multi_key, XK_grave, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_Multi_key, XK_grave, XK_b, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_breve, XK_Agrave, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_breve, XK_dead_grave, XK_A, XK_VoidSymbol}, "Ằ"},
  {{XK_dead_macron, XK_y, XK_VoidSymbol}, "ȳ"}, // U0233 | LATIN SMALL LETTER Y WITH MACRON
  {{XK_Multi_key, XK_macron, XK_y, XK_VoidSymbol}, "ȳ"},
  {{XK_Multi_key, XK_underscore, XK_y, XK_VoidSymbol}, "ȳ"},
  {{XK_dead_circumflex, XK_j, XK_VoidSymbol}, "ĵ"}, // U0135 | LATIN SMALL LETTER J WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_j, XK_VoidSymbol}, "ĵ"},
  {{XK_dead_acute, XK_dead_acute, XK_VoidSymbol}, "´"}, // acute | ACUTE ACCENT
  {{XK_Multi_key, XK_apostrophe, XK_apostrophe, XK_VoidSymbol}, "´"},
  {{XK_dead_tilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾶ"}, // U1FB6 | GREEK SMALL LETTER ALPHA WITH PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_alpha, XK_VoidSymbol}, "ᾶ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_y, XK_VoidSymbol}, "ʸ"}, // U02B8 | MODIFIER LETTER SMALL Y
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_y, XK_VoidSymbol}, "ʸ"},
  {{XK_dead_belowmacron, XK_l, XK_VoidSymbol}, "ḻ"}, // U1E3B | LATIN SMALL LETTER L WITH LINE BELOW
  {{XK_dead_invertedbreve, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̑"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING INVERTED BREVE
  {{XK_Multi_key, XK_parenleft, XK_G, XK_parenright, XK_VoidSymbol}, "Ⓖ"}, // U24BC | CIRCLED LATIN CAPITAL LETTER G
  {{XK_dead_dasia, XK_Greek_omicron, XK_VoidSymbol}, "ὁ"}, // U1F41 | GREEK SMALL LETTER OMICRON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_omicron, XK_VoidSymbol}, "ὁ"},
  {{XK_Multi_key, XK_exclam, XK_exclam, XK_VoidSymbol}, "¡"}, // exclamdown | INVERTED EXCLAMATION MARK
  {{XK_dead_stroke, XK_B, XK_VoidSymbol}, "Ƀ"}, // U0243 | LATIN CAPITAL LETTER B WITH STROKE
  {{XK_dead_cedilla, XK_N, XK_VoidSymbol}, "Ņ"}, // U0145 | LATIN CAPITAL LETTER N WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_N, XK_VoidSymbol}, "Ņ"},
  {{XK_Multi_key, XK_N, XK_comma, XK_VoidSymbol}, "Ņ"},
  {{XK_Multi_key, XK_cedilla, XK_N, XK_VoidSymbol}, "Ņ"},
  {{XK_dead_diaeresis, XK_A, XK_VoidSymbol}, "Ä"}, // Adiaeresis | LATIN CAPITAL LETTER A WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_A, XK_VoidSymbol}, "Ä"},
  {{XK_Multi_key, XK_A, XK_quotedbl, XK_VoidSymbol}, "Ä"},
  {{XK_Multi_key, XK_diaeresis, XK_A, XK_VoidSymbol}, "Ä"},
  {{XK_Multi_key, XK_A, XK_diaeresis, XK_VoidSymbol}, "Ä"},
  {{XK_dead_tilde, XK_Greek_eta, XK_VoidSymbol}, "ῆ"}, // U1FC6 | GREEK SMALL LETTER ETA WITH PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_eta, XK_VoidSymbol}, "ῆ"},
  {{XK_dead_belowcircumflex, XK_n, XK_VoidSymbol}, "ṋ"}, // U1E4B | LATIN SMALL LETTER N WITH CIRCUMFLEX BELOW
  {{XK_dead_abovedot, XK_dead_stroke, XK_j, XK_VoidSymbol}, "ɟ"}, // U025F | LATIN SMALL LETTER DOTLESS J WITH STROKE
  {{XK_dead_stroke, XK_dead_abovedot, XK_j, XK_VoidSymbol}, "ɟ"},
  {{XK_Multi_key, XK_parenleft, XK_W, XK_parenright, XK_VoidSymbol}, "Ⓦ"}, // U24CC | CIRCLED LATIN CAPITAL LETTER W
  {{XK_dead_currency, XK_H, XK_VoidSymbol}, "₴"}, // U20B4 | HRYVNIA SIGN
  {{XK_dead_currency, XK_h, XK_VoidSymbol}, "₴"},
  {{XK_dead_acute, XK_Greek_omega, XK_VoidSymbol}, "ώ"}, // U03CE | GREEK SMALL LETTER OMEGA WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_omega, XK_VoidSymbol}, "ώ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_omega, XK_VoidSymbol}, "ώ"},
  {{XK_Multi_key, XK_Greek_omega, XK_apostrophe, XK_VoidSymbol}, "ώ"},
  {{XK_dead_dasia, XK_Greek_upsilon, XK_VoidSymbol}, "ὑ"}, // U1F51 | GREEK SMALL LETTER UPSILON WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_upsilon, XK_VoidSymbol}, "ὑ"},
  {{XK_dead_acute, XK_Ocircumflex, XK_VoidSymbol}, "Ố"}, // U1ED0 | LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Ocircumflex, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_apostrophe, XK_Ocircumflex, XK_VoidSymbol}, "Ố"},
  {{XK_dead_acute, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_dead_circumflex, XK_Oacute, XK_VoidSymbol}, "Ố"},
  {{XK_dead_circumflex, XK_dead_acute, XK_O, XK_VoidSymbol}, "Ố"},
  {{XK_Multi_key, XK_parenleft, XK_2, XK_3, XK_parenright, XK_VoidSymbol}, "㉓"}, // U3253 | CIRCLED NUMBER TWENTY THREE
  {{XK_Multi_key, XK_parenleft, XK_2, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㉓"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_3, XK_parenright, XK_VoidSymbol}, "㉓"},
  {{XK_Multi_key, XK_parenleft, XK_KP_Space, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㉓"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_3, XK_parenright, XK_VoidSymbol}, "㉓"},
  {{XK_Multi_key, XK_parenleft, XK_KP_2, XK_KP_3, XK_parenright, XK_VoidSymbol}, "㉓"},
  {{XK_dead_acute, XK_r, XK_VoidSymbol}, "ŕ"}, // U0155 | LATIN SMALL LETTER R WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_r, XK_VoidSymbol}, "ŕ"},
  {{XK_Multi_key, XK_apostrophe, XK_r, XK_VoidSymbol}, "ŕ"},
  {{XK_Multi_key, XK_r, XK_apostrophe, XK_VoidSymbol}, "ŕ"},
  {{XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ô"}, // Ocircumflex | LATIN CAPITAL LETTER O WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ô"},
  {{XK_Multi_key, XK_O, XK_asciicircum, XK_VoidSymbol}, "Ô"},
  {{XK_Multi_key, XK_greater, XK_O, XK_VoidSymbol}, "Ô"},
  {{XK_Multi_key, XK_O, XK_greater, XK_VoidSymbol}, "Ô"},
  {{XK_dead_diaeresis, XK_Ukrainian_i, XK_VoidSymbol}, "ї"}, // U0457 | CYRILLIC SMALL LETTER YI
  {{XK_Multi_key, XK_quotedbl, XK_Ukrainian_i, XK_VoidSymbol}, "ї"},
  {{XK_dead_tilde, XK_Greek_iota, XK_VoidSymbol}, "ῖ"}, // U1FD6 | GREEK SMALL LETTER IOTA WITH PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_iota, XK_VoidSymbol}, "ῖ"},
  {{XK_dead_breve, XK_space, XK_VoidSymbol}, "˘"}, // breve | BREVE
  {{XK_dead_breve, XK_dead_breve, XK_VoidSymbol}, "˘"},
  {{XK_Multi_key, XK_space, XK_parenleft, XK_VoidSymbol}, "˘"},
  {{XK_Multi_key, XK_parenleft, XK_space, XK_VoidSymbol}, "˘"},
  {{XK_dead_belowdot, XK_r, XK_VoidSymbol}, "ṛ"}, // U1E5B | LATIN SMALL LETTER R WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_r, XK_VoidSymbol}, "ṛ"},
  {{XK_dead_caron, XK_udiaeresis, XK_VoidSymbol}, "ǚ"}, // U01DA | LATIN SMALL LETTER U WITH DIAERESIS AND CARON
  {{XK_Multi_key, XK_c, XK_udiaeresis, XK_VoidSymbol}, "ǚ"},
  {{XK_dead_caron, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǚ"},
  {{XK_dead_caron, XK_Multi_key, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǚ"},
  {{XK_Multi_key, XK_c, XK_dead_diaeresis, XK_u, XK_VoidSymbol}, "ǚ"},
  {{XK_Multi_key, XK_c, XK_quotedbl, XK_u, XK_VoidSymbol}, "ǚ"},
  {{XK_dead_caron, XK_v, XK_VoidSymbol}, "ǚ"},
  {{XK_dead_diaeresis, XK_dead_caron, XK_u, XK_VoidSymbol}, "ǚ"},
  {{XK_dead_diaeresis, XK_Cyrillic_ZHE, XK_VoidSymbol}, "Ӝ"}, // U04DC | CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_ZHE, XK_VoidSymbol}, "Ӝ"},
  {{XK_dead_acute, XK_Cyrillic_er, XK_VoidSymbol}, "р́"}, // CYRILLIC SMALL LETTER ER WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_er, XK_VoidSymbol}, "р́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_er, XK_VoidSymbol}, "р́"},
  {{XK_dead_dasia, XK_Greek_omega, XK_VoidSymbol}, "ὡ"}, // U1F61 | GREEK SMALL LETTER OMEGA WITH DASIA
  {{XK_Multi_key, XK_parenleft, XK_Greek_omega, XK_VoidSymbol}, "ὡ"},
  {{XK_dead_tilde, XK_Ohorn, XK_VoidSymbol}, "Ỡ"}, // U1EE0 | LATIN CAPITAL LETTER O WITH HORN AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Ohorn, XK_VoidSymbol}, "Ỡ"},
  {{XK_dead_tilde, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ỡ"},
  {{XK_dead_tilde, XK_Multi_key, XK_plus, XK_O, XK_VoidSymbol}, "Ỡ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_horn, XK_O, XK_VoidSymbol}, "Ỡ"},
  {{XK_Multi_key, XK_asciitilde, XK_plus, XK_O, XK_VoidSymbol}, "Ỡ"},
  {{XK_dead_horn, XK_Otilde, XK_VoidSymbol}, "Ỡ"},
  {{XK_dead_horn, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ỡ"},
  {{XK_dead_caron, XK_t, XK_VoidSymbol}, "ť"}, // U0165 | LATIN SMALL LETTER T WITH CARON
  {{XK_Multi_key, XK_c, XK_t, XK_VoidSymbol}, "ť"},
  {{XK_Multi_key, XK_less, XK_t, XK_VoidSymbol}, "ť"},
  {{XK_Multi_key, XK_t, XK_less, XK_VoidSymbol}, "ť"},
  {{XK_dead_diaeresis, XK_a, XK_VoidSymbol}, "ä"}, // adiaeresis | LATIN SMALL LETTER A WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_a, XK_VoidSymbol}, "ä"},
  {{XK_Multi_key, XK_a, XK_quotedbl, XK_VoidSymbol}, "ä"},
  {{XK_Multi_key, XK_diaeresis, XK_a, XK_VoidSymbol}, "ä"},
  {{XK_Multi_key, XK_a, XK_diaeresis, XK_VoidSymbol}, "ä"},
  {{XK_Multi_key, XK_parenleft, XK_8, XK_parenright, XK_VoidSymbol}, "⑧"}, // U2467 | CIRCLED DIGIT EIGHT
  {{XK_Multi_key, XK_parenleft, XK_KP_8, XK_parenright, XK_VoidSymbol}, "⑧"},
  {{XK_dead_tilde, XK_Greek_upsilon, XK_VoidSymbol}, "ῦ"}, // U1FE6 | GREEK SMALL LETTER UPSILON WITH PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_upsilon, XK_VoidSymbol}, "ῦ"},
  {{XK_Multi_key, XK_diaeresis, XK_greater, XK_VoidSymbol}, "⍩"}, // U2369 | ¨ > APL FUNCTIONAL SYMBOL GREATER-THAN DIAERESIS
  {{XK_Multi_key, XK_greater, XK_diaeresis, XK_VoidSymbol}, "⍩"},
  {{XK_Multi_key, XK_parenleft, XK_kana_NO, XK_parenright, XK_VoidSymbol}, "㋨"}, // U32E8 | CIRCLED KATAKANA NO
  {{XK_Multi_key, XK_numbersign, XK_E, XK_VoidSymbol}, "♫"}, // U266b | BEAMED EIGHTH NOTES
  {{XK_dead_ogonek, XK_O, XK_VoidSymbol}, "Ǫ"}, // U01EA | LATIN CAPITAL LETTER O WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_O, XK_VoidSymbol}, "Ǫ"},
  {{XK_Multi_key, XK_O, XK_semicolon, XK_VoidSymbol}, "Ǫ"},
  {{XK_Multi_key, XK_comma, XK_O, XK_VoidSymbol}, "Ǫ"},
  {{XK_Multi_key, XK_O, XK_comma, XK_VoidSymbol}, "Ǫ"},
  {{XK_dead_diaeresis, XK_Cyrillic_E, XK_VoidSymbol}, "Ӭ"}, // U04EC | CYRILLIC CAPITAL LETTER E WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_E, XK_VoidSymbol}, "Ӭ"},
  {{XK_dead_circumflex, XK_equal, XK_VoidSymbol}, "⁼"}, // U207C | SUPERSCRIPT EQUALS SIGN
  {{XK_Multi_key, XK_asciicircum, XK_equal, XK_VoidSymbol}, "⁼"},
  {{XK_dead_circumflex, XK_KP_Equal, XK_VoidSymbol}, "⁼"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_Equal, XK_VoidSymbol}, "⁼"},
  {{XK_Multi_key, XK_o, XK_c, XK_VoidSymbol}, "©"}, // copyright | COPYRIGHT SIGN
  {{XK_Multi_key, XK_o, XK_C, XK_VoidSymbol}, "©"},
  {{XK_Multi_key, XK_O, XK_c, XK_VoidSymbol}, "©"},
  {{XK_Multi_key, XK_O, XK_C, XK_VoidSymbol}, "©"},
  {{XK_Multi_key, XK_C, XK_o, XK_VoidSymbol}, "©"},
  {{XK_Multi_key, XK_C, XK_O, XK_VoidSymbol}, "©"},
  {{XK_dead_tilde, XK_greater, XK_VoidSymbol}, "≳"}, // U2273 | GREATER-THAN OR EQUIVALENT TO
  {{XK_Multi_key, XK_parenleft, XK_c, XK_parenright, XK_VoidSymbol}, "ⓒ"}, // U24D2 | CIRCLED LATIN SMALL LETTER C
  {{XK_dead_circumflex, XK_w, XK_VoidSymbol}, "ŵ"}, // U0175 | LATIN SMALL LETTER W WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_w, XK_VoidSymbol}, "ŵ"},
  {{XK_Multi_key, XK_w, XK_asciicircum, XK_VoidSymbol}, "ŵ"},
  {{XK_Multi_key, XK_u, XK_b, XK_u, XK_n, XK_t, XK_u, XK_VoidSymbol}, ""}, // UBUNTU CIRCLE
  {{XK_Multi_key, XK_U, XK_B, XK_U, XK_N, XK_T, XK_U, XK_VoidSymbol}, ""},
  {{XK_dead_tilde, XK_Greek_omega, XK_VoidSymbol}, "ῶ"}, // U1FF6 | GREEK SMALL LETTER OMEGA WITH PERISPOMENI
  {{XK_Multi_key, XK_asciitilde, XK_Greek_omega, XK_VoidSymbol}, "ῶ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_RU, XK_parenright, XK_VoidSymbol}, "㋸"}, // U32F8 | CIRCLED KATAKANA RU
  {{XK_dead_diaeresis, XK_umacron, XK_VoidSymbol}, "ṻ"}, // U1E7B | LATIN SMALL LETTER U WITH MACRON AND DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_umacron, XK_VoidSymbol}, "ṻ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_macron, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_dead_diaeresis, XK_Multi_key, XK_underscore, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_dead_macron, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_macron, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_Multi_key, XK_quotedbl, XK_underscore, XK_u, XK_VoidSymbol}, "ṻ"},
  {{XK_dead_acute, XK_Aring, XK_VoidSymbol}, "Ǻ"}, // U01FA | LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Aring, XK_VoidSymbol}, "Ǻ"},
  {{XK_Multi_key, XK_apostrophe, XK_Aring, XK_VoidSymbol}, "Ǻ"},
  {{XK_dead_acute, XK_dead_abovering, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_dead_acute, XK_Multi_key, XK_o, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_Multi_key, XK_acute, XK_dead_abovering, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_abovering, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_Multi_key, XK_asterisk, XK_apostrophe, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_dead_abovering, XK_Aacute, XK_VoidSymbol}, "Ǻ"},
  {{XK_dead_abovering, XK_dead_acute, XK_A, XK_VoidSymbol}, "Ǻ"},
  {{XK_dead_stroke, XK_p, XK_VoidSymbol}, "ᵽ"}, // U1D7D | LATIN SMALL LETTER P WITH STROKE
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_n, XK_VoidSymbol}, "ⁿ"}, // U207F | SUPERSCRIPT LATIN SMALL LETTER N
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_n, XK_VoidSymbol}, "ⁿ"},
  {{XK_dead_grave, XK_w, XK_VoidSymbol}, "ẁ"}, // U1E81 | LATIN SMALL LETTER W WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_w, XK_VoidSymbol}, "ẁ"},
  {{XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ἀ"}, // U1F00 | GREEK SMALL LETTER ALPHA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ἀ"},
  {{XK_dead_belowdot, XK_minus, XK_VoidSymbol}, "⨪"}, // U2A2A | MINUS SIGN WITH DOT BELOW
  {{XK_Multi_key, XK_underscore, XK_5, XK_VoidSymbol}, "₅"}, // U2085 | SUBSCRIPT FIVE
  {{XK_Multi_key, XK_underscore, XK_KP_5, XK_VoidSymbol}, "₅"},
  {{XK_dead_caron, XK_5, XK_VoidSymbol}, "₅"},
  {{XK_dead_ogonek, XK_A, XK_VoidSymbol}, "Ą"}, // U0104 | LATIN CAPITAL LETTER A WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_A, XK_VoidSymbol}, "Ą"},
  {{XK_Multi_key, XK_A, XK_semicolon, XK_VoidSymbol}, "Ą"},
  {{XK_Multi_key, XK_comma, XK_A, XK_VoidSymbol}, "Ą"},
  {{XK_Multi_key, XK_A, XK_comma, XK_VoidSymbol}, "Ą"},
  {{XK_dead_belowmacron, XK_B, XK_VoidSymbol}, "Ḇ"}, // U1E06 | LATIN CAPITAL LETTER B WITH LINE BELOW
  {{XK_dead_stroke, XK_u, XK_VoidSymbol}, "ʉ"}, // U0289 | LATIN SMALL LETTER U BAR
  {{XK_dead_diaeresis, XK_nobreakspace, XK_VoidSymbol}, "̈"}, // U0308 | COMBINING DIAERESIS
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"}, // U1F8B | GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾋ"},
  {{XK_dead_invertedbreve, XK_O, XK_VoidSymbol}, "Ȏ"}, // U020E | LATIN CAPITAL LETTER O WITH INVERTED BREVE
  {{XK_dead_circumflex, XK_z, XK_VoidSymbol}, "ẑ"}, // U1E91 | LATIN SMALL LETTER Z WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_z, XK_VoidSymbol}, "ẑ"},
  {{XK_dead_psili, XK_Greek_epsilon, XK_VoidSymbol}, "ἐ"}, // U1F10 | GREEK SMALL LETTER EPSILON WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_epsilon, XK_VoidSymbol}, "ἐ"},
  {{XK_dead_currency, XK_p, XK_VoidSymbol}, "₰"}, // U20B0 | GERMAN PENNY SIGN
  {{XK_dead_breve, XK_E, XK_VoidSymbol}, "Ĕ"}, // U0114 | LATIN CAPITAL LETTER E WITH BREVE
  {{XK_Multi_key, XK_U, XK_E, XK_VoidSymbol}, "Ĕ"},
  {{XK_Multi_key, XK_b, XK_E, XK_VoidSymbol}, "Ĕ"},
  {{XK_dead_acute, XK_Emacron, XK_VoidSymbol}, "Ḗ"}, // U1E16 | LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
  {{XK_Multi_key, XK_acute, XK_Emacron, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_Emacron, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_acute, XK_dead_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_acute, XK_Multi_key, XK_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_acute, XK_Multi_key, XK_underscore, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_acute, XK_dead_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_acute, XK_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_acute, XK_underscore, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_macron, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_Multi_key, XK_apostrophe, XK_underscore, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_macron, XK_Eacute, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_macron, XK_dead_acute, XK_E, XK_VoidSymbol}, "Ḗ"},
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"}, // U1F9B | GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "ᾛ"},
  {{XK_Multi_key, XK_comma, XK_apostrophe, XK_VoidSymbol}, "‚"}, // U201a | SINGLE LOW-9 QUOTATION MARK
  {{XK_Multi_key, XK_apostrophe, XK_comma, XK_VoidSymbol}, "‚"},
  {{XK_dead_caron, XK_H, XK_VoidSymbol}, "Ȟ"}, // U021E | LATIN CAPITAL LETTER H WITH CARON
  {{XK_Multi_key, XK_c, XK_H, XK_VoidSymbol}, "Ȟ"},
  {{XK_dead_belowdot, XK_a, XK_VoidSymbol}, "ạ"}, // U1EA1 | LATIN SMALL LETTER A WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_a, XK_VoidSymbol}, "ạ"},
  {{XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ἠ"}, // U1F20 | GREEK SMALL LETTER ETA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ἠ"},
  {{XK_dead_semivoiced_sound, XK_kana_HE, XK_VoidSymbol}, "ペ"}, // U30DA | KATAKANA LETTER PE
  {{XK_Multi_key, XK_Y, XK_equal, XK_VoidSymbol}, "¥"}, // yen | YEN SIGN
  {{XK_Multi_key, XK_equal, XK_Y, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_y, XK_equal, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_equal, XK_y, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_Y, XK_minus, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_minus, XK_Y, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_y, XK_minus, XK_VoidSymbol}, "¥"},
  {{XK_Multi_key, XK_minus, XK_y, XK_VoidSymbol}, "¥"},
  {{XK_dead_currency, XK_y, XK_VoidSymbol}, "¥"},
  {{XK_dead_circumflex, XK_H, XK_VoidSymbol}, "Ĥ"}, // U0124 | LATIN CAPITAL LETTER H WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_H, XK_VoidSymbol}, "Ĥ"},
  {{XK_dead_diaeresis, XK_H, XK_VoidSymbol}, "Ḧ"}, // U1E26 | LATIN CAPITAL LETTER H WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_H, XK_VoidSymbol}, "Ḧ"},
  {{XK_dead_ogonek, XK_nobreakspace, XK_VoidSymbol}, "̨"}, // U0328 | COMBINING OGONEK
  {{XK_dead_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"}, // U1FAB | GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_dead_iota, XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_grave, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_Multi_key, XK_Greek_iota, XK_grave, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "ᾫ"},
  {{XK_dead_horn, XK_U, XK_VoidSymbol}, "Ư"}, // U01AF | LATIN CAPITAL LETTER U WITH HORN
  {{XK_Multi_key, XK_plus, XK_U, XK_VoidSymbol}, "Ư"},
  {{XK_dead_abovedot, XK_O, XK_VoidSymbol}, "Ȯ"}, // U022E | LATIN CAPITAL LETTER O WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_O, XK_VoidSymbol}, "Ȯ"},
  {{XK_dead_grave, XK_abreve, XK_VoidSymbol}, "ằ"}, // U1EB1 | LATIN SMALL LETTER A WITH BREVE AND GRAVE
  {{XK_Multi_key, XK_grave, XK_abreve, XK_VoidSymbol}, "ằ"},
  {{XK_dead_grave, XK_dead_breve, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_dead_grave, XK_Multi_key, XK_U, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_dead_grave, XK_Multi_key, XK_b, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_Multi_key, XK_grave, XK_dead_breve, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_Multi_key, XK_grave, XK_b, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_dead_breve, XK_agrave, XK_VoidSymbol}, "ằ"},
  {{XK_dead_breve, XK_dead_grave, XK_a, XK_VoidSymbol}, "ằ"},
  {{XK_dead_psili, XK_Greek_iota, XK_VoidSymbol}, "ἰ"}, // U1F30 | GREEK SMALL LETTER IOTA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_iota, XK_VoidSymbol}, "ἰ"},
  {{XK_Multi_key, XK_m, XK_u, XK_VoidSymbol}, "µ"}, // mu | MICRO SIGN
  {{XK_Multi_key, XK_slash, XK_u, XK_VoidSymbol}, "µ"},
  {{XK_Multi_key, XK_u, XK_slash, XK_VoidSymbol}, "µ"},
  {{XK_dead_circumflex, XK_J, XK_VoidSymbol}, "Ĵ"}, // U0134 | LATIN CAPITAL LETTER J WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_J, XK_VoidSymbol}, "Ĵ"},
  {{XK_dead_belowdot, XK_L, XK_VoidSymbol}, "Ḷ"}, // U1E36 | LATIN CAPITAL LETTER L WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_L, XK_VoidSymbol}, "Ḷ"},
  {{XK_Multi_key, XK_parenleft, XK_4, XK_4, XK_parenright, XK_VoidSymbol}, "㊹"}, // U32B9 | CIRCLED NUMBER FORTY FOUR
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㊹"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_4, XK_parenright, XK_VoidSymbol}, "㊹"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㊹"},
  {{XK_dead_stroke, XK_nobreakspace, XK_VoidSymbol}, "̸"}, // U0338 | COMBINING LONG SOLIDUS OVERLAY
  {{XK_Multi_key, XK_period, XK_greater, XK_VoidSymbol}, "›"}, // U203a | SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
  {{XK_Multi_key, XK_parenleft, XK_H, XK_parenright, XK_VoidSymbol}, "Ⓗ"}, // U24BD | CIRCLED LATIN CAPITAL LETTER H
  {{XK_dead_currency, XK_a, XK_VoidSymbol}, "؋"}, // U060B | AFGHANI SIGN
  {{XK_dead_voiced_sound, XK_kana_WO, XK_VoidSymbol}, "ヺ"}, // U30FA | KATAKANA LETTER VO
  {{XK_dead_grave, XK_ecircumflex, XK_VoidSymbol}, "ề"}, // U1EC1 | LATIN SMALL LETTER E WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_ecircumflex, XK_VoidSymbol}, "ề"},
  {{XK_dead_grave, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ề"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_e, XK_VoidSymbol}, "ề"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ề"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_e, XK_VoidSymbol}, "ề"},
  {{XK_dead_circumflex, XK_egrave, XK_VoidSymbol}, "ề"},
  {{XK_dead_circumflex, XK_dead_grave, XK_e, XK_VoidSymbol}, "ề"},
  {{XK_dead_psili, XK_Greek_omicron, XK_VoidSymbol}, "ὀ"}, // U1F40 | GREEK SMALL LETTER OMICRON WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_omicron, XK_VoidSymbol}, "ὀ"},
  {{XK_dead_abovering, XK_A, XK_VoidSymbol}, "Å"}, // Aring | LATIN CAPITAL LETTER A WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_A, XK_VoidSymbol}, "Å"},
  {{XK_Multi_key, XK_asterisk, XK_A, XK_VoidSymbol}, "Å"},
  {{XK_Multi_key, XK_A, XK_asterisk, XK_VoidSymbol}, "Å"},
  {{XK_Multi_key, XK_A, XK_A, XK_VoidSymbol}, "Å"},
  {{XK_dead_acute, XK_n, XK_VoidSymbol}, "ń"}, // U0144 | LATIN SMALL LETTER N WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_n, XK_VoidSymbol}, "ń"},
  {{XK_Multi_key, XK_apostrophe, XK_n, XK_VoidSymbol}, "ń"},
  {{XK_Multi_key, XK_n, XK_apostrophe, XK_VoidSymbol}, "ń"},
  {{XK_dead_belowdot, XK_N, XK_VoidSymbol}, "Ṇ"}, // U1E46 | LATIN CAPITAL LETTER N WITH DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_N, XK_VoidSymbol}, "Ṇ"},
  {{XK_Multi_key, XK_parenleft, XK_X, XK_parenright, XK_VoidSymbol}, "Ⓧ"}, // U24CD | CIRCLED LATIN CAPITAL LETTER X
  {{XK_dead_caron, XK_I, XK_VoidSymbol}, "Ǐ"}, // U01CF | LATIN CAPITAL LETTER I WITH CARON
  {{XK_Multi_key, XK_c, XK_I, XK_VoidSymbol}, "Ǐ"},
  {{XK_dead_stroke, XK_Y, XK_VoidSymbol}, "Ɏ"}, // U024E | LATIN CAPITAL LETTER Y WITH STROKE
  {{XK_dead_acute, XK_ocircumflex, XK_VoidSymbol}, "ố"}, // U1ED1 | LATIN SMALL LETTER O WITH CIRCUMFLEX AND ACUTE
  {{XK_Multi_key, XK_acute, XK_ocircumflex, XK_VoidSymbol}, "ố"},
  {{XK_Multi_key, XK_apostrophe, XK_ocircumflex, XK_VoidSymbol}, "ố"},
  {{XK_dead_acute, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_dead_acute, XK_Multi_key, XK_asciicircum, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_Multi_key, XK_acute, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_Multi_key, XK_acute, XK_asciicircum, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_Multi_key, XK_apostrophe, XK_dead_circumflex, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_Multi_key, XK_apostrophe, XK_asciicircum, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_dead_circumflex, XK_oacute, XK_VoidSymbol}, "ố"},
  {{XK_dead_circumflex, XK_dead_acute, XK_o, XK_VoidSymbol}, "ố"},
  {{XK_dead_circumflex, XK_1, XK_VoidSymbol}, "¹"}, // onesuperior | SUPERSCRIPT ONE
  {{XK_Multi_key, XK_asciicircum, XK_1, XK_VoidSymbol}, "¹"},
  {{XK_Multi_key, XK_1, XK_asciicircum, XK_VoidSymbol}, "¹"},
  {{XK_dead_circumflex, XK_KP_1, XK_VoidSymbol}, "¹"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_1, XK_VoidSymbol}, "¹"},
  {{XK_Multi_key, XK_parenleft, XK_s, XK_parenright, XK_VoidSymbol}, "ⓢ"}, // U24E2 | CIRCLED LATIN SMALL LETTER S
  {{XK_dead_tilde, XK_O, XK_VoidSymbol}, "Õ"}, // Otilde | LATIN CAPITAL LETTER O WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_O, XK_VoidSymbol}, "Õ"},
  {{XK_Multi_key, XK_O, XK_asciitilde, XK_VoidSymbol}, "Õ"},
  {{XK_dead_acute, XK_R, XK_VoidSymbol}, "Ŕ"}, // U0154 | LATIN CAPITAL LETTER R WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_R, XK_VoidSymbol}, "Ŕ"},
  {{XK_Multi_key, XK_apostrophe, XK_R, XK_VoidSymbol}, "Ŕ"},
  {{XK_Multi_key, XK_R, XK_apostrophe, XK_VoidSymbol}, "Ŕ"},
  {{XK_dead_abovedot, XK_P, XK_VoidSymbol}, "Ṗ"}, // U1E56 | LATIN CAPITAL LETTER P WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_P, XK_VoidSymbol}, "Ṗ"},
  {{XK_Multi_key, XK_P, XK_period, XK_VoidSymbol}, "Ṗ"},
  {{XK_dead_abovedot, XK_space, XK_VoidSymbol}, "˙"}, // abovedot | DOT ABOVE
  {{XK_dead_abovedot, XK_dead_abovedot, XK_VoidSymbol}, "˙"},
  {{XK_dead_stroke, XK_a, XK_VoidSymbol}, "ⱥ"}, // U2C65 | LATIN SMALL LETTER A WITH STROKE
  {{XK_dead_currency, XK_I, XK_VoidSymbol}, "៛"}, // U17DB | KHMER CURRENCY SYMBOL RIEL
  {{XK_dead_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"}, // U1F8E | GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
  {{XK_dead_iota, XK_dead_tilde, XK_Multi_key, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_dead_iota, XK_Multi_key, XK_asciitilde, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_tilde, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_dead_psili, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_Multi_key, XK_Greek_iota, XK_asciitilde, XK_parenright, XK_Greek_ALPHA, XK_VoidSymbol}, "ᾎ"},
  {{XK_dead_diaeresis, XK_Cyrillic_zhe, XK_VoidSymbol}, "ӝ"}, // U04DD | CYRILLIC SMALL LETTER ZHE WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_zhe, XK_VoidSymbol}, "ӝ"},
  {{XK_dead_acute, XK_J, XK_VoidSymbol}, "J́"}, // LATIN CAPITAL LETTER J U004A with COMBINING ACUTE ACCENT U0301
  {{XK_Multi_key, XK_apostrophe, XK_J, XK_VoidSymbol}, "J́"},
  {{XK_Multi_key, XK_J, XK_apostrophe, XK_VoidSymbol}, "J́"},
  {{XK_Multi_key, XK_acute, XK_J, XK_VoidSymbol}, "J́"},
  {{XK_Multi_key, XK_J, XK_acute, XK_VoidSymbol}, "J́"},
  {{XK_dead_macron, XK_adiaeresis, XK_VoidSymbol}, "ǟ"}, // U01DF | LATIN SMALL LETTER A WITH DIAERESIS AND MACRON
  {{XK_Multi_key, XK_macron, XK_adiaeresis, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_underscore, XK_adiaeresis, XK_VoidSymbol}, "ǟ"},
  {{XK_dead_macron, XK_dead_diaeresis, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_dead_macron, XK_Multi_key, XK_quotedbl, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_macron, XK_dead_diaeresis, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_macron, XK_quotedbl, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_underscore, XK_dead_diaeresis, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_underscore, XK_quotedbl, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_dead_diaeresis, XK_amacron, XK_VoidSymbol}, "ǟ"},
  {{XK_dead_diaeresis, XK_dead_macron, XK_a, XK_VoidSymbol}, "ǟ"},
  {{XK_Multi_key, XK_parenleft, XK_3, XK_4, XK_parenright, XK_VoidSymbol}, "㉞"}, // U325E | CIRCLED NUMBER THIRTY FOUR
  {{XK_Multi_key, XK_parenleft, XK_3, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㉞"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_4, XK_parenright, XK_VoidSymbol}, "㉞"},
  {{XK_Multi_key, XK_parenleft, XK_KP_3, XK_KP_4, XK_parenright, XK_VoidSymbol}, "㉞"},
  {{XK_dead_tilde, XK_ohorn, XK_VoidSymbol}, "ỡ"}, // U1EE1 | LATIN SMALL LETTER O WITH HORN AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_ohorn, XK_VoidSymbol}, "ỡ"},
  {{XK_dead_tilde, XK_dead_horn, XK_o, XK_VoidSymbol}, "ỡ"},
  {{XK_dead_tilde, XK_Multi_key, XK_plus, XK_o, XK_VoidSymbol}, "ỡ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_horn, XK_o, XK_VoidSymbol}, "ỡ"},
  {{XK_Multi_key, XK_asciitilde, XK_plus, XK_o, XK_VoidSymbol}, "ỡ"},
  {{XK_dead_horn, XK_otilde, XK_VoidSymbol}, "ỡ"},
  {{XK_dead_horn, XK_dead_tilde, XK_o, XK_VoidSymbol}, "ỡ"},
  {{XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ὠ"}, // U1F60 | GREEK SMALL LETTER OMEGA WITH PSILI
  {{XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ὠ"},
  {{XK_dead_grave, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̀"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̀"},
  {{XK_dead_belowmacron, XK_L, XK_VoidSymbol}, "Ḻ"}, // U1E3A | LATIN CAPITAL LETTER L WITH LINE BELOW
  {{XK_dead_abovering, XK_a, XK_VoidSymbol}, "å"}, // aring | LATIN SMALL LETTER A WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_a, XK_VoidSymbol}, "å"},
  {{XK_Multi_key, XK_asterisk, XK_a, XK_VoidSymbol}, "å"},
  {{XK_Multi_key, XK_a, XK_asterisk, XK_VoidSymbol}, "å"},
  {{XK_Multi_key, XK_a, XK_a, XK_VoidSymbol}, "å"},
  {{XK_dead_caron, XK_T, XK_VoidSymbol}, "Ť"}, // U0164 | LATIN CAPITAL LETTER T WITH CARON
  {{XK_Multi_key, XK_c, XK_T, XK_VoidSymbol}, "Ť"},
  {{XK_Multi_key, XK_less, XK_T, XK_VoidSymbol}, "Ť"},
  {{XK_Multi_key, XK_T, XK_less, XK_VoidSymbol}, "Ť"},
  {{XK_dead_macron, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̄"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̄"},
  {{XK_dead_abovedot, XK_Scaron, XK_VoidSymbol}, "Ṧ"}, // U1E66 | LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
  {{XK_Multi_key, XK_period, XK_Scaron, XK_VoidSymbol}, "Ṧ"},
  {{XK_dead_abovedot, XK_dead_caron, XK_S, XK_VoidSymbol}, "Ṧ"},
  {{XK_dead_abovedot, XK_Multi_key, XK_c, XK_S, XK_VoidSymbol}, "Ṧ"},
  {{XK_Multi_key, XK_period, XK_dead_caron, XK_S, XK_VoidSymbol}, "Ṧ"},
  {{XK_dead_caron, XK_Sabovedot, XK_VoidSymbol}, "Ṧ"},
  {{XK_dead_caron, XK_dead_abovedot, XK_S, XK_VoidSymbol}, "Ṧ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_HA, XK_parenright, XK_VoidSymbol}, "㋩"}, // U32E9 | CIRCLED KATAKANA HA
  {{XK_Multi_key, XK_asciitilde, XK_diaeresis, XK_VoidSymbol}, "⍨"}, // U2368 | ~ ¨ APL FUNCTIONAL SYMBOL TILDE DIAERESIS
  {{XK_dead_circumflex, XK_Multi_key, XK_T, XK_M, XK_VoidSymbol}, "™"}, // U2122 | TRADE MARK SIGN
  {{XK_Multi_key, XK_T, XK_M, XK_VoidSymbol}, "™"},
  {{XK_dead_circumflex, XK_Multi_key, XK_t, XK_M, XK_VoidSymbol}, "™"},
  {{XK_Multi_key, XK_t, XK_M, XK_VoidSymbol}, "™"},
  {{XK_dead_circumflex, XK_Multi_key, XK_T, XK_m, XK_VoidSymbol}, "™"},
  {{XK_Multi_key, XK_T, XK_m, XK_VoidSymbol}, "™"},
  {{XK_dead_circumflex, XK_Multi_key, XK_t, XK_m, XK_VoidSymbol}, "™"},
  {{XK_Multi_key, XK_t, XK_m, XK_VoidSymbol}, "™"},
  {{XK_Multi_key, XK_parenleft, XK_kana_NU, XK_parenright, XK_VoidSymbol}, "㋦"}, // U32E6 | CIRCLED KATAKANA NU
  {{XK_dead_diaeresis, XK_Cyrillic_e, XK_VoidSymbol}, "ӭ"}, // U04ED | CYRILLIC SMALL LETTER E WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_e, XK_VoidSymbol}, "ӭ"},
  {{XK_dead_doublegrave, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̏"}, // CYRILLIC CAPITAL LETTER ER WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_ER, XK_VoidSymbol}, "Р̏"},
  {{XK_dead_stroke, XK_less, XK_VoidSymbol}, "≮"}, // U226E | NOT LESS-THAN
  {{XK_dead_belowdot, XK_uhorn, XK_VoidSymbol}, "ự"}, // U1EF1 | LATIN SMALL LETTER U WITH HORN AND DOT BELOW
  {{XK_Multi_key, XK_exclam, XK_uhorn, XK_VoidSymbol}, "ự"},
  {{XK_dead_belowdot, XK_dead_horn, XK_u, XK_VoidSymbol}, "ự"},
  {{XK_dead_belowdot, XK_Multi_key, XK_plus, XK_u, XK_VoidSymbol}, "ự"},
  {{XK_Multi_key, XK_exclam, XK_dead_horn, XK_u, XK_VoidSymbol}, "ự"},
  {{XK_Multi_key, XK_exclam, XK_plus, XK_u, XK_VoidSymbol}, "ự"},
  {{XK_dead_horn, XK_ubelowdot, XK_VoidSymbol}, "ự"},
  {{XK_dead_horn, XK_dead_belowdot, XK_u, XK_VoidSymbol}, "ự"},
  {{XK_dead_abovedot, XK_dead_belowdot, XK_s, XK_VoidSymbol}, "ṩ"}, // U1E69 | LATIN SMALL LETTER S WITH DOT BELOW AND DOT ABOVE
  {{XK_dead_abovedot, XK_Multi_key, XK_exclam, XK_s, XK_VoidSymbol}, "ṩ"},
  {{XK_Multi_key, XK_period, XK_dead_belowdot, XK_s, XK_VoidSymbol}, "ṩ"},
  {{XK_Multi_key, XK_period, XK_exclam, XK_s, XK_VoidSymbol}, "ṩ"},
  {{XK_dead_belowdot, XK_sabovedot, XK_VoidSymbol}, "ṩ"},
  {{XK_dead_belowdot, XK_dead_abovedot, XK_s, XK_VoidSymbol}, "ṩ"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_9, XK_parenright, XK_VoidSymbol}, "⑲"}, // U2472 | CIRCLED NUMBER NINETEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_9, XK_parenright, XK_VoidSymbol}, "⑲"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_9, XK_parenright, XK_VoidSymbol}, "⑲"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_9, XK_parenright, XK_VoidSymbol}, "⑲"},
  {{XK_dead_tilde, XK_o, XK_VoidSymbol}, "õ"}, // otilde | LATIN SMALL LETTER O WITH TILDE
  {{XK_Multi_key, XK_asciitilde, XK_o, XK_VoidSymbol}, "õ"},
  {{XK_Multi_key, XK_o, XK_asciitilde, XK_VoidSymbol}, "õ"},
  {{XK_dead_circumflex, XK_W, XK_VoidSymbol}, "Ŵ"}, // U0174 | LATIN CAPITAL LETTER W WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_W, XK_VoidSymbol}, "Ŵ"},
  {{XK_Multi_key, XK_W, XK_asciicircum, XK_VoidSymbol}, "Ŵ"},
  {{XK_dead_belowcircumflex, XK_U, XK_VoidSymbol}, "Ṷ"}, // U1E76 | LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
  {{XK_Multi_key, XK_parenleft, XK_kana_RE, XK_parenright, XK_VoidSymbol}, "㋹"}, // U32F9 | CIRCLED KATAKANA RE
  {{XK_dead_circumflex, XK_plus, XK_VoidSymbol}, "⁺"}, // U207A | SUPERSCRIPT PLUS SIGN
  {{XK_Multi_key, XK_asciicircum, XK_plus, XK_VoidSymbol}, "⁺"},
  {{XK_dead_circumflex, XK_KP_Add, XK_VoidSymbol}, "⁺"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_Add, XK_VoidSymbol}, "⁺"},
  {{XK_dead_acute, XK_oslash, XK_VoidSymbol}, "ǿ"}, // U01FF | LATIN SMALL LETTER O WITH STROKE AND ACUTE
  {{XK_Multi_key, XK_acute, XK_oslash, XK_VoidSymbol}, "ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_oslash, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_acute, XK_dead_stroke, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_acute, XK_Multi_key, XK_slash, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_Multi_key, XK_acute, XK_slash, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_slash, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_acute, XK_Multi_key, XK_KP_Divide, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_Multi_key, XK_acute, XK_KP_Divide, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_Multi_key, XK_apostrophe, XK_KP_Divide, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_stroke, XK_dead_acute, XK_o, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_stroke, XK_oacute, XK_VoidSymbol}, "ǿ"},
  {{XK_dead_belowring, XK_a, XK_VoidSymbol}, "ḁ"}, // U1E01 | LATIN SMALL LETTER A WITH RING BELOW
  {{XK_dead_iota, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾀ"}, // U1F80 | GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾀ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_alpha, XK_VoidSymbol}, "ᾀ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_alpha, XK_VoidSymbol}, "ᾀ"},
  {{XK_Multi_key, XK_F, XK_i, XK_VoidSymbol}, "ﬃ"}, // Ufb03 | LATIN SMALL LIGATURE FFI
  {{XK_dead_abovedot, XK_W, XK_VoidSymbol}, "Ẇ"}, // U1E86 | LATIN CAPITAL LETTER W WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_W, XK_VoidSymbol}, "Ẇ"},
  {{XK_dead_doublegrave, XK_i, XK_VoidSymbol}, "ȉ"}, // U0209 | LATIN SMALL LETTER I WITH DOUBLE GRAVE
  {{XK_dead_acute, XK_Greek_EPSILON, XK_VoidSymbol}, "Έ"}, // U0388 | GREEK CAPITAL LETTER EPSILON WITH TONOS
  {{XK_Multi_key, XK_acute, XK_Greek_EPSILON, XK_VoidSymbol}, "Έ"},
  {{XK_Multi_key, XK_apostrophe, XK_Greek_EPSILON, XK_VoidSymbol}, "Έ"},
  {{XK_Multi_key, XK_Greek_EPSILON, XK_apostrophe, XK_VoidSymbol}, "Έ"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἃ"}, // U1F0B | GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἃ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἃ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_ALPHA, XK_VoidSymbol}, "Ἃ"},
  {{XK_Multi_key, XK_underscore, XK_plus, XK_VoidSymbol}, "₊"}, // U208A | SUBSCRIPT PLUS SIGN
  {{XK_Multi_key, XK_underscore, XK_KP_Add, XK_VoidSymbol}, "₊"},
  {{XK_dead_caron, XK_plus, XK_VoidSymbol}, "₊"},
  {{XK_dead_grave, XK_Cyrillic_I, XK_VoidSymbol}, "Ѝ"}, // U040D | CYRILLIC CAPITAL LETTER I WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_Cyrillic_I, XK_VoidSymbol}, "Ѝ"},
  {{XK_dead_caron, XK_d, XK_VoidSymbol}, "ď"}, // U010F | LATIN SMALL LETTER D WITH CARON
  {{XK_Multi_key, XK_c, XK_d, XK_VoidSymbol}, "ď"},
  {{XK_Multi_key, XK_less, XK_d, XK_VoidSymbol}, "ď"},
  {{XK_Multi_key, XK_d, XK_less, XK_VoidSymbol}, "ď"},
  {{XK_dead_cedilla, XK_d, XK_VoidSymbol}, "ḑ"}, // U1E11 | LATIN SMALL LETTER D WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_d, XK_VoidSymbol}, "ḑ"},
  {{XK_Multi_key, XK_d, XK_comma, XK_VoidSymbol}, "ḑ"},
  {{XK_Multi_key, XK_cedilla, XK_d, XK_VoidSymbol}, "ḑ"},
  {{XK_dead_iota, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾐ"}, // U1F90 | GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾐ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_eta, XK_VoidSymbol}, "ᾐ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_eta, XK_VoidSymbol}, "ᾐ"},
  {{XK_Multi_key, XK_slash, XK_Cyrillic_GHE, XK_VoidSymbol}, "Ғ"}, // U0492 | CYRILLIC CAPITAL LETTER GHE WITH STROKE
  {{XK_Multi_key, XK_KP_Divide, XK_Cyrillic_GHE, XK_VoidSymbol}, "Ғ"},
  {{XK_dead_belowmacron, XK_h, XK_VoidSymbol}, "ẖ"}, // U1E96 | LATIN SMALL LETTER H WITH LINE BELOW
  {{XK_dead_belowcomma, XK_s, XK_VoidSymbol}, "ș"}, // U0219 | LATIN SMALL LETTER S WITH COMMA BELOW
  {{XK_Multi_key, XK_semicolon, XK_s, XK_VoidSymbol}, "ș"},
  {{XK_Multi_key, XK_s, XK_semicolon, XK_VoidSymbol}, "ș"},
  {{XK_dead_acute, XK_Cyrillic_O, XK_VoidSymbol}, "О́"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING ACUTE ACCENT
  {{XK_Multi_key, XK_acute, XK_Cyrillic_O, XK_VoidSymbol}, "О́"},
  {{XK_Multi_key, XK_apostrophe, XK_Cyrillic_O, XK_VoidSymbol}, "О́"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἓ"}, // U1F1B | GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἓ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἓ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_EPSILON, XK_VoidSymbol}, "Ἓ"},
  {{XK_dead_breve, XK_g, XK_VoidSymbol}, "ğ"}, // U011F | LATIN SMALL LETTER G WITH BREVE
  {{XK_Multi_key, XK_U, XK_g, XK_VoidSymbol}, "ğ"},
  {{XK_Multi_key, XK_g, XK_U, XK_VoidSymbol}, "ğ"},
  {{XK_Multi_key, XK_b, XK_g, XK_VoidSymbol}, "ğ"},
  {{XK_Multi_key, XK_breve, XK_g, XK_VoidSymbol}, "ğ"},
  {{XK_Multi_key, XK_g, XK_breve, XK_VoidSymbol}, "ğ"},
  {{XK_Multi_key, XK_g, XK_parenleft, XK_VoidSymbol}, "ğ"},
  {{XK_dead_doublegrave, XK_Cyrillic_I, XK_VoidSymbol}, "И̏"}, // CYRILLIC CAPITAL LETTER I WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_I, XK_VoidSymbol}, "И̏"},
  {{XK_dead_macron, XK_g, XK_VoidSymbol}, "ḡ"}, // U1E21 | LATIN SMALL LETTER G WITH MACRON
  {{XK_Multi_key, XK_macron, XK_g, XK_VoidSymbol}, "ḡ"},
  {{XK_Multi_key, XK_underscore, XK_g, XK_VoidSymbol}, "ḡ"},
  {{XK_dead_iota, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾠ"}, // U1FA0 | GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI
  {{XK_dead_iota, XK_Multi_key, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾠ"},
  {{XK_Multi_key, XK_Greek_iota, XK_dead_psili, XK_Greek_omega, XK_VoidSymbol}, "ᾠ"},
  {{XK_Multi_key, XK_Greek_iota, XK_parenright, XK_Greek_omega, XK_VoidSymbol}, "ᾠ"},
  {{XK_dead_belowdot, XK_dead_belowdot, XK_VoidSymbol}, "̣"}, // U0323 | COMBINING DOT BELOW
  {{XK_dead_belowdot, XK_nobreakspace, XK_VoidSymbol}, "̣"},
  {{XK_dead_belowdot, XK_space, XK_VoidSymbol}, "̣"},
  {{XK_dead_hook, XK_P, XK_VoidSymbol}, "Ƥ"}, // U01A4 | LATIN CAPITAL LETTER P WITH HOOK
  {{XK_dead_grave, XK_Acircumflex, XK_VoidSymbol}, "Ầ"}, // U1EA6 | LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
  {{XK_Multi_key, XK_grave, XK_Acircumflex, XK_VoidSymbol}, "Ầ"},
  {{XK_dead_grave, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ầ"},
  {{XK_dead_grave, XK_Multi_key, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ầ"},
  {{XK_Multi_key, XK_grave, XK_dead_circumflex, XK_A, XK_VoidSymbol}, "Ầ"},
  {{XK_Multi_key, XK_grave, XK_asciicircum, XK_A, XK_VoidSymbol}, "Ầ"},
  {{XK_dead_circumflex, XK_Agrave, XK_VoidSymbol}, "Ầ"},
  {{XK_dead_circumflex, XK_dead_grave, XK_A, XK_VoidSymbol}, "Ầ"},
  {{XK_dead_cedilla, XK_e, XK_VoidSymbol}, "ȩ"}, // U0229 | LATIN SMALL LETTER E WITH CEDILLA
  {{XK_Multi_key, XK_cedilla, XK_e, XK_VoidSymbol}, "ȩ"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἣ"}, // U1F2B | GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἣ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_ETA, XK_VoidSymbol}, "Ἣ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_ETA, XK_VoidSymbol}, "Ἣ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_a, XK_VoidSymbol}, "ª"}, // ordfeminine | FEMININE ORDINAL INDICATOR
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_a, XK_VoidSymbol}, "ª"},
  {{XK_dead_ogonek, XK_i, XK_VoidSymbol}, "į"}, // U012F | LATIN SMALL LETTER I WITH OGONEK
  {{XK_Multi_key, XK_semicolon, XK_i, XK_VoidSymbol}, "į"},
  {{XK_Multi_key, XK_i, XK_semicolon, XK_VoidSymbol}, "į"},
  {{XK_Multi_key, XK_comma, XK_i, XK_VoidSymbol}, "į"},
  {{XK_Multi_key, XK_i, XK_comma, XK_VoidSymbol}, "į"},
  {{XK_dead_acute, XK_k, XK_VoidSymbol}, "ḱ"}, // U1E31 | LATIN SMALL LETTER K WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_k, XK_VoidSymbol}, "ḱ"},
  {{XK_Multi_key, XK_apostrophe, XK_k, XK_VoidSymbol}, "ḱ"},
  {{XK_dead_breve, XK_Greek_alpha, XK_VoidSymbol}, "ᾰ"}, // U1FB0 | GREEK SMALL LETTER ALPHA WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_alpha, XK_VoidSymbol}, "ᾰ"},
  {{XK_Multi_key, XK_b, XK_Greek_alpha, XK_VoidSymbol}, "ᾰ"},
  {{XK_dead_voiced_sound, XK_kana_U, XK_VoidSymbol}, "ヴ"}, // U30F4 | KATAKANA LETTER VU
  {{XK_dead_breve, XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ặ"}, // U1EB6 | LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
  {{XK_dead_breve, XK_Multi_key, XK_exclam, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_Multi_key, XK_U, XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_Multi_key, XK_U, XK_exclam, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_Multi_key, XK_b, XK_dead_belowdot, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_Multi_key, XK_b, XK_exclam, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_dead_belowdot, XK_Abreve, XK_VoidSymbol}, "Ặ"},
  {{XK_dead_belowdot, XK_dead_breve, XK_A, XK_VoidSymbol}, "Ặ"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἳ"}, // U1F3B | GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἳ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_IOTA, XK_VoidSymbol}, "Ἳ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_IOTA, XK_VoidSymbol}, "Ἳ"},
  {{XK_dead_circumflex, XK_Multi_key, XK_underscore, XK_o, XK_VoidSymbol}, "º"}, // masculine | MASCULINE ORDINAL INDICATOR
  {{XK_Multi_key, XK_asciicircum, XK_underscore, XK_o, XK_VoidSymbol}, "º"},
  {{XK_dead_circumflex, XK_nobreakspace, XK_VoidSymbol}, "̂"}, // U0302 | COMBINING CIRCUMFLEX ACCENT
  {{XK_dead_acute, XK_j, XK_VoidSymbol}, "j́"}, // LATIN SMALL LETTER J U006A with COMBINING ACUTE ACCENT U0301
  {{XK_Multi_key, XK_apostrophe, XK_j, XK_VoidSymbol}, "j́"},
  {{XK_Multi_key, XK_j, XK_apostrophe, XK_VoidSymbol}, "j́"},
  {{XK_Multi_key, XK_acute, XK_j, XK_VoidSymbol}, "j́"},
  {{XK_Multi_key, XK_j, XK_acute, XK_VoidSymbol}, "j́"},
  {{XK_dead_abovedot, XK_L, XK_VoidSymbol}, "Ŀ"}, // U013F | LATIN CAPITAL LETTER L WITH MIDDLE DOT
  {{XK_Multi_key, XK_parenleft, XK_4, XK_9, XK_parenright, XK_VoidSymbol}, "㊾"}, // U32BE | CIRCLED NUMBER FORTY NINE
  {{XK_Multi_key, XK_parenleft, XK_4, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㊾"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_9, XK_parenright, XK_VoidSymbol}, "㊾"},
  {{XK_Multi_key, XK_parenleft, XK_KP_4, XK_KP_9, XK_parenright, XK_VoidSymbol}, "㊾"},
  {{XK_dead_abovedot, XK_m, XK_VoidSymbol}, "ṁ"}, // U1E41 | LATIN SMALL LETTER M WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_m, XK_VoidSymbol}, "ṁ"},
  {{XK_Multi_key, XK_m, XK_period, XK_VoidSymbol}, "ṁ"},
  {{XK_dead_grave, XK_Cyrillic_er, XK_VoidSymbol}, "р̀"}, // CYRILLIC SMALL LETTER ER WITH COMBINING GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_Cyrillic_er, XK_VoidSymbol}, "р̀"},
  {{XK_dead_breve, XK_Cyrillic_zhe, XK_VoidSymbol}, "ӂ"}, // U04C2 | CYRILLIC SMALL LETTER ZHE WITH BREVE
  {{XK_Multi_key, XK_U, XK_Cyrillic_zhe, XK_VoidSymbol}, "ӂ"},
  {{XK_Multi_key, XK_b, XK_Cyrillic_zhe, XK_VoidSymbol}, "ӂ"},
  {{XK_dead_doublegrave, XK_Cyrillic_O, XK_VoidSymbol}, "О̏"}, // CYRILLIC CAPITAL LETTER O WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_O, XK_VoidSymbol}, "О̏"},
  {{XK_dead_macron, XK_Cyrillic_er, XK_VoidSymbol}, "р̄"}, // CYRILLIC SMALL LETTER ER WITH COMBINING MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_er, XK_VoidSymbol}, "р̄"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_er, XK_VoidSymbol}, "р̄"},
  {{XK_dead_circumflex, XK_dead_belowdot, XK_E, XK_VoidSymbol}, "Ệ"}, // U1EC6 | LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
  {{XK_dead_circumflex, XK_Multi_key, XK_exclam, XK_E, XK_VoidSymbol}, "Ệ"},
  {{XK_Multi_key, XK_asciicircum, XK_dead_belowdot, XK_E, XK_VoidSymbol}, "Ệ"},
  {{XK_Multi_key, XK_asciicircum, XK_exclam, XK_E, XK_VoidSymbol}, "Ệ"},
  {{XK_dead_belowdot, XK_Ecircumflex, XK_VoidSymbol}, "Ệ"},
  {{XK_dead_belowdot, XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ệ"},
  {{XK_dead_stroke, XK_j, XK_VoidSymbol}, "ɉ"}, // U0249 | LATIN SMALL LETTER J WITH STROKE
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὃ"}, // U1F4B | GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὃ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὃ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_OMICRON, XK_VoidSymbol}, "Ὃ"},
  {{XK_dead_circumflex, XK_E, XK_VoidSymbol}, "Ê"}, // Ecircumflex | LATIN CAPITAL LETTER E WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_E, XK_VoidSymbol}, "Ê"},
  {{XK_Multi_key, XK_E, XK_asciicircum, XK_VoidSymbol}, "Ê"},
  {{XK_Multi_key, XK_greater, XK_E, XK_VoidSymbol}, "Ê"},
  {{XK_Multi_key, XK_E, XK_greater, XK_VoidSymbol}, "Ê"},
  {{XK_dead_doublegrave, XK_Cyrillic_er, XK_VoidSymbol}, "р̏"}, // CYRILLIC SMALL LETTER ER WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_er, XK_VoidSymbol}, "р̏"},
  {{XK_dead_breve, XK_o, XK_VoidSymbol}, "ŏ"}, // U014F | LATIN SMALL LETTER O WITH BREVE
  {{XK_Multi_key, XK_U, XK_o, XK_VoidSymbol}, "ŏ"},
  {{XK_Multi_key, XK_b, XK_o, XK_VoidSymbol}, "ŏ"},
  {{XK_dead_grave, XK_omacron, XK_VoidSymbol}, "ṑ"}, // U1E51 | LATIN SMALL LETTER O WITH MACRON AND GRAVE
  {{XK_Multi_key, XK_grave, XK_omacron, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_grave, XK_dead_macron, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_grave, XK_Multi_key, XK_macron, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_grave, XK_Multi_key, XK_underscore, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_Multi_key, XK_grave, XK_dead_macron, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_Multi_key, XK_grave, XK_macron, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_Multi_key, XK_grave, XK_underscore, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_macron, XK_ograve, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_macron, XK_dead_grave, XK_o, XK_VoidSymbol}, "ṑ"},
  {{XK_dead_breve, XK_Greek_iota, XK_VoidSymbol}, "ῐ"}, // U1FD0 | GREEK SMALL LETTER IOTA WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_iota, XK_VoidSymbol}, "ῐ"},
  {{XK_Multi_key, XK_b, XK_Greek_iota, XK_VoidSymbol}, "ῐ"},
  {{XK_dead_diaeresis, XK_Cyrillic_A, XK_VoidSymbol}, "Ӓ"}, // U04D2 | CYRILLIC CAPITAL LETTER A WITH DIAERESIS
  {{XK_Multi_key, XK_quotedbl, XK_Cyrillic_A, XK_VoidSymbol}, "Ӓ"},
  {{XK_dead_caron, XK_u, XK_VoidSymbol}, "ǔ"}, // U01D4 | LATIN SMALL LETTER U WITH CARON
  {{XK_Multi_key, XK_c, XK_u, XK_VoidSymbol}, "ǔ"},
  {{XK_dead_tilde, XK_Ocircumflex, XK_VoidSymbol}, "Ỗ"}, // U1ED6 | LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
  {{XK_Multi_key, XK_asciitilde, XK_Ocircumflex, XK_VoidSymbol}, "Ỗ"},
  {{XK_dead_tilde, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ỗ"},
  {{XK_dead_tilde, XK_Multi_key, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ỗ"},
  {{XK_Multi_key, XK_asciitilde, XK_dead_circumflex, XK_O, XK_VoidSymbol}, "Ỗ"},
  {{XK_Multi_key, XK_asciitilde, XK_asciicircum, XK_O, XK_VoidSymbol}, "Ỗ"},
  {{XK_dead_circumflex, XK_Otilde, XK_VoidSymbol}, "Ỗ"},
  {{XK_dead_circumflex, XK_dead_tilde, XK_O, XK_VoidSymbol}, "Ỗ"},
  {{XK_Multi_key, XK_e, XK_e, XK_VoidSymbol}, "ə"}, // U0259 | LATIN SMALL LETTER SCHWA
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὓ"}, // U1F5B | GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὓ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὓ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_UPSILON, XK_VoidSymbol}, "Ὓ"},
  {{XK_dead_acute, XK_U, XK_VoidSymbol}, "Ú"}, // Uacute | LATIN CAPITAL LETTER U WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_U, XK_VoidSymbol}, "Ú"},
  {{XK_Multi_key, XK_U, XK_acute, XK_VoidSymbol}, "Ú"},
  {{XK_Multi_key, XK_apostrophe, XK_U, XK_VoidSymbol}, "Ú"},
  {{XK_Multi_key, XK_U, XK_apostrophe, XK_VoidSymbol}, "Ú"},
  {{XK_dead_grave, XK_Cyrillic_i, XK_VoidSymbol}, "ѝ"}, // U045D | CYRILLIC SMALL LETTER I WITH GRAVE
  {{XK_Multi_key, XK_grave, XK_Cyrillic_i, XK_VoidSymbol}, "ѝ"},
  {{XK_dead_cedilla, XK_s, XK_VoidSymbol}, "ş"}, // U015F | LATIN SMALL LETTER S WITH CEDILLA
  {{XK_Multi_key, XK_comma, XK_s, XK_VoidSymbol}, "ş"},
  {{XK_Multi_key, XK_s, XK_comma, XK_VoidSymbol}, "ş"},
  {{XK_Multi_key, XK_cedilla, XK_s, XK_VoidSymbol}, "ş"},
  {{XK_Multi_key, XK_s, XK_cedilla, XK_VoidSymbol}, "ş"},
  {{XK_Multi_key, XK_o, XK_x, XK_VoidSymbol}, "¤"}, // currency | CURRENCY SIGN
  {{XK_Multi_key, XK_x, XK_o, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_o, XK_X, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_X, XK_o, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_O, XK_X, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_X, XK_O, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_O, XK_x, XK_VoidSymbol}, "¤"},
  {{XK_Multi_key, XK_x, XK_O, XK_VoidSymbol}, "¤"},
  {{XK_dead_currency, XK_dead_currency, XK_VoidSymbol}, "¤"},
  {{XK_dead_currency, XK_nobreakspace, XK_VoidSymbol}, "¤"},
  {{XK_dead_currency, XK_space, XK_VoidSymbol}, "¤"},
  {{XK_dead_abovedot, XK_s, XK_VoidSymbol}, "ṡ"}, // U1E61 | LATIN SMALL LETTER S WITH DOT ABOVE
  {{XK_Multi_key, XK_period, XK_s, XK_VoidSymbol}, "ṡ"},
  {{XK_Multi_key, XK_s, XK_period, XK_VoidSymbol}, "ṡ"},
  {{XK_dead_breve, XK_Greek_upsilon, XK_VoidSymbol}, "ῠ"}, // U1FE0 | GREEK SMALL LETTER UPSILON WITH VRACHY
  {{XK_Multi_key, XK_U, XK_Greek_upsilon, XK_VoidSymbol}, "ῠ"},
  {{XK_Multi_key, XK_b, XK_Greek_upsilon, XK_VoidSymbol}, "ῠ"},
  {{XK_Multi_key, XK_asterisk, XK_diaeresis, XK_VoidSymbol}, "⍣"}, // U2363 | * ¨ APL FUNCTIONAL SYMBOL STAR DIAERESIS
  {{XK_Multi_key, XK_diaeresis, XK_asterisk, XK_VoidSymbol}, "⍣"},
  {{XK_dead_macron, XK_Cyrillic_I, XK_VoidSymbol}, "Ӣ"}, // U04E2 | CYRILLIC CAPITAL LETTER I WITH MACRON
  {{XK_Multi_key, XK_macron, XK_Cyrillic_I, XK_VoidSymbol}, "Ӣ"},
  {{XK_Multi_key, XK_underscore, XK_Cyrillic_I, XK_VoidSymbol}, "Ӣ"},
  {{XK_dead_stroke, XK_G, XK_VoidSymbol}, "Ǥ"}, // U01E4 | LATIN CAPITAL LETTER G WITH STROKE
  {{XK_Multi_key, XK_slash, XK_G, XK_VoidSymbol}, "Ǥ"},
  {{XK_Multi_key, XK_KP_Divide, XK_G, XK_VoidSymbol}, "Ǥ"},
  {{XK_dead_hook, XK_U, XK_VoidSymbol}, "Ủ"}, // U1EE6 | LATIN CAPITAL LETTER U WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_U, XK_VoidSymbol}, "Ủ"},
  {{XK_dead_circumflex, XK_parenleft, XK_VoidSymbol}, "⁽"}, // U207D | SUPERSCRIPT LEFT PARENTHESIS
  {{XK_Multi_key, XK_asciicircum, XK_parenleft, XK_VoidSymbol}, "⁽"},
  {{XK_dead_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὣ"}, // U1F6B | GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
  {{XK_dead_grave, XK_Multi_key, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὣ"},
  {{XK_Multi_key, XK_grave, XK_dead_dasia, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὣ"},
  {{XK_Multi_key, XK_grave, XK_parenleft, XK_Greek_OMEGA, XK_VoidSymbol}, "Ὣ"},
  {{XK_dead_circumflex, XK_e, XK_VoidSymbol}, "ê"}, // ecircumflex | LATIN SMALL LETTER E WITH CIRCUMFLEX
  {{XK_Multi_key, XK_asciicircum, XK_e, XK_VoidSymbol}, "ê"},
  {{XK_Multi_key, XK_e, XK_asciicircum, XK_VoidSymbol}, "ê"},
  {{XK_Multi_key, XK_greater, XK_e, XK_VoidSymbol}, "ê"},
  {{XK_Multi_key, XK_e, XK_greater, XK_VoidSymbol}, "ê"},
  {{XK_Multi_key, XK_parenleft, XK_1, XK_4, XK_parenright, XK_VoidSymbol}, "⑭"}, // U246D | CIRCLED NUMBER FOURTEEN
  {{XK_Multi_key, XK_parenleft, XK_1, XK_KP_4, XK_parenright, XK_VoidSymbol}, "⑭"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_4, XK_parenright, XK_VoidSymbol}, "⑭"},
  {{XK_Multi_key, XK_parenleft, XK_KP_1, XK_KP_4, XK_parenright, XK_VoidSymbol}, "⑭"},
  {{XK_dead_abovering, XK_u, XK_VoidSymbol}, "ů"}, // U016F | LATIN SMALL LETTER U WITH RING ABOVE
  {{XK_Multi_key, XK_o, XK_u, XK_VoidSymbol}, "ů"},
  {{XK_Multi_key, XK_asterisk, XK_u, XK_VoidSymbol}, "ů"},
  {{XK_Multi_key, XK_u, XK_asterisk, XK_VoidSymbol}, "ů"},
  {{XK_Multi_key, XK_parenleft, XK_kana_MA, XK_parenright, XK_VoidSymbol}, "㋮"}, // U32EE | CIRCLED KATAKANA MA
  {{XK_dead_belowcircumflex, XK_t, XK_VoidSymbol}, "ṱ"}, // U1E71 | LATIN SMALL LETTER T WITH CIRCUMFLEX BELOW
  {{XK_dead_doubleacute, XK_Cyrillic_U, XK_VoidSymbol}, "Ӳ"}, // U04F2 | CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
  {{XK_Multi_key, XK_equal, XK_Cyrillic_U, XK_VoidSymbol}, "Ӳ"},
  {{XK_dead_circumflex, XK_5, XK_VoidSymbol}, "⁵"}, // U2075 | SUPERSCRIPT FIVE
  {{XK_Multi_key, XK_asciicircum, XK_5, XK_VoidSymbol}, "⁵"},
  {{XK_dead_circumflex, XK_KP_5, XK_VoidSymbol}, "⁵"},
  {{XK_Multi_key, XK_asciicircum, XK_KP_5, XK_VoidSymbol}, "⁵"},
  {{XK_dead_acute, XK_G, XK_VoidSymbol}, "Ǵ"}, // U01F4 | LATIN CAPITAL LETTER G WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_G, XK_VoidSymbol}, "Ǵ"},
  {{XK_Multi_key, XK_apostrophe, XK_G, XK_VoidSymbol}, "Ǵ"},
  {{XK_dead_hook, XK_Y, XK_VoidSymbol}, "Ỷ"}, // U1EF6 | LATIN CAPITAL LETTER Y WITH HOOK ABOVE
  {{XK_Multi_key, XK_question, XK_Y, XK_VoidSymbol}, "Ỷ"},
  {{XK_dead_doublegrave, XK_Cyrillic_A, XK_VoidSymbol}, "А̏"}, // CYRILLIC CAPITAL LETTER A WITH COMBINING DOUBLE GRAVE ACCENT
  {{XK_Multi_key, XK_grave, XK_grave, XK_Cyrillic_A, XK_VoidSymbol}, "А̏"},
  {{XK_dead_acute, XK_u, XK_VoidSymbol}, "ú"}, // uacute | LATIN SMALL LETTER U WITH ACUTE
  {{XK_Multi_key, XK_acute, XK_u, XK_VoidSymbol}, "ú"},
  {{XK_Multi_key, XK_u, XK_acute, XK_VoidSymbol}, "ú"},
  {{XK_Multi_key, XK_apostrophe, XK_u, XK_VoidSymbol}, "ú"},
  {{XK_Multi_key, XK_u, XK_apostrophe, XK_VoidSymbol}, "ú"},
  {{XK_dead_currency, XK_i, XK_VoidSymbol}, "﷼"}, // UFDFC | RIAL SIGN
  {{XK_Multi_key, XK_f, XK_s, XK_VoidSymbol}, "ſ"}, // U017f | LATIN SMALL LETTER LONG S
  {{XK_Multi_key, XK_f, XK_S, XK_VoidSymbol}, "ſ"},
  {{XK_Multi_key, XK_parenleft, XK_kana_WO, XK_parenright, XK_VoidSymbol}, "㋾"}, // U32FE | CIRCLED KATAKANA WO
};

} //namespace

#endif
