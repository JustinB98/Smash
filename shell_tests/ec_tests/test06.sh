#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST06.SH START ==============="

test_name="Glob Test General Tests"
./smash <(printf "cd src\necho *.c | wc -l") > stdout_file
cd src ; echo *.c | wc -l > ../actual_stdout_file ; cd ..
cmp actual_stdout_file stdout_file
$assert_success

test_name="Glob Test No Files Found"
./smash <(printf "echo *.badfileextension") > stdout_file
echo *.badfileextension > actual_stdout_file
cmp actual_stdout_file stdout_file
$assert_success


printf "==================== TEST06.SH END ====================\n"
test_type="test06.sh"
. shell_tests/finish.sh
