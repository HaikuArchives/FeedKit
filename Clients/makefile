# makefile for all sample code

SUBDIRS = \
	Deskbar \
	EnclosureDownloader \
	FeedInfoPopper \
	RegisterFeed \
	ItemLogger \
	ItemReader

default .DEFAULT :
	-@for f in $(SUBDIRS) ; do \
		$(MAKE) -C $$f -f makefile $@ || exit -1; \
	done

clean:
	-@for f in $(SUBDIRS) ; do \
		$(MAKE) -C $$f -f makefile clean || exit -1; \
	done
