/*
 * Copyright 2012 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Andrea Azzarone <azzaronea@gmail.com>
 *
 */

#include "NuxCore/ColorPrivate.h"
using namespace nux::color;

#include <gmock/gmock.h>
using namespace testing;

namespace
{

TEST(TestColorPrivate, TestIsValidHex)
{
  EXPECT_TRUE(IsValidHex("#abcdef"));
  EXPECT_TRUE(IsValidHex("abcdef"));
  EXPECT_TRUE(IsValidHex("#ABCDEF"));
  EXPECT_TRUE(IsValidHex("ABCDEF"));

  EXPECT_FALSE(IsValidHex(""));
  EXPECT_FALSE(IsValidHex("#"));
  EXPECT_FALSE(IsValidHex("#abcde"));
  EXPECT_FALSE(IsValidHex("#abcdefa"));
  EXPECT_FALSE(IsValidHex("abcde"));
  EXPECT_FALSE(IsValidHex("abcdefa"));
  EXPECT_FALSE(IsValidHex("#abcdeg"));
}


TEST(TestColorPrivate, TestHexToInt)
{
  EXPECT_EQ(HexToInt("ab"), 0xab);
  EXPECT_EQ(HexToInt("AB"), 0xab);
  EXPECT_EQ(HexToInt("01"), 0x01);
  EXPECT_EQ(HexToInt("0a"), 0x0a);
}

TEST(TestColorPrivate, TestHexToRGBA)
{
  float r, g, b, a;

  EXPECT_FALSE(HexToRGBA("", r, g, b, a));
  EXPECT_FALSE(HexToRGBA("#ubuntu", r, g, b, a));

  EXPECT_TRUE(HexToRGBA("#abc", r, g, b, a));
  EXPECT_EQ(r, 0xaa / 255.0f);
  EXPECT_EQ(g, 0xbb / 255.0f);
  EXPECT_EQ(b, 0xcc / 255.0f);
  EXPECT_EQ(a, 1.0f);

  r = g = b = a = 0;
  EXPECT_TRUE(HexToRGBA("#abcd", r, g, b, a));
  EXPECT_EQ(r, 0xaa / 255.0f);
  EXPECT_EQ(g, 0xbb / 255.0f);
  EXPECT_EQ(b, 0xcc / 255.0f);
  EXPECT_EQ(a, 0xdd / 255.0f);

  r = g = b = a = 0;
  EXPECT_TRUE(HexToRGBA("#abcdef", r, g, b, a));
  EXPECT_EQ(r, 0xab / 255.0f);
  EXPECT_EQ(g, 0xcd / 255.0f);
  EXPECT_EQ(b, 0xef / 255.0f);
  EXPECT_EQ(a, 1.0f);

   r = g = b = a = 0;
  EXPECT_TRUE(HexToRGBA("#abcdefdd", r, g, b, a));
  EXPECT_EQ(r, 0xab / 255.0f);
  EXPECT_EQ(g, 0xcd / 255.0f);
  EXPECT_EQ(b, 0xef / 255.0f);
  EXPECT_EQ(a, 0xdd / 255.0f);

  r = g = b = a = 0;
  EXPECT_TRUE(HexToRGBA("abcdef", r, g, b, a));
  EXPECT_EQ(r, 0xab / 255.0f);
  EXPECT_EQ(g, 0xcd / 255.0f);
  EXPECT_EQ(b, 0xef / 255.0f);
}

}

