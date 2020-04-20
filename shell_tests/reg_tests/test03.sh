#!/bin/sh

total_pass=0
total_fail=0

assert_success=". shell_tests/assert_successful_exit.sh"
assert_fail=". shell_tests/assert_unsuccessful_exit.sh"

static_file_path="shell_tests/reg_tests/static_shell_files"
test_files_path="$static_file_path/test_files"
stdout_file_path="$static_file_path/expected_stdout_output_files"
stderr_file_path="$static_file_path/expected_stderr_output_files"

echo "============ TEST03.SH START ==============="

for test_script in `ls $test_files_path`
do
	full_path=$test_files_path/$test_script
	base_name=${test_script%.sh}
	stdout_file="$stdout_file_path/$base_name.out"
	stderr_file="$stdout_file_path/$base_name.out"
	chmod +x $full_path
	./$full_path > actual_stdout 2> actual_stderr
	test_name="$base_name stdout test"
	cmp $stdout_file actual_stdout
	$assert_success
	test_name="$base_name stderr test"
	cmp $stderr_file actual_stderr
	$assert_success
done

printf "==================== TEST03.SH END ====================\n"
test_type="test03.sh"
rm actual_stdout actual_stderr
. shell_tests/finish.sh
