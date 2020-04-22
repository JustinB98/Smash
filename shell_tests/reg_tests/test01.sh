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

test_name="Non GNU Echo Test"
./smash <(echo "echo --help") > test_file 2> /dev/null
cmp test_file <(echo "--help")
$assert_success

test_name="Echo Test One Word"
./smash <(echo "echo hello") > test_file 2> /dev/null
cmp test_file <(echo "hello")
$assert_success

test_name="Echo Test Multiple Words"
./smash <(echo "echo very long echo test") > test_file 2> /dev/null
cmp test_file <(echo "very long echo test")
$assert_success

test_name="Path Env Var Test"
./smash <(echo "echo \$PATH") > test_file
cmp test_file <(echo "$PATH")
$assert_success

test_name="Home Env Var Test"
./smash <(echo "echo \$HOME") > test_file
cmp test_file <(echo "$HOME")
$assert_success

test_name="Env Var Doesn't Exit Test"
./smash <(echo "echo \$WEIRD_ENVIRONMENT_VARIABLE") > test_file
cmp test_file <(echo "$WEIRD_ENVIRONMENT_VARIABLE")
$assert_success

python -c "print('echo test sentence' * 100)" > static_file
test_name="Echo Test Stress Test"
./smash <(printf "echo " ; cat static_file) > test_file 2> /dev/null
cmp test_file static_file
$assert_success

test_name="cd Test 1"
./smash <(echo cd ..; echo pwd) > test_file
cmp test_file <(cd ..; pwd)
$assert_success

test_name="cd Test 2"
./smash <(echo cd /tmp; echo pwd) > test_file
cmp test_file <(cd /tmp ; pwd)
$assert_success

test_name="cd Test 3 (No arg)"
./smash <(echo cd ; echo ls -al ; echo pwd) > test_file
cmp test_file <(cd ; ls -al; pwd)
$assert_success

printf "==================== TEST01.SH END ====================\n"
test_type="test01.sh"
rm test_file static_file
. shell_tests/finish.sh
