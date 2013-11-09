#include "VNC.h"
#include "VentNetworkCalc.h"

bool VNC( Digraph& dg, VNC_EdgeDataMap& ed, VNC_FanDataMap& fd,
          double totalQ, double precise, int maxCount )
{
    VentNetworkCalc vnc( dg, ed, fd );
    vnc.setTotalQ( totalQ );
    vnc.setPrecise( precise );
    vnc.setIterateCount( maxCount );

    return vnc.iterate();
}