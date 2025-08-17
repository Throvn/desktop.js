#!/bin/bash
# Compare memory usage of Node.js vs txiki.js on macOS
# Usage: ./compare_mem.sh

SCRIPT="memtest.js"

# Create a minimal test script if it doesn't exist
if [ ! -f "$SCRIPT" ]; then
  cat <<'EOF' > "$SCRIPT"
console.log("memory test");
setTimeout(() => {}, 10000); // keep alive for 10s
EOF
fi

measure() {
  local cmd=$1
  echo ">>> Testing $cmd"
  $cmd "$SCRIPT" &
  pid=$!
  sleep 1

  # Quick check with ps
  ps -o pid,rss,vsz,comm -p $pid

  # More detailed with vmmap (Physical footprint line)
  vmmap $pid | grep "Physical footprint"

  kill $pid 2>/dev/null
  wait $pid 2>/dev/null
  echo
}

measure "node"
measure "./lib/txiki.js/build/tjs run"
