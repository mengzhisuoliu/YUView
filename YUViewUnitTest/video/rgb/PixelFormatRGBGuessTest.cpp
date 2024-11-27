/*  This file is part of YUView - The YUV player with advanced analytics toolset
 *   <https://github.com/IENT/YUView>
 *   Copyright (C) 2015  Institut für Nachrichtentechnik, RWTH Aachen University, GERMANY
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   In addition, as a special exception, the copyright holders give
 *   permission to link the code of portions of this program with the
 *   OpenSSL library under certain conditions as described in each
 *   individual source file, and distribute linked combinations including
 *   the two.
 *
 *   You must obey the GNU General Public License in all respects for all
 *   of the code used other than OpenSSL. If you modify file(s) with this
 *   exception, you may extend this exception to your version of the
 *   file(s), but you are not obligated to do so. If you do not wish to do
 *   so, delete this exception statement from your version. If you delete
 *   this exception statement from all source files in the program, then
 *   also delete it here.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <common/Testing.h>

#include <filesource/FormatGuessingParameters.h>
#include <video/rgb/PixelFormatRGBGuess.h>

namespace video::rgb::test
{

using filesource::frameFormatGuess::FileInfoForGuess;
using filesource::frameFormatGuess::GuessedFrameFormat;

struct TestParameters
{
  FileInfoForGuess   fileInfoForGuess;
  GuessedFrameFormat guessedFrameFormat;
  PixelFormatRGB     expectedPixelFormat{};
};

class GuessRGBFormatFromFilenameFrameSizeAndFileSize : public TestWithParam<TestParameters>
{
};

std::string getTestName(const testing::TestParamInfo<TestParameters> &testParametersInfo)
{
  const auto testParameters = testParametersInfo.param;
  auto       name = filesource::frameFormatGuess::test::formatFileInfoForGuessAndGuessedFrameFormat(
      testParameters.fileInfoForGuess, testParameters.guessedFrameFormat);
  name +=
      "_" + yuviewTest::replaceNonSupportedCharacters(testParameters.expectedPixelFormat.getName());
  return name;
}

TEST_P(GuessRGBFormatFromFilenameFrameSizeAndFileSize, TestGuess)
{
  const auto parameters = GetParam();

  const auto guessedRGBFormat = video::rgb::guessPixelFormatFromSizeAndName(
      parameters.guessedFrameFormat, parameters.fileInfoForGuess);

  EXPECT_TRUE(guessedRGBFormat.isValid());
  EXPECT_EQ(guessedRGBFormat, parameters.expectedPixelFormat);
}

constexpr auto BytesNoAlpha   = 1920u * 1080 * 12u * 3u; // 12 frames RGB
constexpr auto NotEnoughBytes = 22u;
constexpr auto UnfittingBytes = 1920u * 1080u * 5u;
constexpr auto BytesBayerFile = 512u * 768u * 4u * 12u; // 12 frames raw bayer

INSTANTIATE_TEST_SUITE_P(
    VideoRGBTest,
    GuessRGBFormatFromFilenameFrameSizeAndFileSize,
    Values(
        // Cases that should not detect anything
        TestParameters({FileInfoForGuess({"noIndicatorHere.yuv", "", 0}),
                        GuessedFrameFormat({Size(0, 0), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),

        // No Alpha
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rbg.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RBG)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_grb.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GRB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_gbr.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GBR)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_brg.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BRG)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_bgr.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BGR)}),

        // Alpha First
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_argb.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB, AlphaMode::First)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_arbg.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RBG, AlphaMode::First)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_agrb.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GRB, AlphaMode::First)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_agbr.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GBR, AlphaMode::First)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_abrg.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BRG, AlphaMode::First)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_abgr.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BGR, AlphaMode::First)}),

        // Alpha Last
        TestParameters({FileInfoForGuess({"something_1920x1080_rgba.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB, AlphaMode::Last)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rbga.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RBG, AlphaMode::Last)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_grba.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GRB, AlphaMode::Last)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_gbra.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::GBR, AlphaMode::Last)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_brga.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BRG, AlphaMode::Last)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_bgra.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::BGR, AlphaMode::Last)}),

        // Bit dephts
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb10.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(10, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb12.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(12, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb16.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(16, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb48.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(16, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb64.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(16, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb11.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),

        // Endianness
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb8le.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb8be.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb10le.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(10, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb10be.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(
                 10, DataLayout::Packed, ChannelOrder::RGB, AlphaMode::None, Endianness::Big)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb16be.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(
                 16, DataLayout::Packed, ChannelOrder::RGB, AlphaMode::None, Endianness::Big)}),

        // DataLayout
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb_packed.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb_planar.yuv", "", BytesNoAlpha}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Planar, ChannelOrder::RGB)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb10le_planar.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(10, DataLayout::Planar, ChannelOrder::RGB)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb10be_planar.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(
                 10, DataLayout::Planar, ChannelOrder::RGB, AlphaMode::None, Endianness::Big)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb16_planar.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(16, DataLayout::Planar, ChannelOrder::RGB)}),
        TestParameters(
            {FileInfoForGuess({"something_1920x1080_rgb16be_planar.yuv", "", BytesNoAlpha}),
             GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
             PixelFormatRGB(
                 16, DataLayout::Planar, ChannelOrder::RGB, AlphaMode::None, Endianness::Big)}),

        // File size check
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb10.yuv", "", NotEnoughBytes}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb16be.yuv", "", NotEnoughBytes}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),
        TestParameters({FileInfoForGuess({"something_1920x1080_rgb16be.yuv", "", UnfittingBytes}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB)}),

        // CMYK file
        TestParameters({FileInfoForGuess({"something_512x768.cmyk", "", BytesBayerFile}),
                        GuessedFrameFormat({Size(1920, 1080), {}, {}, {}}),
                        PixelFormatRGB(8, DataLayout::Packed, ChannelOrder::RGB, AlphaMode::Last)})

            ),
    getTestName);

} // namespace video::rgb::test
