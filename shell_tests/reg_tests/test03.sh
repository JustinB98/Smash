#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST03.SH START ==============="

test_name="Debug Flag Test"
./smash -d <(echo "") > /dev/null 2>&1
$assert_success

test_name="Multiple Debug Flags Test"
./smash -d -d -d -d -d -d > /dev/null 2>&1
$assert_fail

test_name="Invalid Flag Test"
./smash -z > /dev/null 2>&1
$assert_fail

test_name="More than 1 input file"
./smash <(echo ls) <(echo bad_file) > /dev/null 2>&1
$assert_fail

test_name="Opening File That Doesn't Exist"
./smash /path/to/invalid/file/that/will/never/work > /dev/null 2>&1
$assert_fail

test_name="Help Flag Test"
./smash -h > /dev/null 2>&1
$assert_success

printf "==================== TEST03.SH END ====================\n"
test_type="test03.sh"
. shell_tests/finish.sh
