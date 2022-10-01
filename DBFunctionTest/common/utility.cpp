#include "utility.h"

#include <iomanip>
#include <cstring>

namespace windcom
{
namespace util
{
    std::string Guid(void)
    {
        GUID guid;
#ifdef WIN32
        CoCreateGuid(&guid);
#else
        uuid_t uuid;
        uuid_generate(uuid);
        memcpy(&guid, uuid, sizeof(GUID));
#endif
        return GuidToString(guid);
    }


    std::string GuidToString(const GUID &guid)
    {
        char buf[64] = {0};
#ifndef _WIN32
        snprintf(
#else // _WIN32
        _snprintf_s(
#endif
            buf,
            sizeof(buf),
            "%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
        return std::string(buf);
    }
    
}
}
