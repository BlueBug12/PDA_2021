#!/bin/bash
make clean > /dev/null
make
echo "==========Running adaptec1 testcase=========="
time ./Lab4 adaptec1.aux
echo "============================================="
./Lab4_verifier adaptec1.aux output.pl | grep Placement
./Lab4_evaluate adaptec1.pl output.pl > adaptec1.log
./../../../GDT-4.0.4/gdt2gds test.gdt adaptec1.gds
echo "==========Running newblue5 testcase=========="
time ./Lab4 newblue5.aux
echo "============================================="
./Lab4_verifier newblue5.aux output.pl | grep Placement
./Lab4_evaluate newblue5.pl output.pl > newblue5.log
./../../../GDT-4.0.4/gdt2gds test.gdt newblue5.gds




