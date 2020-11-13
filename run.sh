#!/bin/sh

BUILDDIR="build"

run()
{
	ninja
	./swone
}

if [ -d "$BUILDDIR" ]; then
	### Take action if $DIR exists ###
	echo "run ninja..."
	cd $BUILDDIR
	run
else
	###  Control will jump here if $DIR does NOT exists ###
	echo "run meson build..."
	meson $BUILDDIR
	cd $BUILDDIR
	run
fi

