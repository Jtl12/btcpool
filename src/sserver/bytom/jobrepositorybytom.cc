#include "jobrepositorybytom.h"
#include <glog/logging.h>
#include "utilities_js.hpp"

/*
    JobRepository is called once every few seconds. So it doesn't really useful to put it on separate thread.
    Less thread can make the code readability better and easier to debug.
*/

namespace BytomStratum
{

std::unordered_map<uint64_t, std::unique_ptr<JobRepositoryBytom_New>> g_jobRepositories;

JobRepositoryBytom_New *FindJobRepository(uint64_t repoJobStaringIdx)
{
    auto iter = g_jobRepositories.find(repoJobStaringIdx);
    if (iter != g_jobRepositories.end())
    {
        auto res = g_jobRepositories.emplace(repoJobStaringIdx, std::unique_ptr<JobRepositoryBytom_New>(new JobRepositoryBytom_New));
        if (res.second)
        {
            iter = res.first;
        }
    }
    return iter->second.get();
}

void UnserializeStratumFromJson(JsonNode msgJson, StratumJobBytom_New *job)
{
  string hHash = msgJson["hHash"].str();
  string sHash = msgJson["sHash"].str();

}

void ConsumeStratumJob(rd_kafka_message_t *rkmessage)
{
    if (rkmessage->err)
    {
        if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
        {
            // Reached the end of the topic+partition queue on the broker.
            return;
        }

        LOG(ERROR) << "consume error for topic " << rd_kafka_topic_name(rkmessage->rkt)
                   << "[" << rkmessage->partition << "] offset " << rkmessage->offset
                   << ": " << rd_kafka_message_errstr(rkmessage);

        if (rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
            rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
        {
            LOG(FATAL) << "consume fatal";
        }
        return;
    }

    const char *msgString = (const char *)rkmessage->payload;
    int msgStringLen = rkmessage->len;
    JsonNode msgJson;
    if (!JsonNode::parse(msgString, msgString + msgStringLen, msgJson))
    {
        return;
    }
    if (j["created_at_ts"].type() != Utilities::JS::type::Int ||
        j["jobId"].type() != Utilities::JS::type::Int ||
        j["sHash"].type() != Utilities::JS::type::Str ||
        j["hHash"].type() != Utilities::JS::type::Str)
    {
        LOG(ERROR) << "parse bytom stratum job failure: " << s;
        return false;
    }

    uint64_t jobId = msgJson["jobId"].uint64();
    //  Note: There's no need to cache last jobRepository since this function will be called once every few seconds. We can always call FindJobRepository
    uint64_t jobIdx = jobId % JOB_REPOSITORY_CHUNK_SIZE;
    uint64_t repoJobStaringIdx = jobId - jobIdx;
    JobRepositoryBytom_New *jobRepository = FindJobRepository(repoJobStaringIdx);
}

} // namespace BytomStratum
