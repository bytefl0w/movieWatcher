## :clapper: Movie Watcher :movie_camera:
# Description
Portfolio Project for CS 201-001 Spring 2019

# Install
Step 1: Download the IMDB dataset, unzip, and grep movies
```
wget https://datasets.imdbws.com/title.basics.tsv.gz
gzip -d title.basics.tsv.gz
grep "movie" title.basics.tsv > movie_records
```

Step 2: build and run project
```
make
./movieWatcher
```
Note: Instructions self-explanitory, use video to see a demonstration of it working and examples of how to use the software.

# Cited Code
I used the AVL data structure coded and tested by Dr. James Aspnes, a professor at Yale that teaches a Data Structures and Programming Techniques course at the university. I decided to use his implementation of an AVL tree since he was a well recognized professor and had a well documented and tested implementation. Even though the usage of his code is heavily modified to work with the project, all of the code used can be found in: src/tree.c and src/tree.h
LINK: http://www.cs.yale.edu/homes/aspnes/classes/223/notes.html

# Video
https://www.youtube.com/channel/UCHxvMdVO0mWFop8EX4IkK0g