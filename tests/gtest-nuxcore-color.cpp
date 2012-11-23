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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include <gmock/gmock.h>

#include "NuxCore/Color.h"

using namespace testing;

static const float epsilon = 0.005f;

namespace {

  TEST(TestColor, TestColorConstructor0)
  {
    nux::Color c;

    EXPECT_EQ(c.red,    0.0f);
    EXPECT_EQ(c.green,  0.0f);
    EXPECT_EQ(c.blue,   0.0f);
    EXPECT_EQ(c.alpha,  1.0);
    EXPECT_EQ(c.IsPremultiplied(), false);
  }

  TEST(TestColor, TestColorConstructor1)
  {
    // testing float inputs
    nux::Color c0(0.1f, 0.2f, 0.3f);
    EXPECT_EQ(c0.red,    0.1f);
    EXPECT_EQ(c0.green,  0.2f);
    EXPECT_EQ(c0.blue,   0.3f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c0.IsPremultiplied(), false);

    // testing float inputs
    nux::Color c1(0.1f, 0.2f, 0.3f, 0.4f);
    EXPECT_EQ(c1.red,    0.1f);
    EXPECT_EQ(c1.green,  0.2f);
    EXPECT_EQ(c1.blue,   0.3f);
    EXPECT_EQ(c1.alpha,  0.4f);
    EXPECT_EQ(c1.IsPremultiplied(), false);

    // testing double inputs
    nux::Color c2(0.1, 0.2, 0.3, 0.4);
    EXPECT_EQ(c2.red,    0.1f);
    EXPECT_EQ(c2.green,  0.2f);
    EXPECT_EQ(c2.blue,   0.3f);
    EXPECT_EQ(c2.alpha,  0.4f);
    EXPECT_EQ(c2.IsPremultiplied(), false);
  }

  TEST(TestColor, TestColorConstructor2)
  {
    nux::Color c0(255, 255, 255);
    EXPECT_EQ(c0.red,    1.0f);
    EXPECT_EQ(c0.green,  1.0f);
    EXPECT_EQ(c0.blue,   1.0f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c0.IsPremultiplied(), false);

    nux::Color c1(10, 128, 192);
    EXPECT_EQ(c1.red,    10/255.0f);
    EXPECT_EQ(c1.green,  128/255.0f);
    EXPECT_EQ(c1.blue,   192/255.0f);
    EXPECT_EQ(c1.alpha,  1.0f);
    EXPECT_EQ(c1.IsPremultiplied(), false);
  }

  TEST(TestColor, TestColorConstructor3)
  {
    nux::Color c0((255<<24)|(255<<16)|(255<<8)|255);
    EXPECT_EQ(c0.red,    1.0f);
    EXPECT_EQ(c0.green,  1.0f);
    EXPECT_EQ(c0.blue,   1.0f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c0.IsPremultiplied(), false);
    
    //             A         R         G       B
    nux::Color c1((128<<24)|(192<<16)|(128<<8)|10);
    EXPECT_EQ(c1.red,    192/255.0f);
    EXPECT_EQ(c1.green,  128/255.0f);
    EXPECT_EQ(c1.blue,   10/255.0f);
    EXPECT_EQ(c1.alpha,  128/255.0f);
    EXPECT_EQ(c1.IsPremultiplied(), false);
  }

  
  TEST(TestColor, TestColorConstructorHex)
  {
    nux::Color c0("#abc");
    EXPECT_EQ(c0.red,    0xaa / 255.0f);
    EXPECT_EQ(c0.green,  0xbb / 255.0f);
    EXPECT_EQ(c0.blue,   0xcc / 255.0f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c0.IsPremultiplied(), false);

    nux::Color c1("#abcd");
    EXPECT_EQ(c1.red,    0xaa / 255.0f);
    EXPECT_EQ(c1.green,  0xbb / 255.0f);
    EXPECT_EQ(c1.blue,   0xcc / 255.0f);
    EXPECT_EQ(c1.alpha,  0xdd / 255.0f);
    EXPECT_EQ(c1.IsPremultiplied(), false);

    nux::Color c2("#aabbcc");
    EXPECT_EQ(c2.red,    0xaa / 255.0f);
    EXPECT_EQ(c2.green,  0xbb / 255.0f);
    EXPECT_EQ(c2.blue,   0xcc / 255.0f);
    EXPECT_EQ(c2.alpha,  1.0f);
    EXPECT_EQ(c2.IsPremultiplied(), false);


    nux::Color c3("#aabbccdd");
    EXPECT_EQ(c3.red,    0xaa / 255.0f);
    EXPECT_EQ(c3.green,  0xbb / 255.0f);
    EXPECT_EQ(c3.blue,   0xcc / 255.0f);
    EXPECT_EQ(c3.alpha,  0xdd / 255.0f);
    EXPECT_EQ(c3.IsPremultiplied(), false);
    
    nux::Color c4("abcdef");
    EXPECT_EQ(c4.red,    0xab / 255.0f );
    EXPECT_EQ(c4.green,  0xcd / 255.0f);
    EXPECT_EQ(c4.blue,   0xef / 255.0f);
    EXPECT_EQ(c4.alpha,  1.0f);
    EXPECT_EQ(c4.IsPremultiplied(), false);
  }

