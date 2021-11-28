using UnityEngine;
using System.Runtime.InteropServices;

namespace uPacketDivision.Samples
{

public class Sender : MonoBehaviour
{
    [SerializeField]
    uint packetSize = 65000;

    [SerializeField]
    uOSC.uOscClient client;

    [SerializeField]
    int sendFrameInterval = 10;

    Divider _divider = new Divider();
    int _width = 0;
    int _height = 0;

    int count = 0;

    void Update()
    {
        if (count-- > 0) return;

        Divide();
        Send();

        count = sendFrameInterval;
    }

    void Divide()
    {
        var renderer = GetComponent<Renderer>();
        if (!renderer) return;

        var texture = renderer.sharedMaterial.mainTexture as Texture2D;
        if (!texture) return;

        _width = texture.width;
        _height = texture.height;

        var pixels = texture.GetPixels32();
        _divider.maxPacketSize = packetSize;
        _divider.Divide(pixels);
    }

    void Send()
    {
        if (!client) return;

        client.Send("/Size", _width, _height);

        for (uint i = 0; i < _divider.GetChunkCount(); ++i)
        {
            client.Send("/Data", _divider.GetChunk(i));
        }
    }
}

}

