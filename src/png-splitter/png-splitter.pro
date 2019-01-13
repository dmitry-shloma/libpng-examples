TEMPLATE = app

CONFIG += console
CONFIG -= qt
CONFIG += png zlib

SOURCES = \
    main.c

QMAKE_CFLAGS += -std=c99
