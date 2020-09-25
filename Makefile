PROJECT			:= lamebtc
VERSION			:= $(shell cat .semver)
COMPILER		:= g++

INCLUDE_FOLDERS		:= $(wildcard lib/*/include)
INCLUDE_FLAGS		:= $(foreach folder,$(INCLUDE_FOLDERS),-I$(folder))
SOURCE_FOLDER		:= src
SOURCE_FILES		:= $(wildcard $(SOURCE_FOLDER)/*.cpp) $(wildcard $(SOURCE_FOLDER)/*/*.cpp)
OBJECT_FOLDER		:= obj
OBJECT_FILES		:= $(patsubst $(SOURCE_FOLDER)/%.cpp,$(OBJECT_FOLDER)/%.o,$(SOURCE_FILES))
BUILD_FOLDER		:= bin
BUILD_NAME		:= $(shell echo $(PROJECT) | tr A-Z a-z)

LD_LIBRARY_PATH         := -L/usr/lib
LDLIBS                  := -lleveldb -lpthread -lsnappy
LDFLAGS			:= 
CPPFLAGS		:= -g -Wall $(INCLUDE_FLAGS) -DPROJECT_NAME='"$(PROJECT)"' -DPROJECT_VERSION='"$(VERSION)"'
CXXFLAGS		:=

$(info Making $(PROJECT) v$(VERSION)...)

all: $(BUILD_NAME)
     

$(BUILD_NAME): $(OBJECT_FILES) $(LDLIBS)
	$(COMPILER) $(LDFLAGS) -o $(BUILD_FOLDER)/$(BUILD_NAME) $^ 

$(OBJECT_FOLDER)/%.o: $(SOURCE_FOLDER)/%.cpp
	mkdir -p $(basename $@)
	$(COMPILER) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $< 

clean:
	find $(BUILD_FOLDER) ! -name '.gitignore' ! -name '$(BUILD_FOLDER)' -exec rm -rf {} +
	find $(OBJECT_FOLDER) ! -name '.gitignore' ! -name '$(OBJECT_FOLDER)' -exec rm -rf {} +
