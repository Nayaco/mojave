ADD_LIBRARY(porter STATIC
        porter.h
        porter.c
        runtime/utilities.o
        runtime/api.o)
ADD_LIBRARY(hash STATIC
        hash.h
        hash.c)
