#include <chrono>
#include <algorithm>
#include "Divider.h"
#include "Common.h"
#include "Log.h"


namespace uPacketDivision
{


void Divider::DivideData(const void *pData, uint32_t size)
{
    using namespace std::chrono;

    chunks_.clear();

    constexpr uint32_t headerSize = sizeof(PacketHeader);
    const uint32_t maxBufSize = maxPacketSize_ - headerSize;
    const uint32_t chunkCount = size / maxBufSize + 1;
    const uint32_t chunkSize = size / chunkCount + 1;

    PacketHeader header = { GetPacketHeaderVersion() };
    header.type = static_cast<uint8_t>(PacketType::Chunk);
    header.totalSize = size;
    header.timestamp = time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count();
    header.frameIndex = frameIndex_++;
    header.chunkCount = static_cast<uint16_t>(chunkCount);

    for (uint16_t i = 0; i < chunkCount; ++i)
    {
        const auto bufSize = static_cast<size_t>(maxBufSize) + headerSize;
        auto buf = std::make_unique<char[]>(bufSize);

        header.chunkIndex = i;
        header.offsetSize = i * chunkSize;
        header.chunkSize = std::min(chunkSize, size - header.offsetSize);
        memcpy_s(buf.get(), headerSize, &header, headerSize);

        const auto *pPartialData = static_cast<const char*>(pData) + header.offsetSize;
        memcpy_s(buf.get() + headerSize, maxBufSize, pPartialData, header.chunkSize);

        Chunk chunk;
        chunk.size = header.chunkSize + headerSize;
        chunk.buf = std::move(buf);
        chunks_.push_back(std::move(chunk));
    }
}


uint32_t Divider::GetChunkSize(uint32_t index) const
{
    if (index >= GetChunkCount()) return 0;

    const auto &chunk = chunks_.at(index);
    return chunk.size;
}


const char * Divider::GetChunkData(uint32_t index) const
{
    if (index >= GetChunkCount()) return nullptr;

    const auto &chunk = chunks_.at(index);
    return chunk.buf.get();
}


}