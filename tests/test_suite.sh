#!/bin/sh

red=`tput setaf 1`
green=`tput setaf 2`
cyan=`tput setaf 6`
yellow=`tput setaf 3`
reset=`tput sgr0`
blue=`tput setaf 4`
bold=`tput bold`

TEST_FILES="tests/grid_tests/grid-*.sku"

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

echo "\nRunning module tests..."

TEST_FILES="tests/module_tests/*_tests.c"

for test_file in $TEST_FILES 
do
    base_name=$(basename "$test_file" .c)

    gcc -I include -c "$test_file" && gcc -o "$base_name" "${base_name}.o" src/grid.o src/colors.o -lm

    if [ $? -eq 0 ]; then
        if [ $? -eq 0 ]; then
            output=$(./"$base_name" 2> /dev/null)
            if echo "$output" | grep -q "failed"; then
                echo "$bold$red[FAIL]$reset $blue--$reset $blue$base_name$reset"
                echo "$output" | grep "failed"
            else
                echo "$bold$green[OK]$reset $blue--$reset $blue$base_name$reset"
            fi
        fi

        rm "$base_name"
        rm "${base_name}.o"
    else
        echo "${RED}Compilation of ${base_name} failed${RESET}"
    fi
done

echo "\nRunning grid solver tests..."


TEST_FILES="tests/grid-solver/*"

for file in $TEST_FILES
do
    output=$(./sudoku $file 2> /dev/null)
    exit_code=$?
    if [ $exit_code -eq 1 ] && echo "$file" | grep -q "inconsistent"; then
        echo "$bold$green[OK]$reset $blue--$reset $blue$file$reset"
    elif [ $exit_code -eq 1 ] && ! echo "$file" | grep -q "inconsistent"; then
        echo "$bold$red[FAIL]$reset $blue--$reset $blue$file$reset"
        echo "Output:"
        echo "$output"
        echo
    elif [ $exit_code -eq 0 ]; then
        echo "$bold$green[OK]$reset $blue--$reset $blue$file$reset"
    elif [ $exit_code -eq 1 ] && ! echo "$file" | grep -q "inconsistent"; then
        echo "$bold$red[FAIL]$reset $blue--$reset $blue$file$reset"
        echo "Output:"
        echo "$output"
        echo
    fi
done
