#
#
#

SUBCOMPONENT_PATH ?=

COMPONENT_SRCDIRS := $(SUBCOMPONENT_PATH)/.
COMPONENT_ADD_INCLUDEDIRS := $(SUBCOMPONENT_PATH)/include

# Always build mkcrofs in case user wants to run it manually
MKCROFS_SRC = $(COMPONENT_PATH)/$(SUBCOMPONENT_PATH)/crofs.cpp \
			  $(COMPONENT_PATH)/$(SUBCOMPONENT_PATH)/miniz/miniz.c

MKCROFS_INCLUDES = -I$(COMPONENT_PATH)/$(SUBCOMPONENT_PATH)/include \
				   -I$(COMPONENT_PATH)/$(SUBCOMPONENT_PATH)/miniz

mkcrofs: CXX = g++
mkcrofs: CFLAGS = -O0 -g -Wall -Werror -pedantic -Wno-misleading-indentation
mkcrofs: CXXFLAGS = $(CFLAGS)
mkcrofs: $(MKCROFS_SRC)
	$(CXX) $(CXXFLAGS) $(MKCROFS_INCLUDES) -o $@ $+

build: lib$(COMPONENT_NAME).a mkcrofs
ifdef CONFIG_CROFS_EMBED

COMPONENT_EMBED_FILES := $(COMPONENT_BUILD_DIR)/crofs
COMPONENT_EXTRA_CLEAN := $(COMPONENT_EMBED_FILES) mkcrofs

CROFS_SRC_PATHS = $(addprefix $(PROJECT_PATH)/,$(call dequote,$(CONFIG_CROFS_SRC)))

$(COMPONENT_EMBED_FILES): $(shell find $(CROFS_SRC_PATHS)) mkcrofs
	$(COMPONENT_BUILD_DIR)/mkcrofs -v -t -l $(CONFIG_CROFS_LEVEL) $@ $(CROFS_SRC_PATHS)

endif
