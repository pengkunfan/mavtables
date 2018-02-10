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


#ifndef RECURSIONDATA_HPP_
#define RECURSIONDATA_HPP_


#include <mutex>
#include <set>
#include <thread>


/** A data structure used by RecursionGuard to detect unwanted recursion.
 */
class RecursionData
{
        friend class RecursionGuard;

    private:
        std::set<std::thread::id> calling_threads_;
        std::mutex mutex_;
    public:
        RecursionData() = default;
        RecursionData(RecursionData &&other)
        {
            std::lock_guard<std::mutex> lock(other.mutex_);
            calling_threads_ = std::move(other.calling_threads_);
        }
};


#endif // RECURSIONDATA_HPP_
