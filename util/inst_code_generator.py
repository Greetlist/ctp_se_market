import datetime as dt

class InstCodeGenerator:
    def __init__(self):
        self.production_info_map = {
            # 能源
            'fu': {
                'deliver_month': '1-12'
            },
            'bu': {
                'deliver_month': ''
            },
            'pg': {
                'deliver_month': ''
            },
            'sc': {
                'deliver_month': ''
            },
            'lu': {
                'deliver_month': ''
            },
            #化工
            'MA': {
                'deliver_month': ''
            },
            'sp': {
                'deliver_month': ''
            },
            'SH': {
                'deliver_month': ''
            },
            'eb': {
                'deliver_month': ''
            },
            'pp': {
                'deliver_month': ''
            },
            'TA': {
                'deliver_month': ''
            },
            'PX': {
                'deliver_month': ''
            },
            'l': {
                'deliver_month': ''
            },
            'PR': {
                'deliver_month': ''
            },
            'eg': {
                'deliver_month': ''
            },
            'UR': {
                'deliver_month': ''
            },
            'ru': {
                'deliver_month': ''
            },
            'br': {
                'deliver_month': ''
            },
            'nr': {
                'deliver_month': ''
            },
            # 非金属建材
            'SA': {
                'deliver_month': ''
            },
            'FG': {
                'deliver_month': ''
            },
            'v': {
                'deliver_month': ''
            },
            'lg': {
                'deliver_month': ''
            },
            # 黑色
            'rb': {
                'deliver_month': ''
            },
            'hc': {
                'deliver_month': ''
            },
            'i': {
                'deliver_month': ''
            },
            'SF': {
                'deliver_month': ''
            },
            'ss': {
                'deliver_month': ''
            },
            'SM': {
                'deliver_month': ''
            },
            'jm': {
                'deliver_month': ''
            },
            'j': {
                'deliver_month': ''
            },
            # 有色金属
            'al': {
                'deliver_month': ''
            },
            'ao': {
                'deliver_month': ''
            },
            'ad': {
                'deliver_month': ''
            },
            'ni': {
                'deliver_month': ''
            },
            'si': {
                'deliver_month': ''
            },
            'lc': {
                'deliver_month': ''
            },
            'ps': {
                'deliver_month': ''
            },
            'zn': {
                'deliver_month': ''
            },
            'sn': {
                'deliver_month': ''
            },
            'pb': {
                'deliver_month': ''
            },
            'cu': {
                'deliver_month': ''
            },
            'bc': {
                'deliver_month': ''
            },
            # 养殖
            'jd': {
                'deliver_month': ''
            },
            'lh': {
                'deliver_month': ''
            },
            # 贵金属
            'au': {
                'deliver_month': ''
            },
            'ag': {
                'deliver_month': ''
            },
            # 油脂
            'RM': {
                'deliver_month': ''
            },
            'y': {
                'deliver_month': ''
            },
            'p': {
                'deliver_month': ''
            },
            'm': {
                'deliver_month': ''
            },
            'OI': {
                'deliver_month': ''
            },
            'a': {
                'deliver_month': ''
            },
            'b': {
                'deliver_month': ''
            },
            # 谷物
            'c': {
                'deliver_month': ''
            },
            'cs': {
                'deliver_month': ''
            },
            # 生鲜
            'AP': {
                'deliver_month': ''
            },
            'CJ': {
                'deliver_month': ''
            },
            'PK': {
                'deliver_month': ''
            },
            # 软商品
            'CF': {
                'deliver_month': ''
            },
            'SR': {
                'deliver_month': ''
            },
            # 航运
            'ec': {
                'deliver_month': ''
            },
        }

    def generate_inst_codes(self, output_dir):
