# dynamic-wallpaper-importer

**Update**: if you experience issues with this tool like getting "No meta box"
errors, see https://github.com/strukturag/libheif/issues/172


## Building dynamic-wallpaper-importer from Git

Before building dynamic-wallpaper-importer from source code, you need to install
a couple of prerequisites.

Arch Linux:

```sh
sudo pacman -S cmake extra-cmake-modules git libheif libplist qt5-base
```

Ubuntu:

```sh
sudo apt install cmake extra-cmake-modules git libheif-dev libplist-dev qtbase5-dev
```

Once all prerequisites are installed, you need to grab the source code

```sh
git clone https://github.com/zzag/dynamic-wallpaper-importer.git
cd dynamic-wallpaper-importer
```

Configure the build

```sh
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    -DCMAKE_INSTALL_LIBDIR=lib
```

Now trigger the build by running the following command

```sh
make
```

To install run

```sh
sudo make install
```


## Install dynamic-wallpaper-importer using a package manager

Arch Linux:

```sh
yay -S dynamic-wallpaper-importer
```


## Usage

```
$ dynamic-wallpaper-importer --source wallpaper.heic --id fancy_wallpaper --label "Fancy Wallpaper"
```

```
$ dynamic-wallpaper-importer --help
Usage: dynamic-wallpaper-importer [options]
Dynamic wallpaper importer

Options:
  -h, --help            Displays this help.
  -v, --version         Displays version information.
  --format <png|jpg>    Preferred image format.
  --source <file>       Path to the source dynamic wallpaper.
  --id <id>             Preferred id of the wallpaper.
  --label <label>       Preferred name of the wallpaper.
  --target <directory>  Directory where wallpaper will be stored.
```


## Related

* [plasma5-wallpapers-dynamic](https://github.com/zzag/plasma5-wallpapers-dynamic) -
  Dynamic wallpaper plugin for KDE Plasma
