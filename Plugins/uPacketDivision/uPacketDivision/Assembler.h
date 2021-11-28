#pragma once

#include <cstdint>
#include <memory>
#include <map>
#include <deque>
#include <functional>
#include "Common.h"


namespace uPacketDivision
{


class Frame final
{
public:
    void AddChunkData(const void *pData, uint32_t size);
    bool IsCompleted() const;
    uint64_t GetTimestamp() const { return timestamp_; }
    uint32_t GetTotalSize() const { return totalSize_; }
    uint32_t GetReceivedSize() const { return receivedSize_; }
    const char * GetData() const { return buf_.get(); }

private:
    void Initialize(const PacketHeader &header);

    std::unique_ptr<char[]> buf_;
    uint32_t totalSize_ = 0;
    uint32_t receivedSize_ = 0;
    uint64_t timestamp_ = 0;
    uint32_t chunkCount_ = 0;
    std::map<uint32_t, bool> receivedChunkIndices_;
};


enum class EventType
{
    None = -1,
    FrameCompleted = 0,
    PacketLoss = 1,
};


enum class LossType
{
    None = -1,
    NotReceived = 0,
    Timeout = 1,
};


class Assembler final
{
public:
    void AddChunkData(const void *pData, uint32_t size);
    EventType GetEventType() const { return eventType_; }
    LossType GetLossType() const { return lossType_; }
    uint64_t GetAssembledFrameIndex() const;
    const char * GetFrameData(uint64_t index) const;
    uint32_t GetFrameSize(uint64_t index) const;
    void RemoveFrame(uint64_t index);
    void SetTimeout(uint32_t milliseconds) { timeout_ = milliseconds; }

private:
    void CheckAll();

    std::map<uint64_t, std::unique_ptr<Frame>> frames_;
    std::deque<uint64_t> assembledFrameIndices_;
    uint64_t oldestFrameIndex_ = UINT64_MAX;
    uint64_t latestFrameIndex_ = UINT64_MAX;
    uint64_t latestTimestamp_ = UINT64_MAX;
    EventType eventType_ = EventType::None;
    LossType lossType_ = LossType::None;
    uint32_t timeout_ = 100U;
};


}
