TESTS := klist0-unit
TEST_STAMPS := $(patsubst %,%_stamp,$(TESTS))
INCLUDES := ../include

test: $(TEST_STAMPS)

clean:
	-rm -f $(TESTS) $(TEST_STAMPS)

klist0-unit: klist0-unit.c
	$(CC) -g -I$(INCLUDES) $< -o $@

klist0-unit_stamp: klist0-unit
	./klist0-unit | tee $@

