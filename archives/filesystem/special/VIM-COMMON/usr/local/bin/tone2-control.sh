#!/bin/bash

source /etc/fenix-release

CONFIG_FIEL="/usr/share/fenix/tone2/tone2.conf"

if [ "$BOARD" == "VIM4" ] || [ "$BOARD" == "VIM1S" ]; then
	CONTROL1=15
	CONTROL2=17
elif [ "$BOARD" == "VIM3" ] || [ "$BOARD" == "VIM3L" ] || [ "$BOARD" == "VIM1" ]; then
	CONTROL1=15
	CONTROL2=16
elif [ "$BOARD" == "VIM2" ]; then
	CONTROL1=18
	CONTROL2=19
fi

if [ -f "$CONFIG_FIEL" ]; then
	gpio mode $CONTROL1 out
	gpio mode $CONTROL2 out
	MODE=`cat $CONFIG_FIEL`
	if [[ "$MODE" =~ "i2s" ]]; then
		gpio write $CONTROL1 1
		gpio write $CONTROL2 0
	else
		gpio write $CONTROL1 0
		gpio write $CONTROL2 1
	fi

fi

