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

#pragma once

#include <array>

#include "Functions.h"

template <class ValueType, std::size_t N>
struct EnumMapper : public std::array<std::pair<ValueType, std::string_view>, N>
{
  using ValueNamePair = std::pair<ValueType, std::string_view>;

  constexpr std::optional<ValueType> getValue(const std::string_view name) const
  {
    const auto it =
        std::find_if(this->begin(),
                     this->end(),
                     [&name](const ValueNamePair &pair) { return pair.second == name; });

    if (it == this->end())
      return {};

    return it->first;
  }

  constexpr std::string_view getName(const ValueType &value) const
  {
    const auto it =
        std::find_if(this->begin(),
                     this->end(),
                     [&value](const ValueNamePair &pair) { return pair.first == value; });

    if (it == this->end())
      return {};

    return it->second;
  }

  constexpr std::optional<ValueType> getValueCaseInsensitive(const std::string_view name) const
  {
    const auto compareToNameLowercase = [&name](const ValueNamePair &valueNamePair)
    {
      if (name.length() != valueNamePair.second.length())
        return false;
      for (std::size_t i = 0; i < name.length(); ++i)
      {
        if (std::tolower(name.at(i)) != std::tolower(valueNamePair.second.at(i)))
          return false;
      }
      return true;
    };

    const auto it = std::find_if(this->begin(), this->end(), compareToNameLowercase);
    if (it == this->end())
      return {};

    return it->first;
  }

  std::optional<ValueType> getValueFromNameOrIndex(const std::string_view nameOrIndex) const
  {
    if (auto index = functions::toUnsigned(nameOrIndex))
      if (*index < N)
        return this->at(*index).first;

    return this->getValue(nameOrIndex);
  }

  constexpr size_t indexOf(const ValueType value) const
  {
    const auto it =
        std::find_if(this->begin(),
                     this->end(),
                     [&value](const ValueNamePair &pair) { return pair.first == value; });

    if (it == this->end())
      throw std::logic_error(
          "The given type T was not registered in the mapper. All possible enums must be mapped.");

    const auto index = std::distance(this->begin(), it);
    return index;
  }

  constexpr std::optional<ValueType> getValueAt(const size_t index) const
  {
    if (index >= N)
      return {};
    return this->at(index).first;
  }

  constexpr std::array<ValueType, N> getValues() const
  {
    std::array<ValueType, N> values{};
    for (std::size_t i = 0; i < N; ++i)
      values[i] = this->at(i).first;
    return values;
  }

  constexpr std::array<std::string_view, N> getNames() const
  {
    std::array<std::string_view, N> names{};
    for (std::size_t i = 0; i < N; ++i)
      names[i] = this->at(i).second;
    return names;
  }
};
