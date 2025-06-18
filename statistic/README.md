## 生成K线
1. 15min is the best time-range for cta so that 15min is the default args.
2. if you want to change the bar's time-range, please define the time-range first

15分钟线是最好匹配行情的，如果你需要额外的K线，比如说1h，30min这种的, 先定义好时间范围
比如对于30min的k线，10:15-10:30这个时间段的数据是单独算一根k线还是跟10:30-10:45的线合并


## 脚本
``` bash
/opt/miniconda/envs/stock_dev/bin/python main.py gen-bar --input-dir /home/greetlist/github_project/ctp_se_market/output/ --output-dir /home/greetlist/github_project/ctp_se_market/bar/
```

`--input-dir`参数是c++那边从mmap dump出来的原始行情数据目录位置
`--output-dir`参数k线输出位置

## 输出目录结构
```
$ tree bar/2025/06/18/
bar/2025/06/18/
├── al2509.csv
├── ao2509.csv
├── c2509.csv
├── CF509.csv
├── cs2509.csv
├── lg2509.csv
├── MA509.csv
├── rb2509.csv
├── SR509.csv
├── ss2509.csv
└── zn2509.csv

0 directories, 11 files
```

## 可能存在的问题
### 数据对不上
原始行情可能会有缺失，互联网收行情，不一定完全正确
