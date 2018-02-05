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


#include <memory>
#include <utility>
#include <optional>
#include <stdexcept>

#include "Action.hpp"
#include "Chain.hpp"
#include "Conditional.hpp"
#include "Packet.hpp"
#include "Accept.hpp"
#include "Reject.hpp"
#include "Call.hpp"
#include "GoTo.hpp"
#include "Rule.hpp"


/** Construct a blank rule.
 *
 *  The default behavior of this rule is to have no effect.  Therefore, unless
 *  \ref accept, \ref reject, \ref call, or \ref goto_ is called the result of
 *  \ref action will always be \ref Action::CONTINUE.
 *
 *  Some examples of how to use the default constructor is:
 *  - `auto rule = Rule().accept();`
 *  - `auto rule = Rule().reject();`
 *  - `auto rule = Rule().call(gcs_in);`
 *  - `auto rule = Rule().goto(autopilot_out);`
 *
 *  And with a conditional:
 *  - `auto rule = Rule().accept().type("PING").from("1.0/8").to("255.0");`
 *  - `auto rule = Rule().reject().type("HEARTBEAT").from("255.0/8");`
 *  - `auto rule = Rule().call(gcs_in).type("SET_MODE").to("255.0/8");`
 *  - `auto rule = Rule().goto(autopilot_out).from("255.0/8");`
 *
 *  \sa accept
 *  \sa reject
 *  \sa call
 *  \sa goto_
 */
Rule::Rule()
{
}


/** Copy constructor.
 *
 * \param other Rule to copy.
 */
Rule::Rule(const Rule &other)
    : action_(other.action_->clone()), condition_(other.condition_),
      priority_(other.priority_)
{
}


/** Construct a rule from and action and optional condition.
 *
 *  \param action The action the rule should take if matches a given
 *      packet/address combination with \ref check.
 *  \param condition The \ref Conditional that must match the packet/address
 *      combination for the rule to apply.  The default is {}, which indicates
 *      that any packet/address combination should match.
 *  \throws std::invalid_argument if the given action pointer is nullptr.
 */
Rule::Rule(std::unique_ptr<Action> action, std::optional<Conditional> condition)
    : action_(std::move(action)), condition_(std::move(condition))
{
    if (action_ == nullptr)
    {
        throw std::invalid_argument("Given Action pointer is null.");
    }
}


/** Set the \ref Rule's action to the \ref Accept action.
 *
 *  \returns A reference to itself.
 */
Rule &Rule::accept()
{
    action_ = std::make_unique<Accept>();
    return *this;
}


/** Set the \ref Rule's action to the \ref Reject action.
 *
 *  \returns A reference to itself.
 */
Rule &Rule::reject()
{
    action_ = std::make_unique<Reject>();
    return *this;
}


/** Set the \ref Rule's action to the \ref Call action.
 *
 *  \param chain The chain to delegate decisions of whether to accept or reject
 *      a packet to.  A nullptr is not a valid input.
 *  \returns A reference to itself.
 *  \throws std::invalid_argument if the given pointer is nullptr.
 */
Rule &Rule::call(std::shared_ptr<Chain> chain)
{
    action_ = std::make_unique<Call>(chain);
    return *this;
}


/** Set the \ref Rule's action to the \ref GoTo action.
 *
 *  \param chain The chain to delegate decisions of whether to accept or reject
 *      a packet to.  A nullptr is not a valid input.
 *  \returns A reference to itself.
 *  \throws std::invalid_argument if the given pointer is nullptr.
 */
Rule &Rule::goto_(std::shared_ptr<Chain> chain)
{
    action_ = std::make_unique<GoTo>(chain);
    return *this;
}


/** Set the priority that will be applied to any packet that matches the rule.
 *
 *  \note A higher priority packet will be routed before a lower priority
 *      packet.
 *
 *  \param priority The priority applied to matching packets.
 *  \returns A reference to itself.
 */
Rule &Rule::with_priority(int priority)
{
    priority_ = priority;
    return *this;
}


/** Set the conditional to the default (match everything).
 *
 *  This is used to add a conditional and return a reference to it so the packet
 *  type, from subnet, and to subnet can be set.  If the rule has a conditional
 *  this will replace it.
 *
 *  \returns A reference to the \ref Rule's internal conditional.
 *  \sa Conditional::type
 *  \sa Conditional::from
 *  \sa Conditional::to
 */
Conditional &Rule::if_()
{
    condition_ = std::make_optional<Conditional>();
    return condition_.value();
}


/** \copydoc Action::action(const Packet &,const MAVAddress &,RecursionChecker &) const
 *
 *  If the rule matches then the priority set on the rule (if any) will be
 *  applied to the \p packet.
 *
 *  \note If the packet and destination address combination does not match the
 *      \ref Rule's condition (see \ref Conditional::check) then \ref
 *      Action::CONTINUE will be returned.
 *
 *  \note If the rule does not have a \ref Conditional then any packet/address
 *      combination will match.
 *
 *  \note If the action has not been set then the return value will always be
 *      Action::CONTINUE.
 *
 *  \sa Conditional::check
 */
Action::Option Rule::action(
    Packet &packet, const MAVAddress &address,
    RecursionChecker &recursion_checker) const
{
    if (action_)
    {
        if (!condition_ || (condition_ && condition_->check(packet, address)))
        {
            if (priority_)
            {
                packet.priority(priority_.value());
            }

            return action_->action(packet, address, recursion_checker);
        }
    }

    return Action::CONTINUE;
}


/** Assignment operator.
 *
 * \param other Rule to copy.
 */
Rule &Rule::operator=(const Rule &other)
{
    action_ = other.action_->clone();
    condition_ = other.condition_;
    priority_ = other.priority_;
    return *this;
}


/** Equality comparison.
 *
 *  \relates Rule
 *  \param lhs The left hand side Rule.
 *  \param rhs The right hand side Rule.
 *  \retval true if \p lhs and \p rhs are equivalent rules.
 *  \retval false if \p lhs and \p rhs are not equivalent rules.
 */
bool operator==(const Rule &lhs, const Rule &rhs)
{
    bool result = lhs.condition_ == rhs.condition_ &&
                  lhs.priority_ == rhs.priority_;

    if (lhs.action_ && rhs.action_)
    {
        return result && (*lhs.action_ == *rhs.action_);
    }
    else if (lhs.action_ == nullptr && rhs.action_ == nullptr)
    {
        return result;
    }

    return false;
}


/** Inequality comparison.
 *
 *  \relates Rule
 *  \param lhs The left hand side Rule.
 *  \param rhs The right hand side Rule.
 *  \retval true if \p lhs and \p rhs are not equivalent rules.
 *  \retval false if \p lhs and \p rhs are are equivalent rules.
 */
bool operator!=(const Rule &lhs, const Rule &rhs)
{
    return !(lhs == rhs);
}


/** Print the conditional to the given output stream.
 *
 *  Some examples are:
 *      - `accept if PING`
 *      - `reject if HEARTBEAT from 192.168`
 *      - `call gcs_in if SET_MODE to 172.16`
 *      - `goto autopilot_out if from 172.0/8 to 192.0/8 `
 *
 *  \note If the action has not been set then nothing is printed.
 *
 *  \relates Rule
 *  \param os The output stream to print to.
 *  \param rule The rule to print.
 *  \return The output stream.
 */
std::ostream &operator<<(std::ostream &os, const Rule &rule)
{
    if (rule.action_)
    {
        os << *(rule.action_);

        if (rule.condition_)
        {
            os << " " << rule.condition_.value();
        }
    }

    return os;
}
