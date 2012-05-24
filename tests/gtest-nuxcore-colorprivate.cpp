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
  EXPECT_TRUE(impl::IsValidHex("#abcdef"));
  EXPECT_TRUE(impl::IsValidHex("abcdef"));
  EXPECT_TRUE(impl::IsValidHex("#ABCDEF"));
  EXPECT_TRUE(impl::IsValidHex("ABCDEF"));

  EXPECT_FALSE(impl::IsValidHex(""));
  EXPECT_FALSE(impl::IsValidHex("#"));
  EXPECT_FALSE(impl::IsValidHex("#abcde"));
  EXPECT_FALSE(impl::IsValidHex("#abcdefa"));
  EXPECT_FALSE(impl::IsValidHex("abcde"));
  EXPECT_FALSE(impl::IsValidHex("abcdefa"));
  EXPECT_FALSE(impl::IsValidHex("#abcdeg"));
}


TEST(TestColorPrivate, TestHexToDec)
{
  EXPECT_EQ(impl::HexToDec("ab"), 0xab);
  EXPECT_EQ(impl::HexToDec("AB"), 0xab);
  EXPECT_EQ(impl::HexToDec("01"), 0x01);
  EXPECT_EQ(impl::HexToDec("0a"), 0x0a);
}

TEST(TestColorPrivate, TestHexToRGB)
{
  float r, g, b;

  EXPECT_FALSE(impl::HexToRGB("", r, g, b));
  EXPECT_FALSE(impl::HexToRGB("#ubuntu", r, g, b));

  EXPECT_TRUE(impl::HexToRGB("abcdef", r, g, b));
  EXPECT_EQ(r, 0xab / 255.0f);
  EXPECT_EQ(g, 0xcd / 255.0f);
  EXPECT_EQ(b, 0xef / 255.0f);

  r = g = b = 0;
  EXPECT_TRUE(impl::HexToRGB("#abcdef", r, g, b));
  EXPECT_EQ(r, 0xab / 255.0f);
  EXPECT_EQ(g, 0xcd / 255.0f);
  EXPECT_EQ(b, 0xef / 255.0f);
}

}

