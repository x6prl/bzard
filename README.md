# bzard
bzard is fancy and flexible notification daemon.

## Screenshots
![0](/screenshots/0.png?raw=true)
![1](/screenshots/1.png?raw=true)
![2](/screenshots/2.png?raw=true)

![3](/screenshots/3.png?raw=true)
![4](/screenshots/4.png?raw=true)

## Features
### History
bzard will store notifications until restart.

![h_0](/screenshots/h_0.png?raw=true)

### TitleToIcon
If icon not presented, bzard will compare title and app name; if its equals, bzard will try to find and set app icon.

### URL icons support
Icon can be simple link to image.

### ReplaceMinusToDash
bzard will replace all occurrences of `-` to `—`.

### BodyToTitleWhenTitleIsAppName
If icon not presented, bzard will compare title and app name; if its equals, bzard will move all text from body to title.

### All fields are optional
Unused parts of notifications will not shown. 

### Mouse control
Right click to close all notifications, middle for all visible.

### Multiple actions
Buttons row at the bottom. :)

### Theming support
Pony theme in default package: just change `theme_name` to `pony` in config file!

### Flexidble
You can configure most parts of bzard. Look at config file.

# Compositing
To make opacity [of popups] works you need compositing. Try `compton -CG `, that should work.

# TODO
- Multiple monitor support
- More features
- ??????

Use `ag TODO` or your favorite IDE to find TODOs in code.

# Build deps
```bash
sudo apt install cmake qtbase5-dev qtdeclarative5-dev libqt5xdg-dev
```

For X11 plugin you also need:
```bash
sudo apt install libx11-dev
```

# Build
To clone this repo with dependencies (GSL) use `--recursive` flag:
```bash
git clone --recursive git@github.com:RussianBruteForce/iq-notifier.git
mkdir iq-notifier/build; cd iq-notifier/build
# set X11 if you want X11 plugin
cmake -DCMAKE_BUILD_TYPE=Release -DX11 ..
make
```

Tested on ubuntu 16.04, GCC 5.4.0 and Clang 3.8.0.

# Config
Config path: `$XDG_CONFIG_HOME/iq-notifier/config` (`~/.config/iq-notifier/config`)

All 'modules' of bzard should be enabled in config file explicitly. To copy example config execute from repo root:
```bash
cp config.example ~/.config/iq-notifier/config
```

Or, if you installed bzard from package jut run it. Config will be copied to your home dir automatically at first start.


# Themes
Theme is a directory with `theme` file which is simple text. For default themes look at `/usr/share/iq-notifier/themes` dir. They will be copied to your home dir automatically at first start.
Themes must be placed in `$XDG_CONFIG_HOME/iq-notifier/themes` (`~/.config/iq-notifier/themes`) directory.

# Deb package
```bash
cpack
sudo apt update
sudo dpkg -i iq-notifier-*-amd64.deb
sudo apt installf -f
```

# RMP repository
[Repo by ZaWertun](https://copr.fedorainfracloud.org/coprs/zawertun/scrapyard/)

# Contributions
Feel free to make pull requests/fork this project. You can also contact me via e-mail: [bruteforce@sigil.tk](mailto:bruteforce@sigil.tk)

You also need `clang-format` and `cpplint`.

# License
Look at COPYING file.

## Spec
[Version 1.2](https://people.gnome.org/~mccann/docs/notification-spec/notification-spec-latest.html)

## Capabilities
- actions
- body
- body-markup
- icon-static
- persistence

Sure! Here's your **bzard color palette** in English, using Markdown with HTML blocks for inline color swatches:

---

### 🎨 **bzard Color Palette (Nord-inspired, minimal + utility)**

| Purpose              | Color Name      | HEX       | Preview   |
|----------------------|------------------|-----------|-----------|
| **Background/Base**  | Dark Gray        | `#202124` | <span style="display:inline-block;width:24px;height:24px;background:#202124;border:1px solid #444;"></span> |
| **Accent Primary**   | Soft Blue        | `#5e81ac` | <span style="display:inline-block;width:24px;height:24px;background:#5e81ac;border:1px solid #444;"></span> |
| **Accent Secondary** | Polar Blue       | `#88c0d0` | <span style="display:inline-block;width:24px;height:24px;background:#88c0d0;border:1px solid #444;"></span> |
| **Neutral Tone**     | Light Gray       | `#d8dee9` | <span style="display:inline-block;width:24px;height:24px;background:#d8dee9;border:1px solid #444;"></span> |
| **Highlight/Focus**  | White            | `#ffffff` | <span style="display:inline-block;width:24px;height:24px;background:#ffffff;border:1px solid #ccc;"></span> |

> Use `#5e81ac → #88c0d0` for smooth action gradients.  
> Reserve white for focus points, pings, or "done" signals.

