/*
   Copyright (C) 2011
   Her Majesty the Queen in Right of Canada (Communications Research
   Center Canada)

   Copyright (C) 2016
   Matthias P. Braendli, matthias.braendli@mpb.li

    http://opendigitalradio.org
 */
/*
   This file is part of ODR-DabMod.

   ODR-DabMod is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   ODR-DabMod is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ODR-DabMod.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Buffer.h"
#include "PcDebug.h"

#include <string>
#include <stdlib.h>
#include <string.h>

Buffer::Buffer(size_t len, const void *data)
{
    PDEBUG("Buffer::Buffer(%zu, %p)\n", len, data);

    m_len = 0;
    m_size = 0;
    m_data = NULL;
    setData(data, len);
}

Buffer::Buffer(const std::vector<uint8_t> &vec)
{
    PDEBUG("Buffer::Buffer(vector [%zu])\n", vec.size());

    m_len = 0;
    m_size = 0;
    m_data = NULL;
    setData(vec.data(), vec.size());
}


Buffer::~Buffer()
{
    PDEBUG("Buffer::~Buffer() len=%zu, data=%p\n", m_len, m_data);
    free(m_data);
}


Buffer &Buffer::operator=(const Buffer &copy)
{
    setData(copy.m_data, copy.m_len);
    return *this;
}

Buffer &Buffer::operator=(const std::vector<uint8_t> &copy)
{
    setData(copy.data(), copy.size());
    return *this;
}

Buffer &Buffer::operator+=(const Buffer &copy)
{
    appendData(copy.m_data, copy.m_len);
    return *this;
}


void Buffer::setLength(size_t len)
{
    if (len > m_size) {
        void *tmp = m_data;

        /* Align to 32-byte boundary for AVX. */
        const int ret = posix_memalign(&m_data, 32, len);
        if (ret != 0) {
            throw std::runtime_error("memory allocation failed: " +
                    std::to_string(ret));
        }

        if (tmp != NULL) {
            memcpy(m_data, tmp, m_len);
            free(tmp);
        }
        m_size = len;
    }
    m_len = len;
}


void Buffer::setData(const void *data, size_t len)
{
    setLength(0);
    appendData(data, len);
}


void Buffer::appendData(const void *data, size_t len)
{
    size_t offset = m_len;
    setLength(m_len + len);
    if (data != NULL) {
        memcpy((char*)m_data + offset, data, len);
    }
}

