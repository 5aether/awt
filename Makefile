all: awt

awt: awt.c
	gcc -O3 awt.c -o awt

install: awt
	install -Dm755 awt /bin/awt

uninstall:
	rm -f /bin/awt

clean:
	rm -f awt
