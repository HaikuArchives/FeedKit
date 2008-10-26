# Makefile template 
# you may edit the original in
#	~/config/settings/Eddie/TemplateMakefile

# (To edit the template now, place the cursor inside the path and press Alt-D)
# This template works with the generic makefile in
#	/boot/develop/etc/makefile-engine
# and is a subset of
#	/boot/develop/etc/makefile

# you may add stuff here 


# name of your binary
NAME= DummyParser
TOPDIR=../..
TARGET_DIR=$(TOPDIR)/build/Parsers

# type of binary (one of APP, SHARED, STATIC, DRIVER)
TYPE= SHARED

# @src->@ 

ORIGIN := /Data/Code/rsskit/Parsers/DummyParser/ 

DummyParser_src := \
	DummyParser.cpp \
#

SRCS := \
	$(DummyParser_src) \
#
# @<-src@ 

ORIGIN := $(shell pwd)

#	specify additional libraries to link against
#	there are two acceptable forms of library specifications
#	-	if your library follows the naming pattern of:
#		libXXX.so or libXXX.a you can simply specify XXX
#		library: libbe.so entry: be
#		
#	- 	if your library does not follow the standard library
#		naming scheme you need to specify the path to the library
#		and it's name
#		library: my_lib.a entry: my_lib.a or path/my_lib.a
LIBS= be feedkit

#	specify additional paths to directories following the standard
#	libXXX.so or libXXX.a naming scheme.  You can specify full paths
#	or paths relative to the makefile.  The paths included may not
#	be recursive, so include all of the paths where libraries can
#	be found.  Directories where source files are found are
#	automatically included.
LIBPATHS= /boot/home/config/lib

#	additional paths to look for system headers
#	thes use the form: #include <header>
#	source file directories are NOT auto-included here
SYSTEM_INCLUDE_PATHS = /boot/home/config/include

#	additional paths to look for local headers
#	thes use the form: #include "header"
#	source file directories are automatically included
LOCAL_INCLUDE_PATHS =

#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= SOME

#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES=

#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS = ALL 

#	specify whether image symbols will be created
#	so that stack crawls in the debugger are meaningful
#	if TRUE symbols will be created
SYMBOLS = TRUE

#	specify debug settings
#	if TRUE will allow application to be run from a source-level
#	debugger.  Note that this will disable all optimzation.
DEBUGGER =

#	specify additional compiler flags for all files
COMPILER_FLAGS =

#	specify additional linker flags
LINKER_FLAGS =

#	specify the version of this particular item
#	(for example, -app 3 4 0 d 0 -short 340 -long "340 "`echo -n -e '\302\251'`"1999 GNU GPL") 
#	This may also be specified in a resource.
APP_VERSION = 

#	(for TYPE == DRIVER only) Specify desired location of driver in the /dev
#	hierarchy. Used by the driverinstall rule. E.g., DRIVER_PATH = video/usb will
#	instruct the driverinstall rule to place a symlink to your driver's binary in
#	~/add-ons/kernel/drivers/dev/video/usb, so that your driver will appear at
#	/dev/video/usb when loaded. Default is "misc".
DRIVER_PATH = 

ifeq ($(CHECK_MEM), true)
	COMPILER_FLAGS += -fcheck-memory-usage -D_NO_INLINE_ASM=1 -D_KERNEL_MODE=1
	DEBUG_BUILD=true
endif

#   Custom overrides that can be set from the command line.
ifeq ($(DEBUG_BUILD), true)
	SYMBOLS := TRUE
	DEBUGGER := TRUE
	OPTIMIZE := NONE
	COMPILER_FLAGS += -DDEBUG=1
	
	ifeq ($(shell uname -r), 5.1)
		COMPILER_FLAGS += -fno-debug-opt
	endif

endif

#	Detect BONE
ifeq ($(shell ls 2>/dev/null -1 /boot/develop/headers/be/bone/bone_api.h), /boot/develop/headers/be/bone/bone_api.h)
	SYSTEM_INCLUDE_PATHS += /boot/develop/headers/be/bone
	LIBS += socket bind
	DEFINES += BONE
endif

INSTALL_DIR = $(shell finddir B_COMMON_ADDONS_DIRECTORY)/feed_kit/Parsers

include $(TOPDIR)/makefile-engine.FeedKit
