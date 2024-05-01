#!/bin/sh

set -xe

DIST_DIR=./dist
DIST_FILE=s_expr

mkdir -p $DIST_DIR

CC=g++

SRC=main.cpp
DIST=${DIST_DIR}/${DIST_FILE}

${CC} $SRC -o $DIST

