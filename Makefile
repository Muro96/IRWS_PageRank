pagerank : pagerank.c
	gcc -O3 pagerank.c -o pagerank
	./pagerank

hits : hits.c
	gcc -O3 hits.c -o hits
	./hits

in : In-Degree.c
	gcc -O3 In-Degree.c -o in
	./in

