BUILD_DIR := build

SRCS := main.cc \
				em.cc

OBJS := $(SRCS:%.cc=$(BUILD_DIR)/%.o)

em: $(OBJS)
	$(CXX) -o $(BUILD_DIR)\$@ $?

$(BUILD_DIR)/%.o: %.cc
	if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	$(CXX) -std=c++11 -c -o $@ $<

run: em
	$(BUILD_DIR)\em observations.txt transition.txt sensory.txt original.txt 2

.PHONY: clean
clean:
	rmdir /S /Q $(BUILD_DIR)
