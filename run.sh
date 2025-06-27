#!/bin/bash
set -e

printf "\e[0;32m[run.sh] Transpiling test.jsx using the typescript compiler... \e[0m\n";
tsc test.jsx --jsx react --allowJs --module es2022;
printf "\e[0;32m[run.sh] Compiling Desktop.js application... \e[0m\n";
LIBGL_ALWAYS_SOFTWARE=1 make run;