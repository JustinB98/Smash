return_val=$?
if [ $return_val -eq $error_code ]
	then
		total_pass=$((total_pass+1))
		printf "\033[0;32m"
		printf "Passed: $test_name\n"
	else
		total_fail=$((total_fail+1))
		printf "\033[0;31m"
		printf "Failed because exit code $return_val != expected $error_code exit: $test_name\n"
fi
printf "\033[0m"
