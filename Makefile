
PACKAGES = $(wildcard packages/*)

.PHONY : default all release clean

default : release

all : default

release : 
	$(foreach file, $(PACKAGES), $(MAKE) -C $(file) build-deps release;)

clean :
	rm -rf build

