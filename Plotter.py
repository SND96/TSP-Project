import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import StrMethodFormatter
from matplotlib import cm


class Plotter:
    def __init__(self, line_alpha=0.7, line_width=1.2, tick_color='0.25',
                 bgc='0.90', fc='0.60', title_color='0.15',
                 xfmt='{x:,.2f}', yfmt='{x:,.1f}', grid_style=None,
                 xtickcnt=7):
        """
        params:
        - line_alpha: plot line opacity
        - line_width: plot line width
        - tick_color: axis tick color
        - bgc: background color
        - fc: foreground color
        - title_color: title color
        - xfmt: x-axis tick label format
        - yfmt: y-axis tick label format
        - grid_style: grid line style
        - xtickcnt: number of x-axis major ticks
        """
        self.line_alpha = line_alpha
        self.line_width = line_width
        self.tick_color = tick_color
        self.bgc = bgc
        self.fc = fc
        self.title_color = title_color
        self.xfmt = StrMethodFormatter(xfmt)
        self.yfmt = StrMethodFormatter(yfmt)
        self.grid_style = grid_style
        self.xtickcnt = xtickcnt
        self._cmap = cm.get_cmap('cividis')

    def boxplot(self, X, yax_label=None, title=None, should_show=False,
                should_save=False, save_path=None):
        """
        params:
        - X: input vals for boxplot of length n
        - yax_label: y-axis label
        - title: plot title
        - should_show: display plot
        - should_save: saves plot to save_path
        - save_path: path to save plot
        """
        fig, ax = plt.subplots()
        ax.boxplot(X)

        if title:
            ax.set_title(title)

        if yax_label:
            ax.set_ylabel(yax_label, color=self.tick_color)

        ax.yaxis.set_major_formatter(self.yfmt)
        ax.tick_params(colors=self.tick_color)
        ax.tick_params(axis='x', which='both', bottom=False,
                       top=False, labelbottom=False)
        ax.set_facecolor(self.bgc)
        plt.setp(ax.spines.values(), color=self.fc)

        if should_save:
            plt.savefig(save_path, bbox_inches='tight')

        if should_show:
            plt.show()

        plt.clf()

    def plot(self, X, Y, labels=None, xax_label=None, yax_label=None,
             colors=None, title=None, should_show=False, should_save=False,
             save_path=None, xscale='log'):
        """
        params:
        - X: x-vals for n plot lines of length d (nxd)
        - Y: y-vals for n plot lines of length d (nxd)
        - labels: plot legend labels; defaults to 'plot <number>'
        - xax_label: x-axis label
        - yax_label: y-axis label
        - colors: plot line colors
        - title: plot title
        - should_show: displays plot
        - should_save: saves plot to save_path
        - save_path: path to save plot
        - xscale: x-axis scale; defaults to log
        """
        if labels is None:
            labels = [f'plot {i+1}' for i in range(Y.shape[0])]

        if colors is None:
            colors = self._get_colors(X.shape[0])

        fig, ax = plt.subplots()

        for x, y, label, c in zip(X, Y, labels, colors):
            ax.plot(x, y, label=label, alpha=self.line_alpha,
                    linewidth=self.line_width, color=c)

        if title:
            ax.set_title(title, color=self.title_color)

        if self.grid_style:
            ax.grid(linestyle=self.grid_style)

        if xax_label:
            ax.set_xlabel(xax_label, color=self.tick_color)

        if yax_label:
            ax.set_ylabel(yax_label, color=self.tick_color)

        # set x-axis scale and remove minor ticks for log scale
        if xscale == 'log':
            ax.set_xscale(xscale)
            plt.minorticks_off()

        ax.xaxis.set_major_formatter(self.xfmt)
        ax.yaxis.set_major_formatter(self.yfmt)

        ax.tick_params(colors=self.tick_color)
        ax.set_facecolor(self.bgc)
        plt.setp(ax.spines.values(), color=self.fc)

        leg = ax.legend()
        for txt in leg.get_texts():
            txt.set_color(self.tick_color)

        for leghand in leg.legendHandles:
            leghand.set_alpha(self.line_alpha)

        if should_save:
            plt.savefig(save_path, bbox_inches='tight')

        if should_show:
            plt.show()

        plt.clf()

    def _get_colors(self, count, scheme=None):
        if scheme is None:
            scheme = self._cmap
        return [scheme(r) for r in np.random.ranf(size=count)]
