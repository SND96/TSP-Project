from os import listdir
from os.path import isfile, join
import pandas as pd
import numpy as np
from Plotter import Plotter


class QRTD(Plotter):
    def __init__(self, outdir, target_loc, algo, solpath="DATA/solutions.csv",
                 line_alpha=0.7, line_width=1.2, tick_color='0.25', bgc='0.90',
                 fc='0.60', title_color='0.15', xfmt='{x:,.2f}',
                 yfmt='{x:,.1f}', grid_style='dotted'):
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

    def build(self, sqs=[0.02, 0.04, 0.06, 0.08], xscale=None,
              yax_label='P(solve)', xax_label='Runtime (CPU sec)',
              title=None, labels=None, colors=['b', 'g', 'r', 'c'],
              should_show=False, should_save=False, save_path=None):
        """
        params:
        - sqs: solution quality plot lines
        - xscale: 'log' for log scale or defaults to linear (None)
        - yax_label: y-axis label
        - xax_label: x-axis label
        - title: chart title, defaults to 'Qualified RTD (<target_loc>)'
        - labels: plot line labels, defaults to <sqs>
        - colors: plot line colors
        - should_show: displays plot
        - should_save: saves chart to save_path
        - save_path: path to save
        """
        sqs = np.array(sqs)
        x = np.sort(self.df_trials.RT.unique())
        X = np.array([x.copy() for _ in range(sqs.shape[0])])
        Y = np.zeros((sqs.shape[0], X.shape[1]))
        for i, sq in enumerate(sqs):
            sqm = self.df_trials.RE <= sq
            for j, rt in enumerate(x):
                rtm = self.df_trials.RT <= rt
                Y[i, j] = self.df_trials[sqm & rtm].Trial.unique().shape[0]
        Y /= self.df_trials.Trial.unique().shape[0]

        if should_show or should_save:
            if labels is None:
                labels = [f'{sq*100:.1f}%' for sq in sqs]

            if title is None:
                title = f'Qualified RTD ({self.target_loc})'

            self.plot(X, Y, labels=labels, xax_label=xax_label,
                      yax_label=yax_label, colors=colors, title=title,
                      should_show=should_show, should_save=should_save,
                      save_path=save_path, xscale=xscale)


if __name__ == '__main__':
    """
    Example: QRTD plot for simulated annealing NYC
    """
    plotter = QRTD('tmp', 'NYC', 'LS1')
    plotter.build(sqs=[0.14, 0.16, 0.18], should_show=True, xscale=None)
