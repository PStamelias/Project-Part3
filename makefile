all: search cluster
search: mainBC.c 
	gcc -o search mainBC.c
cluster: mainD.c
	gcc -o cluster mainD.c

clean:
	rm search
	rm cluster