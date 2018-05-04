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


#include <optional>
#include <ostream>
#include <utility>

#include "Rule.hpp"


/** Base constructor for Rule classes.
 *
 *  \param condition The condition used to determine if the rule matches a
 *      particular packet/address combination given to the \ref action method.
 *      The default is {} which indicates the rule matches any packet/address
 *      combination.
 *  \sa action
 */
Rule::Rule(std::optional<If> condition)
    : condition_(std::move(condition))
{
}


// GCC generates a seemingly uncallable destructor for pure virtual classes.
// Therefore, it must be excluded from test coverage.
// LCOV_EXCL_START
Rule::~Rule()
{
}
// LCOV_EXCL_STOP


/** Print the given rule to the given output stream.
 *
 *  \note This is a polymorphic print, it will work on any child of \ref Rule
 *      even if the pointer/reference is to the base class \ref Rule.
 *
 *  Some examples are:
 *      - `accept`
 *      - `accept with priority 3`
 *      - `reject if ENCAPSULATED_DATA`
 *      - `call gcs_in if from 192.168`
 *      - `goto autopilot_out`
 *
 *  \relates Rule
 *  \param os The output stream to print to.
 *  \param rule The rule (or any child of \ref Rule) to print.
 *  \returns The output stream.
 */
std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
    return rule.print_(os);
}
