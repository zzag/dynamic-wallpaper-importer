# dynamic-wallpaper-importer


## Building dynamic-wallpaper-importer from Git

Before building dynamic-wallpaper-importer from source code, you need to install
a couple of prerequisites.

Arch Linux:

```sh
sudo pacman -S libheif libplist qt5-base
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
    -DCMAKE_INSTALL_PREFIX=/usr
```

Now trigger the build by running the following command

```sh
make
```

To install run

```sh
sudo make install
```


## Usage

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
