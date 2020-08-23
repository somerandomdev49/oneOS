/*
 * Copyright (C) 2020 Nikita Melekhin
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation.
 */

#ifndef __oneOS__ALGO__RINGBUFFER_H
#define __oneOS__ALGO__RINGBUFFER_H

#include <types.h>
#include <mem/vmm/zoner.h>

struct __ringbuffer {
    zone_t zone;
    uint32_t start;
    uint32_t end;
};
typedef struct __ringbuffer ringbuffer_t;

ringbuffer_t ringbuffer_create(uint32_t size);
ringbuffer_t ringbuffer_create_std();
void ringbuffer_free(ringbuffer_t* buf);

uint32_t ringbuffer_space_to_read(ringbuffer_t* buf);
uint32_t ringbuffer_space_to_write(ringbuffer_t* buf);
uint32_t ringbuffer_read(ringbuffer_t* buf, uint8_t*, uint32_t);
uint32_t ringbuffer_write(ringbuffer_t* buf, const uint8_t*, uint32_t);
uint32_t ringbuffer_read_one(ringbuffer_t* buf, uint8_t* data);
uint32_t ringbuffer_write_one(ringbuffer_t* buf, uint8_t data);

#endif //__oneOS__ALGO__RINGBUFFER_H