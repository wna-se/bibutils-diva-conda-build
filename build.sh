#!/bin/sh
make clean
make package
cd update
tar zxvf bibutils_4.9_amd64.tgz
mv bibutils_4.9/* ../../src/main/webapp/WEB-INF/bibutils/.
