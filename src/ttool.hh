#ifndef __TTOOL_H__
#define __TTOOL_H__

#include <string>

#include "ttool_exports.hh"
#include "config.hh"

namespace ttool
{
    class TTOOL_API TTool
    {
    public:
        TTool(std::string config_file)
            : m_Config(config_file)
        {};
        ~TTool(){};

        void Run();


    public:
        ttool::Config& GetConfig() { return m_Config; };

    private:
        ttool::Config m_Config;
    };
}

#endif // __TTOOL_H__