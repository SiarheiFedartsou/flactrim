#!/bin/bash

# if [[ $# != 1 ]]
# then
#	zenity --title "Vim IDE usage error" --error --text "Usage: vim_ide /path/to/project/dir."
#	exit 1
# fi

# if [[ ! -e "~/Projects/flactrim/.vim/ide.vim" ]]
# then
#	zenity --title "Vim IDE usage error" --error --text "'$1' is not a Vim IDE project directory."
#	exit 1
# fi

cd "/home/miksayer/Projects/flactrim/src" || { zenity --title "Vim IDE usage error" --error --text "Can't change current directory to Vim IDE project directory '~/Projects/flactrim/'."; exit 1; }
ctags -R --c++-kinds=+p --fields=+iaS --extra=+q
gvim --class=IDE -S ../.vim/ide.vim
