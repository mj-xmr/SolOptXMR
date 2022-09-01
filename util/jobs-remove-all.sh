#!/bin/sh -e

# Remove all the jobs in the queue

for i in `atq | awk '{print $1}'`;do echo "Removing job $i"; atrm $i;done

