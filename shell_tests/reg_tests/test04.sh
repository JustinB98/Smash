#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST04.SH START ==============="



printf "==================== TEST04.SH END ====================\n"
test_type="test04.sh"
. shell_tests/finish.sh
