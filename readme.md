# Link Analysis
This project has been developed by Marco Murador and Alberto Razza for the course of Information Retrival and Web Search at Ca'Foscari university. It has been develop in C language because of the optimization for large amount of data. We use this dataset for computing link analysis http://snap.stanford.edu/data/web-NotreDame.html

### How to compile

```
gcc -O3 <name_file>.c -o <name_file>
./<name_file>
```

In case you want to use makefile for compiling and running code faster, just run:

Pagerank : 
```
make pagerank 
```
Pagerank with mmap:
```
make pagerank_mmap
```
HITS : 
```
make hits
```
In-Degree:
```
make indegree
```
Jaccard:
```
make jaccard
```

In case you recive, when compiling, the message "make: `<name_file>' is up to date.", just run
```
./name_file
```

