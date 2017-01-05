/*************************************************************************
    > File Name: kcores.cpp
    > Author: stmatengss
    > Mail: stmatengss@163.com 
    > Created Time:
 ************************************************************************/

#include <iostream>
#include <cmath>
#include "core/graph.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
	/* code */
    if (argc < 2) {
        fprintf(stderr, "usage: conductance [path] [memory budget in GB]\n");
        exit(-1);
    }   

    string path = argv[1];
    int k = atoi(argv[2]);

    long memory_bytes = ((argc >= 4)?atol(argv[3]):8l)*1024l*1024l*1024l;

    Graph graph(path);
    graph.set_memory_bytes(memory_bytes);

    // Bitmap* active_in = graph.alloc_bitmap();
    // Bitmap* active_out = graph.alloc_bitmap();

    VertexId black_vertex_num = 0, red_vertex_num = 0;
    VertexId diff_vertex_num = 0;

    auto is_black = [](VertexId i)-> bool {
        return i % 2;
    };

    diff_vertex_num = graph.stream_edges<VertexId>([&](Edge& e){
        if (is_black(e.source)) {
            if (!is_black(e.target)) {
                return 1;
            }
            write_add(&black_vertex_num, 1);
        } else {
            if (is_black(e.target)) {
                return 1; 
            }
            write_add(&red_vertex_num, 1);
        }
        return 0;
    });

    double conductance = 1.0 * diff_vertex_num / 
        (min(black_vertex_num, red_vertex_num) + diff_vertex_num);

    printf("conductance number is %.10lf\n", conductance);

	return 0;
}

