TESTS := klist0-unit atomic-unit
TEST_STAMPS := $(patsubst %,%_stamp,$(TESTS))
INCLUDES := ../include

test: $(TEST_STAMPS)

clean:
	-rm -f $(TESTS) $(TEST_STAMPS) *.d

klist0-unit: klist0-unit.c
	$(CC) -MD -g -I$(INCLUDES) $< -o $@ 2>&1 | tee $@_stamp

klist0-unit_stamp: klist0-unit
	cat $<.d >> $@
	./klist0-unit | tee -a $@

atomic-unit: atomic-unit.c
	$(CC) -MD -g -I$(INCLUDES) $< -o $@ 2>&1 | tee $@_stamp

atomic-unit_stamp: atomic-unit
	cat $<.d >> $@
	./atomic-unit | tee -a $@

