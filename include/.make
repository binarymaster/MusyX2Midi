#!/bin/sh

SRCBASE=../sig

rm *.h *.cpp

ln -s $SRCBASE/MidiFile.h
ln -s $SRCBASE/FileIO.h
ln -s $SRCBASE/Array.h
ln -s $SRCBASE/SigCollection.h
ln -s $SRCBASE/Array.cpp
ln -s $SRCBASE/SigCollection.cpp
ln -s $SRCBASE/sigConfiguration.h
ln -s $SRCBASE/Options.h
ln -s $SRCBASE/Options_private.h

# display the broken links:

file * | grep "broken symbolic link to"

