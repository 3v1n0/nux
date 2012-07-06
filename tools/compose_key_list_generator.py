#!/usr/bin/python
# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Convert XOrg libX11 compose sequences such as
# http://cgit.freedesktop.org/xorg/lib/libX11/plain/nls/en_US.UTF-8/Compose.pre
#
# To use it, just download the Compose.pre file you want, then launch this
# script like:
#   ./compose_key_list_generator.py /path/Compose.pre [/path/output_file.c]
#
# Marco Trevisan (Treviño) <marco.trevisan@canonical.com> -- Copyright 2012
#
# Released under the GPLv2 terms.

import os, sys, re, codecs
from gi.repository import Gdk

DEFAULT_OUTPUT = "/tmp/parsed_keys.c"
input_file = sys.argv[1]
output_file = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_OUTPUT

keys_match = re.compile("(?P<keybinding><[^:]+)[\s]*:[\s]*\"(\\\)?(?P<char>.*)\"[\s]*(?P<charcode>[^\s]+)?[\s]*#[\s]*(?P<desc>.+)")
unicode_keys = re.compile("^U[0-9A-Fa-f]{4,6}$");

combinations = {u"": ["ubuntu", "circle", "((u))", "((o))"]}
descriptions = {u"": "UBUNTU CYRCLE"}

dead_to_char_map = { "dead_grave": 'grave',
                     "dead_acute": 'acute',
                     "dead_circumflex": 'asciicircum',
                     "dead_tilde": 'asciitilde',
                     "dead_macron": 'macron',
                     "dead_breve": 'breve',
                     "dead_abovedot": 'abovedot',
                     "dead_diaeresis": 'diaeresis',
                     "dead_abovering": 'degree',
                     "dead_doubleacute": 'doubleacute',
                     "dead_caron": 'caron',
                     "dead_cedilla": 'cedilla',
                     "dead_ogonek": 'ogonek',
                     "dead_stroke": 'slash',
                     "dead_iota": 'Greek_iota',
                     "dead_belowcomma": "comma",
                     "dead_currency": "currency"
                      }


def key_list_has_invalid_keys(keys):
    for k in keys:
        if unicode_keys.match(k):
            return True
    return False


def escape_char(char):
    escape_list = ["\\", "\""]
    for e in escape_list:
        char = char.replace(e, "\\"+e)

    return char


def make_c_output(output_file):
    keycode = """/* This file has been automatically generated using the
 * compose_key_list_generator.py script.
 */
#include <stdio.h>

/* This compose array lists all the characters needed to get each result, that
 * is listed after the special string "::"
 */
static const char* compose_map[] = {
%s
};
"""
    keyarray = ""

    for c in combinations.keys():
        if not len(combinations[c]):
            print combinations[c]
            print "No way to make "+c.encode('utf-8')
            continue

        for k in combinations[c]:
            k = escape_char(k)

            keyarray += "\"%s\", " % k

        keyarray += "\"::\", \"%s\"" % escape_char(c)
        keyarray += ", // %s\n" % descriptions[c]

    keyarray += "\"\\0\" //NULL so we know when to end"

    output = keycode % (keyarray)

    out = file(output_file, "w")
    out.write(output.encode('utf-8'))
    out.close()


if not os.path.exists(input_file):
    print("Missing argument, you need to pass a Compose.pre file to this!")
    sys.exit()

print "Parsing "+input_file+", saving it to "+output_file+"..."
composefile = codecs.open(input_file, 'r', 'utf-8')

for line in composefile:
    parts = re.match(keys_match, line)
    if not parts:
        continue

    keybinding = parts.group("keybinding")
    if not keybinding:
        print "Error while matching keybinding in line\n\t"+line

    char = parts.group("char")
    keys = re.compile("<([^>]+)>").findall(keybinding)
    desc = parts.group("desc")
    if parts.group("charcode"):
        desc = parts.group("charcode") + " | " + desc

    # Ignore the group if it contains unknown values
    if key_list_has_invalid_keys(keys):
        continue

    # Ignore Multi_key
    if keys[0] == "Multi_key":
        keys.pop(0)

    # Convert dead keys to the relative key character
    old_keys = keys
    keys = [dead_to_char_map[k] if k in dead_to_char_map else k for k in keys]
    has_dead_key = (old_keys != keys)

    # Convert keys to KeySym values and then to the corresponding unicode char
    keys = [unichr(Gdk.keyval_to_unicode(Gdk.keyval_from_name(k))) for k in keys]

    # Ignoring keys where we have invalid values
    if (u'\x00' in keys):
        continue

    # Join the chars to get the keys joined together
    keys = u"".join(keys)

    # If the sequence is already known, we remove it from other combinations
    # or we ignore it depending if this was a dead key or not
    ignore_key = False
    for c in combinations.values():
        if keys in c:
            # Give priority to the non-dead key combo
            if has_dead_key:
                ignore_key = True
            else:
                c.remove(keys)

    if ignore_key:
        continue

    if not char in combinations:
        combinations[char] = [keys]
    else:
        if not keys in combinations[char]:
            combinations[char] += [keys]

    if not char in descriptions:
        descriptions[char] = desc

composefile.close();
make_c_output(output_file)

