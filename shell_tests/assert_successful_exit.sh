if [ $? -ne 0 ]
	then
		total_fail=$((total_fail+1))
		printf "\033[0;31m"
		printf "Failed with non zero exit status: $test_name\n"
	else 
		total_pass=$((total_pass+1))
		printf "\033[0;32m"
		printf "Passed: $test_name\n"
fi
printf "\033[0m"
