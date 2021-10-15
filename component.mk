## Local build configuration
## Parameters configured here will override default and ENV values.

export COMPONENT_SEARCH_DIRS := $(CURDIR)/Libraries

COMPONENT_DEPENDS = Graphics AXP202X


SMING_ARCH := Esp32

COMPONENT_SRCDIRS := app src
COMPONENT_INCDIRS := $(COMPONENT_SRCDIRS)  

ifeq (,$(filter $(SMING_ARCH),Esp32 Host))
$(error "Unsupported platform. Only Esp32 and Host are supported")
endif
