# auxiliary commands for linking kernel images
#
# kernel linkage:
# * generate empty symbol table (so that kernel will link)
$(OBJDIR)/symtab-empty.c: $(OBJDIR)/$(OBJLIST)
	sh $(PRJROOT)/tools/symtab.sh /dev/null $@

$(OBJDIR)/symtab-empty.o: $(OBJDIR)/symtab-empty.c
	@$(call DO_CC,$<,$@,$(CC_FLAGS) -I$(PRJROOT)/include)

	grep "$@" $(OBJDIR)/$(OBJLIST) || \
		echo "$(OBJDIR)/symtab-empty.o" >> $(OBJDIR)/$(OBJLIST)

# * generate stage1 symbol table
$(OBJDIR)/symtab-stage1.c: $(OBJDIR)/$(OBJLIST)
	sh $(PRJROOT)/tools/symtab.sh $(kos_nosyms) $@

$(OBJDIR)/symtab-stage1.o: $(OBJDIR)/symtab-stage1.c
	@$(call DO_CC,$<,$@,$(CC_FLAGS) -I$(PRJROOT)/include)

	sed -i -e 's,symtab-empty,symtab-stage1,' $(OBJDIR)/$(OBJLIST)

# * generate symbol table
$(OBJDIR)/symtab.c: $(OBJDIR)/$(OBJLIST)
	sh $(PRJROOT)/tools/symtab.sh $(kos_nosyms) $@

$(OBJDIR)/symtab.o: $(OBJDIR)/symtab.c
	@$(call DO_CC,$<,$@,$(CC_FLAGS) -I$(PRJROOT)/include)

	sed -i -e 's,symtab-stage1,symtab,' $(OBJDIR)/$(OBJLIST)

# * link an image w/o symbol table
$(kos_nosyms): $(OBJDIR)/$(OBJLIST) $(OBJDIR)/symtab-empty.o
	cat $< | xargs $(LD) $(LD_FLAGS) -o $@

# * link an image w/o symbol table
$(kos_nosyms).1: $(OBJDIR)/$(OBJLIST) $(kos_nosyms) $(OBJDIR)/symtab-stage1.o
	cat $< | xargs $(LD) $(LD_FLAGS) -o $@

# * link final image
$(kos_final): $(kos_nosyms).1 $(OBJDIR)/symtab.o
	cat $(OBJDIR)/$(OBJLIST) | xargs $(LD) $(LD_FLAGS) -o $@

link-kernel: $(kos_final)

CLEAN_FILES += $(kos_final) $(kos_nosyms) $(kos_nosyms).1

.PHONY: link-kernel
