/**
 * TTool
 * Copyright (C) 2023  Andrea Settimi, Naravich Chutisilp (IBOIS, EPFL)
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

#pragma once

namespace ttool
{
    enum class EventType
    {
        None = 0,
        ReadyLiveCamera,
        PoseInput,
        Tracking,
    };

    static int CvtTtoolStateToInt(ttool::EventType state)
    {
        switch (state)
        {
        case ttool::EventType::None:
            return 0;
        case ttool::EventType::PoseInput:
            return 1;
        case ttool::EventType::Tracking:
            return 2;
        default:
            return 0;
        }
    }
    static ttool::EventType CvtIntToTtoolState(int stateInt)
    {
        switch (stateInt)
        {
        case 0:
            return ttool::EventType::None;
        case 1:
            return ttool::EventType::PoseInput;
        case 2:
            return ttool::EventType::Tracking;
        default:
            return ttool::EventType::None;
        }
    }

}