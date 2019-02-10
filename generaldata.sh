#!/bin/bash

print_speed() {
    speed=$1
    prefixes=( B KB MB )
    current_prefix=0

    while [[ (speed -gt 1024) && (current_prefix -lt 3) ]]; do
        speed=$(($speed/1024))
        (( current_prefix++ ))
    done

    echo -n "$speed ${prefixes[$current_prefix]}"
}

print_time() {
    time=$1

    days=0
    hours=0
    mins=0

    if [[ $time -gt 86400 ]]; then
        days=$(($time/86400))
        time=$(($time-$days*86400))
    fi
    if [[ $time -gt 3600 ]]; then
        hours=$(($time/3600))
        time=$(($time-$hours*3600))
    fi
    if [[ $time -gt 60 ]]; then
        mins=$(($time/60))
        time=$(($time-$mins*60))
    fi

    echo "$days Day(s) $hours Hour(s) $mins Minute(s) $time Second(s)"
}

init_total=()
for DEVICE in `grep ':' /proc/net/dev | awk '{print $1}' | sed 's/://'`; do
    init_received=( `grep $DEVICE /proc/net/dev | awk '{print $2}'` )
    init_transmitted=( `grep $DEVICE /proc/net/dev | awk '{print $10}'` )
    init_total+=($(($init_received+$init_transmitted)))
    (( j++ ))
done

tput clear

max_speed=0
i=0

while [[ 1 ]]; do
    received=()
    transmitted=()
    total=()
    j=0
    for DEVICE in `grep ':' /proc/net/dev | awk '{print $1}' | sed 's/://'`; do
        received+=( `grep $DEVICE /proc/net/dev | awk '{print $2}'` )
        transmitted+=( `grep $DEVICE /proc/net/dev | awk '{print $10}'` )
        total+=($((${received[$j]}+${transmitted[$j]})))
        (( j++ ))
    done

    sleep 1
    (( i++ ))

    line=0

    j=0
    speed=()
    avg_speed=()
    for DEVICE in `grep ':' /proc/net/dev | awk '{print $1}' | sed 's/://'`; do
        tput cup $j 0
        rec=(`grep $DEVICE /proc/net/dev | awk '{print $2}'`)
        received[$j]=$rec
        tran=(`grep $DEVICE /proc/net/dev | awk '{print $10}'`)
        transmitted[$j]=$tran

        speed+=($(( ${received[$j]}+${transmitted[$j]}-${total[$j]} )))
        sum=$((${received[$j]}+${transmitted[$j]}-${init_total[$j]}))
        avg_speed+=($(($sum/$i)))

        tput el
        echo -n "$DEVICE : cur: "
        print_speed ${speed[$j]}
        echo -n " avg: "
        print_speed ${avg_speed[$j]}
        echo
        (( j++ ))
    done

    line=$j

    if [[ ${speed[0]} -gt $max_speed ]]; then
        max_speed=${speed[0]}
    fi

    time=`cat /proc/uptime | awk -F '.' '{print $1}' | sed -e 's/^.*up//'`
    tput cup $line 0
    tput el
    print_time $time
    ((line++))

    battery=`grep 'POWER_SUPPLY_CAPACITY=' /sys/class/power_supply/BAT0/uevent | awk -F '=' '{print $2}'`
    tput cup $line 0
    tput el
    echo "Battery level: $battery%"
    ((line++))

    load=`cat /proc/loadavg | awk '{print $1}'`
    tput cup $line 0
    tput el
    echo "Load from last minute: $load"
    ((line++))
    ((line++))
done
