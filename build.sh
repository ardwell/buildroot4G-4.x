#!/bin/bash


cd ./buildroot

# Just in case the buildroot variable BR2_EXTERNAL has been exported from another script
# we sets its value to be empty here and pass it into make

make BR2_EXTERNAL=

