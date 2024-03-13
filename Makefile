PROJECT_NAME = car-firmware
PROJECT_DIR = $(shell pwd)
SHELL = /bin/bash
include *.mk

all: help

help:
	@echo ""
	@echo "   Help Menu"
	@echo ""
	@echo "   make clone            - clones the subrepostories needed"
	@echo "   make pull             - calles git pulls on all subrepostories"
	@echo "   make status           - calles git status on all subrepostories"
	@echo "   make tar              - creates a tar.gz  with the date"
	@echo "   make tar-no-git       - like backup but without .git folders"
	@echo ""

pull:
	git pull origin
	find . -type d -name .git -exec echo {} \; -exec git --git-dir={} --work-tree=${PROJECT_DIR}/{}/.. pull origin \;
	
status:
	git status -s
	find . -type d -name .git -exec echo {} \; -exec git --git-dir={} --work-tree=${PROJECT_DIR}/{}/.. status -s \;

tar:
	mkdir -p backup
	tar  --exclude=./backup \
	     -zcvf ./backup/${PROJECT_NAME}_$(shell date +%Y-%m-%d--%H-%M).tar.gz .

tar-no-git:
	mkdir -p backup
	tar  --exclude=./backup --exclude='.git' \
	     -zcvf ./backup/${PROJECT_NAME}_$(shell date +%Y-%m-%d--%H-%M).tar.gz .