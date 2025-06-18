import pandas as pd
import numpy as np
import argh
import glob
import os

# default args is 15 minute 
def gen_bar(input_dir=None, output_dir=None, period=15):
    if input_dir is None:
        print('Please provide input_dir')
        return

    for f in glob.glob(os.path.join(output_dir, '*')):
        last_price_arr = pd.read_csv(f, usecols=['LastPrice'])['LastPrice']
        data_slice =


if __name__ == '__main__':
    argh.dispatch_commands([
        gen_bar
    ])
