#!/bin/bash -x

## parse gtest options
if [ $# -gt 0 ] ; then
    GETOPT=`getopt -l gtest_output:,start: -- dummy "$@"` ; [ $? != 0 ] && exit 1
    eval set -- "$GETOPT"
    while true
    do
	case $1 in
	    --gtest_output)  TEST_OUTPUT=`echo $2|sed s/^xml\://`     ; shift 2
		;;
	    --start)  FILENAME=$2     ; shift 2
		;;
	    --)  shift; break;
		;;
	esac
    done
    if [ "$TEST_OUTPUT" != "" ] ; then
	touch $TEST_OUTPUT # for dummy gtest output
    fi
fi

## grxui functions
TEST_DIR=`rospack find openhrp3`/test
function wait-grxui {
    while :; do
	rosrun openhrp3 check-online-viewer.py
	if [ $? == 0 ] ; then
	    break;
	fi;
	echo ";; Wait for GRXUI of $filename" 1>&2;
	sleep 1;
    done
}

# install cnee http://blog.livedoor.jp/vine_user/archives/51738792.html, use xnee-3.10.tar.gz
function start-capture-grxui {
    local filename=$1
    # wait for winid
    WINID=""
    while [ "$WINID" == "" ]; do
	sleep 1
	WINID=`xdotool search --name "Eclipse SDK"`
    done
    # fail to start up?
    for winname in "Restoring Problems"
    do
	tmpwinid=`xdotool search "$winname"`
	if [ "$tmpwinid" != "" ]; then
	    xdotool windowfocus --sync $tmpwinid; xdotool key alt+F4
	fi
    done
    # move right for image viewer
    xdotool set_desktop 2
    xdotool search --name "Eclipse SDK " set_desktop_for_window 2
    xdotool search --name "Eclipse SDK " windowmove --sync 0 0
    xdotool search --name "Eclipse SDK " windowactivate --sync
    echo "target  window id    ->"$WINID
    echo "current window focus ->"`xdotool getwindowfocus`
    # start simulator
    xdotool search --name "Eclipse SDK" windowactivate --sync \
	key --clearmodifiers alt+g \
	key --clearmodifiers Down \
	key --clearmodifiers Down \
	key --clearmodifiers Down \
	key --clearmodifiers Down \
	key --clearmodifiers Down \
	key --clearmodifiers Return
    # wait for Time is up
    WINID=""
    i=0
    while [ "$WINID" == "" ]; do
	import -screen -window Eclipse\ SDK\  "${filename%%.*}-$i.${filename#*.}"  1>&2
	sleep 1
	WINID=`xdotool search --name "Time is up"`
	i=`expr $i + 1`
    done
    xdotool search --name "Time is up" windowunmap --sync
    # capure
    import -screen -window Eclipse\ SDK\  $filename  1>&2
    # finish
    xdotool search --name "Time is up" windowmap --sync
    xdotool search --name "Time is up" windowactivate --sync key --clearmodifiers Return
    WINID=""; while [ "$WINID" == "" ]; do sleep 1; WINID=`xdotool search --name "Simulation Finished"`; done
    xdotool search --name "Simulation Finished" windowactivate --sync key --clearmodifiers Return
    # done
    xdotool search --name "Eclipse SDK" windowactivate --sync \
	key --clearmodifiers alt+f \
	key --clearmodifiers x \
    # fail when unable to capture image
    if [ ! -f $filename ] ; then exit 1; fi
    echo "done"
}

if [ "$FILENAME" != "" ]; then
    wait-grxui
    start-capture-grxui $FILENAME
fi