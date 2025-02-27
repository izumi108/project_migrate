#! /bin/bash
cd /data/.health_ctrl_ws/hlog/hlog/build
make
make install
cp -rf include lib /data/.health_ctrl_ws/hlog/test
cd /data/.health_ctrl_ws/hlog/test/build
rm -rf log
make
