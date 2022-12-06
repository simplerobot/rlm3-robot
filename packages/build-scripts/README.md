# build-scripts
A set of scripts used for building packages
The scripts should be used in a makefile as:
```
DEPS += build-scripts
DEPS += test
DEPS += lib-jsoncpp
DEPS += hw-test-agent

include ../build-scripts/release/include.make

default : ...

```

A package in this context is a code module.  If there is a release directory in the source package, that will be exposed to dependent projects.  Otherwise, it will link to a build/<project>/release directory in the workspace dir.  In the above example, build-scripts, test, and lib-jsoncpp packages include a release directory directly.  hw-test-agent is built.  Assuming the project is in a directory called xyz that does not include a release directory directly.  The build will fail if none of the dependencies are built.  However, the build-deps target will build them.

This above example will define the following:

```
build-deps :
  $(MAKE) -C ../build-scripts
  $(MAKE) -C ../test
  $(MAKE) -C ../lib-jsoncpp
  $(MAKE) -C ../hw-test-agent

PKG_BUILD_SCRIPTS_DIR = ../build-scripts/release
PKG_TEST_DIR = ../test/release
PKG_LIB_JSONCPP_DIR = ../lib-jsoncpp/release
PKG_HW_TEST_AGENT_DIR = ../../build/hw-test-agent/release

BUILD_DIR = ../../build/xyz
RELEASE_DIR = $(BUILD_DIR)/release

PKG_NAME = xyz

clean :
  rm -rf $BUILD_DIR

```
