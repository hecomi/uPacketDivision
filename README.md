uPacketDivision
===============

This is a native plug-in that divides a given data (`System.IntPtr` or array) into specified sizes and restores them regardless of the input order. It is intended for use cases such as sending large data via UDP.

Install
-------

- Unity Package
  - Download the latest .unitypackage from [Release page](https://github.com/hecomi/uPacketDivision/releases).
- Git URL (UPM)
  - Add `https://github.com/hecomi/uPacketDivision.git#upm` to Package Manager.
- Scoped Registry (UPM)
  - Add a scoped registry to your project.
    - URL: `https://registry.npmjs.com`
    - Scope: `com.hecomi`
  - Install uPacketDivision in Package Manager.

Platforms
---------

Currently, it is only built for Windows.

How to use
----------

### Divide

Create a `Divider` and call `Divide()` with the array as input (or `System.IntPtr` with the size). This will divide the packet internally.

```cs
Divider divider = new Divider();

void Divide()
{
    Texture2D image;
    var pixels = image.GetPixels32();
    divider.maxPacketSize = packetSize;
    divider.Divide(pixels);
}
```

Then, send the split data to the remote in some way. The following functions are available.
- `GetChunkCount()`.
  - the number of chunks.
- `GetChunk()`
  - The `byte[]` array of the split data
- `GetChunkSize(int index)`.
  - The size of the split data.
- `GetChunkData(int index)`.
  - The pointer of the split data
Here is an example of using [uOSC](https://github.com/hecomi/uOSC).

```cs
uOSC.uOscClient client;

void Send(int width, int height)
{
    client.Send("/Size", width, height);

    for (uint i = 0; i < divider.GetChunkCount(); ++i)
    {
        client.Send("/Data", divider.GetChunk(i));
    }
}
```

If you want to use the pointer and size directly, please use `GetChunkSize()` and `GetChunkData()` instead.

### Assemble

Use `Assembler` to assemble the data sent to you. Here is an example of the receiving part using uOSC.

```cs
Assembler assembler = new Assembler();
Texture2D texture;

public void OnDataReceived(uOSC.Message message)
{
    if (message.address == "/Size")
    {
        var w = (int)message.values[0];
        var h = (int)message.values[1];
        OnSize(w, h);
    }
    else if (message.address == "/Data")
    {
        var data = (byte[])message.values[0];
        OnData(data);
        CheckEvent();
    }
}

void OnSize(int w, int h)
{
    texture = new Texture2D(w, h);
}

void OnData(byte[] data)
{
    assembler.timeout = timeout;
    assembler.Add(data);
}
```

Each time you add data, check for data recovery or loss as follows.

```cs
void CheckEvent()
{
    switch (assembler.GetEventType())
    {
        case EventType.FrameCompleted:
        {
            OnDataAssembled(_assembler.GetAssembledData<Color32>());
            break;
        }
        case EventType.PacketLoss:
        {
            var type = assembler.GetLossType();
            Debug.LogWarning("Loss: " + type);
            break;
        }
        default:
        {
            break;
        }
    }
}
```

If you want to get a pointer and its size instead of an array, the following APIs are available.

```cs
var index = assembler.GetAssembledFrameIndex();
var data = assembler.GetFrameData(index);
var size = assembler.GetFrameSize(index);
OnDataAssembled(data, (int)size);
assembler.RemoveFrame(index);
```

Then, the data reconstruction will be completed as follows.

```cs
void OnDataAssembled(Color32[] pixels)
{
    texture.SetPixels32(pixels);
    texture.Apply();
    GetComponent<Renderer>().material.mainTexture = _texture;
}
```
