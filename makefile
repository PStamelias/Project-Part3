all: search cluster
search: mainB.c funB.c
	gcc -o search mainB.c funB.c
cluster: mainD.c funB.c funD.c
	gcc -o cluster mainD.c funD.c funB.c
clean:
	rm search
	rm cluster
