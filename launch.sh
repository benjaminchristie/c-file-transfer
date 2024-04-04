#!/bin/sh

set -e

if [ ! -d build ]; then
	mkdir build
fi

cd build && cmake .. && make

# ripped from https://stackoverflow.com/questions/1923435/how-do-i-echo-stars-when-reading-password-with-read

unset USER_SOCKET_KEY
unset CHARCOUNT

echo -n "Enter password: "

stty -echo

CHARCOUNT=0
while IFS= read -p "$PROMPT" -r -s -n 1 CHAR
do
    # Enter - accept password
    if [[ $CHAR == $'\0' ]] ; then
        break
    fi
    # Backspace
    if [[ $CHAR == $'\177' ]] ; then
        if [ $CHARCOUNT -gt 0 ] ; then
            CHARCOUNT=$((CHARCOUNT-1))
            PROMPT=$'\b \b'
            USER_SOCKET_KEY="${USER_SOCKET_KEY%?}"
        else
            PROMPT=''
        fi
    else
        CHARCOUNT=$((CHARCOUNT+1))
        PROMPT='*'
        USER_SOCKET_KEY+="$CHAR"
    fi
done

stty echo

export USER_SOCKET_KEY=$(printf '%-32s' "$USER_SOCKET_KEY")

printf "\nLaunching server..."
./server
