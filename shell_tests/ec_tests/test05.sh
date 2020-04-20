#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"
assert_exit_code=". shell_tests/assert_exit_code.sh"

printf "==================== TEST05.SH START ====================\n"

./smash <(printf "find /usr/include | egrep -v /sys/ | sort | uniq | wc -l") > stdout_file 2> stderr_file
find /usr/include | egrep -v /sys/ | sort | uniq | wc -l > actual_stdout_file 2> actual_stderr_file
test_name="Piping Test 1 stdout"
cmp stdout_file actual_stdout_file
$assert_success
test_name="Piping Test 1 stderr"
cmp stderr_file actual_stderr_file
$assert_success

./smash <(printf "ls -al | wc") > stdout_file 2> stderr_file
ls -al | wc > actual_stdout_file 2> actual_stderr_file
test_name="Piping Test 2 stdout"
cmp stdout_file actual_stdout_file
$assert_success
test_name="Piping Test 2 stderr"
cmp stderr_file actual_stderr_file
$assert_success

./smash <(printf "ls -al . badfile >dummy_file 2>stderr_file | grep . | wc -l >stdout_file")
ls -al . badfile >dummy_file 2>actual_stderr_file | grep . | wc -l > actual_stdout_file
test_name="Piping with early redirection stdout"
cmp stdout_file actual_stdout_file
$assert_success
test_name="Piping with early redirection stderr"
cmp stderr_file actual_stderr_file
$assert_success

printf "==================== TEST05.SH END ====================\n"
test_type="test05.sh"
rm stdout_file stderr_file actual_stdout_file actual_stderr_file dummy_file
. shell_tests/finish.sh

