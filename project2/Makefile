BUILD_DIR := build

SRCS := main.cc \
				em.cc

OBJS := $(SRCS:%.cc=$(BUILD_DIR)/%.o)

em: $(OBJS)
	$(CXX) -o $(BUILD_DIR)\$@ $?

$(BUILD_DIR)/%.o: %.cc
	if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	$(CXX) -std=c++11 -c -o $@ $<

run: em test10000

test10:
	$(BUILD_DIR)\em observations_10.txt transition_10.txt sensory_10.txt original_10.txt 1

test100:
	$(BUILD_DIR)\em observations_100.txt transition.txt sensory.txt original_100.txt 4

test10000:
	$(BUILD_DIR)\em observations_10000.txt transition.txt sensory.txt original_10000.txt 8

.PHONY: clean
clean:
	rmdir /S /Q $(BUILD_DIR)
