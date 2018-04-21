#!/bin/bash

echo "Compiling virus.c" 
gcc -o virus src/virus.c 

echo ""

echo "Running virus" 
./virus

echo ""

echo "Copying echo to this directory"
cp /bin/echo .

echo ""

echo "Size of echo"
wc -c echo

echo ""

echo "Running virus, should infect echo"
./virus

echo ""

echo "The size of echo should be larger:"
wc -c echo

echo ""

echo "Running './echo "Hello World!"'" 
./echo "Hello World!"

echo ""

echo "Copying ls to this directory" 
cp /bin/ls . 

echo ""

echo "Running ./echo. Should infect ls"
./echo

echo ""

echo "Running ./ls. Nothing to infect"
./ls

echo ""

echo "Copying cat and grep locally" 
cp /bin/cat .
cp /bin/grep .
chmod u-w-r grep

echo ""

echo "Running ./ls. Should infect cat" 
./ls

echo ""

echo "Creating text.txt"
touch text.txt
echo "This is the first line" >> text.txt
echo "This is the second line" >> text.txt

echo ""

echo "Running './cat text.txt -n"
./cat text.txt -n

echo ""

echo "Running './echo "Hello World"'"
./echo "Hello World"

echo ""

echo "Cleaning up: Removing created files" 
rm -f  echo ls cat grep text.txt virus

echo ""

echo "Exiting"

