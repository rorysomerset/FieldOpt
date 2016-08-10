#ifndef FIELDOPT_MPIRUNNER_H
#define FIELDOPT_MPIRUNNER_H

#include "abstract_runner.h"
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
namespace mpi = boost::mpi;

namespace Runner {
    namespace MPI {
        class Worker;
        class Overseer;
    }
}

namespace Runner {
    namespace MPI {
        /*!
         * @brief The MPIRunner class is the base class for runners using MPI for parallelization.
         */
        class MPIRunner : public AbstractRunner {

            friend class Worker;
            friend class Overseer;

        public:
            mpi::environment &env() { return env_; }

            mpi::communicator &world() { return world_; }

            int rank() const { return rank_; }

            /*!
             * @brief Tags used when sending and receiving.
             *
             * CASE_UNEVAL: To be used when sending unevaluated cases.
             * CASE_EVAL: To be used when sending evaluated cases.
             * MODEL_SYNC: To be used when sending model synchronization objects.
             * TERMINATE: This tag should be sent by the overseer to terminate a worker.
             */
            enum MsgTag : int {
                CASE_UNEVAL = 1, CASE_EVAL = 2, MODEL_SYNC = 3, TERMINATE = 4
            };

            /*!
             * @brief Send a case to the specified destination.
             * @param c Case to be sent.
             * @param dest The rank of the process the case should be sent to.
             * @param tag Tag to be used when sending.
             */
            void SendCase(Optimization::Case *c, int dest, MsgTag tag);


            /*!
             * @brief Receive a message with the specified tag containing a Case from the specified soruce.
             * If the CASE_EVAL tag is passed, i.e. if we want to receive an evaluated case, a case will be received
             * from any source, and the source argument will be set to the rank of the process the case was received from.
             * If the message contains the TERMINATE tag, a nullptr will be returned.
             * @param source
             * @param tag
             * @return
             */
            Optimization::Case *RecvCase(int &source, MsgTag tag);

            /*!
             * @brief Create a ModelSynchronizationObject and send it to all other processes.
             *
             * This should be called by the process with rank 0, in order to make the variable UUIDs match across
             * all processes.
             */
            void BroadcastModel();

            /*!
             * @brief Receive the ModelSynchronizationObject broadcast by the root process. This is applied to the
             * model object.
             */
            void RecvModelSynchronizationObject();

        protected:
            MPIRunner(RuntimeSettings *rts);

            mpi::environment env_;
            mpi::communicator world_;
            int rank_;
            int scheduler_rank_ = 0;
        };
    }
}


#endif //FIELDOPT_MPIRUNNER_H