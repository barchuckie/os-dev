#!/bin/bash

echo -e 'PID:\tPPID:\tOpen Files:\tName:'
for process in /proc/[0-9]*; do
    name=`grep 'Name:' $process/status | awk '{print $2}'`
    pid=`grep 'Pid:' -m 1 $process/status | awk '{print $2}'`
    ppid=`grep 'PPid:' $process/status | awk '{print $2}'`
    of=`sudo ls -1q $process/fd | wc -l`
    echo -e "$pid\t$ppid\t$of\t$name"
done
