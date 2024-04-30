/*
 * Celerity: Jacobi 2D
 * Author: Sergio Miguel-Lopez
 * Date: Aug 2021
 */
#include <CL/sycl.hpp>
#include <celerity.h>
#include <chrono>
#include <ctype.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

// Print help
void printHelp() {
	printf("\njacobi -r <rows> [-c <columns> -d <depth> -s <numIter>] [-h for help]\n");
	printf("Stencil code for 1D, 2D and 3D matrix\n");
	printf("Use -r -c and -d to determine the rows, columns and depth of the matrix. -s is the number for iterations.\n");
	printf("For 1D array, use -r <rows> option with -s\n");
	printf("For 2D array, use -r <rows> and -c <columns> options with -s\n");
	printf("For 3D array, use -r <rows>, -c <columns> and -d <depth> options with -s\n\n");
	printf("Currently only supported 2D matrix:\n\n");
	printf("4 points\n");
	printf("{ 0, 1, 0,\n");
	printf("  1, X, 1,\n");
	printf("  0, 1, 0 }\n\n");
}

// Init kernel
template <typename T, int dim>
void initialization(celerity::distr_queue &q, celerity::buffer<T, dim> mat, celerity::buffer<T, dim> copy, long rows, int columns, int depth) {

	cl::sycl::range<dim> tam{mat.get_range()};

	q.submit([=](celerity::handler &cgh) {
		auto               one_to_one = celerity::access::one_to_one<dim>();
		celerity::accessor writemat{mat, cgh, one_to_one, cl::sycl::write_only, cl::sycl::no_init};
		celerity::accessor writecopy{copy, cgh, one_to_one, cl::sycl::write_only, cl::sycl::no_init};
		cgh.parallel_for<class matInit>(tam, [=](cl::sycl::id<dim> tid) {
			int row = tid[0];
			int col = tid[1];
			if (row == 0) {
				writemat[tid]  = 1;
				writecopy[tid] = 1;
			} else if (row == rows - 1) {
				writemat[tid]  = 2;
				writecopy[tid] = 2;
			} else if (col == 0) {
				writemat[tid]  = 3;
				writecopy[tid] = 3;
			} else if (col == columns - 1) {
				writemat[tid]  = 4;
				writecopy[tid] = 4;
			} else {
				writemat[tid]  = 0.0;
				writecopy[tid] = 0.0;
			}
		});
	});
}

// Compute kernel for 2D 4 points jacobi
template <typename T>
void compute2D4(celerity::distr_queue &q, celerity::buffer<T, 2> mat, celerity::buffer<T, 2> copy, long rows, int columns) {

	cl::sycl::range<2> tam{mat.get_range()};

	q.submit([=](celerity::handler &cgh) {
		auto one_to_one   = celerity::access::one_to_one<2>();
		auto neighborhood = celerity::access::neighborhood<2>(1, 1);

		celerity::accessor readcopy{copy, cgh, neighborhood, cl::sycl::read_only};
		celerity::accessor writemat{mat, cgh, neighborhood, cl::sycl::write_only};
		cgh.parallel_for<class matCompute>(cl::sycl::range<2>(rows - 2, columns - 2), cl::sycl::id<2>(1, 1), [=](cl::sycl::item<2> tid) {
			writemat[tid] = (readcopy[{tid[0] - 1, tid[1]}] + readcopy[{tid[0] + 1, tid[1]}] + readcopy[{tid[0], tid[1] - 1}] + readcopy[{tid[0], tid[1] + 1}]) / 4;
		});
	});
}

int main(int argc, char *argv[]) {

	long rows    = 0;
	int  columns = 0;
	int  depth   = 0;
	int  stages  = 0;
	int  dim     = 0;
	int  c;

	celerity::detail::runtime::init(&argc, &argv);
	int num_procs;
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	celerity::distr_queue q;

	while ((c = getopt(argc, argv, "r:c:d:s:t:h")) != -1) {
		switch (c) {
			case 'r':
				rows = atol(optarg);
				break;
			case 'c':
				columns = atoi(optarg);
				break;
			case 'd':
				depth = atoi(optarg);
				break;
			case 's':
				stages = atoi(optarg);
				break;
			case 'h':
				q.submit([=](celerity::handler &cgh) {
					cgh.host_task(celerity::on_master_node, [=]() {
						printHelp();
					});
				});
				return 0;
				break;
			default:
				abort();
		}
	}

	if (rows <= 0)
		dim = 0;
	else if (columns == 0 || columns == 1)
		dim = 1;
	else if (depth == 0 || depth == 1)
		dim = 2;
	else
		dim = 3;
	if (columns < 0 || depth < 0) dim = 0;

	celerity::experimental::bench::log_user_config({{"rows", std::to_string(rows)}, {"columns", std::to_string(columns)}, {"depth", std::to_string(depth)}, {"stages", std::to_string(stages)}});

	switch (dim) {
		case 1: {
			printf("This version only supports 2D matrices\n\n");
		} break;
		case 2: {
			struct timeval             initTime;
			auto                       tam = cl::sycl::range<2>(rows, columns);
			celerity::buffer<float, 2> buff_mat(tam);
			celerity::buffer<float, 2> buff_copy(tam);

			// Init
			initialization(q, buff_mat, buff_copy, rows, columns, depth);

			// Start timer
			q.slow_full_sync();
			celerity::experimental::bench::begin("compute");
			auto start_time = std::chrono::high_resolution_clock::now();

			// Compute
			int stage;
			for (stage = 0; stage < stages; stage++) {
				// Update copy
				std::swap(buff_mat, buff_copy);

				// Compute iteration
				compute2D4(q, buff_mat, buff_copy, rows, columns);
			}

			q.slow_full_sync();
			auto end_time = std::chrono::high_resolution_clock::now();
			celerity::experimental::bench::end("compute");

			int time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
			printf("\nTiempo: %8d ms\n\n", time);

			// Write
			#ifdef WRITE_RESULTS
			q.submit(celerity::allow_by_ref, [&](celerity::handler &cgh) {
				auto result = buff_mat.get_access<cl::sycl::access::mode::read, cl::sycl::access::target::host_buffer>(cgh, celerity::access::all<2>());
				cgh.host_task(celerity::on_master_node, [=]() {
					// FILE* fichero;
					// fichero = fopen("w", "w+");
					for (size_t i = 0; i < rows; i++) {
						for (size_t j = 0; j < columns; j++) {
							printf("%014.4lf ", result[{i, j}]);
							// fprintf(fichero,"%014.4lf ", result[{i,j}]);
						}
						// fprintf(fichero,"\n");
						printf("\n");
					}
				});
			});
			#endif

		} break;
		case 3: {
			printf("This version only supports 2D matrices\n\n");
		} break;
		default:
			q.submit([=](celerity::handler &cgh) {
				cgh.host_task(celerity::on_master_node, [=]() {
					printf("Only 1, 2 or 3 dimensions\n");
					fprintf(stderr, "Usage: %s -r <rows> -s <numIter> -c <columns> [-d <depth>] [-h for help]\n", argv[0]);
				});
			});
			break;
	}

	// End
	return 0;
}
