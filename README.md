# Distributed Systems Assignment 1
## MPI Programming

### Problem 1
- **Description**:
We have to take sum N times, so we divide N by the number of processes available (also taking care of the remainder) and then each process parallely sums only the range given to it, and then we sum the results of all the individual processes and write it to output file.
- **Analysis**:
    - For N = 2
        - if np = 1 - Total time (s): 0.000170
        - if np = 11 - Total time (s): 0.000393
    - For N = 10000:
        - if np = 1 - Total time (s): 0.000199
        - if np = 11 - Total time (s): 0.000409
    - It is obvious that time will increase almost monotically with increase in N.
    - A but surprising result is that the more the number of processes, the more time it takes which is counter-intutive. But, it can be justified as the overhead of sending and receiving from processes is more than the time it takes to compute the very simple sum. Hence the time increases.

---

### Problem 2
- **Description**:
    - We have sort the array using parallel quick sort. So we divide the array almost equally into np parts. Then each process parallely sorts their portion of the array using quicksort and sends the sorted array back to the root process. Then the root process uses k-way merge technique to merge the sorted blocks. In k-way merge, we use a min heap and initially insert the smallest elements of each block into the heap. Then we take the min out of these and remove it from the heap and add the next smallest element of the block, from which the element was removed, into the heap. We continue this untill we get a sorted list of n elements.
    - **Note** that it is assumed that all the elements of the array fit into the integer datatype. Since, no constraints on the range of the values of elements were given, it was safe to assume that.
- **Analysis**:
    - For N = 10 and array elements randomly initialized
        - if np = 1 - Total time (s): 0.000231
        - if np = 4 - Total time (s): 0.000319
        - if np = 11 - Total time (s): 0.001180
    - For N = 10000 and array elements randomly initialized
        - if np = 1 - Total time (s): 0.005414
        - if np = 4 - Total time (s): 0.007066
        - if np = 11 - Total time (s): 0.007716
    - For N = 1000000 and array elements randomly initialized
        - if np = 1 - Total time (s): 0.560626
        - if np = 4 - Total time (s): 0.606397
        - if np = 11 - Total time (s): 0.731500
    - It is obvious that time will increase almost monotically with increase in N
    - A but surprising result is that the more the number of processes, the more time it takes which is counter-intutive. But, it can be justified as the overhead of sending and receiving a large part of array is more than the very simple quick sort that it has to do. The best time we get is when np = 2. After it, the message passing becomes the bottleneck and there is not much parallelization can do to reduce the time.

---

### Problem 3
- **Description**:
    - We want a valid edge coloring of the graph in no more than delta + 1 colors, where delta is the maximum of the degrees of all the vertices in the original graph or the line graph. We convert the problem into finding a valid vertex coloring of the line graph of the original graph, which is equivalent to finding the edge coloring of the original graph.
    - After getting the line graph, we then random unique weights/values to all the vertices.
    - In each iteration, we find all those vertices that are the maximum amongst their uncolored neighbors. They will form an independent set.
    - Since they form an independent set, we can color them parallely without any contradiction.
    - To color each node, we take the minimum color which has not been yet assigned to any of its colored neighbors.
    - To parallelize this even further, the root process will not find the independent set. Instead, that task is divided with each process where each process finds an independent set from the nodes in it's range.
    - Repeat this until all nodes are colored.
- **Analysis**:
    - For N = 5, M = 10 and the graph randomly intialized according to N and M
        - if np = 1 - Total time (s): 0.000307
        - if np = 3 - Total time (s): 0.003080
        - if np = 11 - Total time (s): 0.009718
    - For N = 80, M = 400 and the graph randomly intialized according to N and M
        - if np = 1 - Total time (s): 0.004600
        - if np = 3 - Total time (s): 0.004159
        - if np = 11 - Total time (s): 0.015204
    - For N = 100, M = 500 and the graph randomly intialized according to N and M
        - if np = 1 - Total time (s): 0.006827
        - if np = 3 - Total time (s): 0.005698
        - if np = 11 - Total time (s): 0.017221
    - It is obvious that time will increase almost monotically with increase in N and M.
    - When there are less number of nodes and edges in the graph, the overhead of sending the large adjacency matrix and all the other supporting arrays and values is larger and hence the time increases with the increase in the number of processes.
    - When the number of nodes and edges start to increase, we see that the time reduces till some number of processes and then it starts increasing again. Till np = 3 or 4, it can exploit the benefits of parallelization, but after that the message passing becomes a bottleneck and the time starts to increase again.

---

### Assignment by
- Tanish Lad (2018114005)
