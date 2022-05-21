#include <parallelzone/Runtime.hpp>

namespace parallelzone {

using namespace madness;

Runtime::Runtime() :
  Runtime(1, nullptr) {} // As of MPI-2 you can actually pass null pointers

Runtime::Runtime(int argc, char** argv) :
  initialized(true),
  num_partitions(1),
  mad_world(madness::initialize(argc, argv, SafeMPI::COMM_WORLD, false)) {}

Runtime::Runtime(const MPI_Comm& comm) : Runtime(SafeMPI::Intracomm(comm)) {}

Runtime::Runtime(const SafeMPI::Intracomm& comm) :
  initialized(false),
  num_partitions(1),
  mad_world(*madness::World::find_instance(comm)) {}

Runtime::~Runtime() {
    if(!initialized) return;
    madness::finalize();
}

} // namespace parallelzone
