# makefile for all sample code

VERSION:=0.1
DIST:=feedkit-$(VERSION)
BACKUP_PATH:=Backup/$(shell date +%Y%m%d%H%M%S).zip
BACKUP_INCLUDE:=$(shell find | egrep "\.cpp|\.h|\.wks|makefile|\.sh|Worksheet|\.rsrc|Icons/")

COMMON_LIB:=$(shell finddir B_COMMON_LIB_DIRECTORY)
COMMON_SERVERS:=$(shell finddir B_COMMON_SERVERS_DIRECTORY)
COMMON_ADDONS:=$(shell finddir B_COMMON_ADDONS_DIRECTORY)
BUILD:=$(shell pwd)/build

SUBDIRS = \
	Preflet \
	Server \
	Parsers \
	Clients \
	
ifneq ($(wildcard /boot/develop/headers/cppunit/TestSuite.h), )
  HAVE_CPPUNIT:=1
endif
ifneq ($(wildcard /boot/home/config/include/cppunit/TestSuite.h), )
  HAVE_CPPUNIT:=1
endif

ifeq ($(HAVE_CPPUNIT), 1)
	SUBDIRS += Tests
endif


FEEDKIT_HEADERS=$(addprefix /boot/home/config/include/, $(wildcard libfeedkit/*.h))

.PHONY: default clean install dist common

default .DEFAULT :
	$(MAKE) -C libfeedkit -f makefile $@ || exit -1; 
	ln -sf "$(BUILD)/libfeedkit.so" "$(COMMON_LIB)"

	-@for f in $(SUBDIRS) ; do \
		$(MAKE) -C $$f -f makefile $@ || exit -1; \
	done

clean:
	-@for f in $(SUBDIRS) ; do \
		$(MAKE) -C $$f -f makefile clean || exit -1; \
	done

symlinks: common
	ln -sf "$(BUILD)/libfeedkit.so" "$(COMMON_LIB)"
	ln -sf "$(BUILD)/feed_server" "$(COMMON_SERVERS)"
	ln -sf "$(BUILD)/Parsers" "$(COMMON_ADDONS)/feed_kit/Parsers"
	ln -sf "$(BUILD)/FeedKitSettings" "/boot/home/config/be/Preferences"

install: common
	copyattr --data --move "$(BUILD)/libfeedkit.so" "$(COMMON_LIB)"
	copyattr --data --move "$(BUILD)/feed_server"  "$(COMMON_SERVERS)"
	copyattr --data --recursive --move "$(BUILD)/Parsers" "$(COMMON_ADDONS)/Feed_Kit/Parsers"

common:
	ln -sf "/boot/home/config/lib/libfeedkit.so" "/boot/develop/lib/x86/libfeedkit.so"

	-if [ ! -d "$(COMMON_SERVERS)" ]; then \
		mkdir -p "$(COMMON_SERVERS)"; \
	fi 

	rm -Rf "$(COMMON_ADDONS)/feed_kit"
	mkdir -p "$(COMMON_ADDONS)/feed_kit"


dist:
	mkdir -p "$(DIST)"
	copyattr --data --recursive build/* "$(DIST)"
	mkdir -p "$(DIST)/doc"
	copyattr --data --recursive build/* "$(DIST)/doc"
	zip -r "$(DIST).zip" "$(DIST)/"
	rm -Rf "$(DIST)"
	
backup:
	zip -r $(BACKUP_PATH) . -i $(BACKUP_INCLUDE)
	
/boot/home/config/include/libfeedkit:
	-mkdir -p $@

/boot/home/config/include/libfeedkit/%.h: libfeedkit/%.h
	cp -f $< $@	
