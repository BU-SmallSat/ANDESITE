#!/bin/bash
cd /dev
echo "current tty0 ports open:"
ls | grep ttyO
echo
echo "enabling UART-1 and UART-4"
echo BB-UART1 > /sys/devices/bone_capemgr.*/slots
echo BB-UART4 > /sys/devices/bone_capemgr.*/slots
echo
echo "current tty0 ports open:"
ls | grep ttyO
echo
