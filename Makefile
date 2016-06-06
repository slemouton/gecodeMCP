SUBDIRS = all-interval chsort gechord hamming interpolm jarrell profils stroppa vrythm
     
all:
	for dir in $(SUBDIRS); do \
		echo "making" $$dir;\
		$(MAKE) -C $$dir; \
		done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
		done
