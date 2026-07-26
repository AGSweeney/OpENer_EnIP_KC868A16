// EXCERPT — source: components/opener/src/ports/nvdata/nvtcpip.h
// EVIDENCE: E1 | symbol: NvTcpipLoad | lines: 18-20

#ifndef _NVTCPIP_H_
#define _NVTCPIP_H_

#include "typedefs.h"
#include "ciptcpipinterface.h"

EipStatus NvTcpipLoad(CipTcpIpObject *p_tcp_ip);
EipStatus NvTcpipStore(const CipTcpIpObject *p_tcp_ip);

#endif
