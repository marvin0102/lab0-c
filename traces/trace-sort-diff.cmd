# Test the different proformance between q_sort and list_sort
new
ih RAND 1000
sort
free
new
ih RAND 10000
sort
new
ih RAND 100000
sort
free
option linuxsort 1
new
ih RAND 1000
sort
free
new
ih RAND 10000
sort
new
ih RAND 100000
sort
free
