#!/bin/bash
root_dir=/home/greetlist/github_project/ctp_se_market

#LD_LIBRARY_PATH=$root_dir/lib/ xmake run ctp_se_market -mmap_base_dir=$root_dir/mmap/ -config_file_path=$root_dir/config/test -secinfo_path=$root_dir/mmap/inst_code.csv -mode=$1
LD_LIBRARY_PATH=$root_dir/lib/ ./build/linux/x86_64/debug/ctp_se_market -output_base_dir=$root_dir/output -mmap_base_dir=$root_dir/mmap/ -config_file_path=$root_dir/config/account.ini -secinfo_path=$root_dir/mmap/inst_code.csv -mode=$1
