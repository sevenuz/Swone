#!/bin/sh

BUILDDIR="build"

clean(){
	echo "clean..."
	rm -r $BUILDDIR
	git restore $BUILDDIR
	meson $BUILDDIR
}

build(){
	if [ -d "$BUILDDIR" ]
	then
		### Take action if $DIR exists ###
		cd $BUILDDIR
	else
		###  Control will jump here if $DIR does NOT exists ###
		echo "run meson build..."
		meson $BUILDDIR
		cd $BUILDDIR
	fi
	echo "run ninja..."
	ninja
	FAILED=$(ninja | grep "build stopped")
	echo "$FAILED"
	cd ..
}

run(){
	if [ -z "$FAILED" ]
	then
		./$BUILDDIR/swone
	fi
}

debug(){
	if [ -z "$FAILED" ]
	then
		gdb -ex=r --args ./$BUILDDIR/swone
	fi
}

todo(){
	grep -rn TODO source/
}

help(){
	echo 'Swone o.O'
	echo '---------'
	echo 'clean : reset build/ directory'
	echo 'build : create build/ if not exists and build project'
	echo 'run : runs the project, if build was not failing'
	echo 'debug : runs the project with gdb, if build was not failing'
	echo 'todo : shows TODO in source/ folder'
	echo '---------'
	echo 'all commands can be combined. ex.: ./swone.sh build run'
}

if [ $# -eq 0 ]
then
	help
fi

for var in "$@"
do
	# 2>/dev/null || echo "$var is not a command. Try help to see all."
    $var
	echo ''
done

