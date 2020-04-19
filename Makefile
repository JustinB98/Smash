CC := gcc
RM := rm
SH := sh
SRCD := src
INCD := include
BIND := .
BLDD := build
TSTD := shell_tests
REGTSTD := $(TSTD)/reg_tests
ECTSTD := $(TSTD)/ec_tests
VALTSTD := $(TSTD)/valgrind_tests
INCF := -I $(INCD)
CFLAGS := -O2 -g -Wall -Werror
DEPFLAGS := -MD
ECFLAGS := -DEXTRA_CREDIT
EXEC := smash
EXECF := $(BIND)/$(EXEC)
ALL_SRCF := $(wildcard $(SRCD)/*.c)
ALL_OBJF := $(patsubst $(SRCD)/%.c, $(BLDD)/%.o, $(ALL_SRCF))
ALL_REG_TESTS := $(wildcard $(REGTSTD)/test*.sh)
ALL_EC_TESTS := $(wildcard $(ECTSTD)/test*.sh)
ALL_VAL_TESTS := $(wildcard $(VALTSTD)/test*.sh)
ALL_DEPS := $(wildcard $(BLDD)/*.d)

.PHONY: all clean tests run_reg_tests run_ec_tests ec

all: $(BLDD) $(EXECF)

clean:
	$(RM) -rf $(BLDD) $(EXECF)

ec: CFLAGS += $(ECFLAGS)
ec: all

run_test_directory = \
		total_tests=0 ; \
		total_passed=0 ; \
		for test_file in $(1) ; do \
			total_tests=$$((total_tests+1)) ; \
			$(SH) $$test_file ; \
			exit=$$? ; \
			if [ $$exit -eq 0 ]; then \
				total_passed=$$((total_passed+1)) ; \
			fi \
		done ; \
		if [ $$total_tests -eq $$total_passed ]; then \
			printf "\033[0;32m" ; \
		else \
			printf "\033[0;31m" ; \
		fi ; \
		printf "\n$$total_passed / $$total_tests test files passed" ; \
		printf "\033[0m" ;

run_val_tests:
	@printf "================= STARTING VALGRIND TESTS =================\n\n"
	@$(call run_test_directory,$(ALL_VAL_TESTS))
	@printf "\n\n================= FINISHED VALGRIND TESTS =================\n" ;

run_ec_tests:
	@printf "================= STARTING EXTRA CREDIT TESTS =================\n\n"
	@$(call run_test_directory,$(ALL_EC_TESTS))
	@printf "\n\n================= FINISHED EXTRA CREDIT TESTS =================\n" ;

run_reg_tests:
	@printf "================= STARTING REGULAR TESTS =================\n\n"
	@$(call run_test_directory,$(ALL_REG_TESTS))
	@printf "\n\n================= FINISHED REGULAR TESTS =================\n"

tests: clean all run_reg_tests

$(BLDD):
	mkdir -p $@

-include $(ALL_DEPS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(INCF) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

$(EXECF): $(ALL_OBJF)
	$(CC) -o $@ $^
