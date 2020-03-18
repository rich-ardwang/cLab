#!/bin/bash

mkdir -p /root/Document/go/src/github.com
cd /root/Document/go/src/github.com
mkdir mattn
cd mattn
git clone https://github.com/mattn/go-shellwords.git
cd ..
mkdir go-sql-driver
cd go-sql-driver
git clone https://github.com/go-sql-driver/mysql.git
cd ..
mkdir golang
cd golang
git clone https://github.com/golang/net.git
echo "go vendors complete."