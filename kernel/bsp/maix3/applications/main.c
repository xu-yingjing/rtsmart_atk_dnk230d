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

int main(void) {
  printf("RT-SMART Hello RISC-V\n");

#ifdef RT_USING_SDIO
  while (mmcsd_wait_cd_changed(100) != MMCSD_HOST_PLUGED) {
  }
  if (0x00 != dfs_mount("sd0", "/bin", "elm", 0, 0)) {
    rt_kprintf("Dir /bin mount failed!\n");
  }
  if (0x00 != dfs_mount("sd1", "/sdcard", "elm", 0, 0)) {
    rt_kprintf("Dir /sdcard mount failed!\n");
  }
#endif
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

  msh_exec("/sdcard/canmv/micropython", 32);

  return 0;
}
