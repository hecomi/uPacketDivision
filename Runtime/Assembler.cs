using System.Runtime.InteropServices;

namespace uPacketDivision
{

public class Assembler : System.IDisposable
{
    public uint id { get; private set; }

    public Assembler()
    {
        id = Lib.Assembler.Create();
    }

    ~Assembler()
    {
        Dispose();
    }

    public void Dispose()
    {
        Lib.Assembler.Destroy(id);
    }

    public bool isValid
    {
        get { return Lib.Assembler.IsValid(id); }
    }

    public uint timeout
    {
        set { Lib.Assembler.SetTimeout(id, value); }
    }

    public void Add(System.IntPtr data, uint size)
    {
        Lib.Assembler.AddChunkData(id, data, size);
    }

    public void Add(System.IntPtr data, int size)
    {
        Add(data, (uint)size);
    }

    public void Add<T>(T[] array)
    {
        var handle = GCHandle.Alloc(array, GCHandleType.Pinned);
        var ptr = handle.AddrOfPinnedObject();
        var size = array.Length * Marshal.SizeOf(typeof(T));
        Add(ptr, size);
        handle.Free();
    }

    public EventType GetEventType()
    {
        return Lib.Assembler.GetEventType(id);
    }

    public LossType GetLossType()
    {
        return Lib.Assembler.GetLossType(id);
    }

    public ulong GetAssembledFrameIndex()
    {
        return Lib.Assembler.GetAssembledFrameIndex(id);
    }

    public System.IntPtr GetFrameData(ulong index)
    {
        return Lib.Assembler.GetFrameData(id, index);
    }

    public uint GetFrameSize(ulong index)
    {
        return Lib.Assembler.GetFrameSize(id, index);
    }

    public void RemoveFrame(ulong index)
    {
        Lib.Assembler.RemoveFrame(id, index);
    }

    public T[] GetAssembledData<T>()
    {
        var index = GetAssembledFrameIndex();
        var data = GetFrameData(index);
        var size = GetFrameSize(index);

        var len = size / Marshal.SizeOf(typeof(T));
        var array = new T[len];
        var handle = GCHandle.Alloc(array, GCHandleType.Pinned);
        var arrayPtr = handle.AddrOfPinnedObject();

        Lib.Memcpy(arrayPtr, data, (int)size);

        handle.Free();

        RemoveFrame(index);

        return array;
    }
}

}
