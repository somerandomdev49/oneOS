/*
 * Copyright (C) 2020 Nikita Melekhin
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation.
 */

#pragma once

#include "../Color.h"
#include "../PixelBitmap.h"
#include "../Rect.h"
#include <libcxx/std/String.h>
#include <libfoundation/ByteOrder.h>

namespace LG::PNG {

struct ChunkHeader {
    size_t len;
    uint8_t type[4];
};

struct IHDRChunk {
    uint32_t width;
    uint32_t height;
    uint8_t depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
};

class DataStreamer {
public:
    DataStreamer() = default;

    DataStreamer(const void* data)
        : m_ptr((uint8_t*)data)
    {
    }

    ~DataStreamer() { }

    template <typename T>
    void read(T& val)
    {
        val = *(T*)m_ptr;
        m_ptr += sizeof(T);
        val = LFoundation::ByteOrder::from_network(val);
    }

    void read(void* buffer, size_t cnt)
    {
        memcpy((uint8_t*)buffer, m_ptr, cnt);
        m_ptr += cnt;
    }

    template <typename T>
    T at(int index) const
    {
        T val = *((T*)m_orig_ptr + index);
        val = LFoundation::ByteOrder::from_network(val);
        return val;
    }

    void skip(size_t cnt) { m_ptr += cnt; }
    void set(const void* data) { m_orig_ptr = m_ptr = (uint8_t*)data; }
    const uint8_t* ptr() const { return m_ptr; }
    uint8_t* ptr() { return m_ptr; }

private:
    uint8_t* m_orig_ptr { nullptr };
    uint8_t* m_ptr { nullptr };
};

class Scanline {
public:
    Scanline() = default;
    Scanline(int type, void* ptr)
        : m_filter_type(type)
        , m_ptr(ptr)
    {
    }
    ~Scanline() { }

    void set(int type, void* ptr) { m_ptr = ptr; }
    int filter_type() const { return m_filter_type; }
    uint8_t* data() const { return (uint8_t*)m_ptr; }

private:
    int m_filter_type { 0 };
    void* m_ptr { nullptr };
    size_t m_len { 0 };
};

class ScanlineKeeper {
public:
    ScanlineKeeper() = default;
    ~ScanlineKeeper() { }

    void init(void* ptr, uint8_t color_length) { m_ptr = ptr, m_color_length = color_length; }
    void invalidate()
    {
        free(m_ptr);
        m_data.clear();
    }

    void add(Scanline&& el) { m_data.push_back(move(el)); }
    uint8_t color_length() const { return m_color_length; }

    Vector<Scanline>& scanlines() { return m_data; }
    const Vector<Scanline>& scanlines() const { return m_data; }

private:
    uint8_t m_color_length { 0 };
    void* m_ptr { nullptr };
    Vector<Scanline> m_data;
};

class PNGLoader {
public:
    PNGLoader() = default;
    ~PNGLoader() { }

    PixelBitmap load_from_file(const String& path);
    PixelBitmap load_from_mem(const uint8_t* ptr);

    inline DataStreamer& streamer() { return m_streamer; }

private:
    bool check_header(const uint8_t* ptr) const;

    void proccess_stream(PixelBitmap& bitmap);
    bool read_chunk(PixelBitmap& bitmap);
    void read_IHDR(ChunkHeader& header, PixelBitmap& bitmap);
    void read_TEXT(ChunkHeader& header, PixelBitmap& bitmap);
    void read_IDAT(ChunkHeader& header, PixelBitmap& bitmap);

    uint8_t paeth_predictor(int a, int b, int c);
    void unfilter_scanlines();
    void copy_scanlines_to_bitmap(PixelBitmap& bitmap);

    DataStreamer m_streamer;
    IHDRChunk m_ihdr_chunk;
    ScanlineKeeper m_scanline_keeper;
};

}