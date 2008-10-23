#ifndef WIFIMON_TYPES_HH
#define WIFIMON_TYPES_HH

namespace wifimon {
    struct Status {
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

