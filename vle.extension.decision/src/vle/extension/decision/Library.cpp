/*
 * @file vle/extension/decision/Library.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/extension/decision/Library.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/utils/Tools.hpp>
#include <sstream>

namespace vle { namespace extension { namespace decision {

void Library::add(const std::string& name, const std::string& content)
{
    Plan p(ctx, mKb, content);

    std::pair < iterator, bool > r;
    r = mLst.insert(std::make_pair(name, p));

    if (not r.second) {
        throw utils::ArgError(
            vle::utils::format("Decision: plan library, plan `%s' already exists",
             name.c_str()));
    }
}

void Library::add(const std::string& name, std::istream& stream)
{
    Plan p(ctx, mKb, stream);

    std::pair < iterator, bool > r;
    r = mLst.insert(std::make_pair(name, p));

    if (not r.second) {
        throw utils::ArgError(
            vle::utils::format("Decision: plan library, plan `%s' already exists",
            name.c_str()));
    }
}

}}} // namespace vle model decision
