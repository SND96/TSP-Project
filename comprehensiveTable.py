from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from Plotter import Plotter
from matplotlib.pylab import plt


class QRTD(Plotter):
    def __init__(self, outdir, solpath="DATA/solutions.csv", line_alpha=0.7,
                 line_width=1.2, tick_color='0.25', bgc='0.90',
                 fc='0.60', title_color='0.15', xfmt='{x:,.2f}',
                 yfmt='{x:,.1f}', grid_style='dotted'):
        # get trace files from given output directory
        trace_paths = [f for f in listdir(outdir) if isfile(join(outdir, f))]
        if len(trace_paths) == 0:
            raise ValueError('outdir must contain trace files')

        # TSP destination ex: 'Atlanta'
        target_loc = trace_paths[0].split('_')[0].capitalize()

        # cutoff in secs
        self.cutoff = int(trace_paths[0].split('_')[2])

        # get optimal value from provided solutions for trace instance
        df_sol = pd.read_csv(solpath)
        self._df_opt = df_sol[df_sol.Instance == target_loc]

        # get trace entries
        cols = ['Trial', 'RT', 'Value']
        self.df_trials = pd.DataFrame(columns=cols)
        for i, f in enumerate(trace_paths):
            df_tmp = pd.read_csv(join(outdir, f), names=["RT", "Value"])
            df_tmp['Trial'] = i+1
            self.df_trials = pd.concat([self.df_trials, df_tmp], sort=True)

        self.df_trials = self.df_trials[cols]

        # calculate rel err for data
        self.df_trials['RE'] = (self.df_trials.Value-self.opt)/self.opt


##        print(self.df_trials.to_numpy())
##        print(type(self.df_trials.to_numpy()))
        
        # init base plotter
        super().__init__(line_alpha=line_alpha, line_width=line_width,
                         tick_color=tick_color, bgc=bgc, fc=fc,
                         title_color=title_color, xfmt=xfmt, yfmt=yfmt,
                         grid_style=grid_style)

    @property
    def target_loc(self):
        # return target location of trials under consideration
        return self._df_opt.Instance.values[0]

    @property
    def opt(self):
        # return optimal value of location
        return self._df_opt.Value.values[0]

    def plot_qrtd(self, re_cuts=np.array([0.02, 0.04, 0.06, 0.08]),
                  should_show=True, should_save=False, save_path=None,
                  labels=None, xax_label='Runtime (CPU sec)',
                  yax_label='P(solve)', colors=['b', 'g', 'r', 'c'],
                  title=None):

        #get each trial data
        dataNp = self.df_trials.to_numpy()
        lastInd = dataNp.shape[0]-1
        lastTrialNum = dataNp[lastInd, 0]
        #initialize time constraints
        times = np.array([0.1,0.3,1,3.2,10])
        #initialize array to store relative quality
        relQuality = np.zeros((times.shape[0], lastTrialNum)) #times are rows, trial number are columns

        #add a small time delta to times column so it isnt mistaken as 
        dataNp[:,1]+= 0.000001
##        print("dataNp", dataNp)
        
        for i in range(1,lastTrialNum+1): #i goes from 1 to 10, not start from 0
            trialInd = np.argwhere(dataNp[:,0]==i)
            trialInd = np.squeeze(trialInd)
##            print(trial)
            trialData = dataNp[trialInd] #have data for a specific trial i where i is 1 to 10
##            print("trialData[:,3]")
##            print(trialData[:,3])

            #for a given time for each trial, find the first index <= time constraint
            for timeInd in range(times.shape[0]):
                boolCond = np.argwhere(trialData[:,1] <= times[timeInd])
##                print(boolCond)
                if boolCond.shape[0] == 0: #this checks if there was no solution that had less than the alotted time
                    relQuality[timeInd, i-1] = 10000.0 #assign some high relative error, so that when we compare to rsq in x axis to calculate appropriate p(solve, it will always fail) the threshold
                else:
                    indTime = boolCond[boolCond.shape[0]-1][0]
##                    print("indTime ", indTime)
                    rq = trialData[indTime, 3]
##                    print("rq", rq)
                    relQuality[timeInd, i-1] = rq
##        print("relQuality")
##        print(relQuality)

        #x = set of predecided relative solution qualities
        x = np.arange(0,5, 0.1)
        X = np.array([x.copy() for _ in range(times.shape[0])])
        Y = np.zeros((times.shape[0],x.shape[0]))
        for timeInd in range(times.shape[0]):
            for rqXInd in range(x.shape[0]):
                psolve = np.mean(relQuality[timeInd, :]*100 <=  x[rqXInd]) #multiplying by 100 since its % comparison
                Y[timeInd, rqXInd] = psolve
##        print("X ", X)
##        print("Y ", Y)
##        print(type(Y))

        if should_show or should_save:
            if labels is None:
                labels = [f'{re} s' for re in times]
##                print(labels)
            if title is None:
                title = f'Qualified RTD ({self.target_loc})'

            self.plot(X, Y, labels=labels, xax_label=xax_label,
                      yax_label=yax_label, colors=['b', 'g', 'r', 'c', 'r'], title="HI",
                      should_show=should_show, should_save=should_save,
                      save_path=save_path)

if __name__ == '__main__':
    #branch and bound table
    #load from the results folder, all the solution files with .trace (this is the only one
    #with execution times in it
    import glob, os
    os.chdir("output")
    for file in glob.glob("*.trace"):
        print(file)
        #for each solution file, append to list the solution quality,
        #the time, and relative error
    #save the list s a csv using np.savetxt
