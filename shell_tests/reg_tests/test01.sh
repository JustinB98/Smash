#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST01.SH START ==============="



printf "==================== TEST01.SH END ====================\n"
test_type="test01.sh"
. shell_tests/finish.sh
