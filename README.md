# FlashMii

Press **A** -> full white screen, loud crack + ringing, rumble, slow fade back. **HOME** exits.
(Warning shown on screen: bright flash + loud audio.)

## Build the app (.dol)
1. Install devkitPro, then: `sudo dkp-pacman -S wii-dev`  (Windows: use the devkitPro installer)
2. `export DEVKITPRO=/opt/devkitpro`
3. `make package`
4. Copy `apps/FlashMii/` to `sd:/apps/FlashMii/` and launch from the Homebrew Channel.
   (Optional: add a 128x48 `icon.png` next to `meta.xml`.)

## Make the WAD (channel on the Wii Menu)
A .dol can't be turned into a WAD by itself. The normal, safe approach is a
**forwarder channel**: a tiny WAD that launches `sd:/apps/FlashMii/boot.dol` through the Homebrew Channel.

1. Use a forwarder-WAD generator from the Wii homebrew community and point it at
   `sd:/apps/FlashMii/boot.dol` (give it the name "FlashMii" and a banner/icon if you like).
2. Install the resulting `FlashMii.wad` with a WAD manager (e.g. Wii Mod Lite / wad manager) on a softmodded Wii.

Only install WADs from tools you trust and always keep BootMii/Priiloader set up
as a safety net.
