#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <dfs_romfs.h>

const struct romfs_dirent _root_dirent[] = {
    {ROMFS_DIRENT_DIR, "bin", RT_NULL, 0},
#ifdef RT_USING_DFS_DEVFS
    {ROMFS_DIRENT_DIR, "dev", RT_NULL, 0},
#endif
#ifdef RT_USING_SDIO
    {ROMFS_DIRENT_DIR, "sdcard", RT_NULL, 0},
#endif
#ifdef RT_USING_DFS_NFS
    {ROMFS_DIRENT_DIR, "nfs", RT_NULL, 0},
#endif
#ifdef RT_USING_PROC
    {ROMFS_DIRENT_DIR, "proc", RT_NULL, 0},
#endif
};

const struct romfs_dirent romfs_root = {
    ROMFS_DIRENT_DIR, "/", (rt_uint8_t *)_root_dirent,
    sizeof(_root_dirent) / sizeof(_root_dirent[0])};

#ifndef RT_USING_DFS_MNTTABLE

const struct dfs_mount_tbl mount_table[] = {{
#if CONFIG_BOARD_K230D_CANMV_BPI_ZERO
                                                "sd10",
#else
                                                "sd0"
#endif
                                                "/bin", "elm", 0, 0},
                                            {
#if CONFIG_BOARD_K230D_CANMV_BPI_ZERO
                                                "sd11",
#else
                                                "sd1"
#endif
                                                "/sdcard", "elm", 0, 0},
                                            {0}};

static int mnt_mount_table(void)
{
    int index = 0;

    while (1)
    {
        if (mount_table[index].path == NULL) break;

        if (dfs_mount(mount_table[index].device_name,
                      mount_table[index].path,
                      mount_table[index].filesystemtype,
                      mount_table[index].rwflag,
                      mount_table[index].data) != 0)
        {
            rt_kprintf("mount fs[%s] on %s failed.\n", mount_table[index].filesystemtype,
                       mount_table[index].path);
            return -RT_ERROR;
        }

        index ++;
    }
    return 0;
}
INIT_APP_EXPORT(mnt_mount_table);
#endif

int mnt_init(void) {
  rt_err_t ret;

  if (dfs_mount(RT_NULL, "/", "rom", 0, &romfs_root) != 0) {
    rt_kprintf("Dir / mount failed!\n");
    return -1;
  }

  mkdir("/dev/shm", 0x777);

  if (dfs_mount(RT_NULL, "/dev/shm", "tmp", 0, 0) != 0) {
    rt_kprintf("Dir /dev/shm mount failed!\n");
  }

#ifndef RT_FASTBOOT
  rt_kprintf("/dev/shm file system initialization done!\n");
#endif

  return 0;
}
INIT_ENV_EXPORT(mnt_init);

#endif
