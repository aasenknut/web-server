web:
	gcc -Wall -o ./web ./c/web.c

clean:
	rm ./web && ./web.o

ex:
	./web