  TEST(TestColor, TestColorConstructorHexInvalid)
  {
    nux::Color c0("");
    EXPECT_EQ(c0.red,    0.0f);
    EXPECT_EQ(c0.green,  0.0f);
    EXPECT_EQ(c0.blue,   0.0f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c0.IsPremultiplied(), false);
    
    nux::Color c1("#abcdeG");
    EXPECT_EQ(c0.red,    0.0f);
    EXPECT_EQ(c0.green,  0.0f);
    EXPECT_EQ(c0.blue,   0.0f);
    EXPECT_EQ(c0.alpha,  1.0f);
    EXPECT_EQ(c1.IsPremultiplied(), false);
  }


  TEST(TestColor, TestColorPremultiplied0)
  {
    nux::Color c0(0.81f, 0.24f, 0.53f, 0.79f);
    EXPECT_EQ(c0.red,    0.81f);
    EXPECT_EQ(c0.green,  0.24f);
    EXPECT_EQ(c0.blue,   0.53f);
    EXPECT_EQ(c0.alpha,  0.79f);
    EXPECT_EQ(c0.IsPremultiplied(), false);

    nux::Color c1 = c0.GetPremultiplied();
    EXPECT_EQ(c1.red,    0.81f * 0.79f);
    EXPECT_EQ(c1.green,  0.24f * 0.79f);
    EXPECT_EQ(c1.blue,   0.53f * 0.79f);
    EXPECT_EQ(c1.alpha,  0.79f);
    EXPECT_EQ(c1.IsPremultiplied(), true);
    EXPECT_EQ(c0.IsPremultiplied(), false);
  }

  TEST(TestColor, TestColorRGBToHSV0)
  {
    nux::color::RedGreenBlue rgb(255/255.0f, 128/255.0f, 169/255.0f);
    nux::color::HueSaturationValue hsv(rgb);

    EXPECT_NEAR(hsv.hue,          341/360.0f, epsilon);
    EXPECT_NEAR(hsv.saturation,   50/100.0f, epsilon);
    EXPECT_NEAR(hsv.value,        100/100.0f, epsilon);
  }

  TEST(TestColor, TestColorHSVToRGB0)
  {
    nux::color::HueSaturationValue hsv(341/360.0f, 50/100.0f, 100/100.0f);
    nux::color::RedGreenBlue rgb(hsv);

    EXPECT_NEAR(rgb.red,        255/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      128/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       169/255.0f, epsilon);
  }

  TEST(TestColor, TestColorRGBToHSV1)
  {
    nux::color::RedGreenBlue rgb(65/255.0f, 28/255.0f, 6/255.0f);
    nux::color::HueSaturationValue hsv(rgb);

    EXPECT_NEAR(hsv.hue,          22/360.0f, epsilon);
    EXPECT_NEAR(hsv.saturation,   91/100.0f, epsilon);
    EXPECT_NEAR(hsv.value,        25/100.0f, epsilon);
  }

  TEST(TestColor, TestColorHSVToRGB1)
  {
    nux::color::HueSaturationValue hsv(22/360.0f, 91/100.0f, 25/100.0f);
    nux::color::RedGreenBlue rgb(hsv);

    EXPECT_NEAR(rgb.red,        65/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      28/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       6/255.0f, epsilon);
  }

  TEST(TestColor, TestColorRGBToHSV2)
  {
    nux::color::RedGreenBlue rgb(90/255.0f, 65/255.0f, 158/255.0f);
    nux::color::HueSaturationValue hsv(rgb);

    EXPECT_NEAR(hsv.hue,          256/360.0f, epsilon);
    EXPECT_NEAR(hsv.saturation,   59/100.0f, epsilon);
    EXPECT_NEAR(hsv.value,        62/100.0f, epsilon);
  }


  TEST(TestColor, TestColorHSVToRGB2)
  {
    nux::color::HueSaturationValue hsv(256/360.0f, 59/100.0f, 62/100.0f);
    nux::color::RedGreenBlue rgb(hsv);

    EXPECT_NEAR(rgb.red,        90/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      65/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       158/255.0f, epsilon);
  }

