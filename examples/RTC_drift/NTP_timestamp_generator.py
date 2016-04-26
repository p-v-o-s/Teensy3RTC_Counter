from __future__ import print_function
import time, warnings, sys
from time import time as local_timestamp
from ntplib import NTPClient, NTPException


DEFAULT_SERVER = 'us.pool.ntp.org'
DEFAULT_TIMEOUT = 0.25

ntp_client = NTPClient()

def ntp_timestamp_generator(server = DEFAULT_SERVER,
                            timeout = DEFAULT_TIMEOUT,
                           ):
    #this function computes latency corrected timestamps
    def get_corrected_timestamps():
        local_send_time = local_timestamp()
        resp = ntp_client.request(server, timeout=timeout)
        local_recv_time = local_timestamp()
        local_time = (local_send_time + local_recv_time)/2.0
        latency = (local_recv_time - local_send_time)/2.0
        corrected_send_time = resp.recv_timestamp - latency
        corrected_recv_time = resp.tx_timestamp   + latency
        return (local_time, corrected_send_time, corrected_recv_time)
    #initialize the time cache
    last_lt, last_cst, last_crt = get_corrected_timestamps()
    while True:
        try:
            last_lt, last_cst, last_crt = get_corrected_timestamps()
            yield (last_lt, last_cst, last_crt)
        except NTPException, err:
            lt = local_timestamp()
            dt = lt - last_lt - timeout #correct for timeout
            print("WARNING: in %s caught \"%s\"" % (__name__,err), file=sys.stderr)
            last_lt, last_cst, last_crt = (lt, last_cst + dt, last_crt + dt)
            yield (last_lt, last_cst, last_crt)
            
            
gen_ntp = ntp_timestamp_generator()

