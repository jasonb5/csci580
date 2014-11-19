SRCS := main.cc \
				ann.cc

OBJS := $(SRCS:%.cc=%.o)

ann: $(OBJS)
	$(CXX) -o $@ $^

%.o: %.cc
	$(CXX) -g $< -c -o $@

run: ann test2

test1:
	./ann train_input.txt train_output.txt test_input.txt test_output.txt structure.txt weights.txt 10000

test2:
	./ann train_input2.txt train_output2.txt test_input2.txt test_output2.txt structure2.txt weights2.txt 10000
