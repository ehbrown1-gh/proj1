#!/bin/bash
#To save: script -c "./test_doit.sh" test_results.txt

echo "TEST 1: Part 1 command execution"
echo "Command: ./doit date"
./doit date
echo

echo "TEST 2: Foreground command"
echo "Command: ./doit ls"
./doit ls
echo

echo "TEST 3: Interactive shell and built-in commands"
printf 'pwd\ncd /tmp\npwd\ncd -\nset prompt = myprompt:\npwd\nexit\n' | ./doit
echo

echo "TEST 4: Background job and jobs command"
printf 'sleep 2 &\njobs\nsleep 3\njobs\nexit\n' | ./doit
echo

echo "TEST 5: Multiple background jobs"
printf 'sleep 3 &\nsleep 1 &\njobs\nsleep 2\njobs\nsleep 2\njobs\nexit\n' | ./doit
echo

echo "TEST 6: exit waits for unfinished background job"
printf 'sleep 2 &\nexit\n' | ./doit
echo

echo "TEST 7: Invalid built-in command"
printf 'cd\nset prompt\nexit\n' | ./doit
echo

echo "Tests complete."