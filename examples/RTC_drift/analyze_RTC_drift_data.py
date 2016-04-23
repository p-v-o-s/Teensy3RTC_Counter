import time
import numpy as np
from matplotlib import pyplot as plt
from sklearn import linear_model

UPDATE_INTERVAL = 10


df = "RTC_drift_data.txt"
print "pulling from %s every %d s" % (df, UPDATE_INTERVAL)

try:
    plt.ion() #turn on interactive plotting
    fig = plt.figure()
    while True:
        #open the most recent datafile
        D = np.loadtxt(df,delimiter=",")

        D = D[1:] #makes early plots look better

        t_ext = D[:,0]
        t_rtc = D[:,1]  #makes early plots look better
        t_mic = D[:,2]
        #t_mic = np.where((t_mic - t_ext) < 0.0,t_mic + 4295-0.03286096,t_mic)
        t_cpu = D[:,3]
        t_ntp = np.round(D[:,4])
        print "updating"
        fig.clf()
        ax  = fig.add_subplot(111)
        ax.plot(t_ext, t_rtc-t_ext, "r.-", label = "RTC time diff.")
        ax.plot(t_ext, t_mic-t_ext, "m.-", label = "MCU time diff.")
        ax.plot(t_ext, t_cpu-t_ext, "c.-", label = "CPU time diff.")
        ax.plot(t_ext, t_ntp-t_ext, "g.-", label = "NTP time diff.")
        ax.legend(loc="upper left")
        #show the graphs but allow program to continue
        plt.pause(UPDATE_INTERVAL)
except KeyboardInterrupt:
    pass
finally:
    plt.close('all')
