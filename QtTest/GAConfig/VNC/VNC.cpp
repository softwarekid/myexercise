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

double VNC_H( Digraph& dg, VNC_EdgeDataMap& ed, VNC_FanDataMap& fd )
{
    return TotalH( dg, ed, fd );
}

void ClearAllVNCDatas( Digraph& dg, VNC_EdgeDataMap& ed, VNC_FanDataMap& fd )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        delete ed[e];
        ed[e] = 0;
        delete fd[e];
        fd[e] = 0;
    }
}
