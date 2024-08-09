#!/bin/bash

# Function to send IRC commands
send_command() {
  local host=$1
  local port=$2
  local nick=$3
  local password=$4
  local command=$5
  {
    echo "PASS $password"
    echo "NICK $nick"
    echo "USER $nick 0 * :Test User"
    echo "$command"
  } | nc $host $port 
}

# Start test
HOST="127.0.0.1"
PORT="6667"
PASSWORD="1234"  # 서버에 사용할 비밀번호를 여기에 설정

# Simulate multiple clients connecting to the server
send_command $HOST $PORT "user1" $PASSWORD "JOIN #channel1"
send_command $HOST $PORT "user2" $PASSWORD "JOIN #channel1"
send_command $HOST $PORT "user1" $PASSWORD "PRIVMSG #channel1 :Hello from user1!"
send_command $HOST $PORT "user2" $PASSWORD "PRIVMSG #channel1 :Hello from user2!"

send_command $HOST $PORT "user3" $PASSWORD "JOIN #channel2"
send_command $HOST $PORT "user4" $PASSWORD "JOIN #channel2"
send_command $HOST $PORT "user3" $PASSWORD "PRIVMSG #channel2 :Hello from user3!"
send_command $HOST $PORT "user4" $PASSWORD "PRIVMSG #channel2 :Hello from user4!"

# Test mode commands
send_command $HOST $PORT "user1" $PASSWORD "MODE #channel1 +o user1"
send_command $HOST $PORT "user1" $PASSWORD "MODE #channel1 +k secretpass"
send_command $HOST $PORT "user1" $PASSWORD "MODE #channel1 +t"
send_command $HOST $PORT "user1" $PASSWORD "TOPIC #channel1 :New Topic"

# Join another channel and send a message
send_command $HOST $PORT "user1" $PASSWORD "JOIN #channel3"
send_command $HOST $PORT "user1" $PASSWORD "PRIVMSG #channel3 :This is a test message in channel3."

# Quit clients
send_command $HOST $PORT "user1" $PASSWORD "QUIT :Goodbye from user1"
send_command $HOST $PORT "user2" $PASSWORD "QUIT :Goodbye from user2"
send_command $HOST $PORT "user3" $PASSWORD "QUIT :Goodbye from user3"
send_command $HOST $PORT "user4" $PASSWORD "QUIT :Goodbye from user4"
