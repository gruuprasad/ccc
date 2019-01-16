#!/bin/bash

awk 'flag{ if (/endunit/){printf "%s", buf; flag=0; buf=""} else buf = buf $0 ORS}; /startunit/{flag=1}' test_codes.c4
