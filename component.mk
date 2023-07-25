## Local build configuration
## Parameters configured here will override default and ENV values.

export COMPONENT_SEARCH_DIRS := $(CURDIR)/libs

LOCAL_COMPONENTS := AXP202X BMA423 FocalTech PCF8563
COMPONENT_DEPENDS := AnimatedGIF Graphics $(LOCAL_COMPONENTS) MillisTaskManager
COMPONENT_SUBMODULES := $(addprefix libs/,$(LOCAL_COMPONENTS))


# Allowed Architectures
SMING_ARCH ?= Esp32

ifeq (,$(filter $(SMING_ARCH),Esp32 Host))
$(error "Unsupported platform. Only Esp32 and Host are supported")
endif

COMPONENT_SRCDIRS := \
    app \
    src \
    src/Arch/$(SMING_ARCH)

COMPONENT_INCDIRS := $(COMPONENT_SRCDIRS)  
