""" SETUP  1. attach function generator signal at 1.000000 Hz to pin 8,
              but do not turn on output
           2. load RTC_drift.ino on teensy device
           3. reset teensy
           4. run this script
           5. turn on function generator output
"""
import glob, time
import serial
from NTP_timestamp_generator import gen_ntp

BAUDRATE = 9600
port = glob.glob("/dev/ttyACM*")[0]

ser = serial.Serial(port, baudrate = BAUDRATE)

def write_out(in_line,t_cpu,t_ntp):
    out_line = "%s,%0.6f,%0.6f" % (in_line.strip(), t_cpu, t_ntp)
    output_file.write(out_line)
    output_file.write("\n")
    output_file.flush()
    print(out_line)

try:
    output_file = open("RTC_drift_data.txt",'w')
    #wait for first line to be sent
    in_line = ser.readline()
    t0_cpu   = time.time()     #set the zero-time
    lt, cst, crt = gen_ntp.next()
    t0_ntp = cst #use the latency corrected request sent time
    write_out(in_line,0,0)
    while True:
        in_line = ser.readline()
        t1_cpu  = time.time()
        lt, cst, crt = gen_ntp.next()
        t1_ntp = cst #use the latency corrected request sent time
        write_out(in_line,t1_cpu-t0_cpu,t1_ntp-t0_ntp)
        
except KeyboardInterrupt:
    pass
finally:
    output_file.close()
