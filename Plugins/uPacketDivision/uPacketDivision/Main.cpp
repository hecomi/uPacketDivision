#include <map>
#include "IUnityInterface.h"
#include "Assembler.h"
#include "Divider.h"

using namespace uPacketDivision;


namespace
{
    uint32_t g_assemblerId = 0U;
    std::map<int, std::unique_ptr<Assembler>> g_assemblers;

    uint32_t g_dividerId = 0U;
    std::map<int, std::unique_ptr<Divider>> g_dividers;
}


extern "C"
{


UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API UpdCreateDivider()
{
    const auto id = g_dividerId++;
    auto divider = std::make_unique<Divider>();
    g_dividers.emplace(id, std::move(divider));
    return id;
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdDestroyDivider(uint32_t id)
{
    g_dividers.erase(id);
}


const std::unique_ptr<Divider> & GetDivider(uint32_t id)
{
    static std::unique_ptr<Divider> invalid = nullptr;
    const auto it = g_dividers.find(id);
    return (it != g_dividers.end()) ? it->second : invalid;
}


UNITY_INTERFACE_EXPORT bool UNITY_INTERFACE_API UpdDividerIsValid(uint32_t id)
{
    const auto it = g_dividers.find(id);
    return it != g_dividers.end();
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdDividerSetMaxPacketSize(uint32_t id, uint32_t size)
{
    if (const auto &divider = GetDivider(id))
    {
        divider->SetMaxPacketSize(size);
    }
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdDividerDivideData(uint32_t id, void *pData, uint32_t size)
{
    if (const auto &divider = GetDivider(id))
    {
        divider->DivideData(pData, size);
    }
}


UNITY_INTERFACE_EXPORT uint32_t UNITY_INTERFACE_API UpdDividerGetChunkCount(uint32_t id)
{
    if (const auto &divider = GetDivider(id))
    {
        return divider->GetChunkCount();
    }
    return 0U;
}


UNITY_INTERFACE_EXPORT const void * UNITY_INTERFACE_API UpdDividerGetChunkData(uint32_t id, uint32_t index)
{
    if (const auto &divider = GetDivider(id))
    {
        return divider->GetChunkData(index);
    }
    return nullptr;
}


UNITY_INTERFACE_EXPORT uint32_t UNITY_INTERFACE_API UpdDividerGetChunkSize(uint32_t id, uint32_t index)
{
    if (const auto &divider = GetDivider(id))
    {
        return divider->GetChunkSize(index);
    }
    return 0;
}


// ---


UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API UpdCreateAssembler()
{
    const auto id = g_assemblerId++;
    auto assembler = std::make_unique<Assembler>();
    g_assemblers.emplace(id, std::move(assembler));
    return id;
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdDestroyAssembler(uint32_t id)
{
    g_assemblers.erase(id);
}


const std::unique_ptr<Assembler> & GetAssembler(uint32_t id)
{
    static std::unique_ptr<Assembler> invalid = nullptr;
    const auto it = g_assemblers.find(id);
    return (it != g_assemblers.end()) ? it->second : invalid;
}


UNITY_INTERFACE_EXPORT bool UNITY_INTERFACE_API UpdAssemblerIsValid(uint32_t id)
{
    const auto it = g_assemblers.find(id);
    return it != g_assemblers.end();
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdAssemblerAddChunkData(uint32_t id, const void *pData, int size)
{
    if (const auto &assembler = GetAssembler(id))
    {
        assembler->AddChunkData(pData, static_cast<uint32_t>(size));
    }
}


UNITY_INTERFACE_EXPORT EventType UNITY_INTERFACE_API UpdAssemblerGetEventType(uint32_t id)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->GetEventType();
    }
    return EventType::None;
}


UNITY_INTERFACE_EXPORT LossType UNITY_INTERFACE_API UpdAssemblerGetLossType(uint32_t id)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->GetLossType();
    }
    return LossType::None;
}


UNITY_INTERFACE_EXPORT uint64_t UNITY_INTERFACE_API UpdAssemblerGetAssembledFrameIndex(uint32_t id)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->GetAssembledFrameIndex();
    }
    return UINT64_MAX;
}


UNITY_INTERFACE_EXPORT const char * UNITY_INTERFACE_API UpdAssemblerGetFrameData(uint32_t id, uint64_t index)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->GetFrameData(index);
    }
    return nullptr;
}


UNITY_INTERFACE_EXPORT uint32_t UNITY_INTERFACE_API UpdAssemblerGetFrameSize(uint32_t id, uint64_t index)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->GetFrameSize(index);
    }
    return 0;
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdAssemblerRemoveFrame(uint32_t id, uint64_t index)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->RemoveFrame(index);
    }
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdAssemblerSetTimeout(uint32_t id, uint32_t milliseconds)
{
    if (const auto &assembler = GetAssembler(id))
    {
        return assembler->SetTimeout(milliseconds);
    }
}


UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UpdMemcpy(void *dest, void *src, int n)
{
    std::memcpy(dest, src, n);
}


}
