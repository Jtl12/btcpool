#ifndef __JOB_REPOSITORY_BYTOM_H_
#define __JOB_REPOSITORY_BYTOM_H_

#include <cstdint>
#include <unordered_map>
#include <memory>
#include "Kafka.h"



namespace BytomStratum
{
    //  Note: XXXHexStr: Hash string is 64 characters + 1 nullptr. Add additional bytes for padding. Reserve 72 bytes.
    struct BlockHeaderBytom_New
    {
        uint64_t version;                 // The version of the block.
        uint64_t height;                  // The height of the block.
        uint64_t timestamp;               // The time of the block in seconds.
        uint64_t bits;                    // Difficulty target for the block.
        uint8_t previousBlockHash[32];  // The hash of the previous block.
        char txMerkleRootHexStr[72];    
        char txStatusHashHexStr[72];
    };

    struct StratumJobBytom_New
    {
        uint64_t jobId;
        BlockHeaderBytom_New blockHeader;
        char seedHexStr[72];    //  used for sendMiningNotify

        uint8_t seedBin[32];    //  cache the seed in binary form for checkProofOfWork
    };

    const int JOB_REPOSITORY_CHUNK_SIZE = 64;
    struct JobRepositoryBytom_New
    {
        uint64_t startingJobId;
        StratumJobBytom_New jobs[JOB_REPOSITORY_CHUNK_SIZE];
    };

    //  key is startingJobId
    extern std::unordered_map<uint64_t, std::unique_ptr<JobRepositoryBytom_New>> g_jobRepositories;

    void ConsumeStratumJob(rd_kafka_message_t *rkmessage);

}

#endif // __JOB_REPOSITORY_BYTOM_H_