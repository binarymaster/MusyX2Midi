#!/bin/bash

SRCBASE=..

rm *.cpp  

# classes borrowed from sigBase:
ln -s $SRCBASE/sig/MidiFile.cpp
ln -s $SRCBASE/sig/FileIO.cpp
ln -s $SRCBASE/sig/Options.cpp
ln -s $SRCBASE/sig/Options_private.cpp

# display any bad links:
file * | grep "broken symbolic link to"


