// FlashMii - press A to get flashbanged. Wii homebrew (libogc).
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#define RATE    48000
#define SECS    5
#define FRAMES  (RATE * SECS)          /* 240000 stereo frames = 960000 bytes (32-byte multiple) */

static void *xfb;
static GXRModeObj *rmode;
static s16 *snd;

static void show(u32 color) {
    VIDEO_ClearFrameBuffer(rmode, xfb, color);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

static u32 gray(u8 y) {                /* YCbCr gray, y in 16..235 */
    return ((u32)y << 24) | (0x80u << 16) | ((u32)y << 8) | 0x80u;
}

static void menu(void) {
    show(COLOR_BLACK);
    CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    printf("\x1b[2;0H");
    printf("  FlashMii\n\n");
    printf("  WARNING: bright flash + very loud ringing.\n");
    printf("  Turn the volume down. Not for anyone sensitive to flashing lights.\n\n");
    printf("  A    - get flashbanged\n");
    printf("  HOME - exit\n");
}

/* Detonation crack (noise burst) + decaying high-pitched ring, generated at startup. */
static void make_sound(void) {
    snd = (s16 *)memalign(32, FRAMES * 4);
    if (!snd) return;
    for (int i = 0; i < FRAMES; i++) {
        float t = (float)i / RATE;
        float ring  = (sinf(6.2831853f * 3150.f * t) * 0.55f +
                       sinf(6.2831853f * 3420.f * t) * 0.35f) * expf(-t * 0.9f);
        float crack = ((rand() / (float)RAND_MAX) * 2.f - 1.f) * expf(-t * 14.f);
        float v = ring * 0.6f + crack * 0.9f;
        if (v > 1.f) v = 1.f; else if (v < -1.f) v = -1.f;
        s16 s = (s16)(v * 30000.f);
        snd[i * 2] = s; snd[i * 2 + 1] = s;
    }
    DCFlushRange(snd, FRAMES * 4);
}

static void flashbang(void) {
    ASND_SetVoice(0, VOICE_STEREO_16BIT, RATE, 0, snd, FRAMES * 4, 255, 255, NULL);
    WPAD_Rumble(0, 1);

    for (int f = 0; f < 60; f++) {                 /* full white hold ~1s */
        show(COLOR_WHITE);
        if (f == 20) WPAD_Rumble(0, 0);
    }
    for (int f = 0; f < 240; f++) {                /* slow fade back to dark */
        float p = 1.f - (float)f / 240.f;
        show(gray((u8)(16 + 219 * p * p)));
    }
    ASND_StopVoice(0);
    menu();
}

int main(void) {
    VIDEO_Init();
    WPAD_Init();
    ASND_Init();
    ASND_Pause(0);

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    make_sound();
    menu();

    while (1) {
        WPAD_ScanPads();
        u32 down = WPAD_ButtonsDown(0);
        if (down & WPAD_BUTTON_HOME) exit(0);
        if ((down & WPAD_BUTTON_A) && snd) flashbang();
        VIDEO_WaitVSync();
    }
    return 0;
}
