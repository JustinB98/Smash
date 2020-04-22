#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST02.SH START ==============="

test_name="ls Test 1"
./smash <(echo ls) > test_file
cmp test_file <(ls)
$assert_success

test_name="ls Test 2"
./smash <(echo ls /tmp -al) > test_file
cmp test_file <(ls /tmp -al)
$assert_success

test_name="wc Test"
./smash <(echo wc ./smash) > test_file
cmp test_file <(wc ./smash)
$assert_success

test_name="git status Test"
./smash <(echo git status) > test_file
cmp test_file <(git status)
$assert_success

printf "==================== TEST02.SH END ====================\n"
test_type="test02.sh"
rm test_file
. shell_tests/finish.sh
