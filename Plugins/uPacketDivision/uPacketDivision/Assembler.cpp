#include <chrono>
#include <algorithm>
#include "Assembler.h"
#include "Log.h"


namespace uPacketDivision
{


void Frame::AddChunkData(const void *pData, uint32_t)
{
    const auto &header = *reinterpret_cast<const PacketHeader*>(pData);

    if (header.version != GetPacketHeaderVersion())
    {
        DebugLog("Packet version (%u) is wrong.", header.version);
        return;
    }

    if (header.type != static_cast<uint32_t>(PacketType::Chunk))
    {
        DebugLog("Packet type (%u) is wrong.", header.type);
        return;
    }

    if (!buf_)
    {
        Initialize(header);
    }

    if (header.chunkCount != chunkCount_ ||
        header.timestamp != timestamp_ ||
        header.totalSize != totalSize_ ||
        header.chunkIndex > chunkCount_)
    {
        DebugLog("Detected a wrong packet (count: %u / %u, time: %u / %u, size: %u / %u, index: %u).",
            header.chunkCount, chunkCount_, 
            header.timestamp, timestamp_, 
            header.totalSize, totalSize_, 
            header.chunkIndex);
        return;
    }

    bool &flag = receivedChunkIndices_.at(header.chunkIndex);
    if (flag)
    {
        DebugLog("Received a same chunkIndex data.");
        return;
    }
    flag = true;

    const auto offset = header.offsetSize;
    if (offset + header.chunkSize > totalSize_)
    {
        DebugLog("Received data exceeds the buffer size.");
        return;
    }

    auto *pDst = buf_.get() + offset;
    const auto *pSrc = reinterpret_cast<const char*>(pData) + sizeof(PacketHeader);
    memcpy(pDst, pSrc, header.chunkSize);

    receivedSize_ += header.chunkSize;
}


bool Frame::IsCompleted() const
{
    if (receivedChunkIndices_.empty()) return false;

    for (const auto &pair : receivedChunkIndices_)
    {
        if (!pair.second) return false;
    }

    if (receivedSize_ != totalSize_)
    {
        DebugLog("Receive size is wrong.");
        return false;
    }

    return true;
}


void Frame::Initialize(const PacketHeader &header)
{
    totalSize_ = header.totalSize;
    timestamp_ = header.timestamp;
    chunkCount_ = header.chunkCount;

    for (uint32_t i = 0; i < chunkCount_; ++i)
    {
        receivedChunkIndices_.emplace(i, false);
    }

    buf_ = std::make_unique<char[]>(totalSize_);
}


// ---


void Assembler::AddChunkData(const void* pData, uint32_t size)
{
    constexpr uint32_t headerSize = sizeof(PacketHeader);

    if (size < headerSize)
    {
        DebugLog("Received data size (%u) is smaller than header (%u).", 
            size, headerSize);
        return;
    }

    const auto &header = *reinterpret_cast<const PacketHeader*>(pData);

    if (header.version != GetPacketHeaderVersion())
    {
        DebugLog("Packet version is wrong.");
        return;
    }

    if (header.chunkSize + headerSize != size)
    {
        DebugLog("Packet size is wrong (size: %u, chunk: %u, header: %u).", 
            size, header.chunkSize, headerSize);
        return;
    }

    if (header.type != static_cast<uint32_t>(PacketType::Chunk))
    {
        DebugLog("Header type is not supported (%u).", header.type);
        return;
    }

    const auto index = header.frameIndex;

    if (index > latestFrameIndex_ || latestFrameIndex_ == -1)
    {
        latestFrameIndex_ = index;
        latestTimestamp_ = header.timestamp;
    }

    if (oldestFrameIndex_ == -1)
    {
        oldestFrameIndex_ = index;
    }

    auto it = frames_.find(index);
    if (it == frames_.end())
    {
        auto frame = std::make_unique<Frame>();
        frame->AddChunkData(pData, size);
        frames_.emplace(index, std::move(frame));
    }
    else
    {
        const auto &frame = it->second;
        frame->AddChunkData(pData, size);
    }

    CheckAll();
}


void Assembler::CheckAll()
{
    using namespace std::chrono;

    eventType_ = EventType::None;
    lossType_ = LossType::None;

    if (frames_.empty()) return;

    auto it = frames_.find(oldestFrameIndex_);
    if (it == frames_.end())
    {
        DebugLog("packet loss.");

        eventType_ = EventType::PacketLoss;
        lossType_ = LossType::NotReceived;

        if (latestFrameIndex_ > oldestFrameIndex_)
        {
            ++oldestFrameIndex_;
        }

        return;
    }

    const auto &packet = it->second;
    if (packet->IsCompleted())
    {
        assembledFrameIndices_.push_back(oldestFrameIndex_);
        eventType_ = EventType::FrameCompleted;
        ++oldestFrameIndex_;
    }
    else
    {
        const auto dt = latestTimestamp_ - packet->GetTimestamp();
        if (dt > timeout_)
        {
            DebugLog("packet timeout (> %u ms, %u / %u).", 
                timeout_, packet->GetReceivedSize(), packet->GetTotalSize());

            eventType_ = EventType::PacketLoss;
            lossType_ = LossType::Timeout;
            frames_.erase(oldestFrameIndex_);

            if (latestFrameIndex_ > oldestFrameIndex_)
            {
                ++oldestFrameIndex_;
            }

            return;
        }
    }
}


uint64_t Assembler::GetAssembledFrameIndex() const
{
    return !assembledFrameIndices_.empty() ? 
        *assembledFrameIndices_.begin() : 
        -1;
}


const char * Assembler::GetFrameData(uint64_t index) const
{
    const auto it = frames_.find(index);
    return (it != frames_.end()) ? 
        it->second->GetData() : 
        nullptr;
}


uint32_t Assembler::GetFrameSize(uint64_t index) const
{
    const auto it = frames_.find(index);
    return (it != frames_.end()) ? 
        it->second->GetTotalSize() : 
        0;
}


void Assembler::RemoveFrame(uint64_t index)
{
    const auto it = frames_.find(index);
    if (it != frames_.end())
    {
        frames_.erase(it);
    }

    auto &indices = assembledFrameIndices_;
    indices.erase(
        std::remove(indices.begin(), indices.end(), index),
        indices.end());
}


}