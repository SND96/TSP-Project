from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from PlotterForSQD import Plotter


class QRTD(Plotter):
    def __init__(self, outdir, target_loc, algo, solpath="DATA/solutions.csv",
                 line_alpha=0.7, line_width=1.2, tick_color='0.25', bgc='0.90',
                 fc='0.60', title_color='0.15', xfmt='{x:,.2f}',
                 yfmt='{x:,.1f}', grid_style='dotted'):
        """
        params:
        - outdir: output directory (ex 'output')
        - target_loc: target location (ex 'Atlanta')
        - algo: algorithm (ex 'LS1')
        - solpath: solution path
        - line_alpha: plot line opacity
        - line_width: plot line width
        - tick_color: axis tick color
        - bgc: background color
        - fc: foreground color
        - title_color: title color
        - xfmt: x-axis label format
        - yfmt: y-axis label format
        - grid_style: grid line style
        """
        # get trace paths from given output directory, target_loc, algo
        tps = [f for f in listdir(outdir) if isfile(join(outdir, f))]
        tps = [f for f in tps if f.endswith('.trace')]
        tps = [f for f in tps if f.startswith(f'{target_loc}_{algo}')]

        if len(tps) == 0:
            raise ValueError('outdir must contain trace files')

        # cutoff in secs
        self.cutoff = int(tps[0].split('_')[2])

        # get optimal value from provided solutions for trace instance
        df_sol = pd.read_csv(solpath)
        self._df_opt = df_sol[df_sol.Instance == target_loc]

        # get trace entries
        cols = ['Trial', 'RT', 'Value']
        self.df_trials = pd.DataFrame(columns=cols)
        for i, f in enumerate(tps):
            df_tmp = pd.read_csv(join(outdir, f), names=["RT", "Value"])
            df_tmp['Trial'] = i+1
            self.df_trials = pd.concat([self.df_trials, df_tmp], sort=True)

        self.df_trials = self.df_trials[cols]

        # calculate rel err for data
        self.df_trials['RE'] = (self.df_trials.Value-self.opt)/self.opt

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

    def build(self, times=np.array([0.1,0.3,1,3.2,10]), maxSolQualPerc = 20, solutQualPercInterval = 0.1, xscale=None,
              yax_label='P(solve)', xax_label='Relative Solution Quality (%)',
              title=None, labels=None, colors=['b', 'g', 'r', 'c'],
              should_show=False, should_save=False, save_path=None, linestyles = ['-','--','-.',':', '-']):
##        """
##        params:
##        - sqs: solution quality plot lines
##        - xscale: 'log' for log scale or defaults to linear (None)
##        - yax_label: y-axis label
##        - xax_label: x-axis label
##        - title: chart title, defaults to 'Qualified RTD (<target_loc>)'
##        - labels: plot line labels, defaults to <sqs>
##        - colors: plot line colors
##        - should_show: displays plot
##        - should_save: saves chart to save_path
##        - save_path: path to save
##        """
##        sqs = np.array(sqs)
##        x = np.sort(self.df_trials.RT.unique())
##        X = np.array([x.copy() for _ in range(sqs.shape[0])])
##        Y = np.zeros((sqs.shape[0], X.shape[1]))
##        for i, sq in enumerate(sqs):
##            sqm = self.df_trials.RE <= sq
##            for j, rt in enumerate(x):
##                rtm = self.df_trials.RT <= rt
##                Y[i, j] = self.df_trials[sqm & rtm].Trial.unique().shape[0]
##        Y /= self.df_trials.Trial.unique().shape[0]
##
##        if should_show or should_save:
##            if labels is None:
##                labels = [f'{sq*100:.1f}%' for sq in sqs]
##
##            if title is None:
##                title = f'Qualified RTD ({self.target_loc})'
##
##            self.plot(X, Y, labels=labels, xax_label=xax_label,
##                      yax_label=yax_label, colors=colors, title=title,
##                      should_show=should_show, should_save=should_save,
##                      save_path=save_path, xscale=xscale)

######################################
        #INPUTS
        #initialize time constraints
##        times = np.array([0.1,0.3,1,3.2,10]) #number of seconds for cutoff
##        maxSolQualPerc = 20 #in %
##        solutQualPercInterval = 0.1
######################################        
        #get each trial data
        dataNp = self.df_trials.to_numpy()
        lastInd = dataNp.shape[0]-1
        lastTrialNum = dataNp[lastInd, 0]

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
        x = np.arange(0,maxSolQualPerc, solutQualPercInterval)
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
                      yax_label=yax_label, colors=colors, title=title,
                      should_show=should_show, should_save=should_save,
                      save_path=save_path,xscale=xscale, linestyles = linestyles)

if __name__ == '__main__':
    """
    Example: QRTD plot for simulated annealing NYC results
    """
    City = 'Champaign'
    Algorithm = 'LS1'
    plotter = QRTD('output', City, Algorithm)
    #plotter.build(times=np.array([0.1,0.3,1, 3.2,10]), colors=['b', 'g', 'r', 'c', 'y'], linestyles = ['-','--','-.',':', '-'], maxSolQualPerc = 40, solutQualPercInterval = 0.1, should_show=True, xscale=None)
    plotter.build(title = 'SQD_' + City +'_' + Algorithm,times=np.array([0.1,0.6, 0.7, 1, 3.2,10]), colors=['g', 'r', 'c', 'y', 'b', 'g'], linestyles = ['--','-.',':', '-','-.',':'], maxSolQualPerc = 40, solutQualPercInterval = 0.1, should_show=True, xscale=None)
    #0.1, 0.6, 
