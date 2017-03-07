#!/bin/sh
ls | grep -Ev '*.json|iptop|*.sh' | xargs rm -rf
ls
