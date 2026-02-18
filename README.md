# bzard
bzard is fancy and flexible notification daemon.

## Developement 

### Dependencies

```bash
sudo apt update
sudo apt install \
  build-essential cmake pkg-config \
  qt6-base-dev qt6-base-dev-tools \
  qt6-declarative-dev qt6-qmltooling-plugins \
  qt6-tools-dev qt6-tools-dev-tools \
  libqt6xdg-dev \
  libudev-dev \
  libx11-dev # for X11 plugin
```

You need layer-shell to use bzard in Wayland.

```
sudo apt install layer-shell-qt
```

### C++ Naming Convention

| Entity                              | Style                   | Example                        |
| ----------------------------------- | ----------------------- | ------------------------------ |
| **Classes**                         | `CamelCase`             | `MyClass`                      |
| **Methods (member functions)**      | `camelCase`             | `processData()`                |
| **Free functions**                  | `snake_case`            | `calculate_sum()`              |
| **Source files**                    | `snake_case`            | `data_processor.cpp`           |
| **Header files**                    | `snake_case`            | `data_processor.h`             |
| **Class member variables (fields)** | `camelCase` (no prefix) | `currentValue`                 |
| **Local variables**                 | `camelCase`             | `localValue`                   |
| **Constants**                       | `UPPER_CASE`            | `MAX_BUFFER_SIZE`              |
| **Enums (types)**                   | `CamelCase`             | `ColorType`                    |
| **Enum values**                     | `UPPER_CASE`            | `RED`, `BLUE`                  |
| **Template parameters**             | `CamelCase`             | `template<typename ValueType>` |
| **Macros**                          | `UPPER_CASE`            | `MY_LIBRARY_EXPORT`            |
| **Namespaces**                      | `snake_case`            | `my_project::utils`            |
| **Type aliases (`using`)**          | `CamelCase`             | `using StringMap = ...`        |
| **Global static variables**         | `g_snake_case`          | `g_config_path`                |

## Build Release

```bash
git clone https://github.com/x6prl/bzard.git
cd bzard
cmake -S . -B build
cmake --build build --config Release 
```

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
# Config
Config path: `$XDG_CONFIG_HOME/bzard/config` (`~/.config/bzard/config`)

All 'modules' of bzard should be enabled in config file explicitly. To copy example config execute from repo root:
```bash
cp config.example ~/.config/bzard/config
```

Or, if you installed bzard from package jut run it. Config will be copied to your home dir automatically at first start.


# Themes
Theme is a directory with `theme` file which is simple text. For default themes look at `/usr/share/bzard/themes` dir. They will be copied to your home dir automatically at first start.
Themes must be placed in `$XDG_CONFIG_HOME/bzard/themes` (`~/.config/bzard/themes`) directory.

# Deb package
```bash
cpack
sudo apt update
sudo dpkg -i bzard-*-amd64.deb
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

