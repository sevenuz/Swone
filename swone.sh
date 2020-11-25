#!/bin/sh

BUILDDIR="build"

if [ $# -eq 0 ]
then
	echo 'Swone o.O'
	echo '---------'
	echo 'build : create build/ if not exists and build project'
	echo 'build run : runs the project after build command'
	echo 'todo : shows TODO in source/ folder'
	exit 0
fi

if [ "$1" = "build" ]
then
	if [ -d "$BUILDDIR" ]
	then
		### Take action if $DIR exists ###
		echo "run ninja..."
		cd $BUILDDIR
	else
		###  Control will jump here if $DIR does NOT exists ###
		echo "run meson build..."
		meson $BUILDDIR
		cd $BUILDDIR
	fi
	ninja
	FAILED=$(ninja | grep "build stopped")
	echo "$FAILED"
	if [ "$2" = "run" -a -z "$FAILED" ]
	then
		./swone
	fi
fi
if [ "$1" = "run" ]
then
	cd $BUILDDIR
	./swone
fi
if [ "$1" = "todo" ]
then
	grep -rn TODO source/
fi

