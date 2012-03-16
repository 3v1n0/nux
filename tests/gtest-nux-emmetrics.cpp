#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/EMMetrics.h"

namespace {
  TEST(TestMetrics, TestCreate)
  {
    // Test fallback (no display) of constructor, DPI of 96.0 will be used
    nux::EMMetrics* metrics5  = new nux::EMMetrics(NULL, 0,  5.0);
    nux::EMMetrics* metrics10 = new nux::EMMetrics(NULL, 0, 10.0);
    nux::EMMetrics* metrics15 = new nux::EMMetrics(NULL, 0, 15.0);
    nux::EMMetrics* metrics20 = new nux::EMMetrics(NULL, 0, 20.0);

    // Test conversion from pixel to EM (testing fallback-case) 5-point font
    EXPECT_EQ(metrics5->Pixel2EM(30), 4.5);
    EXPECT_EQ(metrics5->Pixel2EM(25), 3.75);
    EXPECT_EQ(metrics5->Pixel2EM(20), 3.0);
    EXPECT_EQ(metrics5->Pixel2EM(15), 2.25);
    EXPECT_EQ(metrics5->Pixel2EM(10), 1.5);
    EXPECT_EQ(metrics5->Pixel2EM(5),  0.75);
    EXPECT_EQ(metrics5->Pixel2EM(0),  0.0);

    // Test conversion from EM to pixel (testing fallback-case) 5-point font
    EXPECT_EQ(metrics5->EM2Pixel(4.5),  30);
    EXPECT_EQ(metrics5->EM2Pixel(3.75), 25);
    EXPECT_EQ(metrics5->EM2Pixel(3.0),  20);
    EXPECT_EQ(metrics5->EM2Pixel(2.25), 15);
    EXPECT_EQ(metrics5->EM2Pixel(1.5),  10);
    EXPECT_EQ(metrics5->EM2Pixel(0.75),  5);
    EXPECT_EQ(metrics5->EM2Pixel(0.0),   0);

    // Test conversion from pixel to EM (testing fallback-case) 10-point font
    EXPECT_EQ(metrics10->Pixel2EM(30), 2.25);
    EXPECT_EQ(metrics10->Pixel2EM(25), 1.875);
    EXPECT_EQ(metrics10->Pixel2EM(20), 1.5);
    EXPECT_EQ(metrics10->Pixel2EM(15), 1.125);
    EXPECT_EQ(metrics10->Pixel2EM(10), 0.75);
    EXPECT_EQ(metrics10->Pixel2EM(5),  0.375);
    EXPECT_EQ(metrics10->Pixel2EM(0),  0.0);

    // Test conversion from EM to pixel (testing fallback-case) 10-point font
    EXPECT_EQ(metrics10->EM2Pixel(2.25),  30);
    EXPECT_EQ(metrics10->EM2Pixel(1.875), 25);
    EXPECT_EQ(metrics10->EM2Pixel(1.5),   20);
    EXPECT_EQ(metrics10->EM2Pixel(1.125), 15);
    EXPECT_EQ(metrics10->EM2Pixel(0.75),  10);
    EXPECT_EQ(metrics10->EM2Pixel(0.375),  5);
    EXPECT_EQ(metrics10->EM2Pixel(0.0),    0);

    // Test conversion from pixel to EM (testing fallback-case) 15-point font
    EXPECT_EQ(metrics15->Pixel2EM(30), 1.5);
    EXPECT_EQ(metrics15->Pixel2EM(25), 1.25);
    EXPECT_EQ(metrics15->Pixel2EM(20), 1.0);
    EXPECT_EQ(metrics15->Pixel2EM(15), 0.75);
    EXPECT_EQ(metrics15->Pixel2EM(10), 0.5);
    EXPECT_EQ(metrics15->Pixel2EM(5),  0.25);
    EXPECT_EQ(metrics15->Pixel2EM(0),  0.0);

    // Test conversion from EM to pixel (testing fallback-case) 15-point font
    EXPECT_EQ(metrics15->EM2Pixel(1.5),  30);
    EXPECT_EQ(metrics15->EM2Pixel(1.25), 25);
    EXPECT_EQ(metrics15->EM2Pixel(1.0),  20);
    EXPECT_EQ(metrics15->EM2Pixel(0.75), 15);
    EXPECT_EQ(metrics15->EM2Pixel(0.5),  10);
    EXPECT_EQ(metrics15->EM2Pixel(0.25),  5);
    EXPECT_EQ(metrics15->EM2Pixel(0.0),   0);

    // Test conversion from pixel to EM (testing fallback-case) 20-point font
    EXPECT_EQ(metrics20->Pixel2EM(30), 1.125);
    EXPECT_EQ(metrics20->Pixel2EM(25), 0.9375);
    EXPECT_EQ(metrics20->Pixel2EM(20), 0.75);
    EXPECT_EQ(metrics20->Pixel2EM(15), 0.5625);
    EXPECT_EQ(metrics20->Pixel2EM(10), 0.375);
    EXPECT_EQ(metrics20->Pixel2EM(5),  0.1875);
    EXPECT_EQ(metrics20->Pixel2EM(0),  0.0);

    // Test conversion from EM to pixel (testing fallback-case) 20-point font
    EXPECT_EQ(metrics20->EM2Pixel(1.125),  30);
    EXPECT_EQ(metrics20->EM2Pixel(0.9375), 25);
    EXPECT_EQ(metrics20->EM2Pixel(0.75),   20);
    EXPECT_EQ(metrics20->EM2Pixel(0.5625), 15);
    EXPECT_EQ(metrics20->EM2Pixel(0.375),  10);
    EXPECT_EQ(metrics20->EM2Pixel(0.1875),  5);
    EXPECT_EQ(metrics20->EM2Pixel(0.0),     0);

    delete metrics5;
    delete metrics10;
    delete metrics15;
    delete metrics20;
  }
} // unnamed namespace

