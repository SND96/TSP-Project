from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from Plotter import Plotter


class BoxPlot(Plotter):
    def __init__(self, outdir, target_loc, algo, solpath="DATA/solutions.csv",
                 line_alpha=0.7, line_width=1.2, tick_color='0.25', bgc='0.90',
                 fc='0.60', title_color='0.15', xfmt='{x:,.2f}',
                 yfmt='{x:,.1f}', grid_style='dotted'):
        """
        params:
        - outdir: directory to pull files from (ex: 'output')
        - target_loc: data location (ex: 'Atlanta')
        - algo: algorithm (ex: 'LS1')
        - solpath: path to solution file
        - line_alpha: plot line opacity
        - line_width: plot line width
        - tick_color: axis tick color
        - bgc: background color
        - fc: foreground color
        - title_color: title color
        - xfmt: x-axis tick label format
        - yfmt: y-axis tick label format
        - grid_style: grid line style
        """
        # get trace paths from given outdir, target_loc, algo
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

    def build(self, sq=0.0, title=None, yax_label=f'Runtime (CPU sec)',
              should_show=False, should_save=False, save_path=None, ):
        """
        params:
        - sq: solution quality threshold; default optimal 0.0
        - title: default '<target_loc> Runtimes with <sq> Solution Quality'
        - yax_label: y-axis label
        - should_show: displays plot
        - should_save: saves plot to save_path
        - save_path: path to save plot
        """
        # get best runtime of given solution quality for each trial
        groups = self.df_trials[self.df_trials.RE <= sq].groupby(['Trial'])
        rts = groups.RT.max()

        if title is None:
            title = f'{self.target_loc} Runtimes with {sq} Solution Quality'

        self.boxplot(rts, yax_label=yax_label, title=title,
                     should_save=should_save, save_path=save_path,
                     should_show=should_show)


if __name__ == '__main__':
    """
    Example: boxplot for simulated annealing NYC results
    """
    BP = BoxPlot('tmp', 'NYC', 'LS1')
    BP.build(sq=0.2, should_show=True)
