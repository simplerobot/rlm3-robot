
PACKAGES = $(wildcard packages/*)

define NEWLINE


endef

.PHONY : default all release test build-deps clean

default : release

all : release test

$(eval build-deps : $(foreach file, $(PACKAGES), $(NEWLINE)	$(MAKE) -C $(file) --no-print-directory build-deps))

$(eval release : build-deps $(foreach file, $(PACKAGES), $(NEWLINE)	$(MAKE) -C $(file) --no-print-directory release))

$(eval test : build-deps $(foreach file, $(PACKAGES), $(NEWLINE)	$(MAKE) -C $(file) --no-print-directory test))

clean :
	rm -rf build

