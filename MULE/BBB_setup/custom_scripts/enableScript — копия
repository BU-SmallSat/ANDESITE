#!/bin/bash
# enable necesarry uart and gpio scipts

echo 51 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio51/direction
echo 1 > /sys/class/gpio/gpio51/value

echo BB-UART5 > /sys/devices/platform/bone_capemgr/slots
