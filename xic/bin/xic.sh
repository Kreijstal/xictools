#! /bin/bash

# Wrapper script for starting Xic.

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

# Hook to Custom Compiler, PyCellStudio (Synopsys only!)

snps_xic=$(dirname $(readlink -f "$0"))/snps_xic
if [ -f "$snps_xic" ]; then
    source $snps_xic
fi

# Handle the XIC_LIBRARY_PATH variable by prepending it to the dynamic
# linker search path.  This can be used to pass the location of
# libraries needed by plugins, for example for OpenAccess.  Set
# XIC_LIBRARY_PATH in your shell startup script (e.g., .bashrc)
# instead of LD_LIBRARY_PATH.

if [ -n "$XIC_LIBRARY_PATH" ]; then
    if [ -n "$LD_LIBRARY_PATH" ]; then
        LD_LIBRARY_PATH="$XIC_LIBRARY_PATH:$LD_LIBRARY_PATH"
    else
        LD_LIBRARY_PATH="$XIC_LIBRARY_PATH"
    fi
fi

# Call the Xic program, passing along the argument list.

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
            # Check in xic.current directory
            current_path=$(dirname $mypath)/xic.current/bin
            if [ -d $current_path/QT6 ]; then
                grpref=QT6
            elif [ -d $current_path/GTK2 ]; then
                grpref=GTK2
            elif [ -d $current_path/QT5 ]; then
                grpref=QT5
            else
                echo "Error: No graphics backend found in $mypath or $current_path"
                echo "Available backends: QT6, GTK2, QT5"
                exit 1
            fi
        fi
    else
        mypath=$(dirname $mypath)/xic/bin
        if [ -d $mypath/QT6 ]; then
            grpref=QT6
        elif [ -d $mypath/GTK2 ]; then
            grpref=GTK2
        elif [ -d $mypath/QT5 ]; then
            grpref=QT5
        else
            # Check in xic.current directory
            current_path=$(dirname $mypath)/xic.current/bin
            if [ -d $current_path/QT6 ]; then
                grpref=QT6
            elif [ -d $current_path/GTK2 ]; then
                grpref=GTK2
            elif [ -d $current_path/QT5 ]; then
                grpref=QT5
            else
                echo "Error: No graphics backend found in $mypath or $current_path"
                echo "Available backends: QT6, GTK2, QT5"
                exit 1
            fi
        fi
    fi
fi
if [ $grpref != GTK2 -a $grpref != QT6 -a $grpref != QT5 ]; then
    echo "Unknown graphics setting $grpref, known are GTK2, QT6, and QT5."
    exit 1
fi

export LD_LIBRARY_PATH
mypath=$(dirname $(readlink -f "$0"))
# mypath is now the full path to the directory containing this file.
# If the directory is named "bin" and it has the grpref subdirectory,
# execute the binary from grpref.
if [ $(basename $mypath) == bin ]; then
    if [ -d $mypath/$grpref ]; then
        $mypath/$grpref/xic $*
        exit $?
    fi
fi
# Otherwise, as in installed area, go to ../xic/bin.
mypath=$(dirname $mypath)/xic/bin
if [ -d $mypath/$grpref ]; then
    $mypath/$grpref/xic $*
    exit $?
else
    # Check in xic.current directory
    current_path=$(dirname $mypath)/xic.current/bin
    if [ -d $current_path/$grpref ]; then
        $current_path/$grpref/xic $*
        exit $?
    else
        echo "Error: Graphics backend '$grpref' not found in $mypath or $current_path"
        echo "Available backends: $(ls -d $mypath/*/ $current_path/*/ 2>/dev/null | xargs -I {} basename {} | tr '\n' ' ')"
        exit 1
    fi
fi
