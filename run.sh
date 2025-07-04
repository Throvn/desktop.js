#!/bin/bash
set -e

printf "\e[0;32m[run.sh] Transpiling test.jsx using the typescript compiler... \e[0m\n";
tsc ./js/test.jsx --jsx react --allowJs --module es2022 --jsx react --jsxFactory GUI.createElement --jsxFragmentFactory GUI.Fragment
printf "\e[0;32m[run.sh] Compiling Desktop.js application... \e[0m\n";
make run;