#pragma once

#include <madness/world/MADworld.h>
#include <madness/world/safempi.h>

namespace parallelzone {

class Runtime {
public:
    /**
     * @brief Default constructor
     * @details Runtime instance initializes MADness runtime, sets partitions
     * to 1.
     */
    Runtime();

    /**
     * @brief constructor
     * @details Runtime instance initializes MADness runtime, sets partitions
     * to 1.
     */
    Runtime(int argc, char** argv);

    /**
     * @brief constructor
     * @details Starts MADness using a MPI Communicator. Runtime instance
     * doesn't explicitly start MADness
     */
    Runtime(const MPI_Comm& comm);

    /**
     * @brief constructor
     * @details Starts MADness using a SafeMPI Communicator. Runtime instance
     * doesn't explicitly start MADness
     */
    Runtime(const SafeMPI::Intracomm& comm);

    /// @return MPI Communicator, associated with MADWorld
    MPI_Comm& mpi_comm() { return mad_world.mpi.comm().Get_mpi_comm(); }

    /// @return MADWorld, ref handle to madness::World
    madness::World& madness_world() { return mad_world; }

    /// @return int, number of partitions; Defaults to 1
    constexpr int get_num_partitions() const noexcept { return num_partitions; }

    /// @return true, if Runtime (and, necessarily, MADWorld runtime) is in an
    /// initialized state
    constexpr bool is_initialized() const noexcept { return initialized; }

    /**
     * @brief destructor
     * @details Destructs the MADness runtime appropriately, if Runtime is
     * initialized
     */
    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    Runtime(Runtime&&)                 = delete;
    Runtime& operator=(Runtime&&) = delete;

private:
    bool initialized;
    int num_partitions;
    madness::World& mad_world;
};

} // End namespace parallelzone
