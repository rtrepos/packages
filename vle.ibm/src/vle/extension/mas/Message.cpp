/*
 * @file vle/extension/mas/Message.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2013-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <vle/extension/mas/Message.hpp>

namespace vle {
namespace extension {
namespace mas {

const std::string Message::BROADCAST = "BROADCAST";

Message::Message(const std::string& sender,
                 const std::string& receiver,
                 const std::string& subject)
:mSender(sender),mReceiver(receiver),mSubject(subject)
{}

}}}//namespace vle extension mas