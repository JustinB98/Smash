#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST02.SH START ==============="



printf "==================== TEST02.SH END ====================\n"
test_type="test02.sh"
rm test_file
. shell_tests/finish.sh
