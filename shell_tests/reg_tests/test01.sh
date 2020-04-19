#!/bin/bash

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"
assert_exit_code=". shell_tests/assert_exit_code.sh"

echo "============ TEST01.SH START ==============="

test_name="Exit code Test 1"
error_code=20
./smash < <(echo exit 20) > /dev/null 2>&1
$assert_exit_code

test_name="Exit code Test 2"
error_code=0
./smash < <(echo exit) > /dev/null 2>&1
$assert_exit_code

test_name="Exit code Test 3"
error_code=0
./smash < <(echo exit 0) > /dev/null 2>&1
$assert_exit_code

test_name="Exit code Test 4"
error_code=255
./smash < <(echo exit -1) > /dev/null 2>&1
$assert_exit_code

test_name="Exit code Test 5"
error_code=127
./smash < <(echo unknown command ; echo exit) >/dev/null 2>&1
$assert_exit_code

printf "==================== TEST01.SH END ====================\n"
test_type="test01.sh"
. shell_tests/finish.sh
