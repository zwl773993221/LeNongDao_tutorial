#!/bin/sh

insmod rtutil7601Uap.ko
insmod mt7601Uap.ko
insmod rtnet7601Uap.ko

ifconfig ra0 up
