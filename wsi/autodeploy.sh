#!/bin/bash
while true
do
	killall -9 node
	git stash
	git pull --rebase
	git stash pop
	node server.js
done
