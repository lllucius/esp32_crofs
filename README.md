# CroFS filesystem for the ESP32
CroFS is a compressed, read-only filesystem for the ESP32 suitable for
embedding during the build of an application.

To use, just add the "crofs" component (or the entire "crofs" repo) to your
components directory and run "make menuconfig" to set the parameters (look
for "CroFS Configuration").

The configuration settings are:

#### Source paths
This specifies the blank separated path name(s) to one or more directories whose
files will be added to the CroFS filesystem image.

If this setting is blank, a filesystem will not be embedded during the build, so
you will need to supply the image in some other means.

#### Compression level
Allows you to specify the compression level with a range of 0 to 10.

A value of 0 disables compression, while a value of 1 to 10 can be used
with increasing amounts of compression.

The default is 6.

### Example

Setting up an embedded filesystem for use is pretty easy:

```
#include "littleflash.h"

void app_main()
{
    const crofs_config_t cfg =
    {
        .fs_addr = CroFS::get_embedded_start(),
        .fs_size = CroFS::get_embedded_size(),
        .base_path = "/crofs",
        .open_files = 4
    };

    CroFS crofs;

    if (crofs.init(&cfg) != 0)
    {
        ...
        abort();
    }

    ...
}
```

The configuration options for CroFS are:

```
typedef struct
{
    const uint8_t *fs_addr;     // address to (in-memory) filesystem
    size_t fs_size;             // size of (in-memory) filesystem
    const char *base_path;      // mount point
    int open_files;             // number of open files to support
} crofs_config_t;
```

### Creating "crofs" filesystem images

During the build process a command line tool (mkcrofs) is built that
can be used to create image files.  It will be run automatically if
you specify at least one path for the "Source paths" menu configuration
option.

However, if you want to create the filesystem image in some other way,
you can use the "mkcrofs" command via a predefined Makefile variable
with something like:

```
mytarget:
    $(MKCROFS) imagename my/input/files
```

The command itself is built in the "build/crofs" directory within your
project.

#### Usage

Usage: build/crofs/mkcrofs [-l level] [-t] [-v] imagepath sourcepath ...

    -l level    compression level, 0=no compression or 1 to 10 (default=6)
    -t          run tests against the built image
    -v          verbose
    imagepath   path to the built image
    sourcepath  one or more paths to the input directories


