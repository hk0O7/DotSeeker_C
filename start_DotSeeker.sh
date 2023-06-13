#!/bin/bash
## DotSeeker start script.


DS_dir="$(dirname "$0")"
DS_path=""$DS_dir"/DotSeeker"

### If not in a desktop environment, just run and exit.
if [[ -z "$DESKTOP_SESSION" ]]; then
	"$DS_path"
	exit "$?"
fi

### In a desktop environment...
## If xterm is not present, complain.
if ! {
	command -v xterm 1>/dev/null 2>&1
}; then
	## If zenity is not present, just complain to STDOUT.
	if ! {
		command -v zenity 1>/dev/null 2>&1
	}; then
		printf "Error: xterm does not appear to be installed."
		printf " Please, install xterm.\n"
		printf "Also, zenity is not installed.\n"
	else ## Use zenity.
		zenity \
		  --error \
		  --no-wrap \
		  --text='Error: xterm does not appear to be installed.\nPlease, install xterm.'
	fi
	exit 1
fi

## Run in xterm:
xterm \
	-fn 12x24 \
	-T "DotSeeker" \
	-e "$DS_path"

