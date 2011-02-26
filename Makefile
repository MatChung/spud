TARGET		:= spud/spud
SOURCE		:= spud

CXXFLAGS	:= -O2 -Wall -D_DEBUG
LDFLAGS		:= 

CXX			:= g++

CXXFILES	:= $(foreach dir,$(SOURCE),$(wildcard $(dir)/*.cpp))

all: $(TARGET)

run: $(TARGET)
	@{ pushd $(SOURCE) >> /dev/null; ./spud; popd >> /dev/null; }

clean:
	rm -rf $(TARGET)

$(TARGET): $(CXXFILES)
	$(CXX) $(CXXFILES) $(LDFLAGS) $(CXXFLAGS) -o $@
