using System.Runtime.InteropServices;

namespace uPacketDivision
{ 

public enum EventType
{
    None = -1,
    FrameCompleted = 0,
    PacketLoss = 1,
};

public enum LossType
{
    None = -1,
    NotReceived = 0,
    Timeout = 1,
};

public static class Lib
{
    public const string dllName = "uPacketDivision";

    public static class Divider
    {
        [DllImport(dllName, EntryPoint = "UpdCreateDivider")]
        public static extern uint Create();

        [DllImport(dllName, EntryPoint = "UpdDestroyDivider")]
        public static extern void Destroy(uint id);

        [DllImport(dllName, EntryPoint = "UpdDividerIsValid")]
        public static extern bool IsValid(uint id);

        [DllImport(dllName, EntryPoint = "UpdDividerSetMaxPacketSize")]
        public static extern void SetMaxPacketSize(uint id, uint size);

        [DllImport(dllName, EntryPoint = "UpdDividerDivideData")]
        public static extern void DivideData(uint id, System.IntPtr data, uint size);

        [DllImport(dllName, EntryPoint = "UpdDividerGetChunkCount")]
        public static extern uint GetChunkCount(uint id);

        [DllImport(dllName, EntryPoint = "UpdDividerGetChunkData")]
        public static extern System.IntPtr GetChunkData(uint id, uint index);

        [DllImport(dllName, EntryPoint = "UpdDividerGetChunkSize")]
        public static extern uint GetChunkSize(uint id, uint index);
    }

    public static class Assembler
    {
        [DllImport(dllName, EntryPoint = "UpdCreateAssembler")]
        public static extern uint Create();

        [DllImport(dllName, EntryPoint = "UpdDestroyAssembler")]
        public static extern void Destroy(uint id);

        [DllImport(dllName, EntryPoint = "UpdAssemblerIsValid")]
        public static extern bool IsValid(uint id);

        [DllImport(dllName, EntryPoint = "UpdAssemblerAddChunkData")]
        public static extern void AddChunkData(uint id, System.IntPtr data, uint size);

        [DllImport(dllName, EntryPoint = "UpdAssemblerGetEventType")]
        public static extern EventType GetEventType(uint id);

        [DllImport(dllName, EntryPoint = "UpdAssemblerGetLossType")]
        public static extern LossType GetLossType(uint id);

        [DllImport(dllName, EntryPoint = "UpdAssemblerGetAssembledFrameIndex")]
        public static extern ulong GetAssembledFrameIndex(uint id);

        [DllImport(dllName, EntryPoint = "UpdAssemblerGetFrameData")]
        public static extern System.IntPtr GetFrameData(uint id, ulong index);

        [DllImport(dllName, EntryPoint = "UpdAssemblerGetFrameSize")]
        public static extern uint GetFrameSize(uint id, ulong index);

        [DllImport(dllName, EntryPoint = "UpdAssemblerRemoveFrame")]
        public static extern void RemoveFrame(uint id, ulong index);

        [DllImport(dllName, EntryPoint = "UpdAssemblerSetTimeout")]
        public static extern void SetTimeout(uint id, uint milliseconds);
    }

    [DllImport(dllName, EntryPoint = "UpdMemcpy")]
    public static extern void Memcpy(System.IntPtr dest, System.IntPtr src, int n);
}

}
