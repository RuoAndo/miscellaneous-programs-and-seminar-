#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>
#include <algorithm>
#include <cstdlib>
#include "util.h"
#include "timer.h"

int main(void)
{
  unsigned int t, sorting_time;

  // generate *M random numbers serially
  // thrust::host_vector<int> h_vec(1024 << 20);
  thrust::host_vector<int> h_vec(1024 << 16);
  std::generate(h_vec.begin(), h_vec.end(), rand);

  std::vector<int> sv;

  static int counter;
  static int size;
  
  size = h_vec.size();

  std::cout << "size:" << size << std::endl;

  for(int i = 0; i < h_vec.size(); i++)
  	  sv.push_back(h_vec[i]);

  begin_timer(&t);
  std::sort(sv.begin(), sv.end());
  sv.erase(std::unique(sv.begin(), sv.end()), sv.end());
  sorting_time = stop_timer(&t); 
  print_timer(sorting_time);

  begin_timer(&t);
  thrust::device_vector<int> d_vec = h_vec;
  thrust::sort(d_vec.begin(), d_vec.end());
  sorting_time = stop_timer(&t); 
  print_timer(sorting_time);

  return 0;
}
