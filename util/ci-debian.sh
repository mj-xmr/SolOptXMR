#!/bin/sh -e

python3 src/tests.py
# Now test unpickling:
python3 src/tests.py

