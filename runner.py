import subprocess
import os

if __name__ == '__main__':
    data, _ = os.pipe()
    locs = ['Atlanta', 'Berlin', 'Boston', 'Champaign', 'Cincinnati', 'Denver', 'NYC', 'Philadelphia', 'Roanoke', 'SanFrancisco', 'Toronto', 'UKansasState', 'UMissouri']
    for loc in locs:
        for seed in range(1, 11):
            trial = f'./main.out -inst {loc}.tsp -alg LS2 -time 20 -seed {seed}'
            subprocess.check_output(trial, stdin=data, shell=True)
