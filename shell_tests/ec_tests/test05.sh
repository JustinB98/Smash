#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"
assert_exit_code=". shell_tests/assert_exit_code.sh"

printf "==================== TEST05.SH START ====================\n"



printf "==================== TEST05.SH END ====================\n"
test_type="test05.sh"
. shell_tests/finish.sh

