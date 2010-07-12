" Конфигурационный файл Vim IDE

" Запрещаем восстановление настроек из сессии,
" т. к. тогда при изменении ~/.vimrc даже после
" перезагрузки IDE новые настройки не будут
" вступать в силу.

set sessionoptions-=options

" Добавляем пути к библиотекам
" set path+=/usr/include/gtk-2.0
" set path+=./netlog

" Устанавливает правила синтаксиса, специфичные для данного проекта.
" -->
function! MySetIdeSyntax()
	if getfsize(".vim/syntax.vim") >= 0
		source .vim/syntax.vim
	endif
endfunction
" <--
" Настраиваем работу с ctags -->
" set tags=.vim/ctags
" function! MyUpdateIdeCtags()
"	!make vim
" endfunction

" let MyUpdateCtagsFunction = "MyUpdateIdeCtags"

" Настраиваем работу с ctags <--
" Обновляем базу ctags при старте IDE
" call {MyUpdateCtagsFunction}()
" При открытии нового буфера устанавливаем для него
" правила синтаксиса, специфичные для данного проекта.
au BufReadPost * :call MySetIdeSyntax()
" Загружаем ранее сохраненную сессию
"
function! PushGit(comment)
	cd .. 
	Git add .
	Git commit -m string(a:comment)
	Git push origin master
	cd src
endfunction

" :command! PushGit -nargs=1 Comment  call PushGit(Comment)
