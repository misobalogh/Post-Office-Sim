#!/bin/bash


#./proj2 NZ NU TZ TU F

testsVal=(
    "1 1 1 1 1"
    "100 010 123 100 123"
)


testsInv=(
    ""
    "a 1 1 1 1"
    "1 a 1 1 1"
    "1 1 a 1 1"
    "1 1 1 a 1"
    "1 1 1 1 a"
    "1 1 1 1 1 a"
    "-1 1 1 1 1"
    "1 1 1 1 -1"
    "100 10 10001 100 10000"
    "100 10 10000 101 10000"
    "100 10 10000 100 10001"
)

echo "Valid aruments:"
for i in "${!testsVal[@]}"; do
    echo "Test $((i+1)):"
    ./proj2 ${testsVal[$i]}
    if [ $? -eq 0 ]; then
        echo "OK✅"
    else
        echo "FAIL❌"
    fi
    echo ""
done

echo "Invalid aruments:"
for i in "${!testsInv[@]}"; do
    echo "Test $((i+1)):"
    ./proj2 ${testsInv[$i]}
    if [ $? -eq 1 ]; then
        echo "OK✅"
    else
        echo "FAIL❌"
    fi
    echo ""
done