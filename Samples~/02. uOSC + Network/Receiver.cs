using UnityEngine;

namespace uPacketDivision.Samples
{

public class Receiver : MonoBehaviour
{
    [SerializeField]
    uint timeout = 100;

    Assembler _assembler = new Assembler();
    Texture2D _texture;

    void Update()
    {
        _assembler.timeout = timeout;
    }

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
        if (_texture && _texture.width == w && _texture.height == h) return;
        _texture = new Texture2D(w, h);
    }

    void OnData(byte[] data)
    {
        _assembler.Add(data);
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
                var type = _assembler.GetLossType();
                Debug.LogWarning("Loss: " + type);
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
        if (!_texture) return;

        if (pixels.Length != _texture.width * _texture.height) return;

        _texture.SetPixels32(pixels);
        _texture.Apply();
        GetComponent<Renderer>().material.mainTexture = _texture;
    }
}

}
