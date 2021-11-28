using UnityEngine;
using System;
using System.Runtime.InteropServices;

namespace uPacketDivision.Samples
{

public class DivideAndAssemble : MonoBehaviour
{
    [SerializeField]
    uint packetSize = 8192;

    [SerializeField]
    uint timeout = 100;

    [SerializeField, Range(0f, 1f)]
    float errorProbability = 0f;

    [SerializeField]
    Texture2D image;

    Divider _divider = new Divider();
    Assembler _assembler = new Assembler();
    Color32[] _pixels;

    [DllImport("msvcrt.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = false)]
    public static extern IntPtr memcpy(IntPtr dest, IntPtr src, int count);

    void Start()
    {
        if (!image) return;

        _pixels = image.GetPixels32();
    }

    void Update()
    {
        if (_pixels == null) return;

        Divide();
        Assemble();
    }

    void Divide()
    {
        _divider.maxPacketSize = packetSize;
        _divider.Divide(_pixels);

        Debug.Log($"Divider - chunk: {_divider.GetChunkCount()}");
    }

    void Assemble()
    {
        _assembler.timeout = timeout;

        for (uint i = 0; i < _divider.GetChunkCount(); ++i)
        {
            if (UnityEngine.Random.value < errorProbability) continue;
            var size = _divider.GetChunkSize(i);
            var chunk = _divider.GetChunkData(i);
            _assembler.Add(chunk, size);
            CheckEvent();
        }
    }

    void CheckEvent()
    {
        switch (_assembler.GetEventType())
        {
            case EventType.FrameCompleted:
            {
                OnDataAssembled(_assembler.GetAssembledData<Color32>());
                break;
            }
            case EventType.PacketLoss:
            {
                Debug.LogWarning("Loss");
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void OnDataAssembled(Color32[] pixels)
    {
        var texture = new Texture2D(image.width, image.height);
        texture.SetPixels32(pixels);
        texture.Apply();
        GetComponent<Renderer>().material.mainTexture = texture;
    }
}

}
