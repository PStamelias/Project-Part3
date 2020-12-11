all: search cluster
search: mainBC.c 
	gcc -o search mainBC.c funBC.c
cluster: mainD.c
	gcc -o cluster mainD.c funD.c

clean:
	rm search
	rm cluster