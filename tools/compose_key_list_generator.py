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

DEFAULT_OUTPUT = "/tmp/parsed_keys.cpp"
input_file = sys.argv[1]
output_file = sys.argv[2] if len(sys.argv) > 2 else DEFAULT_OUTPUT

keys_match = re.compile("(?P<keybinding><[^:]+)[\s]*:[\s]*\"(\\\)?(?P<char>.*)\"[\s]*(?P<charcode>[^\s]+)?[\s]*#[\s]*(?P<desc>.+)")

unicode_keys = re.compile("^U[0-9A-Fa-f]{4,6}$");
blacklisted_keys = ["EZH", "ezh", "dead_double_grave", "dead_inverted_breve",
                    "dead_greek", "greaterthanequal", "lessthanequal",
                    "underbar", "rightarrow", "leftarrow"]

combinations = {u"": ["ubuntu", "circle"]}
descriptions = {u"": "UBUNTU CIRCLE"}


def key_list_has_invalid_keys(keys):
    if len([k for k in keys if unicode_keys.match(k)]):
        return True

    if 0 in [Gdk.keyval_from_name(k) for k in keys]:
        return True

    if len([k for k in keys if k in blacklisted_keys]):
        return True

    return False


def escape_char(char):
    escape_list = ["\\", "\""]
    for e in escape_list:
        char = char.replace(e, "\\"+e)

    return char


def make_cpp_output(output_file):
    keycode = """/* This file has been automatically generated using the
 * compose_key_list_generator.py script.
 */
#include <vector>
#include <X11/Xutils.h>

struct ComposeSequence
{
  std::vector<KeySym> symbols;
  const char* result;
};

static const ComposeSequence NUX_COMPOSE_SEQUENCIES[] = {
%s};
"""
    keyarray = ""

    for c in combinations.keys():
        if not len(combinations[c]):
            continue

        first = True
        for combo in combinations[c]:
            # Add 'XK_' prefix to key names
            combo = ["XK_" + k for k in combo]
            desc = " // " + descriptions[c] if first else ""
            keyarray += "  {{%s}, \"%s\"},%s\n" % (", ".join(combo), escape_char(c), desc)
            first = False

    output = keycode % (keyarray)

    out = file(output_file, "w")
    out.write(output.encode('utf-8'))
    out.close()


if not os.path.exists(input_file):
    print("Missing argument, you need to pass a Compose.pre file to this!")
    sys.exit()

print "Parsing "+input_file+", saving it to "+output_file+"..."
composefile = codecs.open(input_file, 'r', 'utf-8')

invalid = []

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

    # Ignoring unknown keys
    if 0 in [Gdk.keyval_from_name(k) for k in keys]:
        for k in keys:
            if Gdk.keyval_from_name(k) == 0 and k not in invalid:
                invalid.append(k)
                print k
        continue

    if not char in combinations:
        combinations[char] = [keys]
    else:
        if not keys in combinations[char]:
            combinations[char] += [keys]

    if not char in descriptions:
        descriptions[char] = desc

composefile.close();
make_cpp_output(output_file)
