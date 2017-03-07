#!/bin/bash

BIN=iptop

pidof ${BIN} | xargs kill
./${BIN} > out.log 2>&1
