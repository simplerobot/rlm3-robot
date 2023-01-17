# Read the README.md file for an idea of how to use this file.

# Some basic defines

EMPTY = 
SPACE = $(EMPTY) $(EMPTY)
define NEWLINE


endef

# Some package defines

PKG_NAME = $(shell basename $(CURDIR))

BUILD_DIR = ../../build/$(PKG_NAME)

ifneq ($(wildcard release),)
LIBRARY_DIR = release
else
RELEASE_DIR = $(BUILD_DIR)/release
endif

# Define targets.

.PHONY : default deps build-deps build-dep test release

default : # Defined first so it will be the default target.  Generally this should build everything.  Your Makefile should define what happens when this target is executed.

deps : # Ensures all dependencies are available.  Your targets should depend on this.

build-deps : # Will recursively call build-dep for this project.

build-dep : # Called to make anything that may be needed by dependent packages.  Essentially, this should build or package any compile time dependencies.  Your Makefile should define what happens when this target is executed. 

test : # Called to test your package.  Your Makefile should define what happens when this target is executed.

release : # Called to build your package.  Your Makefile should define what happens when this target is executed.

clean : 
	rm -rf $(BUILD_DIR)

# Define a macro that will build depenent packages and gather information about them.

define BUILD_SCRIPTS_DEFINE_DEPENDENCY # (dependency_name, dependency_define_name)

ifeq ($$(wildcard ../$1),)
$$(error Dependency $1 is not in the project.)
else ifneq ($$(wildcard ../$1/release),)
PKG_$2_DIR = ../$1/release
else
PKG_$2_DIR = ../../build/$1/release
endif

$$(PKG_$2_DIR) :
	$$(error Dependency $1 needs to be built.)

BUILD_SCRIPTS_DEPS_DIRS += $$(PKG_$2_DIR)

ifeq ($$(wildcard $$(PKG_$2_DIR)),)
BUILD_SCRIPTS_BUILD_DEPS += $1
endif

endef # BUILD_SCRIPTS_DEFINE_DEPENDENCY

# Call the above macro with each passed in dependency.

$(foreach dep,$(DEPS),$(eval $(call BUILD_SCRIPTS_DEFINE_DEPENDENCY,$(dep),$(shell echo '$(subst -,_,$(dep))' | tr '[:lower:]' '[:upper:]'))))

# Add the deps target that makes sure all the dependencies have been built.

deps : | $(BUILD_SCRIPTS_DEPS_DIRS)

# Add a target to recursively ensure dependencies are available.

$(eval build-deps : build-dep $(foreach dep, $(BUILD_SCRIPTS_BUILD_DEPS), $(NEWLINE)	$(MAKE) -C ../$(dep) --no-print-directory build-deps))

