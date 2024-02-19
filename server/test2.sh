#!/usr/bin/env bash

./aesdsocket --clear &
bash ../assignment-autotest/test/assignment5/sockettest.sh
kill %1
