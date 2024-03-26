audio_trigger: audio_trigger.c
	gcc -o audio_trigger audio_trigger.c -lm -lc

install: audio_trigger
	cp audio_trigger /usr/bin
