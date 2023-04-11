#!/bin/sh
cc -c min.c
cc -c max.c
cc -c main.c
cc -o app main.o min.o max.o
