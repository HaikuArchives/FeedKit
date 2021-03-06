## BeOS Generic Makefile v2.2 ##

## Fill in this file to specify the project being created, and the referenced
## makefile-engine will do all of the hard work for you.  This handles both
## Intel and PowerPC builds of the BeOS.

## Application Specific Settings ---------------------------------------------

LIBFEEDKIT_HAVE_PCRE=
LIBFEEDKIT_HAVE_OPENSSL=

ifneq ($(wildcard /boot/develop/headers/openssl/sha.h), )
  LIBFEEDKIT_HAVE_OPENSSL:=1
endif
ifneq ($(wildcard /boot/home/config/include/openssl/sha.h), )
  LIBFEEDKIT_HAVE_OPENSSL:=1
endif

ifneq ($(wildcard /boot/develop/headers/pcre/pcre.h), )
  LIBFEEDKIT_HAVE_PCRE:=1
endif
ifneq ($(wildcard /boot/home/config/include/pcre/pcre.h), )
  LIBFEEDKIT_HAVE_PCRE:=1
endif

ifeq ($(LIBFEEDKIT_HAVE_OPENSSL), )

	.PHONY default
	default:
		@echo libfeedkit requires OpenSSL to be installed. Skipping libfeedkit.

else

  ifeq ($(LIBFEEDKIT_HAVE_PCRE), )

	.PHONY default
	default:
		@echo libfeedkit requires Perl Compatible Regular Expressions to be installed. Skipping libfeedkit

	else
	
		# specify the name of the binary
		NAME= libfeedkit.so
		SVG_ICON=
		TOPDIR=..
		TARGET_DIR=$(TOPDIR)/build
		# specify the type of binary
		#	APP:	Application
		#	SHARED:	Shared library or add-on
		#	STATIC:	Static library archive
		#	DRIVER: Kernel Driver
		TYPE= SHARED
		
		#	add support for new Pe and Eddie features
		#	to fill in generic makefile
		
		#%{
# @src->@ 

ORIGIN := /Data/Code/FeedKit/libfeedkit/ 

libfeedkit_src := \
	BubbleHelper.cpp \
	Channel.cpp \
	ConfigItemView.cpp \
	Content.cpp \
	DownloadProgress.cpp \
	Enclosure.cpp \
	$(TOPDIR)Common/IMKitUtilities.cpp \
	ErrorDetails.cpp \
	Item.cpp \
	Feed.cpp \
	FeedListener.cpp \
	FeedHandler.cpp \
	FeedParser.cpp \
	ObjectCache.cpp \
	SettingsManager.cpp \
	SettingsView.cpp \
	TraceFeedHandler.cpp \
#

Specifications_src := \
	ItemSpecification.cpp \
#

ConfigItemViews_src := \
	$(TOPDIR)/Common/IconTextItem.cpp \
	ConfigItemViews/FilePickerView.cpp \
	ConfigItemViews/MenuItemView.cpp \
	ConfigItemViews/MultiTextItemView.cpp \
	ConfigItemViews/TextItemView.cpp \
#

SRCS := \
	$(libfeedkit_src) \
	$(Specifications_src) \
	$(ConfigItemViews_src) \
#

# @<-src@ 

		#	end support for Pe and Eddie
		
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
		LIBS= be stdc++.r4 pcre crypto tracker
		
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
		LOCAL_INCLUDE_PATHS = ../
		
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
		
		#Detect Zeta
		ifeq ($(wildcard /boot/develop/headers/be/locale/Formatter.h), )
		# BeOS
		else
		#Zeta
		  LIBS += zeta
		endif
		
		INSTALL_DIR=$(shell finddir B_COMMON_LIB_DIRECTORY)
		
		include $(TOPDIR)/makefile-engine.FeedKit
		
	endif
endif
