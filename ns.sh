#!/usr/bin/env bash

set -Eeuo pipefail


# —————— Common utility functions ——————
log() {
  # [YYYY-MM-DD HH:MM:SS] LEVEL message
  printf '[%(%F %T)T] %-5s %s\n' -1 "$1" "$2"
}
require() {
  command -v "$1" &>/dev/null || {
    log FAIL "$1 not found, aborting"
    exit 127
  }
}

# —————— Default settings ——————
DELAY="${DELAY:-0.2}"    # pause between notifications
COUNT="${COUNT:-20}"     # number of notifications in loop tests

# —————— Test descriptions ——————
declare -A DESCRIPTIONS=(
  [1]="Basic notification"
  [2]="Expire: -t 10000"
  [3]="Urgency levels"
  [4]="Series load"
  [5]="Multiline & Unicode"
  [6]="Icon display"
  [7]="Big text stress"
  [8]="Invalid-flag robustness"
  [9]="Mixed urgencies"
  [10]="Actions (buttons)"
  [11]="Hints (transient,category)"
)

# —————— Mapping test numbers to functions ——————
declare -A FUNCS=(
  [1]=test_basic
  [2]=test_expire
  [3]=test_urgency
  [4]=test_series
  [5]=test_multiline
  [6]=test_icon
  [7]=test_bigtext
  [8]=test_invalid
  [9]=test_mixed_urgency
  [10]=test_actions
  [11]=test_hints
)

# —————— Test functions ——————

test_basic()       { notify-send "1/Basic" "Hello from test 1"; }
test_expire()      { notify-send -t 10000 "2/Expire" "10s lifetime"; }
test_urgency() {
  for u in low normal critical; do
    notify-send -u "$u" "3/Urgency-$u" "Priority: $u"
  done
}
test_series() {
  for i in $(seq 1 "$COUNT"); do
    notify-send "4/Series $i/$COUNT" "Payload"; sleep "$DELAY"
  done
}
test_multiline()   { notify-send "5/Multiline" $'Line1\nLine2\n🙂🚀'; }
test_icon()        { notify-send -i dialog-information "6/Icon" "Icon test"; }
test_bigtext() {
  big=$(printf 'X%.0s' $(seq 1 5000))
  notify-send "7/BigText" "$big"
}
test_invalid()     {
  if notify-send --no-such-flag &>/dev/null; then return 1; else return 0; fi
}
test_mixed_urgency() {
  for i in $(seq 1 12); do
    case $((i%3)) in
      0) u=low;; 1) u=normal;; 2) u=critical;;
    esac
    notify-send -u "$u" "9/Mixed-$i" "urgency=$u"; sleep "$DELAY"
  done
}
test_actions()     { notify-send --action="term:Open Terminal" "10/Actions" "Click to open terminal" & }
test_hints()       { notify-send --hint=int:transient:1 --hint=string:category:example "11/Hints" "Transient & category"; }

# —————— Run a single test ——————
run_one() {
  local num="$1"
  # Check that description and function exist
  if ! [[ -v DESCRIPTIONS[$num] && -v FUNCS[$num] ]]; then
    log FAIL "Unknown test: $num"
    return 1
  fi
  local fn="${FUNCS[$num]}"
  log INFO "Starting $num – ${DESCRIPTIONS[$num]}"
  if $fn; then
    log PASS "Test $num passed"
  else
    log FAIL "Test $num failed"
  fi
}

# —————— Help ——————
usage() {
  cat <<EOF
Usage: ${0##*/} <num>|list|all
  list   – показать доступные тесты
  all    – выполнить все tests (последовательно)
Env vars:
  DELAY – пауза между уведомлениями (по умолчанию 0.2)
  COUNT – количество в цикле (по умолчанию 20)
EOF
}

# —————— Main ——————
main() {
  require notify-send
  [[ $# -ne 1 ]] && { usage; exit 2; }

  case "$1" in
    list)
      for k in $(printf '%s\n' "${!DESCRIPTIONS[@]}" | sort -n); do
        printf "  %2d) %s\n" "$k" "${DESCRIPTIONS[$k]}"
      done
      ;;
    all)
      for k in $(printf '%s\n' "${!DESCRIPTIONS[@]}" | sort -n); do
        run_one "$k"
      done
      ;;
    *)
      run_one "$1"
      ;;
  esac
}

main "$@"
