#!/bin/bash

./procchat 2> server_log.txt &
./client1 > client1_log.txt &
./client2 > client2_log.txt &
./client3 > client3_log.txt &
sleep 45
pkill client1
pkill client2
pkill client3
sleep 45
killall procchat
diff client1_log.txt client1_expected.txt
diff client2_log.txt client2_expected.txt
diff client3_log.txt client3_expected.txt
diff server_log.txt server_expected.txt

echo "Finished running all tests. If no message was displayed, the actual output of the client programs matched the expected output. Else,
try running the script a couple more times or in the Ed workspace environment to verify it is working properly."