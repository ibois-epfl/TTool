#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <glog/logging.h>

namespace tk {

inline bool IsFileExist (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

} // namespace tk