/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NVIDIA_AUDIO_HW_H
#define NVIDIA_AUDIO_HW_H

#include <cutils/list.h>
#include <hardware/audio.h>

#include <tinyalsa/asoundlib.h>
#include <tinycompress/tinycompress.h>
/* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
#include <audio_utils/resampler.h>
#include <audio_route/audio_route.h>

/* Retry for delay in FW loading*/
#define RETRY_NUMBER 10
#define RETRY_US 500000


#define OFFLOAD_FX_LIBRARY_PATH "/system/lib/soundfx/libnvidiaoffloadfx.so"
#define HTC_ACOUSTIC_LIBRARY_PATH "/system/lib/libhtcacoustic.so"

#define TTY_MODE_OFF    1
#define TTY_MODE_FULL   2
#define TTY_MODE_VCO    4
#define TTY_MODE_HCO    8

#define DUALMIC_CONFIG_NONE 0
#define DUALMIC_CONFIG_1 1

/* Sound devices specific to the platform
 * The DEVICE_OUT_* and DEVICE_IN_* should be mapped to these sound
 * devices to enable corresponding mixer paths
 */
enum {
    SND_DEVICE_NONE = 0,

    /* Playback devices */
    SND_DEVICE_MIN,
    SND_DEVICE_OUT_BEGIN = SND_DEVICE_MIN,
    SND_DEVICE_OUT_HANDSET = SND_DEVICE_OUT_BEGIN,
    SND_DEVICE_OUT_SPEAKER,
    SND_DEVICE_OUT_SPEAKER_REVERSE,
    SND_DEVICE_OUT_HEADPHONES,
    SND_DEVICE_OUT_SPEAKER_AND_HEADPHONES,
    SND_DEVICE_OUT_VOICE_HANDSET,
    SND_DEVICE_OUT_VOICE_SPEAKER,
    SND_DEVICE_OUT_VOICE_HEADPHONES,
    SND_DEVICE_OUT_HDMI,
    SND_DEVICE_OUT_SPEAKER_AND_HDMI,
    SND_DEVICE_OUT_BT_SCO,
    SND_DEVICE_OUT_VOICE_TTY_FULL_HEADPHONES,
    SND_DEVICE_OUT_VOICE_TTY_VCO_HEADPHONES,
    SND_DEVICE_OUT_VOICE_TTY_HCO_HANDSET,
    SND_DEVICE_OUT_END,

    /*
     * Note: IN_BEGIN should be same as OUT_END because total number of devices
     * SND_DEVICES_MAX should not exceed MAX_RX + MAX_TX devices.
     */
    /* Capture devices */
    SND_DEVICE_IN_BEGIN = SND_DEVICE_OUT_END,
    SND_DEVICE_IN_HANDSET_MIC  = SND_DEVICE_IN_BEGIN,
    SND_DEVICE_IN_SPEAKER_MIC,
    SND_DEVICE_IN_HEADSET_MIC,
    SND_DEVICE_IN_HANDSET_MIC_AEC,
    SND_DEVICE_IN_SPEAKER_MIC_AEC,
    SND_DEVICE_IN_HEADSET_MIC_AEC,
    SND_DEVICE_IN_VOICE_SPEAKER_MIC,
    SND_DEVICE_IN_VOICE_HEADSET_MIC,
    SND_DEVICE_IN_HDMI_MIC,
    SND_DEVICE_IN_BT_SCO_MIC,
    SND_DEVICE_IN_CAMCORDER_MIC,
    SND_DEVICE_IN_VOICE_DMIC_1,
    SND_DEVICE_IN_VOICE_SPEAKER_DMIC_1,
    SND_DEVICE_IN_VOICE_TTY_FULL_HEADSET_MIC,
    SND_DEVICE_IN_VOICE_TTY_VCO_HANDSET_MIC,
    SND_DEVICE_IN_VOICE_TTY_HCO_HEADSET_MIC,
    SND_DEVICE_IN_VOICE_REC_MIC,
    SND_DEVICE_IN_VOICE_REC_DMIC_1,
    SND_DEVICE_IN_VOICE_REC_DMIC_NS_1,
    SND_DEVICE_IN_END,

    SND_DEVICE_MAX = SND_DEVICE_IN_END,

};


#define MIXER_CARD 1
#define SOUND_CARD 1

/*
 * tinyAlsa library interprets period size as number of frames
 * one frame = channel_count * sizeof (pcm sample)
 * so if format = 16-bit PCM and channels = Stereo, frame size = 2 ch * 2 = 4 bytes
 * DEEP_BUFFER_OUTPUT_PERIOD_SIZE = 1024 means 1024 * 4 = 4096 bytes
 * We should take care of returning proper size when AudioFlinger queries for
 * the buffer size of an input/output stream
 */
