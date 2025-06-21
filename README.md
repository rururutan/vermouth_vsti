# Vermouth VSTi2

**Vermouth VSTi2** compatible with the *[Vermouth](http://retropc.net/yui/hoot/)* interface.
Vermouth is a Timidity compatible software synthesizer that commonly used with **[NP2](https://www.yui.ne.jp/np2/)** and **[hoot](http://dmpsoft.s17.xrea.com/hoot)**.
It allows you to in supported programs.

This version is a lightly modified fork of the original Vermouth project's VSTi branch by Yui.

## Features

- Supports VSTi2 plugins (VST3 is not supported) plugins via `.ini` configuration
- Compatible with NP2 and hoot

## Requirements

- Windows 10 or later
- NP2 (`np2.exe`) or hoot (`hoot.exe`)
- A compatible VSTi2 plugin

## Usage

1. Download or build `vermouth.dll`.
2. Place `vermouth.dll` in the same directory as `np2.exe` or `hoot.exe`.
3. Create or edit `vermouth.ini` to specify the path to your desired VSTi2 plugin.
4. Launch NP2 or hoot and configure them to use Vermouth as the MIDI output device.

## Configuration File (`vermouth.ini`)

```ini
[VST]
Module=\path\to\vsti\foo.dll
```

Ensure that at the specified path and is compatible with VSTi2.

## Original Repository

This is a fork of the following SVN repository:

http://amethyst.yui.ne.jp/svn/sound/vermouth/branches/WORK_SOUNDCanvasVA/

## License

This project is licensed under the BSD 3-Clause License.

