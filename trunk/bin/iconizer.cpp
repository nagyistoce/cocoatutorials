//------------------------------------------
//Here comes the listing
//------------------------------------------

#include <stdio.h>
#include <CoreServices/CoreServices.h>
#include <sys/mount.h>

#include <DiskArbitration/DiskArbitration.h>

extern "C" kern_return_t DiskArbInit(void)
__attribute__((weak_import));
extern "C" kern_return_t DiskArbDiskAppearedWithMountpointPing_auto(
  char     *disk,
  unsigned  flags,
  char     *mountpoint
) __attribute__((weak_import));

static int
ping_diskarb(char *mntpath)
{
    int ret;
    struct statfs sb;
    enum {
        kDiskArbDiskAppearedEjectableMask   = 1 << 1,
        kDiskArbDiskAppearedWholeDiskMask   = 1 << 2,
        kDiskArbDiskAppearedNetworkDiskMask = 1 << 3
    };

    ret = statfs(mntpath, &sb);
    if (ret < 0) {
        return ret;
    }

    if (!DiskArbInit || !DiskArbDiskAppearedWithMountpointPing_auto) {
        return 0;
    }

    ret = DiskArbInit();

    /* we ignore the return value from DiskArbInit() */

    ret = DiskArbDiskAppearedWithMountpointPing_auto(
              sb.f_mntfromname,
              (kDiskArbDiskAppearedEjectableMask |
               kDiskArbDiskAppearedNetworkDiskMask),
              mntpath);

    /* we ignore the return value from the ping */

    return 0;
}

OSStatus IconizeAndShowVfs(char *path)
{
        FSRef ref;
        OSStatus st = FSPathMakeRef((const UInt8*)path, &ref, 0);
        if (st != KERN_SUCCESS)
                return st;
        FSCatalogInfo info;
        st = FSGetCatalogInfo(&ref, kFSCatInfoFinderInfo, &info, 0, 0, 0);
        if (st != KERN_SUCCESS)
                return st;
        ((FolderInfo*)info.finderInfo)->finderFlags = kHasCustomIcon;
        st = FSSetCatalogInfo(&ref, kFSCatInfoFinderInfo, &info);
        if (st != KERN_SUCCESS)
                return st;
        ping_diskarb(path);

        return KERN_SUCCESS;
}

int main (int argc, char * const argv[]) {
        if (argc < 2)
        {
                printf("Usage: iconizer <mount path>\n");
                return 1;
        }
        OSStatus st = IconizeAndShowVfs(argv[1]);
        if (st != KERN_SUCCESS)
                printf("Cannot iconize with status %d!\n", st);
    return 0;
}
