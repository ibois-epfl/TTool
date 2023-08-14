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