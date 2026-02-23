#!/usr/bin/env bash
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
PROG="$HERE/../main"
if [ ! -x "$PROG" ]; then
  echo "Program not found; build it first (run 'make')."
  exit 2
fi

declare -a inputs=(
"sin10x"
"sinx"
"cos5x"
"exp(2x)"
"1/x"
"3x^2"
"x^3"
"-2.5x"
"sin(10)"
"sin(-x)"
)

declare -a expect=(
"-0.1 cos(10x)"
"-cos(x)"
"0.2 sin(5x)"
"0.5 exp(2x)"
"ln|x|"
"1 x^3"
"0.25 x^4"
"-1.25 x^2"
"-0.544021 x"
"cos(-x)"
)

pass=0
fail=0
for i in "${!inputs[@]}"; do
  in="${inputs[$i]}"
  exp="${expect[$i]}"
  out=$(printf "%s\n" "$in" | "$PROG" | tail -n1)
  if echo "$out" | grep -Fq -- "$exp"; then
    echo "PASS: $in -> $out"
    pass=$((pass+1))
  else
    echo "FAIL: $in -> $out (expected contains '$exp')"
    fail=$((fail+1))
  fi
done

echo "Passed: $pass, Failed: $fail"
if [ $fail -ne 0 ]; then exit 1; fi