#define PLAYBACK_PERIOD_SIZE 512
#define PLAYBACK_PERIOD_COUNT 2
#define PLAYBACK_DEFAULT_CHANNEL_COUNT 2
#define PLAYBACK_DEFAULT_SAMPLING_RATE 48000
#define PLAYBACK_START_THRESHOLD 1023
#define PLAYBACK_STOP_THRESHOLD 1024
#define PLAYBACK_AVAILABLE_MIN 1


#define SCO_PERIOD_SIZE 86
#define SCO_PERIOD_COUNT 2
#define SCO_DEFAULT_CHANNEL_COUNT 2
#define SCO_DEFAULT_SAMPLING_RATE 8000
#define SCO_START_THRESHOLD 171
#define SCO_STOP_THRESHOLD 172
#define SCO_AVAILABLE_MIN 1

#define PLAYBACK_HDMI_MULTI_PERIOD_SIZE  1024
#define PLAYBACK_HDMI_MULTI_PERIOD_COUNT 4
#define PLAYBACK_HDMI_MULTI_DEFAULT_CHANNEL_COUNT 6
#define PLAYBACK_HDMI_MULTI_PERIOD_BYTES \
    (PLAYBACK_HDMI_MULTI_PERIOD_SIZE * PLAYBACK_HDMI_MULTI_DEFAULT_CHANNEL_COUNT * 2)
#define PLAYBACK_HDMI_MULTI_START_THRESHOLD 4095
#define PLAYBACK_HDMI_MULTI_STOP_THRESHOLD 4096
#define PLAYBACK_HDMI_MULTI_AVAILABLE_MIN 1

#define PLAYBACK_HDMI_DEFAULT_CHANNEL_COUNT   2

#define CAPTURE_PERIOD_SIZE 1024
#define CAPTURE_PERIOD_COUNT 4
#define CAPTURE_DEFAULT_CHANNEL_COUNT 2
#define CAPTURE_DEFAULT_SAMPLING_RATE 48000
#define CAPTURE_DEFAULT_CHANNEL_COUNT 2
#define CAPTURE_START_THRESHOLD 1

#define COMPRESS_CARD       2
#define COMPRESS_DEVICE     0
#define COMPRESS_OFFLOAD_FRAGMENT_SIZE (32 * 1024)
#define COMPRESS_OFFLOAD_NUM_FRAGMENTS 4
/* ToDo: Check and update a proper value in msec */
#define COMPRESS_OFFLOAD_PLAYBACK_LATENCY 96
#define COMPRESS_PLAYBACK_VOLUME_MAX 0x2000

#define DEEP_BUFFER_OUTPUT_SAMPLING_RATE 48000
#define DEEP_BUFFER_OUTPUT_PERIOD_SIZE 480
#define DEEP_BUFFER_OUTPUT_PERIOD_COUNT 8

#define MAX_SUPPORTED_CHANNEL_MASKS 2

typedef int snd_device_t;

/* These are the supported use cases by the hardware.
 * Each usecase is mapped to a specific PCM device.
 * Refer to pcm_device_table[].
 */
typedef enum {
    USECASE_INVALID = -1,
    /* Playback usecases */
    USECASE_AUDIO_PLAYBACK = 0,
    USECASE_AUDIO_PLAYBACK_MULTI_CH,
    USECASE_AUDIO_PLAYBACK_OFFLOAD,
    USECASE_AUDIO_PLAYBACK_DEEP_BUFFER,

    /* Capture usecases */
    USECASE_AUDIO_CAPTURE,

    USECASE_VOICE_CALL,
    AUDIO_USECASE_MAX
} audio_usecase_t;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/*
 * tinyAlsa library interprets period size as number of frames
 * one frame = channel_count * sizeof (pcm sample)
 * so if format = 16-bit PCM and channels = Stereo, frame size = 2 ch * 2 = 4 bytes
 * DEEP_BUFFER_OUTPUT_PERIOD_SIZE = 1024 means 1024 * 4 = 4096 bytes
 * We should take care of returning proper size when AudioFlinger queries for
 * the buffer size of an input/output stream
 */

enum {
    OFFLOAD_CMD_EXIT,               /* exit compress offload thread loop*/
    OFFLOAD_CMD_DRAIN,              /* send a full drain request to DSP */
    OFFLOAD_CMD_PARTIAL_DRAIN,      /* send a partial drain request to DSP */
    OFFLOAD_CMD_WAIT_FOR_BUFFER,    /* wait for buffer released by DSP */
};

enum {
    OFFLOAD_STATE_IDLE,
    OFFLOAD_STATE_PLAYING,
    OFFLOAD_STATE_PAUSED,
};

typedef enum {
    PCM_PLAYBACK = 0x1,
    PCM_CAPTURE = 0x2,
    VOICE_CALL = 0x4
} usecase_type_t;

