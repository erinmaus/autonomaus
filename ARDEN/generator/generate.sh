#!/usr/bin/env bash

SCRIPT_LOCATION=$(readlink -f "$0")
ROOT_PATH=$(dirname "$SCRIPT_LOCATION")
LUA_PATH="${ROOT_PATH}/?.lua;${ROOT_PATH}/generator/?.lua;${LUA_PATH}"

exec moon "${ROOT_PATH}/generator/${1}/Main.moon" "${@:2}"
