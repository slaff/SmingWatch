COMPONENT_SRCDIRS := $(COMPONENT_PATH)/src
COMPONENT_INCDIRS := $(COMPONENT_SRCDIRS)

COMPONENT_VARS := MTM_USE_CPU_USAGE
MTM_USE_CPU_USAGE := 1

COMPONENT_CXXFLAGS = -DMTM_USE_CPU_USAGE=$(MTM_USE_CPU_USAGE)