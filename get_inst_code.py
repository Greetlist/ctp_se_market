import argh
import pandas as pd
import datetime as dt
import os

def extract():
    current_dir = os.path.dirname(__file__)
    date_str = dt.datetime.now().strftime("%Y%m%d")
    tianqi_csv = os.path.join(current_dir, "mmap", "{}.csv".format(date_str))
    df = pd.read_csv(tianqi_csv, usecols=["inst_code"])
    dst_csv = os.path.join(current_dir, "mmap", "inst_code.csv")
    df.to_csv(dst_csv, index=False)

if __name__ == '__main__':
    argh.dispatch_commands([
        extract
    ])
