#!/bin/sh

arm-hisiv300-linux-gcc -Wall -g \
-lpthread -lm -ldl \
-o read \
read.c