struct offload_cmd {
    struct listnode node;
    int             cmd;
    int             data[];
};

struct pcm_device_profile {
    struct pcm_config config;
    int               card;
    int               id;
    usecase_type_t    type;
    audio_devices_t   devices;
};

struct pcm_device {
    struct listnode            stream_list_node;
    struct pcm_device_profile* pcm_profile;
    struct pcm*                pcm;
    int                        status;
    /* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
    struct resampler_itfe*     resampler;
    int16_t*                   res_buffer;
    size_t                     res_byte_count;
};

struct stream_out {
    struct audio_stream_out     stream;
    pthread_mutex_t             lock; /* see note below on mutex acquisition order */
    pthread_cond_t              cond;
    struct pcm_config           config;
    struct listnode             pcm_dev_list;
    struct compr_config         compr_config;
    struct compress*            compr;
    int                         standby;
    unsigned int                sample_rate;
    audio_channel_mask_t        channel_mask;
    audio_format_t              format;
    audio_devices_t             devices;
    audio_output_flags_t        flags;
    audio_usecase_t             usecase;
    /* Array of supported channel mask configurations. +1 so that the last entry is always 0 */
    audio_channel_mask_t        supported_channel_masks[MAX_SUPPORTED_CHANNEL_MASKS + 1];
    bool                        muted;
    /* total frames written, not cleared when entering standby */
    uint64_t                    written;
    audio_io_handle_t           handle;

    int                         non_blocking;
    int                         playback_started;
    int                         offload_state;
    pthread_cond_t              offload_cond;
    pthread_t                   offload_thread;
    struct listnode             offload_cmd_list;
    bool                        offload_thread_blocked;

    stream_callback_t           offload_callback;
    void*                       offload_cookie;
    struct compr_gapless_mdata  gapless_mdata;
    int                         send_new_metadata;

    struct audio_device*        dev;
};

struct stream_in {
    struct audio_stream_in              stream;
    pthread_mutex_t                     lock; /* see note below on mutex acquisition order */
    struct pcm_config                   config;
    struct listnode                     pcm_dev_list;
    int                                 standby;
    audio_source_t                      source;
    audio_devices_t                     devices;
    audio_channel_mask_t                channel_mask;
    audio_usecase_t                     usecase;
    bool                                enable_aec;

    /* TODO: remove resampler if possible when AudioFlinger supports downsampling from 48 to 8 */
    unsigned int                        requested_rate;
    struct resampler_itfe*              resampler;
    struct resampler_buffer_provider    buf_provider;
    int16_t*                            buffer;
    size_t                              buffer_size;
    size_t                              frames_in;
    int                                 read_status;

    struct audio_device*                dev;
};

struct mixer_card {
    struct listnode     adev_list_node;
    struct listnode     uc_list_node[AUDIO_USECASE_MAX];
    int                 card;
    struct mixer*       mixer;
    struct audio_route* audio_route;
};

struct audio_usecase {
    struct listnode         adev_list_node;
    audio_usecase_t         id;
    usecase_type_t          type;
    audio_devices_t         devices;
    snd_device_t            out_snd_device;
    snd_device_t            in_snd_device;
    struct audio_stream*    stream;
    struct listnode         mixer_list;
};


struct audio_device {
    struct audio_hw_device  device;
    pthread_mutex_t         lock; /* see note below on mutex acquisition order */
    struct listnode         mixer_list;
    audio_mode_t            mode;
    struct stream_out*      primary_output;
    int                     in_call;
    float                   voice_volume;
    bool                    mic_mute;
    int                     tty_mode;
    bool                    bluetooth_nrec;
    bool                    screen_off;
    int*                    snd_dev_ref_cnt;
    struct listnode         usecase_list;
    bool                    speaker_lr_swap;
    unsigned int            cur_hdmi_channels;
    int                     dualmic_config;
    bool                    ns_in_voice_rec;

    void*                   offload_fx_lib;
    int                     (*offload_fx_start_output)(audio_io_handle_t);
    int                     (*offload_fx_stop_output)(audio_io_handle_t);

    void*                   htc_acoustic_lib;
    int                     (*htc_acoustic_init_rt5506)();
    int                     (*htc_acoustic_set_rt5506_amp)(int, int);
    int                     (*htc_acoustic_set_amp_mode)(int, int, int, int, bool);

    int                     tfa9895_init;
    int                     deepbuf_thread_timeout;
    int                     deepbuf_thread_cancel;
    int                     deepbuf_thread_active;
    pthread_mutex_t         deepbuf_thread_lock;
    pthread_t               deepbuf_thread;
};

/*
 * NOTE: when multiple mutexes have to be acquired, always take the
 * stream_in or stream_out mutex first, followed by the audio_device mutex.
 */

#endif // NVIDIA_AUDIO_HW_H
