# Read the README.md file

# Define the default target first so it will be the default.

.PHONY : default all build-deps

default :

all : default # Added since this is the default for eclipse.

# Some basic defines

SPACE = $(NOT_DEFINED) $(NOT_DEFINED)

define NEWLINE


endef

PKG_NAME = $(shell basename $(CURDIR))

# Define a macro that will build depenent packages and gather information about them.

define BUILD_SCRIPTS_DEFINE_DEPENDENCY # (dependency_name, dependency_define_name)

ifeq ($$(wildcard ../$1),)
$$(error Dependency $1 is not in the project.)
else ifneq ($$(wildcard ../$1/release),)
PKG_$2_DIR = ../$1/release
else ifneq ($$(wildcard ../../build/$1/release),)
PKG_$2_DIR = ../../build/$1/release
BUILD_SCRIPTS_BUILD_DEPS_COMMAND += $$(NEWLINE)	$$(MAKE) -C ../$1 build-deps release
else
$$(error Dependency $1 needs to be built.)
endif

endef # BUILD_SCRIPTS_DEFINE_DEPENDENCY

# Call the above macro with each passed in dependency.

BUILD_SCRIPTS_BUILD_DEPS_COMMAND = build-deps :

$(foreach dep,$(DEPS),$(eval $(call BUILD_SCRIPTS_DEFINE_DEPENDENCY,$(dep),$(shell echo '$(subst -,_,$(dep))' | tr '[:lower:]' '[:upper:]'))))

# define a target to build all the dependencies.
$(eval $(BUILD_SCRIPTS_BUILD_DEPS_COMMAND))

BUILD_DIR = ../../build/$(PKG_NAME)

ifneq ($(wildcard release),)
LIBRARY_DIR = release
else
RELEASE_DIR = $(BUILD_DIR)/release
endif

clean : 
	rm -rf $(BUILD_DIR)

