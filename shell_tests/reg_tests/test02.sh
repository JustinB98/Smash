#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

echo "============ TEST02.SH START ==============="

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

python -c "print('echo test sentence' * 100)" > static_file
test_name="Echo Test Stress Test"
./smash <(printf "echo " ; cat static_file) > test_file 2> /dev/null
cmp test_file static_file
$assert_success

printf "==================== TEST02.SH END ====================\n"
test_type="test02.sh"
rm test_file
. shell_tests/finish.sh
