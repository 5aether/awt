all: awt

awt: main.c
	gcc -O3 main.c -o awt

install: awt
	install -Dm755 awt /bin/awt

uninstall:
	rm -f /bin/awt

clean:
	rm -f awt
