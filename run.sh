#!/bin/bash
set -e

JSXPATH=./.sandbox/javascript/focus.jsx

if [[ $OSTYPE != "darwin"* && $OSTYPE != "linux"* ]]; then
    printf "Script only works on macOS for now.\nPlease open an issue (or better PR) if you want to use this script from another platform."
    exit 1
else
    printf "\e[0;32m[run.sh] Running script on supported OS... $OSTYPE.\e[0m\n"
fi

if [[ $OSTYPE == "darwin"* ]]; then
    LASTTIME=`stat -f %m $JSXPATH`
elif [[ $OSTYPE == "linux"* ]]; then
    LASTTIME=`stat -c %Y $JSXPATH`
fi


if command -v tsc >/dev/null 2>&1; then
    TSC=tsc
elif command -v `npx tsc` >/dev/null 2>&1; then
    TSC="npx tsc"
else
    printf "Could not find global or local tsc command. Needed for compiling tsx to js."
    exit 1
fi

runTypescriptAndCompile() {
    printf "\e[0;32m[run.sh] Transpiling ${JSXPATH##*/} using the typescript compiler... \e[0m\n"
    $TSC $JSXPATH --jsx react --allowJs --module es2022 --jsx react --jsxFactory GUI.createElement --jsxFragmentFactory GUI.Fragment
    printf "\e[0;32m[run.sh] Compiling Desktop.js application... \e[0m\n"
    make main
    (DYLD_LIBRARY_PATH=build \
	./djs-aarch64-macos run ${JSXPATH%.jsx}.js) || printf "\e[0;31m[run.sh] make run failed... \e[0m\n"
}

runTypescriptAndCompile
while true; do
    if read -t 1 -n 1 key; then
        printf "\e[0;33m[run.sh] Key pressed, rerunning...\e[0m\n"
        runTypescriptAndCompile
        LASTTIME=`stat -f %m "$JSXPATH"`
        continue
    fi

    MODIFIEDTIME=`stat -f %m $JSXPATH`
    if [[ $LASTTIME != $MODIFIEDTIME ]]; then
        runTypescriptAndCompile

        LASTTIME=$MODIFIEDTIME
    fi
done
