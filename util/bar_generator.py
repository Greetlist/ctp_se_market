import pandas as pd
import numpy as np
import glob
import os
import datetime as dt

class BarGenerator:
    def __init__(self, input_dir, output_dir):
        self.input_dir = input_dir
        self.output_dir = output_dir
        self.time_range = [
            #morning
            {'start_time': '09:00:00', 'end_time': '09:15:00'},
            {'start_time': '09:15:00', 'end_time': '09:30:00'},
            {'start_time': '09:30:00', 'end_time': '09:45:00'},
            {'start_time': '09:45:00', 'end_time': '10:00:00'},
            {'start_time': '10:00:00', 'end_time': '10:15:00'},
    
            # rest 15 min
            {'start_time': '10:30:00', 'end_time': '10:45:00'},
            {'start_time': '10:45:00', 'end_time': '11:00:00'},
            {'start_time': '11:00:00', 'end_time': '11:15:00'},
            {'start_time': '11:15:00', 'end_time': '11:30:00'},
    
            #afternoon
            {'start_time': '13:30:00', 'end_time': '13:45:00'},
            {'start_time': '13:45:00', 'end_time': '14:00:00'},
            {'start_time': '14:00:00', 'end_time': '14:15:00'},
            {'start_time': '14:15:00', 'end_time': '14:30:00'},
            {'start_time': '14:30:00', 'end_time': '14:45:00'},
            {'start_time': '14:45:00', 'end_time': '15:00:00'},
    
            # night, for most cta instrument
            {'start_time': '21:00:00', 'end_time': '21:15:00'},
            {'start_time': '21:15:00', 'end_time': '21:30:00'},
            {'start_time': '21:30:00', 'end_time': '21:45:00'},
            {'start_time': '21:45:00', 'end_time': '22:00:00'},
            {'start_time': '22:00:00', 'end_time': '22:15:00'},
            {'start_time': '22:15:00', 'end_time': '22:30:00'},
            {'start_time': '22:30:00', 'end_time': '22:45:00'},
            {'start_time': '22:45:00', 'end_time': '23:00:00'},
    
            # for metals
            {'start_time': '23:00:00', 'end_time': '23:15:00'},
            {'start_time': '23:15:00', 'end_time': '23:30:00'},
            {'start_time': '23:30:00', 'end_time': '23:45:00'},
            {'start_time': '23:45:00', 'end_time': '00:00:00'},
            {'start_time': '00:00:00', 'end_time': '00:15:00'},
            {'start_time': '00:15:00', 'end_time': '00:30:00'},
            {'start_time': '00:30:00', 'end_time': '00:45:00'},
            {'start_time': '00:45:00', 'end_time': '01:00:00'},
    
            # for au/ag/sc 
            {'start_time': '01:00:00', 'end_time': '01:15:00'},
            {'start_time': '01:15:00', 'end_time': '01:30:00'},
            {'start_time': '01:30:00', 'end_time': '01:45:00'},
            {'start_time': '01:45:00', 'end_time': '02:00:00'},
            {'start_time': '02:00:00', 'end_time': '02:15:00'},
            {'start_time': '02:15:00', 'end_time': '02:30:00'},
        ]

    def start(self):
        for f in glob.glob(os.path.join(self.input_dir, '*')):
            self.generate_single_file(f)

    def generate_single_file(self, file_path):
        date = dt.datetime.now().strftime('%Y-%m-%d')
        df = pd.read_csv(
            file_path,
            usecols=['Uid', 'LastPrice', 'VendorUpdateTime', 'VendorTimeMilliSeconds'],
            dtype={'LastPrice': float, 'VendorTimeMilliSeconds': int}
        )
        if len(df) == 0:
            print('[{}] is empty'.format(file_path))
            return
        inst_code = list(df['Uid'])[0]
        res_item_list = []
        for item in self.time_range:
            slice_df = df[
                (df['VendorUpdateTime'] >= item['start_time']) \
                & (df['VendorUpdateTime'] < item['end_time'])
            ]
            if len(slice_df) == 0:
                #print('Time Range: [{}] data is empty'.format(item))
                continue
            price_arr = np.array(slice_df['LastPrice'])
            res_item = {
                'Uid': inst_code,
                'StartTime': item['start_time'],
                'EndTime': item['end_time'],
                'High': np.max(price_arr),
                'Open': price_arr[0],
                'Low': np.min(price_arr),
                'Close': price_arr[-1],
            }
            res_item_list.append(res_item)
        res_df = pd.DataFrame(res_item_list)
        final_output_dir = os.path.join(self.output_dir, date.replace('-', '/'))
        os.makedirs(final_output_dir, exist_ok=True)
        res_df.to_csv(os.path.join(final_output_dir, '{}.csv'.format(inst_code)), index=False)
