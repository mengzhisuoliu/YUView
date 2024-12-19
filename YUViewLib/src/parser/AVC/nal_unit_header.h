/*  This file is part of YUView - The YUV player with advanced analytics toolset
 *   <https://github.com/IENT/YUView>
 *   Copyright (C) 2015  Institut f�r Nachrichtentechnik, RWTH Aachen University, GERMANY
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

#pragma once

#include <common/EnumMapper.h>
#include <parser/common/SubByteReaderLogging.h>

namespace parser::avc
{

// All the different NAL unit types (T-REC-H.265-201504 Page 85)
enum class NalType
{
  UNSPECIFIED,
  CODED_SLICE_NON_IDR,
  CODED_SLICE_DATA_PARTITION_A,
  CODED_SLICE_DATA_PARTITION_B,
  CODED_SLICE_DATA_PARTITION_C,
  CODED_SLICE_IDR,
  SEI,
  SPS,
  PPS,
  AUD,
  END_OF_SEQUENCE,
  END_OF_STREAM,
  FILLER,
  SPS_EXT,
  PREFIX_NAL,
  SUBSET_SPS,
  DEPTH_PARAMETER_SET,
  RESERVED_17,
  RESERVED_18,
  CODED_SLICE_AUX,
  CODED_SLICE_EXTENSION,
  CODED_SLICE_EXTENSION_DEPTH_MAP,
  RESERVED_22,
  RESERVED_23
};

constexpr EnumMapper<NalType, 24> NalTypeMapper = {
    std::make_pair(NalType::UNSPECIFIED, "UNSPECIFIED"),
    std::make_pair(NalType::CODED_SLICE_NON_IDR, "CODED_SLICE_NON_IDR"),
    std::make_pair(NalType::CODED_SLICE_DATA_PARTITION_A, "CODED_SLICE_DATA_PARTITION_A"),
    std::make_pair(NalType::CODED_SLICE_DATA_PARTITION_B, "CODED_SLICE_DATA_PARTITION_B"),
    std::make_pair(NalType::CODED_SLICE_DATA_PARTITION_C, "CODED_SLICE_DATA_PARTITION_C"),
    std::make_pair(NalType::CODED_SLICE_IDR, "CODED_SLICE_IDR"),
    std::make_pair(NalType::SEI, "SEI"),
    std::make_pair(NalType::SPS, "SPS"),
    std::make_pair(NalType::PPS, "PPS"),
    std::make_pair(NalType::AUD, "AUD"),
    std::make_pair(NalType::END_OF_SEQUENCE, "END_OF_SEQUENCE"),
    std::make_pair(NalType::END_OF_STREAM, "END_OF_STREAM"),
    std::make_pair(NalType::FILLER, "FILLER"),
    std::make_pair(NalType::SPS_EXT, "SPS_EXT"),
    std::make_pair(NalType::PREFIX_NAL, "PREFIX_NAL"),
    std::make_pair(NalType::SUBSET_SPS, "SUBSET_SPS"),
    std::make_pair(NalType::DEPTH_PARAMETER_SET, "DEPTH_PARAMETER_SET"),
    std::make_pair(NalType::RESERVED_17, "RESERVED_17"),
    std::make_pair(NalType::RESERVED_18, "RESERVED_18"),
    std::make_pair(NalType::CODED_SLICE_AUX, "CODED_SLICE_AUX"),
    std::make_pair(NalType::CODED_SLICE_EXTENSION, "CODED_SLICE_EXTENSION"),
    std::make_pair(NalType::CODED_SLICE_EXTENSION_DEPTH_MAP, "CODED_SLICE_EXTENSION_DEPTH_MAP"),
    std::make_pair(NalType::RESERVED_22, "RESERVED_22"),
    std::make_pair(NalType::RESERVED_23, "RESERVED_23")};

class nal_unit_header
{
public:
  nal_unit_header()  = default;
  ~nal_unit_header() = default;
  void parse(reader::SubByteReaderLogging &reader);

  unsigned nal_ref_idc{};

  NalType  nal_unit_type;
  unsigned nalUnitTypeID;
};

} // namespace parser::avc
