SRCS := main.cc \
				ann.cc

OBJS := $(SRCS:%.cc=%.o)

ann: $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cc
	$(CXX) $< -c -o $@

run: ann test1

test1:
	./ann train_input.txt train_output.txt test_input.txt test_output.txt structure.txt weights.txt 10000
