@echo off
ipconfig | head -30 | grep -v -e "^$"