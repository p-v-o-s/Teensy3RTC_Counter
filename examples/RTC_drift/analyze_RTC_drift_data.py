import time
import numpy as np
from scipy.signal import medfilt
from matplotlib import pyplot as plt
import statsmodels.api as sm

UPDATE_INTERVAL = 10
MEDFILT_KERNEL_SIZE = 3

#df = "Unit3_RTC_drift_28pF_no_comp.txt"
df = "RTC_drift_data.txt"
print "pulling from %s every %d s" % (df, UPDATE_INTERVAL)


try:
    plt.ion() #turn on interactive plotting
    fig = plt.figure()
    while True:
        #open the most recent datafile
        D = np.loadtxt(df,delimiter=",",comments="#")

        D = D[1:] #makes early plots look better

        t_ext = D[:,0]
        t_rtc = D[:,1]  #makes early plots look better
        t_mcu = D[:,2]
        t_cpu = medfilt(D[:,3], MEDFILT_KERNEL_SIZE)[:-MEDFILT_KERNEL_SIZE//2]         #get rid of nasty spikes
        t_ntp = medfilt(np.round(D[:,4]),MEDFILT_KERNEL_SIZE)[:-MEDFILT_KERNEL_SIZE//2]  #convert to seconds and get rid of nasty spikes
        t_ext2 = t_ext[:-MEDFILT_KERNEL_SIZE//2]
        #arrays for linear fit model
        X = t_ext.reshape((-1,1))
        y = t_rtc-t_ext
        model = sm.OLS(y, X)
        res = model.fit()
        print "\nupdating..."
        print 'Drift PPM: %0.10f +/- %0.10f' % (res.params[0]*1e6, res.bse[0]*1e6)
        #print res.summary()
       
        fig.clf()
        ax  = fig.add_subplot(111)
        ax.plot(t_ext, t_rtc-t_ext, "r.-", label = "RTC time diff.")
        ax.plot(X, res.fittedvalues, "k--", label = "RTC time diff fit")
        ax.plot(t_ext, t_mcu-t_ext, "m.-", label = "MCU time diff.")
        ax.plot(t_ext2, t_cpu-t_ext2, "c.-", label = "CPU time diff.")
        ax.plot(t_ext2, t_ntp-t_ext2, "g.-", label = "NTP time diff.")
        ax.legend(loc="upper left")
        #ax.set_ylim(-0.2,0.2)
        #show the graphs but allow program to continue
        plt.pause(UPDATE_INTERVAL)
except KeyboardInterrupt:
    pass
finally:
    plt.close('all')
