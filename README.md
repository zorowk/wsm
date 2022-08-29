# WSM

WSM is an Mobile Phone [Wayland] compositor.

## Release Signatures

## Installation

### From Packages

### Compiling from Source

Install dependencies:

* meson \*
* [wlroots]
* wayland
* wayland-protocols \*
* wayland-scanner

Run these commands:
```shell
    meson build/
    ninja -C build/
    sudo ninja -C build/ install
```

## Configuration


## Running

Run `WAM` from a TTY or in Xorg desktop environment. Some display managers may work but are not supported by WSM (gdm is known to work fairly well).

[en]: README.md
[wlroots]: https://gitlab.freedesktop.org/wlroots/wlroots
