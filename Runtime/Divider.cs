using System.Runtime.InteropServices;

namespace uPacketDivision
{

public class Divider : System.IDisposable
{
    public uint id { get; private set; }

    public Divider()
    {
        id = Lib.Divider.Create();
    }

    ~Divider()
    {
        Dispose();
    }

    public void Dispose()
    {
        Lib.Divider.Destroy(id);
    }

    public bool isValid
    {
        get { return Lib.Divider.IsValid(id); }
    }

    public uint maxPacketSize
    {
        set { Lib.Divider.SetMaxPacketSize(id, value); }
    }

    public void Divide(System.IntPtr data, uint size)
    {
        Lib.Divider.DivideData(id, data, size);
    }

    public void Divide(System.IntPtr data, int size)
    {
        Divide(data, (uint)size);
    }

    public void Divide<T>(T[] array)
    {
        int size = array.Length * Marshal.SizeOf(typeof(T));
        var handle = GCHandle.Alloc(array, GCHandleType.Pinned);
        var ptr = handle.AddrOfPinnedObject();
        Divide(ptr, size);
        handle.Free();
    }

    public uint GetChunkCount()
    {
        return Lib.Divider.GetChunkCount(id);
    }

    public System.IntPtr GetChunkData(uint index)
    {
        return Lib.Divider.GetChunkData(id, index);
    }

    public uint GetChunkSize(uint index)
    {
        return Lib.Divider.GetChunkSize(id, index);
    }

    public byte[] GetChunk(uint index)
    {
        var size = GetChunkSize(index);
        var data = GetChunkData(index);
        var buf = new byte[size];
        Marshal.Copy(data, buf, 0, (int)size);
        return buf;
    }
}

}
