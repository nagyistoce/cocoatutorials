@echo off
dumpbin /headers %*% | grep 'machine '
