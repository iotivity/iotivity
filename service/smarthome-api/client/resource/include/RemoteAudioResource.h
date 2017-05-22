#ifndef SMARTHOME_API_CLIENT_REMOTEAUDIO_H_
#define SMARTHOME_API_CLIENT_REMOTEAUDIO_H_
#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class RemoteAudioResourceDelegate
            {
            public:
                virtual ~RemoteAudioResourceDelegate () {}
                virtual void onChangeVolume(int volume, bool mute, ResultCode ret) = 0;
                virtual void onGetVolume(int volume, bool mute, ResultCode ret) = 0;
            };
            class RemoteAudioResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                virtual ~RemoteAudioResource();
                void setDelegate(RemoteAudioResourceDelegate *delegate);
                void changeVolume(int volume, bool mute);
                void getVolume();

            protected:
                RemoteAudioResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);

            private:
                RemoteAudioResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTEAUDIO_H_ */
