#! /bin/bash

# Wrapper script for starting xeditor.

# This file can be customized if necessary, but beware that it may be
# be replaced on package update.  If you need to modify, make a copy
# for yourself and keep it somewhere safe.

# This is used to change the graphics package to use.  The .xtrc file
# in the home or current directory can contain things to use here,
# specifically a line like "grpref=QT6" or similar (used below).

if [ -f ./.xtrc ]; then
    source ./.xtrc
elif [ -f $HOME/.xtrc ]; then
    source $HOME/.xtrc
fi

# Call the program, passing along the argument list.

if [ -z "$grpref" ]; then
    # Auto-detect available graphics backends
    mypath=$(dirname $(readlink -f "$0"))
    if [ $(basename $mypath) == bin ]; then
        if [ -d $mypath/QT6 ]; then
            grpref=QT6
        elif [ -d $mypath/GTK2 ]; then
            grpref=GTK2
        elif [ -d $mypath/QT5 ]; then
            grpref=QT5
        else
            grpref=GTK2
        fi
    else
        mypath=$(dirname $mypath)/bin
        if [ -d $mypath/QT6 ]; then
            grpref=QT6
        elif [ -d $mypath/GTK2 ]; then
            grpref=GTK2
        elif [ -d $mypath/QT5 ]; then
            grpref=QT5
        else
            grpref=GTK2
        fi
    fi
fi
if [ $grpref != GTK2 -a $grpref != QT6 -a $grpref != QT5 ]; then
    echo "Unknown graphics setting $grpref, known are GTK2, QT6, and QT5."
    exit 1
fi

mypath=$(dirname $(readlink -f "$0"))
# mypath is now the full path to the directory containing this file.
# If the directory is named "bin" and it has the grpref subdirectory,
# execute the binary from grpref.
if [ $(basename $mypath) == bin ]; then
    if [ -d $mypath/$grpref ]; then
        $mypath/$grpref/xeditor $*
        exit $?
    fi
fi
# Otherwise, as in installed area, go to ../bin.
mypath=$(dirname $mypath)/bin
if [ -d $mypath/$grpref ]; then
    $mypath/$grpref/xeditor $*
    exit $?
else
    # Check in mozy.current directory
    current_path=$(dirname $(dirname $mypath))/mozy.current/bin
    if [ -d $current_path/$grpref ]; then
        $current_path/$grpref/xeditor $*
        exit $?
    else
        echo "Error: Graphics backend '$grpref' not found in $mypath or $current_path"
        echo "Available backends: $(ls -d $mypath/*/ $current_path/*/ 2>/dev/null | xargs -I {} basename {} | tr '\n' ' ')"
        exit 1
    fi
fi
