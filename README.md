## dependency
`glog gflags ctp_api`

## compile
``` bash
xmake
```

## run
``` bash
bash run.sh recorder # for receive ctp market data 
bash run.sh reader # for dump market data to csv file
```

## perf
``` bash
perf record -ag -p $pid
perf script > out.perf
~/test/perf_test/stackcollapse-perf.pl out.perf > out.folded
~/test/perf_test/flamegraph.pl out.folded > flame.svg
```

可以看到大部分的时间都在so里面的select里面, 花费在回调里面写mmap的时间只有%0.1, 暂时没有什么性能问题
