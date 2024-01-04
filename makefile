#!/bin/make

DEST=/usr/local
SHARE=$(DEST)/share

make:
	make update
	mkdir $(SHARE)/bcc

update:
	g++ main.cpp -lncursesw -ljsoncpp -o bcc

install:
	cp database.json "$(SHARE)/bcc/database.json"
	cp enemies.json "$(SHARE)/bcc/enemies.json"
	cp stages.json "$(SHARE)"/bcc/stages.json
	cp ./bcc $(DEST)/bin/bcc