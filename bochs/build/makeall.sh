#!/bin/bash

for i in build-*; do
  echo "*** Starting make in $i ***"
  make -C $i
  echo "*** make done in $i ***"
  echo ""
  echo ""
  echo ""
done
