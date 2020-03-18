#!/bin/bash

mkdir -p /root/Document/go/github.com
cd /root/Document/go/github.com
git clone https://github.com/mattn/go-shellwords.git
git clone https://github.com/go-sql-driver/mysql.git
echo "go vendors complete."