  TEST(TestColor, TestColorRGBToHLS0)
  {
    nux::color::RedGreenBlue rgb(127/255.0f, 0/255.0f, 255/255.0f);
    nux::color::HueLightnessSaturation hls(rgb);

    EXPECT_NEAR(hls.hue,          270/360.0f, epsilon);
    EXPECT_NEAR(hls.lightness,    50/100.0f, epsilon);
    EXPECT_NEAR(hls.saturation,   100/100.0f,  epsilon);
  }

  TEST(TestColor, TestColorHLSToRGB0)
  {
    nux::color::HueLightnessSaturation hls(270/360.0f, 50/100.0f, 100/100.0f);
    nux::color::RedGreenBlue rgb(hls);

    EXPECT_NEAR(rgb.red,        127/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      0/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       255/255.0f, epsilon);
  }

  TEST(TestColor, TestColorRGBToHLS1)
  {
    nux::color::RedGreenBlue rgb(50/255.0f, 84/255.0f, 13/255.0f);
    nux::color::HueLightnessSaturation hls(rgb);

    EXPECT_NEAR(hls.hue,          89/360.0f, epsilon);
    EXPECT_NEAR(hls.lightness,    19/100.0f, epsilon);
    EXPECT_NEAR(hls.saturation,   73/100.0f, epsilon);
  }

  TEST(TestColor, TestColorHLSToRGB1)
  {
    nux::color::HueLightnessSaturation hls(89/360.0f, 19/100.0f, 73/100.0f);
    nux::color::RedGreenBlue rgb(hls);

    EXPECT_NEAR(rgb.red,        50/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      84/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       13/255.0f, epsilon);
  }

  TEST(TestColor, TestColorRGBToHLS2)
  {
    nux::color::RedGreenBlue rgb(201/255.0f, 200/255.0f, 239/255.0f);
    nux::color::HueLightnessSaturation hls(rgb);

    EXPECT_NEAR(hls.hue,          242/360.0f, epsilon);
    EXPECT_NEAR(hls.lightness,    86/100.0f,  epsilon);
    EXPECT_NEAR(hls.saturation,   55/100.0f,  epsilon);
  }

  TEST(TestColor, TestColorHLSToRGB2)
  {
    nux::color::HueLightnessSaturation hls(242/360.0f, 86/100.0f, 55/100.0f);
    nux::color::RedGreenBlue rgb(hls);

    EXPECT_NEAR(rgb.red,        201/255.0f, epsilon);
    EXPECT_NEAR(rgb.green,      200/255.0f, epsilon);
    EXPECT_NEAR(rgb.blue,       239/255.0f, epsilon);
  }


TEST(TestColor, TestSubtraction)
{
  nux::Color start = nux::Color(1.0f, 0.0f, 0.0f);
  nux::Color finish = nux::Color(0.0f, 0.5f, 0.0f);
  nux::Color difference = finish - start;

  EXPECT_THAT(difference.red, FloatEq(-1));
  EXPECT_THAT(difference.green, FloatEq(0.5));
  EXPECT_THAT(difference.blue, FloatEq(0));
  EXPECT_THAT(difference.alpha, FloatEq(0));
}

TEST(TestColor, TestColorToRGB)
{
  nux::Color color = nux::Color(0.3f, 0.8f, 0.1f, 0.5f);
  nux::color::RedGreenBlue rgb = color;

  EXPECT_EQ(rgb.red, color.red);
  EXPECT_EQ(rgb.green, color.green);
  EXPECT_EQ(rgb.blue, color.blue);
}

TEST(TestColor, TestRandomColor)
{
    float rmin = 1.0f;
    float rmax = 0.0f;
    float gmin = 1.0f;
    float gmax = 0.0f;
    float bmin = 1.0f;
    float bmax = 0.0f;

    for (int i = 0; i < 100; ++i)
    {
        nux::Color c = nux::color::RandomColor();
        EXPECT_EQ(1.0f, c.alpha);
        rmin = std::min(rmin, c.red);
        rmax = std::max(rmax, c.red);
        gmin = std::min(gmin, c.green);
        gmax = std::max(gmax, c.green);
        bmin = std::min(bmin, c.blue);
        bmax = std::max(bmax, c.blue);
    }

    // Ultra unlikely that we create 100 identical values in a row.
    // The universe will end long before this happens.
    EXPECT_NE(rmin, rmax);
    EXPECT_NE(gmin, gmax);
    EXPECT_NE(bmin, bmax);

    // Values must have changed from the initial settings
    EXPECT_NE(1.0f, rmin);
    EXPECT_NE(0.0f, rmax);
    EXPECT_NE(1.0f, gmin);
    EXPECT_NE(0.0f, gmax);
    EXPECT_NE(1.0f, bmin);
    EXPECT_NE(0.0f, bmax);
}

}
