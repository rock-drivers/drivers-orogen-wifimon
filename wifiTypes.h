#ifndef WIFIMON_TYPES_HH
#define WIFIMON_TYPES_HH

#ifndef __orogen
#include <stdint.h>
#include <string.h>
#endif

#include <base/Time.hpp>

namespace wifimon {
    struct Status {
        base::Time time;

        //! true if the interface is associated
        uint8_t associated;
        //! the currently associate AP
        uint8_t ap[6]; 
        //! the bit rate in bps
        uint32_t rate;

        // link quality, in [0, 1]
        float quality;

        // total packet drop count at the MAC level
        uint16_t drops;

#ifndef __orogen
        Status()
        {
            memset(this, 0, sizeof(Status));
        }
#endif
    };
}

#endif

