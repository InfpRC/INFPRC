#!/bin/bash

# Function to send IRC commands
send_command() {
  local host=$1
  local port=$2
  local nick=$3
  local command=$4
  {
    echo "NICK $nick"
    echo "USER $nick 0 * :Test User"
    echo "$command"
  } | nc $host $port 
}

# Start test
HOST="127.0.0.1"
PORT="6667"

# Simulate multiple clients connecting to the server
send_command $HOST $PORT "user1" "JOIN #channel1"
send_command $HOST $PORT "user2" "JOIN #channel1"
send_command $HOST $PORT "user1" "PRIVMSG #channel1 :Hello from user1!"
send_command $HOST $PORT "user2" "PRIVMSG #channel1 :Hello from user2!"

send_command $HOST $PORT "user3" "JOIN #channel2"
send_command $HOST $PORT "user4" "JOIN #channel2"
send_command $HOST $PORT "user3" "PRIVMSG #channel2 :Hello from user3!"
send_command $HOST $PORT "user4" "PRIVMSG #channel2 :Hello from user4!"

# Test mode commands
send_command $HOST $PORT "user1" "MODE #channel1 +o user1"
send_command $HOST $PORT "user1" "MODE #channel1 +k secretpass"
send_command $HOST $PORT "user1" "MODE #channel1 +t"
send_command $HOST $PORT "user1" "TOPIC #channel1 :New Topic"

# Simulate an OPER command
send_command $HOST $PORT "user1" "OPER admin adminpassword"

# Join another channel and send a message
send_command $HOST $PORT "user1" "JOIN #channel3"
send_command $HOST $PORT "user1" "PRIVMSG #channel3 :This is a test message in channel3."

# Quit clients
send_command $HOST $PORT "user1" "QUIT :Goodbye from user1"
send_command $HOST $PORT "user2" "QUIT :Goodbye from user2"
send_command $HOST $PORT "user3" "QUIT :Goodbye from user3"
send_command $HOST $PORT "user4" "QUIT :Goodbye from user4"
