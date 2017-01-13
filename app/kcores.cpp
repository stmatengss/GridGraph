/*************************************************************************
    > File Name: kcores.cpp
    > Author: stmatengss
    > Mail: stmatengss@163.com 
    > Created Time:
 ************************************************************************/

#include <iostream>
#include "core/graph.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	/* code */
    if (argc < 2) {
        fprintf(stderr, "usage: k-cores [path] [memory budget in GB]\n");
        exit(-1);
    }   

    string path = argv[1];
    int k = atoi(argv[2]);

    long memory_bytes = ((argc >= 4)?atol(argv[3]):8l)*1024l*1024l*1024l;

    Graph graph(path);
    graph.set_memory_bytes(memory_bytes);

    Bitmap* active_in = graph.alloc_bitmap();
    Bitmap* active_out = graph.alloc_bitmap();
    BigVector<VertexId> degree(graph.path + "/degree", graph.vertices);
    graph.set_vertex_data_bytes(graph.vertices * sizeof(VertexId));

    active_out->fill();

    int iters = 0;

    VertexId need_del;
    VertexId pre_need_del = -1; 

    while (true) {
        printf("iters = %4d\n", iters++ );
        if (iters > 50) break;
        degree.fill(0);

        swap(active_in, active_out);
        active_out->clear();
        graph.hint(degree);
        // cal the degree
        graph.stream_edges<VertexId>([&](Edge& e) {
            if (active_in->get_bit(e.target)) {
            //if (active_in->get_bit(e.target)) {
                write_add(&degree[e.source], 1); 
                write_add(&degree[e.target], 1); 
            }   
            return 0;
        }, active_in);

        need_del = graph.stream_vertices<VertexId>([&](VertexId i){
            if (degree[i] <= k ) {
                return 1;
            }
            active_out->set_bit(i);
            return 0;
        });

        if (need_del == pre_need_del) break;

        printf("needDel = %7d\n", need_del - pre_need_del);
        pre_need_del = need_del;
    }
	return 0;
}

