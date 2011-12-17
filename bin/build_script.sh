#!/bin/csh

setenv | sort > "$SCRIPT_OUTPUT_FILE_0"

#
# set up the defaults
#
set echo    = 1
set f_arch  = ppc
set f_stage = debug

#
# set up stage and architecture
#
if "$TARGET_NAME" == "debug" then
  set f_stage = debug
else
  set f_stage = release
endif

if      "$ARCHS" == "ppc" then
   set f_arch = PPC
else if "$ARCHS" == "i386" then
   set f_arch = X86
else
   set f_arch = universal
endif

#
# copy the product to its output location
#
ditto $BUILD_DIR/$CONFIGURATION/$TARGET_NAME/$FULL_PRODUCT_NAME ../../../../libraries/$f_arch/$f_stage/$FULL_PRODUCT_NAME
 
 
