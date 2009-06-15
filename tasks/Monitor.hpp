#ifndef WIFIMON_MONITOR_TASK_HPP
#define WIFIMON_MONITOR_TASK_HPP

#include "wifimon/MonitorBase.hpp"
#include <rtt/Ports.hpp>

namespace wifimon {
    class Monitor : public MonitorBase
    {
	friend class MonitorBase;

    protected:
        int iw_com_fd;
        typedef std::map<std::string, RTT::OutputPort<Status>*> OutMap;
        OutMap output_ports;

        bool configureHook();
        void updateHook();
        void stopHook();

        static int createOutputPorts(int skfd, char *ifname, char *args[], int count);
        static int updateOutputPorts(int skfd, char *ifname, char *args[], int count);

    public:
        Monitor(std::string const& name = "wifimon::Monitor");
    };
}

#endif

