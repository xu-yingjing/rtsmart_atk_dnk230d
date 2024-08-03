/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <dfs_fs.h>
#include <rthw.h>
#include <rtthread.h>
#include <stdio.h>
#include <string.h>
#ifdef PKG_CHERRYUSB_HOST
#include "usbh_core.h"
#endif
#ifdef PKG_CHERRYUSB_DEVICE
#include "usbd_core.h"
#endif

#ifdef RT_USING_SDIO

static const struct dfs_mount_tbl custom_mount_table[] = {{
#ifdef CONFIG_BOARD_K230D_CANMV_BPI_ZERO
                                                "sd10",
#else
                                                "sd00",
#endif
                                                "/bin", "elm", 0, 0},
                                            {
#ifdef CONFIG_BOARD_K230D_CANMV_BPI_ZERO
                                                "sd11",
#else
                                                "sd01",
#endif
                                                "/sdcard", "elm", 0, 0},
                                            {0}};

static int mnt_mount_table(void)
{
    int index = 0;

    while (1)
    {
        if (custom_mount_table[index].path == NULL) break;

        if (0x00 != dfs_mount(custom_mount_table[index].device_name,
                      custom_mount_table[index].path,
                      custom_mount_table[index].filesystemtype,
                      custom_mount_table[index].rwflag,
                      custom_mount_table[index].data))
        {
            rt_kprintf("mount fs[%s] on %s failed, error %d.\n", custom_mount_table[index].filesystemtype,
                       custom_mount_table[index].path, errno);
            return -RT_ERROR;
        }

        index ++;
    }
    return 0;
}
#endif

int main(void) {
  printf("CanMV Start\n");

#ifdef RT_USING_SDIO
  while (mmcsd_wait_cd_changed(100) != MMCSD_HOST_PLUGED) {
  }
  mnt_mount_table();
#endif //RT_USING_SDIO

#ifdef PKG_CHERRYUSB_HOST
  void *usb_base;
#ifdef CHERRYUSB_HOST_USING_USB1
  usb_base = (void *)rt_ioremap((void *)0x91540000UL, 0x10000);
#else
  usb_base = (void *)rt_ioremap((void *)0x91500000UL, 0x10000);
#endif
  usbh_initialize(0, (uint32_t)usb_base);
#endif
#ifdef PKG_CHERRYUSB_DEVICE
  extern void cdc_acm_mtp_init(void);
  cdc_acm_mtp_init();
  rt_thread_delay(10);
#endif

  msh_exec("/sdcard/micropython", 32);

  return 0;
}
