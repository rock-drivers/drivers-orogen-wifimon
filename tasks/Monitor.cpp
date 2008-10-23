#include "Monitor.hpp"
#include <iwlib.h>
#include <rtt/Logger.hpp>

using namespace wifimon;
using namespace RTT;
using namespace std;

Monitor::Monitor(std::string const& name)
    : MonitorBase(name)
{
}

int Monitor::createOutputPorts(int skfd, char *ifname, char *args[], int count)
{
    wireless_info info;
    if(iw_get_basic_config(skfd, ifname, &(info.b)) < 0)
    {
        log(Info) << ifname << " is not a wireless interface" << endlog();
        return 1;
    }

    log(Info) << "monitoring " << ifname << endlog();
    Monitor* task = (Monitor*) args;
    WriteDataPort<Status>* port =  new WriteDataPort<Status>(ifname);
    task->output_ports.insert( make_pair(string(ifname), port));
    task->ports()->addPort(port);
    return 1;
}

bool Monitor::configureHook()
{
    if((iw_com_fd = iw_sockets_open()) < 0)
        return false;
    iw_enum_devices(iw_com_fd, &Monitor::createOutputPorts, (char**)this, 0);

    if (output_ports.empty())
        log(Warning) << "no wireless interfaces to monitor" << endlog();

    return true;
}

int Monitor::updateOutputPorts(int skfd, char *ifname, char *args[], int count)
{
    iwreq wrq;
    wireless_info info;
    memset(&info, 0, sizeof(info));
    if(iw_get_basic_config(skfd, ifname, &(info.b)) < 0)
        return 1;

    Monitor* task = (Monitor*) args;
    WriteDataPort<Status>* port = task->output_ports[ifname];
    if (! port)
    {
        log(Warning) << ifname << " is not monitored" << endlog();
        return 1;
    }

    Status status;

    if(iw_get_range_info(skfd, ifname, &(info.range)) >= 0)
        info.has_range = 1;

    if(iw_get_ext(skfd, ifname, SIOCGIWAP, &wrq) >= 0)
    {
        uint8_t null_ap[6]  = { 0, 0, 0, 0, 0, 0 };
        uint8_t bcast_ap[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        uint8_t hack_ap[6]  = { 0x44, 0x44, 0x44, 0x44, 0x44, 0x44 };
        memcpy(&(status.ap), &(wrq.u.ap_addr.sa_data), 6);
        if (memcmp(null_ap, status.ap, sizeof(status.ap)) &&
                memcmp(bcast_ap, status.ap, sizeof(status.ap)) &&
                memcmp(hack_ap, status.ap, sizeof(status.ap)))
            status.associated = true;
    }

    if(iw_get_ext(skfd, ifname, SIOCGIWRATE, &wrq) >= 0)
        status.rate = wrq.u.bitrate.value;

    if(iw_get_stats(skfd, ifname, &(info.stats),
                &info.range, info.has_range) >= 0)
    {
        if (!(info.stats.qual.updated & IW_QUAL_QUAL_INVALID))
        {
            status.quality      = static_cast<float>(info.stats.qual.qual) / info.range.max_qual.qual;
        }
        status.drops        =
            info.stats.discard.fragment +
            info.stats.discard.retries +
            info.stats.discard.misc +
            info.stats.miss.beacon;
    }


    port->Set(status);

    return 1;
}


void Monitor::updateHook()
{
    iw_enum_devices(iw_com_fd, &Monitor::updateOutputPorts, (char**)this, 0);
}

void Monitor::stopHook()
{
    while (!output_ports.empty())
    {
        delete output_ports.begin()->second;
        output_ports.erase(output_ports.begin());
    }
    iw_sockets_close(iw_com_fd);
}

