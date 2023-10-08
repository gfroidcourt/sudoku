#!/bin/sh

red=`tput setaf 1`
green=`tput setaf 2`
cyan=`tput setaf 6`
yellow=`tput setaf 3`
reset=`tput sgr0`
blue=`tput setaf 4`
bold=`tput bold`

TEST_FILES="tests/grid-*.sku"

for f in $TEST_FILES
do
    output="$(./sudoku $f 2> /dev/null)"
    exit_code=$?
    test_should_pass=$(echo $f | grep -c "pass")

    if [ $exit_code -ne $test_should_pass ]
    then
        echo "$bold$green[OK]$reset $blue--$reset $blue$f$reset"
    else
        echo "$bold$red[FAIL]$reset $blue--$reset $blue$f$reset"
        echo "Output:"
        echo "$output"
        echo
    fi 

done