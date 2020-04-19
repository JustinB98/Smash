if [ $total_fail -eq 0 ]
then
	printf "\033[0;32m"
	printf "All $test_type tests passed!\n"
	printf "\033[0m"
	exit 0
else
	printf "\033[1;33m"
	printf "Total Amount Passed: $total_pass\n"
	printf "Total Amount Failed: $total_fail\n"
	printf "\033[0m"
	exit 1
fi
