#!/bin/bash

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"
echo "============ TEST04.SH START ==============="

test_name="stdout Redirection Test With Echo One Word"
./smash <(echo test >stdout_file)
cmp stdout_file <(echo test)
$assert_success

test_name="stdout Redirection Test With Multiple Words"
./smash <(echo test sentence that is long >stdout_file) 
cmp stdout_file <(echo test sentence that is long)
$assert_success

test_name="stdout Redirection Test with ls"
./smash <(echo "ls >stdout_file")
cmp stdout_file <(ls)
$assert_success

test_name="Clear stdout file Test"
./smash <(echo "bad >stdout_file") 2> /dev/null
cmp stdout_file <(printf "")
$assert_success

test_name="stderr Redirection Test 1"
./smash <(echo "ls x/y/z 2>stderr_file")
ls x/y/z 2> actual_stderr_file
cmp stderr_file actual_stderr_file
$assert_success

test_name="stderr Redirection Test 2"
./smash <(echo "git bad_command 2>stderr_file")
git bad_command 2>actual_stderr_file
cmp stderr_file actual_stderr_file
$assert_success

python -c "print('Good test ' * 100)" > stdin_file
test_name="stdin Redirection Test 1"
./smash <(echo "cat <stdin_file >stdout_file")
cat < stdin_file > actual_stdout_file
cmp actual_stdout_file stdout_file
$assert_success

test_name="stdin Redirection Test 2"
./smash <(echo "cat <bad_file_that_doesn't exit") 2>/dev/null
$assert_fail

printf "==================== TEST04.SH END ====================\n"
test_type="test04.sh"
rm stdin_file stdout_file stderr_file actual_stderr_file actual_stdout_file
. shell_tests/finish.sh
