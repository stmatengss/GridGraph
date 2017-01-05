#include "core/graph.hpp"

int main(int argc, char ** argv) {
    if (argc <= 2) {
        fprintf(stderr, "usage: kcores [path] [k]\n");
        exit(-1);
    }   

    std::string path = argv[1];
    int k = atoi(argv[2]);

    Graph graph(path);
    Bitmap * active_in = graph.alloc_bitmap();
    Bitmap * active_out = graph.alloc_bitmap();
    BigVector<VertexId> degree(graph.path+"/degree", graph.vertices);
    graph.set_vertex_data_bytes( graph.vertices * sizeof(VertexId) );

    active_out->fill();

    int iters = 0;
    while (true) {
        printf("iters %d\n", iters++);
        graph.stream_vertices<VertexId>([&](VertexId i){ 
            degree[i] = 0;
            return 1;
        }); 
    
        std::swap(active_in, active_out);
        active_out->clear();

        //Calc degree
        graph.stream_edges<VertexId>([&](Edge & e){ 
            if (active_in->get_bit(e.target)) {
                write_add(&degree[e.source], 1); 
                write_add(&degree[e.target], 1); 
            }   
            return 0;
        }, active_in);

        //Find min
        VertexId min_degree = graph.vertices;
        VertexId needDel = graph.stream_vertices<VertexId>([&](VertexId i){ 
            if (min_degree > degree[i]) {
                write_min(&min_degree, degree[i]);
            }   
            if (degree[i] <= k) {
                return 1;
            } else return 0;
        }); 

        if (needDel == 0) break;
        printf("%d %d\n", min_degree, needDel);

        //Delete vertices
        graph.stream_vertices<VertexId>([&](VertexId i){ 
            if (degree[i] != min_degree) {
                active_out->set_bit(i);
            }
            return 0;
        });
    }

    return 0;
}
