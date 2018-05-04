// MAVLink router and firewall.
// Copyright (C) 2018  Michael R. Shannon <mrshannon.aerospace@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include <boost/filesystem.hpp>

#include "Filesystem.hpp"


// GCC generates a seemingly uncallable destructor for pure virtual classes.
// Therefore, it must be excluded from test coverage.
// LCOV_EXCL_START
Filesystem::~Filesystem()
{
}
// LCOV_EXCL_STOP


/** Returns true if the given path exists.
 *
 *  \param p The path to check for existence.
 */
bool Filesystem::exists(const Filesystem::path &p) const
{
    return boost::filesystem::exists(p);
}
