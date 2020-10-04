pagerank : pagerank.c
	gcc -O3 pagerank.c -o pagerank
	./pagerank

pagerank_mmap : pagerank_mmap.c
	gcc -O3 pagerank_mmap.c -o pagerank_mmap
	./pagerank_mmap

hits : hits.c
	gcc -O3 hits.c -o hits
	./hits

in : In-Degree.c
	gcc -O3 In-Degree.c -o in
	./in

jaccard: jaccard.c
	gcc -O3 jaccard.c -o jaccard
	./jaccard

