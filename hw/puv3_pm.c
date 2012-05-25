/*
 * Power Management device simulation in PKUnity SoC
 *
 * Copyright (C) 2010-2012 Guan Xuetao
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation, or any later version.
 * See the COPYING file in the top-level directory.
 */
#include "hw.h"
#include "sysbus.h"

#undef DEBUG_PUV3
#include "puv3.h"

typedef struct {
    SysBusDevice busdev;
    MemoryRegion iomem;

    uint32_t reg_PMCR;
    uint32_t reg_PCGR;
    uint32_t reg_PLL_SYS_CFG;
    uint32_t reg_PLL_DDR_CFG;
    uint32_t reg_PLL_VGA_CFG;
    uint32_t reg_DIVCFG;
} PUV3PMState;

static uint64_t puv3_pm_read(void *opaque, target_phys_addr_t offset,
        unsigned size)
{
    PUV3PMState *s = (PUV3PMState *) opaque;
    uint32_t ret;

    switch (offset) {
    case 0x14:
        ret = s->reg_PCGR;
        break;
    case 0x18:
        ret = s->reg_PLL_SYS_CFG;
        break;
    case 0x1c:
        ret = s->reg_PLL_DDR_CFG;
        break;
    case 0x20:
        ret = s->reg_PLL_VGA_CFG;
        break;
    case 0x24:
        ret = s->reg_DIVCFG;
        break;
    case 0x28: /* PLL SYS STATUS */
        ret = 0x00002401;
        break;
    case 0x2c: /* PLL DDR STATUS */
        ret = 0x00100c00;
        break;
    case 0x30: /* PLL VGA STATUS */
        ret = 0x00003801;
        break;
    case 0x34: /* DIV STATUS */
        ret = 0x22f52015;
        break;
    case 0x38: /* SW RESET */
        ret = 0x0;
        break;
    case 0x44: /* PLL DFC DONE */
        ret = 0x7;
        break;
    default:
        hw_error("%s: Bad offset 0x%x\n", __func__, offset);
    }
    DPRINTF("offset 0x%x, value 0x%x\n", offset, ret);

    return ret;
}

static void puv3_pm_write(void *opaque, target_phys_addr_t offset,
        uint64_t value, unsigned size)
{
    PUV3PMState *s = (PUV3PMState *) opaque;

    switch (offset) {
    case 0x0:
        s->reg_PMCR = value;
    case 0x14:
        s->reg_PCGR = value;
        break;
    case 0x18:
        s->reg_PLL_SYS_CFG = value;
        break;
    case 0x1c:
        s->reg_PLL_DDR_CFG = value;
        break;
    case 0x20:
        s->reg_PLL_VGA_CFG = value;
        break;
    case 0x24:
    case 0x38:
        break;
    default:
        hw_error("%s: Bad offset 0x%x\n", __func__, offset);
    }
    DPRINTF("offset 0x%x, value 0x%x\n", offset, value);
}

static const MemoryRegionOps puv3_pm_ops = {
    .read = puv3_pm_read,
    .write = puv3_pm_write,
    .impl = {
        .min_access_size = 4,
        .max_access_size = 4,
    },
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static int puv3_pm_init(SysBusDevice *dev)
{
    PUV3PMState *s = FROM_SYSBUS(PUV3PMState, dev);

    s->reg_PCGR = 0x0;

    memory_region_init_io(&s->iomem, &puv3_pm_ops, s, "puv3_pm",
            PUV3_REGS_OFFSET);
    sysbus_init_mmio(dev, &s->iomem);

    return 0;
}

static void puv3_pm_class_init(ObjectClass *klass, void *data)
{
    SysBusDeviceClass *sdc = SYS_BUS_DEVICE_CLASS(klass);

    sdc->init = puv3_pm_init;
}

static const TypeInfo puv3_pm_info = {
    .name = "puv3_pm",
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(PUV3PMState),
    .class_init = puv3_pm_class_init,
};

static void puv3_pm_register_type(void)
{
    type_register_static(&puv3_pm_info);
}

type_init(puv3_pm_register_type)
