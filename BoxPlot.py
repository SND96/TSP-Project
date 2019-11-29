from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from Plotter import Plotter


class BoxPlot(Plotter):
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

    def build(self, title=None, should_show=True,
              should_save=False, save_path=None, yax_label=f'Runtime (CPU sec)'):
        # get runtimes for each trial and send to plotter
        rts = self.df_trials.groupby(['Trial']).RT.max()
        if title is None:
            title = f'Runtimes for Last Best Solution ({self.target_loc})'
        self.boxplot(rts, yax_label=yax_label, title=title, should_save=should_save,
                     save_path=save_path, should_show=should_show)


if __name__ == '__main__':
    BP = BoxPlot("tmp")
    BP.build()
