from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from Plotter import Plotter


class QRTD(Plotter):
    def __init__(self, outdir, solpath="DATA/solutions.csv", xcnt=100,
                 line_alpha=0.7, line_width=1.2, tick_color='0.25', bgc='0.90',
                 fc='0.60', title_color='0.15', xfmt='{x:,.2f}',
                 yfmt='{x:,.0f}', grid_style=None):
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

        # calculate solution qualities for data
        self.df_trials['SQ'] = (self.df_trials.Value-self.opt)/self.opt

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

    def plot_qrtd(self, sq_cuts=np.array([0.3, 0.4, 0.5, 0.6]),
                  should_show=True, should_save=False, save_path=None,
                  labels=None, xax_label='run-time (CPU sec)',
                  yax_label='P(solve)', colors=None, title=None):
        x = np.sort(self.df_trials.RT.unique())
        X = np.array([x.copy() for _ in range(sq_cuts.shape[0])])
        Y = np.zeros((sq_cuts.shape[0], X.shape[1]))
        for i, sq_cut in enumerate(sq_cuts):
            sqm = self.df_trials.SQ <= sq_cut
            for j, rt_cut in enumerate(x):
                rtm = self.df_trials.RT <= rt_cut
                Y[i, j] = self.df_trials[sqm & rtm].Trial.unique().shape[0]
        Y /= self.df_trials.Trial.unique().shape[0]

        if should_show or should_save:
            if labels is None:
                labels = [f'{sq}%' for sq in sq_cuts]

            if title is None:
                title = f'Qualified RTD ({self.target_loc})'

            self.plot(X, Y, labels=labels, xax_label=xax_label,
                      yax_label=yax_label, colors=colors, title=title,
                      should_show=should_show, should_save=should_save,
                      save_path=save_path)


if __name__ == '__main__':
    print(f'Testing QRTD Plotter...')
    plotter = QRTD("tmp")
    plotter.plot_qrtd()
