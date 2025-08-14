#!/usr/bin/env bash
set -euo pipefail
out=games_manifest.c
{
  echo "// Auto-generated. Edit games/* instead."
  echo "#include \"games.h\""
  echo "#include <stddef.h>"
  echo "typedef struct GameEntry GameEntry;"
  echo "const GameEntry g_games[] = {"
  shopt -s nullglob
  for d in games/*; do
    namefile="$d/game.txt"; screenfile="$d/screen.txt"; runnerfile="$d/runner.sym"
    if [[ -f "$namefile" ]]; then
      name=$(head -n1 "$namefile" | tr -d '\r')
      echo "  { .name = \"$name\", .screen = {"
      if [[ -f "$screenfile" ]]; then
        mapfile -t lines < "$screenfile"
      else
        lines=("" "" "" "" "" "" "")
      fi
      for i in {0..13}; do
        s="${lines[$i]:-}"
        s="${s//$'\r'/}"
        # trim to 20 and pad
        s="${s:0:20}"
        printf -v row '%-20s' "$s"
        echo "    \"${row}\"," 
      done
      # Detect optional runner symbol name (C function)
      runptr=0
      if [[ -f "$runnerfile" ]]; then
        sym=$(head -n1 "$runnerfile" | tr -d '\r')
        [[ -n "$sym" ]] && runptr="$sym"
      fi
      if [[ "$runptr" = 0 ]]; then
        echo "  }, .run = 0 },"
      else
        echo "  }, .run = $runptr },"
      fi
    fi
  done
  echo "};"
  echo "const size_t g_games_count = sizeof(g_games)/sizeof(g_games[0]);"
} > "$out"
