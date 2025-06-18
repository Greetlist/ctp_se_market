import pandas as pd
import numpy as np
import argh
import glob
import os

from bar_generator import *

# default args is 15 minute 
def gen_bar(input_dir=None, output_dir=None, period=15):
    if input_dir is None or output_dir is None:
        print('Please provide input_dir and output_dir')
        return
    bar_generator = BarGenerator(input_dir, output_dir)
    bar_generator.start()

if __name__ == '__main__':
    argh.dispatch_commands([
        gen_bar
    ])
